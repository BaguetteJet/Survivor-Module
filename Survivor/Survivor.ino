#include <Arduino_LPS22HB.h> // pressure, alt temperature

float alpha = 0.6;                  // smoothing factor (0–1)

float refPressure = 1013.25;        // hPa at avg. sea level
float pressure;
float lastPressure = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // check sensors
  if (!BARO.begin()) {
    Serial.println("[LPS22HB] Failed to initialize pressure sensor!");
    while (1);
  }

  delay(2000); // let sensors stabilize 

  //refPressure = getAveragePressure(); // set current pressure
  pressure = refPressure;

}

void loop() {
  // read inputs
  readInput(); 

  // measure time
  unsigned long currentTime = millis();

  // float hPa = pressure - 10; // simulation ascent

  // read the sensor data
  float hPa = BARO.readPressure() * 10; // read kPa and convert to hPa

  // smooth pressure data with exponential moving average
  pressure = alpha * hPa + (1 - alpha) * pressure;

  // calculate rate of pressure change (hPa/s)
  float deltaTime = (currentTime - lastTime) / 1000.0;
  float rateOfChange = (lastPressure - pressure) / deltaTime; // positive = ascending

  // estimate static TUC (s) based on pressure (approx. using TUC table)
  float tucStatic;
  if (pressure > 550) {
    tucStatic = 3600; // safe
  } else if (pressure > 100) { 
    tucStatic = 1.837 * pow(1.013, pressure); // approx.
  } else {
    tucStatic = 5;
  }

  float penalty = 1.0;
  if (rateOfChange > 2.0) {
    penalty = max(0.5f, 1.0f - (rateOfChange / 50.0f)); 
  }

  float tucDynamic = tucStatic * penalty;

  // calculate pressure altitude in meters using barometric formula
  float altitude = 44330.0 * (1.0 - pow(pressure / refPressure, 0.1903));

  // calculate time useful 
  float tuc = (pressure > 600) ? 3600.0 : pow(10, (0.0055 * pressure) - 0.1);

  Serial.println(millis());
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Approx. TUC static: ");
  Serial.print(tucStatic);
  Serial.println(" s");

  Serial.print("Approx. TUC dynamic: ");
  Serial.print(tucDynamic);
  Serial.println(" s");

  Serial.print("Approx. TUC: ");
  Serial.print(tuc);
  Serial.println(" s");

  lastPressure = pressure;
  lastTime = currentTime;
  delay(1000);
}
