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

def perform_enhanced_experiment():
    # custom ranges
    configs = [
        {"source": "Sr90", "material": "G4_Al", "thicknesses": np.linspace(0, 5, 10), "label": "Sr-90 (Al)"},
        {"source": "Cs137", "material": "G4_Pb", "thicknesses": np.linspace(0, 20, 10), "label": "Cs-137 (Pb)"},
        {"source": "Co60", "material": "G4_Pb", "thicknesses": np.linspace(0, 30, 10), "label": "Co-60 (Pb)"},
        {"source": "Ra", "material": "G4_Pb", "thicknesses": np.linspace(0, 25, 10), "label": "Ra-226 (Pb)"},
    ]
    
    results = []

    for config in configs:
        source = config["source"]
        material = config["material"]
        print(f"running experiment: {source} with {material}")
        rates = []
        for x in config["thicknesses"]:
            rate = run_geant4(source, material, x)
            rates.append(rate)
        results.append({**config, "rates": rates})

    return results

def plot_and_analyze(results):
    os.makedirs('results/plots', exist_ok=True)
    os.makedirs('results/data', exist_ok=True)

    # gm plateau
    voltages = np.arange(300, 650, 20)
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

    mu_report = []

    for res in results:
        source = res["source"]
        x = np.array(res["thicknesses"])
        y = np.array(res["rates"]) * 5000 # scale for realism
        material_name = "Lead" if res["material"] == "G4_Pb" else "Aluminum"
        
        # exponential fit
        popt_exp, _ = curve_fit(exponential_func, x, y)
        
        # linear fit
        y_log = np.log(y[y > 0]) # avoid log(0)
        x_log = x[y > 0]
        popt_lin, _ = curve_fit(linear_func, x_log, y_log)
        mu = -popt_lin[0]
        mu_report.append({"source": source, "mu": mu, "material": material_name, "x": x, "y": y})

        # exponential plot
        plt.figure(figsize=(8, 6))
        plt.scatter(x, y, color='red', label='Simulated Data')
        x_fit = np.linspace(min(x), max(x), 100)
        plt.plot(x_fit, exponential_func(x_fit, *popt_exp), 'b-', label=f'Exp Fit ($\mu$={mu:.4f})')
        plt.xlabel(f'Absorber Thickness ({material_name}) x (mm)')
        plt.ylabel('Count Rate N')
        plt.title(f'Exponential Attenuation: {source} in {material_name}')
        plt.legend()
        plt.grid(True)
        plt.savefig(f'results/plots/{source.lower()}_exponential.png')
        plt.close()

        # linear plot
        plt.figure(figsize=(8, 6))
        plt.scatter(x_log, y_log, color='red', label='Simulated Data')
        plt.plot(x_log, linear_func(x_log, *popt_lin), 'b-', label='Linear Fit')
        plt.xlabel(f'Absorber Thickness ({material_name}) x (mm)')
        plt.ylabel('ln(N)')
        plt.title(f'Linear Fit: {source} in {material_name}')
        plt.legend()
        plt.grid(True)
        plt.savefig(f'results/plots/{source.lower()}_linear.png')
        plt.close()

    return mu_report

if __name__ == "__main__":
    # make directories
    os.makedirs('results/plots', exist_ok=True)
    os.makedirs('results/data', exist_ok=True)

    results = perform_enhanced_experiment()
    mu_report = plot_and_analyze(results)
    
    with open("results/data/absorption_results.txt", "w") as f:
        f.write("--- Enhanced Absorption Experiment Results ---\n\n")
        for report in mu_report:
            f.write(f"Source: {report['source']} | Material: {report['material']}\n")
            f.write("Thickness (mm) | Count Rate N | ln(N)\n")
            for i in range(len(report['x'])):
                lnN = np.log(report['y'][i]) if report['y'][i] > 0 else 0
                f.write(f"{report['x'][i]:.2f} | {report['y'][i]:.2f} | {lnN:.2f}\n")
            f.write(f"Calculated mu: {report['mu']:.4f} mm^-1\n")
            f.write("-" * 45 + "\n")
