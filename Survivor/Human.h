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

//private:

    // Hypoxia model
    void updateHypoxia(float dt, float pressure) {

        // basic blood oxygen available model
        float PO2 = pressure;             // hPa barometric pressure
        float PH2O = 62;                  // hPa water vapour pressure (62hPa at 37°C) -- UPDATE
        float FiO2 = 0.21;                // 21% oxygen in air

        // calculate inspired oxygen partial pressure
        float PiO2 = (PO2 - PH2O) * FiO2; // hPa
        
        // calculate peripheral blood oxygen saturation
        float SpO2 = 100.0 / (1.0 + exp(-0.0201 * (PiO2 - 50.9521))); // %
    
        // rate of change
        float m = 1;               // rate of change modifier (0.02)
        float changeRate = (SpO2 - bloodOxygen) * m;

        // update blood oxygen % SpO2 level
        bloodOxygen += changeRate * dt;

        // limits
        if (bloodOxygen > 100) bloodOxygen = 100;
        if (bloodOxygen < 0) bloodOxygen = 0;

        Serial.print("Oxygen: ");
        Serial.print(PiO2);
        Serial.println(" hPa PiO2");

        Serial.print("Oxygen: ");
        Serial.print(changeRate);
        Serial.println(" % SpO2 rate");
        
        Serial.print("Oxygen: ");
        Serial.print(bloodOxygen);
        Serial.println(" % SpO2");

        Serial.print("Core Temp: ");
        Serial.print(coreTemp);
        Serial.println(" °C");
    }

    // ---------------------------
    // Thermal model (heat loss)
    // ---------------------------
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
