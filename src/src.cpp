#include <algorithm>
#include <iostream>
#include <WiFi.h>
#include <BH1750.h>             //https://github.com/claws/BH1750
#include <InfluxDbClient.h>
#include "configuration.h"
#include <Adafruit_BME280.h>




RTC_DATA_ATTR int bootCount = 0;

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, "");
Point sensor("citrus");

typedef enum {
    BME280_SENSOR_ID,
    SHT3x_SENSOR_ID,
    BHT1750_SENSOR_ID,
    SOIL_SENSOR_ID,
    SALT_SENSOR_ID,
    DS18B20_SENSOR_ID,
    VOLTAGE_SENSOR_ID,
} sensor_id_t;

typedef struct {
    uint32_t timestamp;     /**< time is in milliseconds */
    float temperature;      /**< temperature is in degrees centigrade (Celsius) */
    float light;            /**< light in SI lux units */
    float pressure;         /**< pressure in hectopascal (hPa) */
    float humidity;         /**<  humidity in percent */
    float altitude;         /**<  altitude in m */
    float voltage;           /**< voltage in volts (V) */
    uint8_t soli;           //Percentage of soil
    uint8_t salt;           //Percentage of salt
} higrow_sensors_event_t;

BH1750              lightMeter(OB_BH1750_ADDRESS);  //0x23
Adafruit_BME280     bme;                            //0x77

bool                has_bmeSensor   = true;
bool                has_lightSensor = true;
uint64_t            timestamp       = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.print("Wakeup reason:");
  Serial.println(wakeup_reason);

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void deviceProbe(TwoWire &t);

void setupWiFi()
{
    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi connect fail!");
        delay(3000);
        esp_restart();
    }
    Serial.print("WiFi connected! , ");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

bool get_higrow_sensors_event(sensor_id_t id, higrow_sensors_event_t &val)
{
    memset(&val, 0, sizeof(higrow_sensors_event_t));
    switch (id) {
    case BME280_SENSOR_ID: {
        if (has_bmeSensor) {
            val.temperature = bme.readTemperature();
            val.pressure = (bme.readPressure() / 100.0F);
            val.humidity = bme.readHumidity();
            val.altitude = bme.readAltitude(1013.25);
        }
    }
    break;

    case BHT1750_SENSOR_ID: {
        if (has_lightSensor) {
            val.light = lightMeter.readLightLevel();
        } else {
            val.light = 0;
        }
    }
    break;
    case SOIL_SENSOR_ID: {
        uint16_t soil = analogRead(SOIL_PIN);
        val.soli = map(soil, 0, 4095, 100, 0);
    }
    break;
    case SALT_SENSOR_ID: {
        uint8_t samples = 120;
        uint32_t humi = 0;
        uint16_t array[120];
        for (int i = 0; i < samples; i++) {
            array[i] = analogRead(SALT_PIN);
            delay(2);
        }
        std::sort(array, array + samples);
        for (int i = 1; i < samples - 1; i++) {
            humi += array[i];
        }
        humi /= samples - 2;
        val.salt = humi;
    }
    break;
    case VOLTAGE_SENSOR_ID: {
        int vref = 1100;
        uint16_t volt = analogRead(BAT_ADC);
        val.voltage = ((float)volt / 4095.0) * 6.6 * (vref);
    }
    break;
    default:
        break;
    }
    return true;
}


void setup()
{
    Serial.begin(115200);

    //! Sensor power control pin , use deteced must set high
    pinMode(POWER_CTRL, OUTPUT);
    digitalWrite(POWER_CTRL, 1);
    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);

    deviceProbe(Wire);

    if (!lightMeter.begin()) {
        Serial.println(F("Could not find BH1750"));
        has_lightSensor = false;
    }

    if (!bme.begin()) {
        Serial.println(F("Could not find BMP280"));
        has_bmeSensor = false;
    }

    setupWiFi();

    // Influx
    // Accurate time is necessary for certificate validation
    // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
    // Syncing progress and the time will be printed to Serial
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    // Check server connection
    if (client.validateConnection()) {
      Serial.print("Connected to Influx: ");
      Serial.println(client.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }

    sensor.addTag("plant", CITRUS_PLANT);
    sensor.addTag("SSID", WiFi.SSID());

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    //Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));
 
    //Print the wakeup reason for ESP32
    print_wakeup_reason();

    /*
    First we configure the wake up source
    We set our ESP32 to wake up every 5 seconds
    */
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
    " Seconds");
}


void loop()
{
    sensor.clearFields();
    sensor.addField("rssi", WiFi.RSSI());

    timestamp = millis();

    higrow_sensors_event_t val = {0};

    get_higrow_sensors_event(BHT1750_SENSOR_ID, val);
    sensor.addField("light", val.light);

    get_higrow_sensors_event(SOIL_SENSOR_ID, val);
    sensor.addField("soil", val.soli);

    get_higrow_sensors_event(SALT_SENSOR_ID, val);
    sensor.addField("salt", val.salt);

    get_higrow_sensors_event(VOLTAGE_SENSOR_ID, val);
    sensor.addField("voltage", val.voltage);

    get_higrow_sensors_event(BME280_SENSOR_ID, val);
    sensor.addField("temperature", val.temperature);
    sensor.addField("humidity", val.humidity);
    sensor.addField("altitude", val.altitude);
    sensor.addField("pressure", val.pressure);

    sensor.addField("boot_count", bootCount);

    // Write Influx Data
    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensor));
    // If no Wifi signal, try to reconnect it
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("Wifi connection lost");
    }
    // Write point
    if (!client.writePoint(sensor)) {
        Serial.print("Influx write failed: ");
        Serial.println(client.getLastErrorMessage());
    }

    /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}



void deviceProbe(TwoWire &t)
{
    uint8_t err, addr;
    int nDevices = 0;
    for (addr = 1; addr < 127; addr++) {
        t.beginTransmission(addr);
        err = t.endTransmission();
        if (err == 0) {
            Serial.print("I2C device at address 0x");
            if (addr < 16)
                Serial.print("0");
            Serial.print(addr, HEX);
            Serial.println(" !");
            switch (addr) {
            case OB_BME280_ADDRESS:
                has_bmeSensor = true;
                break;
            default:
                break;
            }
            nDevices++;
        } else if (err == 4) {
            Serial.print("Unknow error at address 0x");
            if (addr < 16)
                Serial.print("0");
            Serial.println(addr, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
}
