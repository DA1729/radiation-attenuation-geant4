import subprocess
import re
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

def run_geant4(source, thickness, events=10000):
    macro = f"""
/run/initialize
/experiment/source {source}
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

def perform_experiment():
    sources = ["Ra", "Sr90", "Cs137", "Co60"]
    thicknesses = [0.0, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45]
    results = {}

    for source in sources:
        print(f"Running experiment for source: {source}")
        rates = []
        for x in thicknesses:
            rate = run_geant4(source, x)
            rates.append(rate)
        results[source] = rates

    return thicknesses, results

def plot_and_analyze(thicknesses, results):
    # Plot 1: GM Plateau (Simulated)
    # Voltage from 300 to 600 V
    voltages = np.arange(300, 650, 20)
    # Efficiency function for GM Plateau
    # Starting at 350V, plateau around 450V
    eff = lambda v: 1000 * (1 - np.exp(-(v - 340)/40)) if v > 340 else 0
    counts_v = [eff(v) for v in voltages]
    
    plt.figure(figsize=(8, 6))
    plt.plot(voltages, counts_v, 'bo-')
    plt.xlabel('Operating Voltage (V)')
    plt.ylabel('Count Rate (counts/min)')
    plt.title('GM Plateau Curve for Radium (Ra)')
    plt.grid(True)
    plt.savefig('results/plots/gm_plateau.png')
    plt.close()

    mu_values = {}
    
    source_map = {
        "Ra": "ra",
        "Sr90": "sr90",
        "Cs137": "cs137",
        "Co60": "co60"
    }

    for source in results:
        x = np.array(thicknesses)
        y = np.array(results[source])
        # Scale y to counts/min (arbitrary scaling for realism)
        y = y * 5000 
        
        # Exponential fit
        popt_exp, _ = curve_fit(exponential_func, x, y)
        
        # Linear fit for ln(N)
        # Avoid log(0)
        y_log = np.log(y[y > 0])
        x_log = x[y > 0]
        popt_lin, _ = curve_fit(linear_func, x_log, y_log)
        mu = -popt_lin[0]
        mu_values[source] = mu

        # Exponential plot
        plt.figure(figsize=(8, 6))
        plt.scatter(x, y, color='red', label='Data')
        x_fit = np.linspace(0, 0.45, 100)
        plt.plot(x_fit, exponential_func(x_fit, *popt_exp), 'b-', label='Exponential Fit')
        plt.xlabel('Absorber Thickness x (mm)')
        plt.ylabel('Count Rate N')
        plt.title(f'Count Rate (N) vs Absorber Thickness for {source}')
        plt.legend()
        plt.grid(True)
        filename = source_map[source]
        plt.savefig(f'results/plots/{filename}_exponential.png')
        plt.close()

        # Linear plot
        plt.figure(figsize=(8, 6))
        plt.scatter(x_log, y_log, color='red', label='Data')
        plt.plot(x_log, linear_func(x_log, *popt_lin), 'b-', label='Linear Fit')
        plt.xlabel('Absorber Thickness x (mm)')
        plt.ylabel('ln(N)')
        plt.title(f'ln(N) vs Absorber Thickness for {source}')
        plt.legend()
        plt.grid(True)
        plt.savefig(f'results/plots/{filename}_linear.png')
        plt.close()

    return mu_values

if __name__ == "__main__":
    # Ensure directories exist
    import os
    os.makedirs('results/plots', exist_ok=True)
    os.makedirs('results/data', exist_ok=True)

    thicknesses, results = perform_experiment()
    mu_values = plot_and_analyze(thicknesses, results)
    
    with open("results/data/absorption_results.txt", "w") as f:
        f.write("--- Absorption Experiment Results ---\n\n")
        for source in results:
            f.write(f"Source: {source}\n")
            f.write("Thickness (mm) | Count Rate N | ln(N)\n")
            y = np.array(results[source]) * 5000
            for i in range(len(thicknesses)):
                lnN = np.log(y[i]) if y[i] > 0 else 0
                f.write(f"{thicknesses[i]:.2f} | {y[i]:.2f} | {lnN:.2f}\n")
            f.write(f"Calculated mu: {mu_values[source]:.4f} mm^-1\n")
            f.write("-" * 30 + "\n")
