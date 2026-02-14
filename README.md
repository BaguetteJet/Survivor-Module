# Survivor Module
Rocket payload module to calculate survivability measures of high power flight.

## Hardware
Using Arduino Nano microcontroller because combines a tiny form factor, different environment sensors and has the possibility to run AI using TinyML and TensorFlow Lite.
- Aruino Nano 33 BLE Sense Rev2
  > LPS22HB - barometric pressure

## Calculations
Using the onboard environmental sensors we can calculate various measures in relation to human performance and survivability.
### Time of Useful Consciousness (TUC)
The period between the loss of an adequate oxygen supply (hypoxia) and the point where an individual is no longer capable of taking corrective or self-protective action. It is not the time until you pass out. It is the time until you become too cognitively impaired to save yourself. TUC is an estimate at rest, meaning any exercise will reduce the time considerably. Rapid decompression can reduce TUC by up to 50%.   

Regression formula to estimate TUC created based on the data from TUC tables. (TUC in seconds, pressure in hPa) 

$$tuc_{static} = 1.837 \times 1.013^{pressure}$$

Adjust for rapid decompression using the pressure rate of change. ($$\Delta P$$ in hPa/s)

$$tuc_{dynamic} = tuc_{static} \times \max(0.5, 1.0 - \frac{\Delta P}{50})$$

Output Example:   
<img width="370" height="103" alt="image" src="https://github.com/user-attachments/assets/08512510-9c71-4de3-b850-4ac6190ebedc" />


sources:   
https://www.scribd.com/document/47260191/time-of-useful-consciousness   
https://skybrary.aero/articles/time-useful-consciousness   
https://en.wikipedia.org/wiki/Time_of_useful_consciousness   
https://stats.blue/Stats_Suite/exponential_regression_calculator.html   
