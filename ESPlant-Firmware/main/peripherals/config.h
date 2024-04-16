// ANALOG INPUT
#define ADC_MOISTURE_SENSOR GPIO_NUM_0
#define ADC_MOISTURE_SENSOR_CHANNEL ADC_CHANNEL_0     // GPIO 0, LP
#define ADC_LIGHT_SENSOR_CHANNEL ADC_CHANNEL_3        // GPIO 3, LP
#define ADC_VOLTAGE_MEASUREMENT_CHANNEL ADC_CHANNEL_6 // GPIO 6, LP

// DIGITAL INPUT
#define POWER_USB_VIN GPIO_NUM_15

// DIGITAL OUTPUT
#define LIGHT_SENSOR_IN GPIO_NUM_2             // LP
#define LED_RED GPIO_NUM_4                     // LP
#define LED_GREEN GPIO_NUM_5                   // LP
#define MOISTURE_SQUARE_WAVE_SIGNAL GPIO_NUM_7 // LP
#define BUZZER GPIO_NUM_21

// BOOT BUTTON
#define BOOT_BUTTON GPIO_NUM_9

// LEDC CHANNELS
#define BUZZER_CHANNEL LEDC_CHANNEL_0
#define LED_RED_CHANNEL LEDC_CHANNEL_1
#define LED_GREEN_CHANNEL LEDC_CHANNEL_2
#define MOISTURE_SQUARE_WAVE_SIGNAL_CHANNEL LEDC_CHANNEL_3

// DIGITAL INPUT/OUTPUT
#define LIGHT_SENSOR_SELECT GPIO_NUM_1 // LP
#define VOLTAGE_MEASUREMENT_SELECT GPIO_NUM_14

// I2C
#define TEMPERATURE_SENSOR_SDA GPIO_NUM_22
#define TEMPERATURE_SENSOR_SCL GPIO_NUM_23

// USED FOR PROGRAMMING
// #define USB_DM GPIO_NUM_12
// #define USB_DP GPIO_NUM_13
// #define TXD GPIO_NUM_?
// #define RXD GPIO_NUM_?

// Init shutdown when pressing boot button in configuration mode
// # define ENABLE_BOOT_BUTTON_INTERRUPT