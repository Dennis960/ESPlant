import cors from "cors";
import { json, Router } from "express";
import sensorRoutes from "./routes/sensors.js";
import espApiRoutes from "./routes/esp-api.js";
import webPushRoutes from "./routes/web-push.js";
import superjson from "./middlewares/superjson.js";
import passport from "./middlewares/passport.js";
import authRoutes from "./routes/auth.js";

const router = Router();
router.use(json());
router.use(cors());
router.use(passport);

router.use(authRoutes);

router.use(espApiRoutes);

router.use(superjson); // apply superjson to all dashboard API routes
router.use(sensorRoutes);
router.use(webPushRoutes);

export default router;
