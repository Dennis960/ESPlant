import type {
	SensorDTO,
	SensorHistoryDTO,
	SensorOverviewDTO,
	SensorValueDistributionDTO,
	SensorConfigurationDTO
} from './types/api';
import SuperJSON from 'superjson';

const BASE_URL = import.meta.env.VITE_API_URL ?? 'https://esplant.hoppingadventure.com/api';

export async function fetchSensorOverview(): Promise<SensorOverviewDTO> {
	return await fetch(`${BASE_URL}/sensors`)
		.then((response) => response.text())
		.then((text) => SuperJSON.parse<SensorOverviewDTO>(text));
}

export async function fetchSensor(id: number): Promise<SensorDTO> {
	return await fetch(`${BASE_URL}/sensors/${id}`)
		.then((response) => response.text())
		.then((text) => SuperJSON.parse(text));
}

export async function fetchSensorHistory(
	id: number,
	startDate: Date,
	endDate: Date
): Promise<SensorHistoryDTO> {
	const params = new URLSearchParams({
		startDate: startDate.getTime().toString(),
		endDate: endDate.getTime().toString(),
		maxDataPoints: '1000'
	});
	return await fetch(`${BASE_URL}/sensors/${id}/history?` + params.toString())
		.then((response) => response.text())
		.then((text) => SuperJSON.parse(text));
}

export async function fetchSensorValueDistribution(
	id: number
): Promise<SensorValueDistributionDTO> {
	return await fetch(`${BASE_URL}/sensors/${id}/value-distribution`)
		.then((response) => response.text())
		.then((text) => SuperJSON.parse(text));
}

export async function submitSensorConfig(
	id: number,
	config: SensorConfigurationDTO
): Promise<SensorConfigurationDTO> {
	return await fetch(`${BASE_URL}/sensors/${id}/config`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: SuperJSON.stringify(config)
	})
		.then((response) => response.text())
		.then((text) => SuperJSON.parse(text));
}
