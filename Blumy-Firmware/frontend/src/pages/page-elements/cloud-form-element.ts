import { BlumyCloudConfiguration } from "api";
import { InputElement } from "./input-element";
import { LitElement, html } from "lit";
import { customElement, query } from "lit/decorators.js";

@customElement("cloud-form-element")
export class CloudFormElement extends LitElement {
    @query("#token") tokenInput: InputElement;
    @query("#url") urlInput: InputElement;

    public getConfig(): BlumyCloudConfiguration {
        return {
            type: "cloud",
            token: this.tokenInput.input.value,
            url: this.urlInput.input.value,
        };
    }

    public setConfig(config: BlumyCloudConfiguration) {
        this.tokenInput.input.value = config.token;
        this.urlInput.input.value = config.url;
    }

    private handleChange() {
        this.dispatchEvent(
            new CustomEvent<BlumyCloudConfiguration>("input-config", {
                detail: {
                    type: "cloud",
                    token: this.tokenInput.input.value,
                    url: this.urlInput.input.value,
                },
            })
        );
    }

    render() {
        return html`
            <input-element-grid>
                <input-element
                    id="token"
                    type="text"
                    label="Zugangsschlüssel"
                    @input=${this.handleChange}
                ></input-element>
                <input-element
                    id="url"
                    type="text"
                    label="Endpunkt Url"
                    @input=${this.handleChange}
                ></input-element>
            </input-element-grid>
        `;
    }
}
