import { beforeAll, afterAll, expect, test } from 'vitest'
import { knex } from '../config/knex.js';
import fs from 'fs';
import path from 'path';
import SensorRepository from '../repositories/SensorRepository';
import SensorEntity from '../entities/SensorEntity.js';
import { ESPSensorReadingDTO } from '../entities/SensorReadingEntity';
import SensorController from '../controllers/SensorController.js';
import { migrateDatabase } from '../migrations.js';

const API_URL = 'http://localhost:4803/api';

let server: any

const testSensor: SensorEntity = {
    sensorAddress: 99,
    name: 'test-sensor',
    image: null,
    fieldCapacity: 1024,
    permanentWiltingPoint: 100,
    lowerThreshold: 300,
    upperThreshold: 800,
    owner: 1,
    writeToken: 'test-token-write',
    readToken: 'test-token-read',
}

const testDate: Omit<ESPSensorReadingDTO, 'sensorAddress'> = {
    light: 1024,
    voltage: 3.2,
    temperature: 24.5,
    humidity: 53.10,
    isUsbConnected: false,
    moisture: 879,
    moistureStabilizationTime: 2034,
    isMoistureMeasurementSuccessful: true,
    humidityRaw: 123123,
    temperatureRaw: 123123,
    rssi: -36,
    duration: 3083,
}

async function countData() {
    return (await knex('data').count({
        count: '*',
    }).first())!['count'] as number
}

beforeAll(async () => {
    const dump = await fs.promises.readFile(path.join(__dirname, 'fixtures/prod_cleaned_2024-04-01_v1.sql'), 'utf-8')
    for (const statement of dump.split(';')) {
        if (!statement.trim()) {
            continue
        }
        await knex.raw(statement)
    }

    await migrateDatabase()

    await SensorRepository.create(testSensor)

    server = (await import('../index.js')).default
})

afterAll(async () => {
    server.close()
})

test('/profile returns 401 if not authenticated', async () => {
    const response = await fetch(`${API_URL}/profile`)
    expect(response.status).toBe(401)
})

test('/profile returns 403 if authenticated with sensor token', async () => {
    const response = await fetch(`${API_URL}/profile`, {
        headers: {
            'Authorization': `Bearer ${testSensor.writeToken}`,
        },
    })
    expect(response.status).toBe(403)
})

test('/data returns 401 if not authenticated', async () => {
    const response = await fetch(`${API_URL}/v2/data`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(testDate),
    })
    expect(response.status).toBe(401)
})

test('/data returns 403 if sending illegal token', async () => {
    const response = await fetch(`${API_URL}/v2/data`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': 'Bearer doesnotexist',
        },
        body: JSON.stringify(testDate),
    })
    expect(response.status).toBe(401)
})

test('/data inserts data if authenticated', async () => {
    const beforeCount = await countData()

    const response = await fetch(`${API_URL}/v2/data`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${testSensor.writeToken}`,
        },
        body: JSON.stringify(testDate),
    })
    expect(response.status).toBe(200)

    const afterCount = await countData()

    expect(afterCount).toBe(beforeCount + 1)
})

test('inserts data received over legacy MQTT API', async () => {
    const beforeCount = await countData()

    const sensorController = new SensorController()
    await sensorController.addLegacySensorData({
        plantName: testSensor.name,
        sensorAddress: testSensor.sensorAddress,
        water: 879,
        rssi: -36,
        duration: 3083,
        measurementDuration: 2034,
    })

    const afterCount = await countData()

    expect(afterCount).toBe(beforeCount + 1)
})