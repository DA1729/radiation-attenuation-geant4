# Absorption of Beta and Gamma Radiation

This Geant4 simulation studies the attenuation of radiation from various radioactive sources (Ra-226, Sr-90, Cs-137, Co-60) passing through an Aluminum absorber. The goal is to determine the linear absorption coefficient ($\mu$) for each source.

## Building the Project

Ensure you have Geant4 installed and configured.

```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

## Running the Experiment

The experiment is fully automated with a Python script that:
1. Runs the Geant4 simulation for each source.
2. Iterates through absorber thicknesses (0.0 mm to 0.45 mm).
3. Parses the counts from the detector.
4. Performs data analysis and generates plots.

```bash
python3 run_experiment.py
```

## Results Summary

| Source | $\mu$ (mm$^{-1}$) | Primary Radiation |
| :--- | :--- | :--- |
| **Sr-90** | 3.6889 | Beta (High Absorption) |
| **Cs-137** | 0.0182 | Gamma (662 keV) |
| **Radium (Ra)** | 0.0131 | Gamma (1.0 MeV rep.) |
| **Co-60** | 0.0089 | Gamma (1.25 MeV rep.) |

Detailed results are available in `results/data/absorption_results.txt`.

## Plots and Analysis

### GM Plateau
The GM Plateau curve shows the simulated counting efficiency versus operating voltage.
![GM Plateau](results/plots/gm_plateau.png)

### Strontium-90 (Beta)
Sr-90 emits beta particles which are significantly attenuated by thin layers of Aluminum.
![Sr90 Exponential](results/plots/sr90_exponential.png)
![Sr90 Linear](results/plots/sr90_linear.png)

### Gamma Sources (Ra, Cs-137, Co-60)
Gamma rays exhibit much lower absorption in Aluminum compared to beta particles, as seen in the near-horizontal linear fits.
- **Radium**: [Exponential](results/plots/ra_exponential.png) | [Linear](results/plots/ra_linear.png)
- **Cesium-137**: [Exponential](results/plots/cs137_exponential.png) | [Linear](results/plots/cs137_linear.png)
- **Cobalt-60**: [Exponential](results/plots/co60_exponential.png) | [Linear](results/plots/co60_linear.png)

## Project Structure
- `src/`, `include/`: Geant4 source code for geometry and physics.
- `results/plots/`: Generated plots for all experiments.
- `results/data/`: Tabulated counts and calculated $\mu$ values.
- `run_experiment.py`: Automation script for running the simulation and analysis.
- `main.cc`: Entry point for the Geant4 application.
