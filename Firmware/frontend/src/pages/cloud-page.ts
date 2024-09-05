import { StateController } from "@lit-app/state";
import { html } from "lit";
import { customElement, query, state } from "lit/decorators.js";
import {
    CloudConfiguration,
    CloudConfigurationType,
    cloudConfigurationTypes,
    disableCloudConnection,
    getCloudCredentials,
    setCloudCredentials,
    testCloudConnection,
} from "../api";
import { loadingState } from "../states";
import { BasePage } from "./base-page";
import "./page-elements/cloud-form-element";
import { CloudFormElement } from "./page-elements/cloud-form-element";
import "./page-elements/collapsible-element";
import "./page-elements/http-form-element";
import { HttpFormElement } from "./page-elements/http-form-element";
import "./page-elements/mqtt-form-element";
import { MqttFormElement } from "./page-elements/mqtt-form-element";

type CloudConfigurationState = {
    open: boolean;
    configuration?: CloudConfiguration;
    message: {
        content: string;
        type: "error" | "success";
    };
    testMessage: {
        success: string;
        error: string;
    };
    formElement?: CloudFormElement | HttpFormElement | MqttFormElement;
};

@customElement("cloud-page")
export class CloudPage extends BasePage {
    @query("cloud-form-element") cloudFormElement!: CloudFormElement;
    @query("http-form-element") httpFormElement!: HttpFormElement;
    @query("mqtt-form-element") mqttFormElement!: MqttFormElement;

    @state() errorText: string = "";
    @state() configurationStates: Record<
        CloudConfigurationType,
        CloudConfigurationState
    > = {
        blumy: {
            open: false,
            message: { content: "", type: "error" },
            testMessage: {
                success: "Blumy Cloud verbunden",
                error: "Verbindung fehlgeschlagen",
            },
        },
        http: {
            open: false,
            message: { content: "", type: "error" },
            testMessage: {
                success: "HTTP verbunden",
                error: "Verbindung fehlgeschlagen",
            },
        },
        mqtt: {
            open: false,
            message: { content: "", type: "error" },
            testMessage: {
                success: "MQTT verbunden",
                error: "Verbindung fehlgeschlagen",
            },
        },
    };
    @state() configured: boolean = false;

    loadingStateController = new StateController(this, loadingState);

    private async submit() {
        let numberOfCorrectConfigurations = 0;
        let numberOfFailedConfigurations = 0;
        for (const key of cloudConfigurationTypes) {
            const configurationState = this.configurationStates[key];
            if (configurationState.open) {
                const apiConfig = configurationState.formElement!.getConfig();
                const testResult = await testCloudConnection(apiConfig);
                if (testResult) {
                    configurationState.message = {
                        content: configurationState.testMessage.success,
                        type: "success",
                    };
                } else {
                    configurationState.message = {
                        content: configurationState.testMessage.error,
                        type: "error",
                    };
                    if (
                        window.confirm(
                            `Verbindungstest für ${key} fehlgeschlagen. Trotzdem speichern?`
                        )
                    ) {
                        numberOfCorrectConfigurations++;
                    } else {
                        numberOfFailedConfigurations++;
                        continue;
                    }
                }
                const res = await setCloudCredentials(apiConfig);
                if (!res.ok) {
                    this.errorText = "Fehler, Gerät antwortet nicht";
                    continue;
                }
                numberOfCorrectConfigurations++;
            } else {
                await disableCloudConnection(key);
            }
        }
        this.configurationStates = { ...this.configurationStates };
        if (numberOfCorrectConfigurations === 0) {
            this.errorText =
                "Bitte konfiguriere mindestens eine Schnittstelle.";
            return;
        }
        if (numberOfFailedConfigurations > 0) {
            this.errorText = "Es gab Fehler bei der Konfiguration.";
            return;
        }
        this.next();
    }

    private updateConfigured() {
        this.configured = Object.values(this.configurationStates).some(
            (value) =>
                value.open &&
                value.configuration &&
                Object.values(value.configuration).every((v) => !!v)
        );
        if (this.configured) {
            this.errorText = "";
        }
    }

    private handleOpen(target: CloudConfigurationType) {
        this.configurationStates[target].open = true;
        this.updateConfigured();
    }

    private handleClose(target: CloudConfigurationType) {
        this.configurationStates[target].open = false;
        this.updateConfigured();
    }

