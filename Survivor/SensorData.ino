// average pressure in hPa
float getAveragePressure() {
  int num_samples = 20;
  int time_delay = 50;
  float sum = 0;
  for (int i = 0; i < num_samples; i++) {
    sum += BARO.readPressure();
    delay(time_delay);
  }
  return 10 * sum / num_samples;
}

// read all sensor data
void readAllSensors() {

  hPa = BARO.readPressure() * 10; // read pressure kPa and convert to hPa
  // hPa = pressure - 10; // simulation ascent

  // smooth pressure data with exponential moving average
  pressure = alpha * hPa + (1 - alpha) * pressure;

  temperature = HS300x.readTemperature();
  humidity = HS300x.readHumidity();

}