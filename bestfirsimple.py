import pandas as pd
import numpy as np
from scipy.optimize import curve_fit

# Define the "shape" you think the data has
def custom_formula(x, a, b, c):
    return a * np.exp(b * x) + c

# data
file_path = 'data/Estimated_Tuc-Frontiers-May2021-modified.csv'
xLabel = 'Pressure_(hPa)'
yLabel = 'Saturation_%'

df = pd.read_csv(file_path)

x_data = df[xLabel].values
y_data = df[yLabel].values

# Find the best 'a', 'b', and 'c'
popt, _ = curve_fit(custom_formula, x_data, y_data)

print(f"Best parameters: a={popt[0]:.2f}, b={popt[1]:.2f}, c={popt[2]:.2f}")