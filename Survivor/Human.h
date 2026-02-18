// Human.h
#ifndef HUMAN_H
#define HUMAN_H

class Human {
public:
    // state
    float bloodOxygen;      // % SpO2 
    float coreTemp;         // °C
    float gDose;            // accumulated G exposure
    float timeUnconscious;  // seconds

    bool conscious;
    bool alive;

    // limits
    float hypoxiaThreshold = 60.0;   // % oxygen blackout
    float fatalTempLow = 24.0;       // °C
    float fatalTempHigh = 42.0;      // °C
    float blackoutG = 6.0;           // sustained G causes blackout

    Human() {
        reset();
    }

    void reset() {
        bloodOxygen = 98.0;
        coreTemp = 37.0;
        gDose = 0.0;
        timeUnconscious = 0.0;
        conscious = true;
        alive = true;
    }

    // ---- Main update ----
    // dt = timestep in seconds
    // altitudeMeters
    // externalTempC
    // gForce (absolute)
    void update(float dt, float pressure, float temp, float gForce) {
        //if (!alive) return;

        updateHypoxia(dt, pressure);
        updateThermal(dt, temp);
        updateG(dt, gForce);
        evaluateState(dt);
    }

private:

    // Hypoxia model
    // DONE 17/02/26 - update PH2O once body Thermal model agreed

    void updateHypoxia(float dt, float pressure) {

        // basic blood oxygen available model
        float PO2 = pressure;             // hPa barometric pressure
        float PH2O = 62.7;                // hPa water vapour pressure (62hPa at 37°C) -- UPDATE?
        float FiO2 = 0.21;                // 21% oxygen in air

        // calculate hPa water vapour pressure at core body temperature using the Arden Buck equation T > 0 °C
        //PH2O = 6.1121 * exp((18.678 - (coreTemp / 234.5)) * (coreTemp / (234.5 + coreTemp)))

        // calculate inspired oxygen partial pressure
        float PiO2 = (PO2 - PH2O) * FiO2; // hPa

        // calculate peripheral blood oxygen saturation
        float SpO2 = 99.0 / (1.0 + exp(-0.035 * (PiO2 - 69))); // %

        // update blood oxygen % SpO2 level
        bloodOxygen += (SpO2 - bloodOxygen) * 0.1 * dt; // 0.1 rate of change

        // limits
        if (bloodOxygen > 100) bloodOxygen = 100;
        if (bloodOxygen < 0) bloodOxygen = 0;


        Serial.print("Oxygen PiO2: ");
        Serial.print(PiO2);
        Serial.print(" hPa | ");
        Serial.print(PiO2 * 0.7500637);
        Serial.println(" mmHg");

        Serial.print("Oxygen SpO2: ");
        Serial.print(SpO2);
        Serial.println(" % predicted");
        
        Serial.print("Oxygen SpO2: ");
        Serial.print(bloodOxygen);
        Serial.println(" % current");
    }

    void updateTUC(float dt) {
        if (currentSpO2 > 90.0) {
            // Recover TUC slowly if oxygen is restored
            tucRemaining += dt * 0.5; 
            if (tucRemaining > 1800.0) tucRemaining = 1800.0;
        } 
        else if (currentSpO2 <= 90.0 && currentSpO2 > 80.0) {
            tucRemaining -= dt; // Standard drain
        } 
        else if (currentSpO2 <= 80.0 && currentSpO2 > 70.0) {
            tucRemaining -= dt * 3.0; // Rapid drain
        } 
        else if (currentSpO2 <= 70.0) {
            tucRemaining -= dt * 10.0; // Critical drain (Seconds left)
        }

        if (tucRemaining < 0) tucRemaining = 0;
    }

    // Thermal model
    void updateThermal(float dt, float externalTemp) {

        // simple Newton cooling
        float heatLossRate = 0.01; // tunable constant
        coreTemp += (externalTemp - coreTemp) * heatLossRate * dt;
    }

    // ---------------------------
    // G-force exposure model
    // ---------------------------
    void updateG(float dt, float gForce) {

        if (gForce > blackoutG) {
            gDose += (gForce - blackoutG) * dt;
        } else {
            // recovery when G is low
            gDose -= dt * 0.5;
            if (gDose < 0) gDose = 0;
        }
    }

    // ---------------------------
    // Determine consciousness & survival
    // ---------------------------
    void evaluateState(float dt) {

        // check fatal temperature
        if (coreTemp < fatalTempLow || coreTemp > fatalTempHigh) {
            alive = false;
            conscious = false;
            return;
        }

        // hypoxia blackout
        bool hypoxiaBlackout = (bloodOxygen < hypoxiaThreshold);

        // G blackout
        bool gBlackout = (gDose > 5.0);

        if (hypoxiaBlackout || gBlackout) {
            conscious = false;
            timeUnconscious += dt;
        } else {
            conscious = true;
            timeUnconscious = 0;
        }
    }
};

#endif
