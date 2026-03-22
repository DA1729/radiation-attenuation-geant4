import subprocess
import re
import matplotlib.pyplot as plt
import numpy as np
import os
from scipy.optimize import curve_fit

def run_geant4(source, material, thickness, events=10000):
    macro = f"""
/run/initialize
/experiment/source {source}
/experiment/material {material}
/experiment/thickness {thickness} mm
/run/beamOn {events}
"""
    with open("temp.mac", "w") as f:
        f.write(macro)
    
    process = subprocess.Popen(['./build/bragg_curve', 'temp.mac'], 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.STDOUT, 
                               text=True)
    
    rate = 0.0
    for line in process.stdout:
        match = re.search(r'rate: ([\d\.]+)', line)
        if match:
            rate = float(match.group(1))
    process.wait()
    return rate

def exponential_func(x, a, b):
    return a * np.exp(-b * x)

def linear_func(x, a, b):
    return a * x + b

def perform_v1_experiment():
    # v1 parameters: all sources use thin aluminum (0.45mm)
    sources = ["Ra", "Sr90", "Cs137", "Co60"]
    thicknesses = np.linspace(0.05, 0.45, 9)
    results = []

    for source in sources:
        print(f"running v1 experiment: {source} with aluminum")
        rates = []
        for x in thicknesses:
            rate = run_geant4(source, "G4_Al", x)
            rates.append(rate)
        results.append({"source": source, "thicknesses": thicknesses, "rates": rates})

    return results

def plot_and_analyze_v1(results):
    os.makedirs('v1_initial/plots', exist_ok=True)
    os.makedirs('v1_initial/data', exist_ok=True)

    with open("v1_initial/data/v1_results.txt", "w") as f:
        f.write("--- v1 initial approach results ---\n\n")
        for res in results:
            source = res["source"]
            x = np.array(res["thicknesses"])
            y = np.array(res["rates"]) * 5000 
            
            popt_exp, _ = curve_fit(exponential_func, x, y)
            y_log = np.log(y[y > 0])
            x_log = x[y > 0]
            popt_lin, _ = curve_fit(linear_func, x_log, y_log)
            mu = -popt_lin[0]

            plt.figure(figsize=(8, 6))
            plt.scatter(x, y, color='red', label='v1 data (thin al)')
            x_fit = np.linspace(min(x), max(x), 100)
            plt.plot(x_fit, exponential_func(x_fit, *popt_exp), 'b-', label=f'exp fit ($\mu$={mu:.4f})')
            plt.xlabel('absorber thickness (aluminum) x (mm)')
            plt.ylabel('count rate n')
            plt.title(f'v1: {source} in aluminum (straight line observation)')
            plt.legend()
            plt.grid(True)
            plt.savefig(f'v1_initial/plots/{source.lower()}_v1.png')
            plt.close()

            f.write(f"Source: {source} | mu: {mu:.4f}\n")

if __name__ == "__main__":
    results = perform_v1_experiment()
    plot_and_analyze_v1(results)
    if os.path.exists("temp.mac"):
        os.remove("temp.mac")
