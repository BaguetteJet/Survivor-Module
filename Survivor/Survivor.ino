#include <Arduino_HS300x.h>   // humidity, temperature
#include <Arduino_LPS22HB.h>  // pressure, temperature

#include "Human.h"

Human astronaut;

// programmable variables
float alpha = 0.6;                  // smoothing factor (0–1)
float refPressure = 1013.25;        // hPa at avg. sea level

// calculation variables
float lastPressure = 0;             // 
unsigned long lastTime = 0;         // 

// SensorData
float hPa;                           // LPS22HB raw hPa pressure
float pressure;                      // LPS22HB smooth hPa pressure
float temperature;                   // HS3003 raw °C temperature
float humidity;                      // HS3003 raw % relative humidity

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // check sensors
  if (!HS300x.begin()) {
    Serial.println("[HS3003] Failed to initialize humidity temperature sensor!");
    while (1);
  }
  if (!BARO.begin()) {
    Serial.println("[LPS22HB] Failed to initialize pressure sensor!");
    while (1);
  }

  delay(2000); // let sensors stabilize 

  //refPressure = getAveragePressure(); // set current pressure SensorData
  pressure = refPressure;
}

void loop() {
  // measure time
  unsigned long currentTime = millis();

  // read inputs
  readInput(); 

  // read data from sensors
  readAllSensors(); // SensorData

  // float hPa = pressure - 10; // simulation ascent

  // calculate rate of pressure change (hPa/s)
  float deltaTime = (currentTime - lastTime) / 1000.0;
  float rateOfChange = (lastPressure - pressure) / deltaTime; // positive = ascending

  float gForce = 2.5;        // replace with IMU

  // static TUC estimate based on pressure in seconds (regression formula using TUC table)
  float tucStatic;
  if (pressure > 600) {
    tucStatic = 9999999999; // safe
  } else if (pressure > 125) { 
    tucStatic = 1.486 * exp(0.0138 * pressure); // approx.
  } else {
    tucStatic = 9;
  }

  // rapid ascent tuc decrease
  float penalty = 1.0;
  if (rateOfChange > 2.0) {
    penalty = max(0.5, 1.0 - (rateOfChange / 50.0)); 
  }

  // combined formula
  float tucDynamic = tucStatic * penalty;

  // calculate pressure altitude in meters using barometric formula
  // altitude = ((pow((seaLevelPressure / pressure), 1/5.257) - 1.0) * (temperature + 273.15)) / 0.0065;
  float altitude = (pow((refPressure / pressure), 0.190263) - 1.0) * (temperature + 273.15) * 153.84615;


  Serial.println(millis());

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa | ");
  Serial.print(pressure * 0.7500637);
  Serial.println(" mmHg");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.print(" m | ");
  Serial.print(altitude * 3.28084);
  Serial.println(" ft");

  Serial.print("Approx. TUC static: ");
  if (tucStatic > 3600) {
    Serial.println("unlimited");
  } else {
    Serial.print(formatTime(tucStatic));
    Serial.println(" ±20%");
  }

  Serial.print("Approx. TUC dynamic: ");
  if (tucDynamic > 3600) {
    Serial.println("unlimited");
  } else {
    Serial.print(formatTime(tucDynamic));
    Serial.println(" ±20%");
  }
  
  astronaut.update(deltaTime, pressure, temperature, gForce);

  lastPressure = pressure;
  lastTime = currentTime;
  delay(1000);
}
