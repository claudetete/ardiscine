/* Ardiscine: tiny project to have simple temperature sensor into a pool using standard Dallas DS18B20 */
/* Dallas DS18B20 used is waterproof one from: */
/* https://www.aliexpress.com/item/Free-Shipping-1PCS-Digital-Temperature-Temp-Sensor-Probe-DS18B20-For-Thermometer-1m-Waterproof-100CM/32522322459.html */
/*  - Red    = VCC */
/*  - Yellow = Data */
/*  - Black  = GND */

/* TODO: add ph and/or chlore sensor */

#include <Arduino.h>

/* MYSENSORS SETTINGS */
/* Enable debug prints to serial monitor */
#define MY_DEBUG
/* Enable and select radio type attached (NRF24) */
#define MY_RADIO_NRF24
/* Enable repeater functionality for this node */
#define MY_REPEATER_FEATURE

/* do not use to high baud rate when not needed */
#define MY_BAUD_RATE 9600
/* CE/CS PIN for NRF24L01+ can be redefined (9 and 10 by default but use by IR to have PWM) */
#define MY_RF24_CE_PIN 5
#define MY_RF24_CS_PIN 6

/* node id used for gateway (must be uniq) */
/* when not set it leave the gateway to assign an id (do not work always) */
#define MY_NODE_ID 64

/* MySensors will override usual function of a sketch, it for a node it need a gateway to start */
#include <MySensors.h>

#include <DallasTemperature.h>
#include <OneWire.h>

/* ARDISCINE SETTINGS */
/* Sensor id to present and finally received */
#define SENSOR_ID_TEMPERATURE_POOL (0)
#define SENSOR_ID_TEMPERATURE_OUTSIDE (1)

/* PIN number of temperature sensor */
#define PIN_ID_TEMPERATURE (3)

/* 30 second in milliseconds (30 000) */
#define TEMPERATURE_POOLING_TIME (30000)

/* ARDISCINE CODE */
/* declare message about temperature sensor */
MyMessage TemperatureMessagePool(SENSOR_ID_TEMPERATURE_POOL, V_TEMP);
MyMessage TemperatureMessageOutside(SENSOR_ID_TEMPERATURE_OUTSIDE, V_TEMP);

/* Setup a oneWire instance to communicate with any OneWire devices */
OneWire OneWire(PIN_ID_TEMPERATURE);
/* Pass the OneWire reference to Dallas Temperature library */
DallasTemperature TemperatureSensors(&OneWire);
/* arrays to hold device address */
DeviceAddress ThermometerPool;
DeviceAddress ThermometerOutside;

void before()
{
  /* everything before MySensors execution */
  /* pinMode(PIN_ID_TEMPERATURE, INPUT); */

  /* Startup up the OneWire library */
  TemperatureSensors.begin();
}

void setup()
{
  /* This pipes to the serial monitor */
  /* Serial.begin(9600); */

  if (!TemperatureSensors.getAddress(ThermometerPool, 0)) Serial.println("Unable to find address for Device 0");
  /* for (uint8_t i = 0; i < 8; i++) */
  /* { */
  /*   Serial.print("Address: "); */
  /*   if (ThermometerPool[i] < 16) */
  /*   { */
  /*     Serial.print("0"); */
  /*   } */
  /*   else */
  /*   { */
  /*     Serial.print(ThermometerPool[i], HEX); */
  /*   } */
  /*   Serial.println(); */
  /* } */
  if (!TemperatureSensors.getAddress(ThermometerOutside, 1)) Serial.println("Unable to find address for Device 1");

  /* requestTemperatures() method will not block thread */
  TemperatureSensors.setWaitForConversion(false);
}

void presentation()
{
  /* Send the sketch version information to the gateway and Node */
  sendSketchInfo("Pool Sensor System", "1.0");
  /* everything to present each sensors/actuators on this node to the gateway (so domotic box will register it) */
  present(SENSOR_ID_TEMPERATURE_POOL, S_TEMP);
  /* everything to present each sensors/actuators on this node to the gateway (so domotic box will register it) */
  present(SENSOR_ID_TEMPERATURE_OUTSIDE, S_TEMP);
}

void loop()
{
  /* Fetch temperatures from Dallas sensors */
  TemperatureSensors.requestTemperatures();

  /* query conversion time and sleep until conversion completed */
  int16_t conversionTime = TemperatureSensors.millisToWaitForConversion(TemperatureSensors.getResolution());
  /* sleep() call can be replaced by wait() call if node need to process incoming messages (or if node is repeater) */
  wait(conversionTime);

  /* Fetch and round temperature to one decimal */
  float temperaturePool    = static_cast<float>(static_cast<int>(TemperatureSensors.getTempC(ThermometerPool) * 10.)) / 10.;
  float temperatureOutside = static_cast<float>(static_cast<int>(TemperatureSensors.getTempC(ThermometerOutside) * 10.)) / 10.;

  /* Send in read temperature */
  send(TemperatureMessagePool.set(temperaturePool, 1));
  send(TemperatureMessageOutside.set(temperatureOutside, 1));
  /* Serial.print("Temp C: "); */
  /* Serial.print(temperature); */
  /* Serial.println(); */

  wait(TEMPERATURE_POOLING_TIME);
}
