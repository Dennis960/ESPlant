import type { Sensor, SensorStub } from '$lib/api';

export enum SortKey {
	NAME,
	ID,
	WATER_CAPACITY,
	NEXT_WATERING,
	SENSOR_HEALTH
}
export function sortQueryDataBy(
	queryData: SensorStub[],
	sensors: Record<number, Sensor>,
	sortKey: SortKey,
	ascending = true
) {
	return queryData.sort((a, b) => {
		if (sortKey === SortKey.NAME) return a.name.localeCompare(b.name);
		const sensorA = sensors[a.id];
		const sensorB = sensors[b.id];
		if (sensorA == undefined) return 1;
		if (sensorB == undefined) return -1;
		const comparison = compareSensorsBy(sensorA, sensorB, sortKey);
		return ascending ? comparison : -comparison;
	});
}
function compareSensorsBy(sensorA: Sensor, sensorB: Sensor, sortKey: SortKey) {
	if (sortKey === SortKey.ID) return sensorA.id - sensorB.id;
	if (sortKey === SortKey.NEXT_WATERING) {
		if (sensorA.estimatedNextWatering == undefined) return 1;
		if (sensorB.estimatedNextWatering == undefined) return -1;
		return sensorA.estimatedNextWatering.getTime() - sensorB.estimatedNextWatering.getTime();
	}
	if (sortKey === SortKey.WATER_CAPACITY) {
		if (sensorA.waterCapacityHistory == undefined || sensorA.waterCapacityHistory.length == 0)
			return 1;
		if (sensorB.waterCapacityHistory == undefined || sensorA.waterCapacityHistory.length == 0)
			return -1;
		// TODO check if this is correct, I want to compare the last water capacity
		return (
			sensorA.waterCapacityHistory[sensorA.waterCapacityHistory.length - 1].availableWaterCapacity -
			sensorB.waterCapacityHistory[sensorB.waterCapacityHistory.length - 1].availableWaterCapacity
		);
	}
	if (sortKey === SortKey.SENSOR_HEALTH) {
        // TODO add a better way to compare sensor health
		if (sensorA.status.signalStrength == 'offline') return 1;
		if (sensorB.status.signalStrength == 'offline') return -1;
		if (sensorA.status.signalStrength == 'weak') return 1;
		if (sensorB.status.signalStrength == 'weak') return -1;
		if (sensorA.status.lowBattery) return 1;
		if (sensorB.status.lowBattery) return -1;
		return 0;
	}
	return 0;
}
