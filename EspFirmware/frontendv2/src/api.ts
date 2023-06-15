import { loadingState } from './states';
export type Network = {
    rssi: number;
    ssid: string;
    bssid: string;
    channel: number;
    secure: number;
    hidden: boolean;
};

export enum WifiStatus {
    IDLE = 0,
    NO_SSID_AVAIL = 1,
    CONNECTED = 3,
    CONNECT_FAILED = 4,
    CONNECT_WRONG_PASSWORD = 6,
    DISCONNECTED = 7,
    ERROR = 8,
}

/* fetch loading state indicator */
const fetch = new Proxy(window.fetch, {
    apply: async (target, thisArg, args) => {
      loadingState.state = true;
      const res = await target.apply(thisArg, args);
      loadingState.state = false;
      return res;
    },
  });

async function postDataToEsp(url: string, params?: URLSearchParams) {
    return await fetch(url, {
        method: "POST",
        body: params,
        headers: {
            "Content-Type": "application/x-www-form-urlencoded",
        },
    });
}

async function getDataFromEsp(url: string) {
    return await fetch(url)
        .then((response) => response.json())
        .catch((error) => {
            console.error(error);
            return null;
        });
}

export async function setPlantName(name: string) {
    const params = new URLSearchParams();
    params.append("name", name);
    return await postDataToEsp("/plantName", params);
}

export async function getNetworks(): Promise<Network[]> {
    return await getDataFromEsp("/networks");
}

export async function getConnectedNetwork(): Promise<Network> {
    // TODO: implement this on the esp
    return null;
    return await getDataFromEsp("/connectedNetwork");
}

export async function connectToNetwork(ssid: string, password: string) {
    const params = new URLSearchParams();
    params.append("ssid", ssid);
    params.append("password", password);
    return await postDataToEsp("/connect", params);
}

export async function resetEsp() {
    return await postDataToEsp("/reset");
}

/**
 * @returns WifiStatus
 */
export async function isEspConnected(): Promise<WifiStatus> {
    const res = await getDataFromEsp("/isConnected");
    if (res == null) {
        return WifiStatus.ERROR;
    }
    return Number(res);
}

export async function setMqttCredentials(
    server: string,
    port: string,
    username: string,
    password: string
) {
    const params = new URLSearchParams();
    params.append("mqttServer", server);
    params.append("mqttPort", port);
    params.append("mqttUsername", username);
    params.append("mqttPassword", password);
    return await postDataToEsp("/mqttSetup", params);
}

export async function setSensorId(id: number) {
    const params = new URLSearchParams();
    params.append("sensorId", id.toString());
    return await postDataToEsp("/sensorId", params);
}

export async function getSensorId() {
    return Number(await getDataFromEsp("/sensorId"));
}

export async function getUpdatePercentage() {
    return Number(await getDataFromEsp("/update/percentage"));
}

async function uploadFile(file: File, url: string) {
    const formData = new FormData();
    formData.append("upload", file);
    return await fetch(url, {
        method: "POST",
        body: formData,
    });
}

export async function updateFs(file: File) {
    return await uploadFile(file, "/update/littlefs");
}

export async function updateFirmware(file: File) {
    return await uploadFile(file, "/update/firmware");
}