    private handleChange(event: CustomEvent<CloudConfiguration>) {
        this.configurationStates[event.detail.type].configuration =
            event.detail;
        this.updateConfigured();
    }

    private async testConnections(): Promise<boolean> {
        this.errorText = "";
        let success = true;
        for (const key of cloudConfigurationTypes) {
            const configurationState = this.configurationStates[key];
            if (configurationState.open) {
                const res = await testCloudConnection(
                    configurationState.formElement!.getConfig()
                );
                if (res) {
                    configurationState.message = {
                        content: configurationState.testMessage.success,
                        type: "success",
                    };
                } else {
                    configurationState.message = {
                        content: configurationState.testMessage.error,
                        type: "error",
                    };
                    success = false;
                }
            }
        }
        this.configurationStates = { ...this.configurationStates };
        return success;
    }

    async firstUpdated() {
        this.configurationStates.blumy.formElement = this.cloudFormElement;
        this.configurationStates.http.formElement = this.httpFormElement;
        this.configurationStates.mqtt.formElement = this.mqttFormElement;
        for (const key of cloudConfigurationTypes) {
            const config = await getCloudCredentials(key);
            const configurationState = this.configurationStates[key];
            if (config) {
                configurationState.configuration = config;
                configurationState.formElement!.setConfig(config as never);
                configurationState.open = true;
            }
        }
        this.configurationStates = { ...this.configurationStates };
        this.updateConfigured();
    }

    async handleExternalSetup() {
        const originHttps = location.origin.replace("http", "https");
        location.href = `https://blumy.cloud/selector?redirect=${location.origin}/?page=5&apiUrl=${originHttps}/api/cloudSetup/blumy`;
    }

    render() {
        return html`
            <title-element
                >Automatische Schnittstellen-Konfiguration</title-element
            >
            <button-element
                name="Über die Blumy Cloud einrichten"
                @click="${() => this.handleExternalSetup()}"
                ?disabled="${loadingState.state > 0}"
                ?secondary="${true}"
            ></button-element>
            <br />
            <title-element>Manuelle Schnittstellen-Konfiguration</title-element>
            <collapsible-element
                summary="Blumy Cloud"
                ?open=${this.configurationStates["blumy"].open}
                @opened=${() => this.handleOpen("blumy")}
                @closed=${() => this.handleClose("blumy")}
            >
                <cloud-form-element
                    @input-config=${this.handleChange}
                ></cloud-form-element>
                <text-element
                    text="${this.configurationStates.blumy.message.content}"
                    color="${this.configurationStates.blumy.message.type}"
                />
            </collapsible-element>
            <collapsible-element
                summary="HTTP"
                ?open=${this.configurationStates["http"].open}
                @opened=${() => this.handleOpen("http")}
                @closed=${() => this.handleClose("http")}
            >
                <http-form-element
                    @input-config=${this.handleChange}
                ></http-form-element>
                <text-element
                    text="${this.configurationStates.http.message.content}"
                    color="${this.configurationStates.http.message.type}"
                />
            </collapsible-element>
            <collapsible-element
                summary="MQTT"
                ?open=${this.configurationStates["mqtt"].open}
                @opened=${() => this.handleOpen("mqtt")}
                @closed=${() => this.handleClose("mqtt")}
            >
                <mqtt-form-element
                    @input-config=${this.handleChange}
                ></mqtt-form-element>
                <text-element
                    text="${this.configurationStates.mqtt.message.content}"
                    color="${this.configurationStates.mqtt.message.type}"
                />
            </collapsible-element>
            <text-element text="${this.errorText}"></text-element>
            <button-nav-element>
                <button-element
                    name="Zurück"
                    @click="${this.back}"
                    ?secondary="${false}"
                    ?disabled="${loadingState.state > 0}"
                ></button-element>
                <button-element
                    name="Überspringen"
                    @click="${() => this.next()}"
                    ?secondary="${true}"
                    ?disabled="${loadingState.state > 0}"
                ></button-element>
                <button-element
                    name="Speichern"
                    @click="${this.submit}"
                    ?secondary="${true}"
                    ?disabled="${!this.configured || loadingState.state > 0}"
                ></button-element>
                <button-element
                    name="Testen"
                    @click="${this.testConnections}"
                    ?secondary="${true}"
                    ?disabled="${!this.configured || loadingState.state > 0}"
                ></button-element>
            </button-nav-element>
        `;
    }
}