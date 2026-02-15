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

    // Hypoxia (Hypoxemia) model
    void updateHypoxia(float dt, float pressure) {

        // basic blood oxygen available model
        float P02 = pressure * 0.21; // hPa oxygen partial pressure with 21% air
        float P50 = 40.0;             // hPa oxygen partial pressure for 50% Sp02
        float n = 2.8;                // sigmoid function hill coefficient
        float m = 1;               // rate of change modifier (0.02)

        // sigmoid function to get available % Sp02
        float oxygenAvailable = 100.0 * (pow(P02, n)/(pow(P50, n) + pow(P02, n))); 

        // rate of change
        float changeRate = (oxygenAvailable - bloodOxygen) * m;

        // update blood oxygen % Sp02 level
        bloodOxygen += changeRate * dt;

        // limits
        if (bloodOxygen > 100) bloodOxygen = 100;
        if (bloodOxygen < 0) bloodOxygen = 0;

        Serial.print("Oxygen: ");
        Serial.print(oxygenAvailable);
        Serial.println(" % P02");
        
        Serial.print("Oxygen: ");
        Serial.print(bloodOxygen);
        Serial.println(" % Sp02");

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
