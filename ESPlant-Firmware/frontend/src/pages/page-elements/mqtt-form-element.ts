import { InputElement } from "./input-element";
import { LitElement, html } from "lit";
import { customElement, query } from "lit/decorators.js";
import { MqttCloudConfiguration } from "../../api";

@customElement("mqtt-form-element")
export class MqttForm extends LitElement {
    @query("#server") serverInput: InputElement;
    @query("#port") portInput: InputElement;
    @query("#user") userInput: InputElement;
    @query("#password") passwordInput: InputElement;
    @query("#topic") topicInput: InputElement;
    @query("#client-id") clientIdInput: InputElement;

    private handleChange() {
        this.dispatchEvent(new CustomEvent<MqttCloudConfiguration>('input', {
            detail: {
                type: 'mqtt',
                server: this.serverInput.input.value,
                port: this.portInput.input.value,
                username: this.userInput.input.value,
                password: this.passwordInput.input.value,
                topic: this.topicInput.input.value,
                clientId: this.clientIdInput.input.value,
            }
        }))
    }

    render() {
        return html`
            <input-element-grid>
                <input-element
                    id="server"
                    type="text"
                    label="Server"
                    @input=${this.handleChange}
                ></input-element>
                <input-element
                    id="port"
                    type="number"
                    label="Port"
                    @input=${this.handleChange}
                ></input-element>
                <input-element
                    id="user"
                    type="text"
                    label="Nutzername"
                    @input=${this.handleChange}
                ></input-element>
                <input-element
                    id="password"
                    type="password"
                    label="Passwort"
                    @input=${this.handleChange}
                ></input-element>
                <input-element
                    id="topic"
                    type="text"
                    label="Topic"
                    @input=${this.handleChange}
                ></input-element>
                <input-element
                    id="client-id"
                    type="text"
                    label="Client-ID"
                    @input=${this.handleChange}
                ></input-element>
            </input-element-grid>
        `;
    }
}
