#pragma once

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

// Deep Sleep Interval
#define uS_TO_S_FACTOR          1000000ULL
#define TIME_TO_SLEEP           3600       

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