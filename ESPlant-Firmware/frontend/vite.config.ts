import { resolve } from "path";
import { defineConfig } from "vite";
import { viteSingleFile } from "vite-plugin-singlefile";
import mockDevServerPlugin from "vite-plugin-mock-dev-server";

export default defineConfig({
    plugins: [mockDevServerPlugin(), viteSingleFile()],
    build: {
        rollupOptions: {
            input: {
                index: resolve(__dirname, "index.html"),
            },
        },
        outDir: "../main/data",
        target: "es2022",
        modulePreload: false,
    },
    server: {
        proxy: {
            "/api": {},
        },
    },
});
