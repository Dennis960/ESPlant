import { defineMock } from "vite-plugin-mock-dev-server";

export default defineMock({
    url: "/api/cloudTest/mqtt",
    body: "true",
});
