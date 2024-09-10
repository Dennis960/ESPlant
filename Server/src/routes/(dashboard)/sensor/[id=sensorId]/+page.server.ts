import SensorController from '$lib/server/controllers/SensorController';
import { error } from '@sveltejs/kit';
import type { PageServerLoad } from './$types';

export const load: PageServerLoad = async function ({ params, url, depends, locals }) {
	await locals.middleware.security.isOwnerOrThisSensorRead(
		params.id,
		url.searchParams.get('token')
	);
	depends('sensor-history');
	depends('sensor');
	const id = parseInt(params.id);

	let startDate = new Date();
	startDate.setDate(startDate.getDate() - 3);
	startDate.setHours(0, 0, 0, 0);
	let endDate = new Date();
	endDate.setHours(23, 59, 59, 999);
	let maxDataPoints = 100;

	if (url.searchParams.has('from')) {
		const from = parseInt(url.searchParams.get('from') ?? '');
		if (!isNaN(from)) {
			startDate = new Date(from);
		}
	}

	if (url.searchParams.has('to')) {
		const to = parseInt(url.searchParams.get('to') ?? '');
		if (!isNaN(to)) {
			endDate = new Date(to);
		}
	}

	if (url.searchParams.has('maxDataPoints')) {
		const max = parseInt(url.searchParams.get('maxDataPoints') ?? '');
		if (!isNaN(max)) {
			maxDataPoints = max;
		}
	}

	const sensor = await new SensorController().getSensor(id);
	if (sensor == undefined) {
		throw error(404, 'Sensor nicht gefunden');
	}

	const sensorData = await new SensorController().getSensorHistory(
		id,
		startDate,
		endDate,
		maxDataPoints
	);

	return {
		id,
		startDate,
		endDate,
		sensor,
		sensorData
	};
};
