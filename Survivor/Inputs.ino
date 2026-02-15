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

  if (parts[0].equalsIgnoreCase("alpha")) {

    if (isFloat(parts[1]) && parts[1].toFloat() >= 0 && parts[1].toFloat() <= 1) {
      alpha = parts[1].toFloat();
      Serial.print("[SUCCESS] alpha set to: ");
      Serial.println(alpha);
    } else if (parts[1].equalsIgnoreCase("")) {
      Serial.println("[INFO] alpha used to weigh data smoothing. Must be float value between 0 and 1.");
    } else {
      Serial.println("[FAILED] alpha invalid arg!");
    }
  } else if (parts[0].equalsIgnoreCase("refpressure")) {

    if (isFloat(parts[1])) {
      refPressure = parts[1].toFloat();
      Serial.print("[SUCCESS] Reference Pressure set to: ");
      Serial.println(refPressure);
      Serial.print(" hPa");
    } else if (parts[1].equalsIgnoreCase("local")) {
      Serial.println("[INPUT] Reference Pressure calibrating...");
      refPressure = getAveragePressure();
      Serial.print("[SUCCESS] Reference Pressure set to LOCAL: ");
      Serial.println(refPressure);
      Serial.print(" hPa");
    } else if (parts[1].equalsIgnoreCase("default")) {
      refPressure = 1013.25;
      Serial.print("[SUCCESS] Reference Pressure set to DEFAULT: ");
      Serial.println(refPressure);
      Serial.print(" hPa"); 
    } else if (parts[1].equalsIgnoreCase("")) {
      Serial.println("[INFO] Reference Pressure in hPa used for calculating altitude. Any float value, LOCAL to use current pressure, or DEFAULT for 1013.25");
    } else {
      Serial.println("[FAILED] refpressure invalid arg!");
    }
  } else {
    
    Serial.println("[FAILED] invalid command!");
  }
}