#include <Arduino_LPS22HB.h> // pressure, alt temperature

// programmable variables
float alpha = 0.6;                  // smoothing factor (0–1)
float refPressure = 1013.25;        // hPa at avg. sea level

// calculation variables
float lastPressure = 0;             // 
unsigned long lastTime = 0;         // 

// SensorData
float hPa;                           // LPS22HB raw hPa pressure
float pressure;                      // smooth hPa pressure

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // check sensors
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

  // static TUC estimate based on pressure in seconds (regression formula using TUC table)
  float tucStatic;
  if (pressure > 550) {
    tucStatic = 99999999; // safe
  } else if (pressure > 100) { 
    tucStatic = 1.837 * pow(1.013, pressure); // approx.
  } else {
    tucStatic = 5;
  }

  // rapid ascent tuc decrease
  float penalty = 1.0;
  if (rateOfChange > 2.0) {
    penalty = max(0.5f, 1.0f - (rateOfChange / 50.0f)); 
  }

  // combined formula
  float tucDynamic = tucStatic * penalty;

  // calculate pressure altitude in meters using barometric formula
  float altitude = 44330.0 * (1.0 - pow(pressure / refPressure, 0.1903));

  Serial.println(millis());
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Approx. TUC static: ");
  if (tucStatic > 3600) {
    Serial.println("n/a");
  } else {
    Serial.print(tucStatic);
    Serial.println(" s ±20%");
  }

  Serial.print("Approx. TUC dynamic: ");
  if (tucDynamic > 3600) {
    Serial.println("n/a");
  } else {
    Serial.print(tucDynamic);
    Serial.println(" s ±20%");
  }

  lastPressure = pressure;
  lastTime = currentTime;
  delay(1000);
}
