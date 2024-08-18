import SensorController from "$lib/server/controllers/SensorController";
import type { PageServerLoad } from './$types';

export const load: PageServerLoad = (async (event) => {
    event.depends("sensor-overview");
    event.locals.middleware.auth.isAuthenticated();
    const sensorOverview = await new SensorController().getSensorOverview(event.locals.user!.id);
    return sensorOverview;
});