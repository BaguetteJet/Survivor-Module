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

// check if valid integer
bool isInt(String str) {
  str.trim();
  if (str.length() == 0) return false;
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (i == 0 && (c == '-' || c == '+')) continue;
    if (c < '0' || c > '9') return false;
  }
  return true;
}

// check if valid float
bool isFloat(String str) {
  str.trim();
  if (str.length() == 0) return false;
  bool decimalFound = false;
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (i == 0 && (c == '-' || c == '+')) continue;
    if (c == '.') {
      if (decimalFound) return false;
      decimalFound = true;
      continue;
    }
    if (c < '0' || c > '9') return false;
  }
  return true;
}

// respond to input
void readInput() {
  if (Serial.available() < 1) return;

  String line = Serial.readString();
  line.trim();

  int index = 0;
  String parts[5];

  // split line and fill array
  while (line.length() > 0 && index < 5) {
    int spaceIndex = line.indexOf(' ');
    if (spaceIndex == -1) {
      parts[index++] = line;
      break;
    }
    parts[index++] = line.substring(0, spaceIndex);
    line = line.substring(spaceIndex + 1);
    line.trim();
  }

  if (parts[0].equalsIgnoreCase("/alpha")) {
    if (isFloat(parts[1]) && parts[1].toFloat() >= 0 && parts[1].toFloat() <= 1) {
      alpha = parts[1].toFloat();
      Serial.print("[SUCCESS] alpha set to: ");
      Serial.println(alpha);
    } else {
      Serial.println("[FAILED] alpha must be float value between 0 and 1!");
    }
  } 

  if (parts[0].equalsIgnoreCase("/refpressure")) {
    if (isFloat(parts[1])) {
      refPressure = parts[1].toFloat();
      Serial.print("[SUCCESS] Reference Pressure set to: ");
      Serial.println(refPressure);
      Serial.print(" hPa");
    } else if (parts[1] == "") {
      Serial.println("[INPUT] Reference Pressure calibrating...");
      refPressure = getAveragePressure();
      Serial.print("[SUCCESS] Reference Pressure auto set to: ");
      Serial.println(refPressure);
      Serial.print(" hPa");
    } else {
      Serial.println("[FAILED] refpressure must be float in hPa or blank to auto calibrate! (avg. sea level 1013.25hPa)");
    }
  }
}