#pragma once

/*
    The default is hotspot mode, the note will be connected to the configured WiFi
*/
// #define SOFTAP_MODE



/*
    Sensor selection, the default is the onboard sensor
*/
#define __HAS_BME280__               //BME280 temperature, humidity, pressure, height sensor
// #define __HAS_MOTOR__                //High and low level control relay, or other level drive motor
// #define __HAS_RGB__                  //WS2812 single point colorful light


// Only one of the following two can be selected, otherwise there will be conflicts
// #define __HAS_SHT3X__                //SHT3X temperature and humidity sensor
// #define __HAS_DS18B20__              //DS18B20 temperature and humidity sensor


// Wireless access point ssid password
#define WIFI_SSID               "Awesome."
#define WIFI_PASSWD             "baconsandwich!12345"

// Citrus
#define CITRUS_PLANT            "Kalamansi"

// Influx
#define INFLUXDB_URL            "http://10.0.10.41:8086"
#define INFLUXDB_ORG            "awesome"
#define INFLUXDB_BUCKET         "citrus"
#define INFLUXDB_TOKEN          "_OqP0cgQZjd4jKMJQbIQiBYxtKnCmFHXwk6v-QHCE7yc-X76EmkqO9AUwM2S1vH4mW4rZ-cnTLKjQMue-Wbpdw=="

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO                 "PST8PDT"

#define I2C_SDA                 (25)
#define I2C_SCL                 (26)
#define DHT12_PIN               (16)
#define BAT_ADC                 (33)
#define SALT_PIN                (34)
#define SOIL_PIN                (32)
#define BOOT_PIN                (0)
#define POWER_CTRL              (4)
#define USER_BUTTON             (35)
#define DS18B20_PIN             (21)                  //18b20 data pin

#define MOTOR_PIN               (19)
#define RGB_PIN                 (18)

#define OB_BH1750_ADDRESS       (0x23)
#define OB_BME280_ADDRESS       (0x77)
#define OB_SHT3X_ADDRESS        (0x44)

#define DASH_BME280_TEMPERATURE_STRING  "Temperature"
#define DASH_BME280_PRESSURE_STRING     "Pressure"
#define DASH_BME280_ALTITUDE_STRING     "Altitude"
#define DASH_BME280_HUMIDITY_STRING     "Humidity"
#define DASH_BH1750_LUX_STRING          "BH1750"
#define DASH_SOIL_VALUE_STRING          "Soil Value"
#define DASH_SALT_VALUE_STRING          "Salt Value"
#define DASH_BATTERY_STRING             "Battery"
#define DASH_DS18B20_STRING             "18B20 Temperature"
#define DASH_SHT3X_TEMPERATURE_STRING   "SHT3X Temperature"
#define DASH_SHT3X_HUMIDITY_STRING      "SHT3X Humidity"
#define DASH_MOTOR_CTRL_STRING          "Water pump"