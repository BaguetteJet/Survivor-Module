import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# 1. Load data from CSV
# Ensure the path matches your local file structure
file_path = 'data/Estimated_Tuc-Frontiers-May2021-modified.csv'
xLabel = 'Pressure_(hPa)'
yLabel = 'Saturation_%'

df = pd.read_csv(file_path)

x_data = df[xLabel].values
y_data = df[yLabel].values

# 2. Define the models
def linear_model(x, a, b):
    return a * x + b

def exponential_model(x, a, b):
    # Safety clip to prevent math overflow
    return a * np.exp(np.clip(b * x, -700, 700))

def sigmoid_model(x, L, k, x0):
    return L / (1 + np.exp(-np.clip(k * (x - x0), -700, 700)))

# 3. Perform Curve Fitting

# --- Linear Fit ---
popt_lin, _ = curve_fit(linear_model, x_data, y_data)

# --- STABLE Exponential Fit (Log-Linear Trick) ---
# Filter for y > 0 to avoid log(0) errors
mask = y_data > 0
try:
    # Fitting ln(y) = bx + ln(a)
    log_params = np.polyfit(x_data[mask], np.log(y_data[mask]), 1)
    exp_b = log_params[0]
    exp_a = np.exp(log_params[1])
    popt_exp = [exp_a, exp_b]
except Exception as e:
    print(f"Exponential fit failed: {e}")
    popt_exp = [0, 0]

# --- Sigmoid Fit ---
# Initial guess: L=max(y), k=1, x0=median(x)
p0_sig = [max(y_data), 1, np.median(x_data)]
try:
    popt_sig, _ = curve_fit(sigmoid_model, x_data, y_data, p0=p0_sig, maxfev=10000)
except Exception as e:
    print(f"Sigmoid fit failed: {e}")
    popt_sig = [0, 0, 0]

# 4. Create the Plot
plt.figure(figsize=(12, 8))
plt.scatter(x_data, y_data, color='black', label='Data Points', zorder=5)

# Generate fit lines for the 1.5x range
x_max_view = max(x_data) * 1.5
y_max_view = max(y_data) * 1.5
x_fit = np.linspace(0, x_max_view, 500)

plt.plot(x_fit, linear_model(x_fit, *popt_lin), color='orange', 
         linestyle='--', label='Linear Fit', alpha=0.8)
plt.plot(x_fit, exponential_model(x_fit, *popt_exp), color='blue', 
         linestyle='-.', label='Exponential Fit', alpha=0.8)
plt.plot(x_fit, sigmoid_model(x_fit, *popt_sig), color='red', 
         linewidth=2, label='Sigmoid Fit')

# --- SET AXIS WINDOW (0,0 to 1.5x max) ---
plt.xlim(0, x_max_view)
plt.ylim(0, y_max_view)

# 5. Add Equations to the Graph
# Using LaTeX formatting for professional-looking math
equation_text = (
    f"Linear: y = {popt_lin[0]:.4f}x + {popt_lin[1]:.4f}\n"
    f"Exponential: y = {popt_exp[0]:.4f} * e^({popt_exp[1]:.4f}x)\n"
    f"Sigmoid: y = {popt_sig[0]:.2f} / (1 + e^(-{popt_sig[1]:.2f} * (x - {popt_sig[2]:.2f})))"
)

# Place text in a white box in the lower right
plt.gca().text(0.95, 0.05, equation_text, transform=plt.gca().transAxes,
               fontsize=10, verticalalignment='bottom', horizontalalignment='right',
               bbox=dict(boxstyle='round,pad=0.5', facecolor='white', edgecolor='gray', alpha=0.9))

# Formatting details
plt.title(f"{xLabel} vs {yLabel} Curve Fit Analysis", fontsize=14, fontweight='bold')
plt.xlabel(xLabel, fontsize=12)
plt.ylabel(yLabel, fontsize=12)
plt.legend(loc='upper left')
plt.grid(True, linestyle=':', alpha=0.6)

# 6. Save and Show
output_filename = file_path.replace(".csv", ".png")
plt.savefig(output_filename, dpi=300, bbox_inches='tight')
print(f"Successfully saved plot as: {output_filename}")

# Print to console for verification
print("-" * 30)
print(f"Linear: y = {popt_lin[0]:.4f}x + {popt_lin[1]:.4f}")
print(f"Exponential: y = {popt_exp[0]:.4f} * e^({popt_exp[1]:.4f}x)")
print(f"Sigmoid: L={popt_sig[0]:.4f}, k={popt_sig[1]:.4f}, x0={popt_sig[2]:.4f}")
print("-" * 30)

plt.show()