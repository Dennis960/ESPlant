import crypto from "crypto";
import {
  PlantHealthDTO,
  RSSIHistoryEntry,
  SensorConfigurationDTO,
  SensorCreatedDTO,
  SensorDTO,
  SensorHealthDTO,
  SensorHistoryDTO,
  SensorOverviewDTO,
  SensorReadingDTO,
  SensorValueDistributionDTO,
  WaterCapacityHistoryEntry,
} from "../types/api.js";
import SensorDataRepository from "../repositories/SensorDataRepository.js";
import SensorRepository from "../repositories/SensorRepository.js";
import SensorService from "../services/SensorService.js";
import { ESPSensorReadingDTO } from "../entities/SensorReadingEntity.js";
import SensorEntity from "../entities/SensorEntity.js";

const OFFLINE_TIMEOUT = 120 * 60 * 1000; // 2 hours

export default class SensorController {
  public async getSensorHistory(
    id: number,
    startDate: Date,
    endDate: Date,
    limit: number = 1000
  ): Promise<SensorHistoryDTO | undefined> {
    const sensorData = await SensorService.getReadings(
      id,
      startDate,
      endDate,
      limit
    );

    const detectedWateringReadings =
      SensorService.detectWateringReadings(sensorData);

    const rssiHistory: RSSIHistoryEntry[] = sensorData.map((reading) => ({
      timestamp: reading.timestamp,
      rssi: reading.rssi,
    }));

    const waterCapacityHistory: WaterCapacityHistoryEntry[] = sensorData.map(
      (reading) => ({
        timestamp: reading.timestamp,
        detectedWatering: detectedWateringReadings.includes(reading),
        waterCapacity: reading.availableWaterCapacity,
      })
    );

    return {
      id,
      waterCapacityHistory,
      rssiHistory,
    };
  }

  public async getSensor(id: number): Promise<SensorDTO | undefined> {
    const sensorEntity = await SensorRepository.getById(id);
    if (sensorEntity == undefined) {
      return undefined;
    }
    const config = SensorEntity.toDTO(sensorEntity);

    const sensorData = await SensorService.getRecentReadings(id);

    const lastReading = sensorData[sensorData.length - 1];
    const model = SensorService.fitModel(sensorData);

    const sensorHealth = this.getSensorHealth(lastReading);
    const plantHealth = this.getPlantHealth(lastReading, config);

    return {
      id,
      config,
      lastUpdate:
        lastReading != undefined
          ? {
              timestamp: lastReading.timestamp,
              waterCapacity: lastReading.availableWaterCapacity,
            }
          : undefined,
      prediction:
        model != undefined
          ? {
              nextWatering: model.predictTimestamp(config.lowerThreshold),
              predictedWaterCapacity: model.predictEntries(60 * 60 * 1000, 24),
            }
          : undefined,
      sensorHealth,
      plantHealth,
    };
  }

  public async getSensorOverview(ownerId: number): Promise<SensorOverviewDTO> {
    const sensorsIds = await SensorRepository.getAllForOwner(ownerId);

    const sensors = await Promise.all(
      sensorsIds.map(
        async (sensor) => (await this.getSensor(sensor.sensorAddress))!
      )
    );

    // sort by water capacity
    sensors.sort((a, b) => {
      if (a.lastUpdate == undefined && b.lastUpdate == undefined) {
        return 0;
      }
      if (a.lastUpdate == undefined) {
        return 1;
      }
      if (b.lastUpdate == undefined) {
        return -1;
      }
      return a.lastUpdate.waterCapacity - b.lastUpdate.waterCapacity;
    });

    return {
      sensors,
    };
  }

  private getPlantHealth(
    lastReading: SensorReadingDTO | undefined,
    config: SensorConfigurationDTO
  ): PlantHealthDTO {
    const status = {
      drowning:
        lastReading != undefined && lastReading.availableWaterCapacity > 1.0,
      wilting:
        lastReading != undefined && lastReading.availableWaterCapacity < 0.0,
      overwatered:
        lastReading != undefined &&
        lastReading.availableWaterCapacity > config.upperThreshold,
      underwatered:
        lastReading != undefined &&
        lastReading.availableWaterCapacity < config.lowerThreshold,
    };

    const warning = status.overwatered || status.underwatered;
    const critical = status.drowning || status.wilting;
    return {
      ...status,
      warning,
      critical,
    };
  }

  private getSensorHealth(
    lastReading: SensorReadingDTO | undefined
  ): SensorHealthDTO {
    const status = {
      signalStrength:
        lastReading == undefined ||
        lastReading.timestamp < new Date(Date.now() - OFFLINE_TIMEOUT)
          ? ("offline" as const)
          : lastReading.rssi > -55
          ? ("strong" as const)
          : lastReading.rssi > -67
          ? ("moderate" as const)
          : ("weak" as const),
      lowBattery: lastReading != undefined && false, // TODO
    };

    const warning = status.signalStrength == "weak" || status.lowBattery;
    const critical = status.signalStrength == "offline";
    return {
      ...status,
      warning,
      critical,
    };
  }

  public async addSensorData(data: ESPSensorReadingDTO) {
    return await SensorDataRepository.create({
      sensorAddress: data.sensorAddress,
      date: Date.now(),
      water: data.water,
      voltage: data.voltage,
      duration: data.duration,
      rssi: data.rssi,
      measurementDuration: data.measurementDuration,
    });
  }

  public async getSensorValueDistribution(
    sensorId: number
  ): Promise<SensorValueDistributionDTO> {
    const since = new Date(Date.now() - 14 * 24 * 60 * 60 * 1000);
    const bucketSize = 16;
    const waterCapacityDistribution =
      await SensorDataRepository.getCountByWaterCapacityBucket(
        sensorId,
        since,
        bucketSize
      );

    return {
      waterCapacityDistribution: {
        bucketSize,
        entries: waterCapacityDistribution,
      },
    };
  }

  public async updateSensorConfig(
    sensorId: number,
    config: SensorConfigurationDTO
  ): Promise<SensorConfigurationDTO> {
    const sensorEntityPartial = await SensorEntity.fromDTO(sensorId, config);
    const sensorEntity = await SensorRepository.update(
      sensorId,
      sensorEntityPartial
    );
    return SensorEntity.toDTO(sensorEntity);
  }

  private generateToken(): string {
    let token = 'blumy_';
    const possibleChars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    for (let i = 0; i < 32; i++) {
        let randomIndex = crypto.randomInt(0, possibleChars.length);
        token += possibleChars[randomIndex];
    }
    return token;
  }

  public async create(ownerId: number, config: SensorConfigurationDTO): Promise<SensorCreatedDTO> {
    const token = this.generateToken();
    const sensorEntityPartial = await SensorEntity.fromDTO(0, config);
    const creatingSensorEntity = {
      ...sensorEntityPartial,
      sensorAddress: undefined,
      owner: ownerId,
      token,
    };
    const sensorEntity = await SensorRepository.create(creatingSensorEntity);

    return {
      token,
      id: sensorEntity.sensorAddress,
      config: SensorEntity.toDTO(sensorEntity),
    };
  }
}
