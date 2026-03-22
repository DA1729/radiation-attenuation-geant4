# Enhanced Absorption Study of Beta and Gamma Radiation

**Author:** Daksh Pandey  
**Course:** PHT-106 (Charged Particles Spectroscopy)  
**College:** IIT Roorkee

## Overview
This study utilizes Geant4 simulations to analyze the attenuation of radiation through matter. By employing tailored absorber materials and thickness ranges, we successfully demonstrate the characteristic exponential decay and determine the linear absorption coefficients ($\mu$) for various sources.

## Simulation Setup
- **Sr-90 (Beta):** Simulated using a Y-90 beta spectrum ($E_{max} = 2.28$ MeV). Absorber: **Aluminum** (0–5 mm).
- **Gamma Sources (Cs-137, Co-60, Ra-226):** Simulated using characteristic photon energies. Absorber: **Lead** (0–30 mm) to ensure measurable attenuation.
- **Scoring:** Event-by-event counting in an Argon-filled detector volume.

---

## 1. Summary of Results

| Source | Material | $\mu$ (mm$^{-1}$) | Inferences |
| :--- | :--- | :--- | :--- |
| **Sr-90** | Aluminum | 1.0529 | Characteristic beta range behavior. |
| **Cs-137** | Lead | 0.1203 | Strong gamma attenuation in high-Z material. |
| **Ra-226** | Lead | 0.0779 | High energy penetration. |
| **Co-60** | Lead | 0.0642 | Lowest $\mu$ due to highest photon energy. |

---

## 2. Graphical Analysis

### GM Plateau Curve
Establishing the stable operating voltage for the Geiger-Muller detector.
![GM Plateau](results/plots/gm_plateau.png)

---

### Strontium-90 (Beta in Aluminum)
With the introduction of a beta spectrum and thicker Aluminum (up to 5 mm), the "tail" of the absorption curve becomes visible as higher energy electrons penetrate deeper.
| Exponential Fit | Linear Fit ($\ln(N)$ vs $x$) |
| :---: | :---: |
| ![Sr90 Exp](results/plots/sr90_exponential.png) | ![Sr90 Lin](results/plots/sr90_linear.png) |

---

### Cesium-137 (662 keV Gamma in Lead)
By using Lead as an absorber, we observe a clear exponential decay for gamma radiation, which was previously "flat" in thin Aluminum.
| Exponential Fit | Linear Fit ($\ln(N)$ vs $x$) |
| :---: | :---: |
| ![Cs137 Exp](results/plots/cs137_exponential.png) | ![Cs137 Lin](results/plots/cs137_linear.png) |

---

### Radium-226 (1.0 MeV Gamma in Lead)
![Ra Exp](results/plots/ra_exponential.png)
![Ra Lin](results/plots/ra_linear.png)

---

### Cobalt-60 (1.25 MeV Gamma in Lead)
Co-60 shows the most linear-like exponential behavior because its high energy requires even more Lead to fully stop.
![Co60 Exp](results/plots/co60_exponential.png)
![Co60 Lin](results/plots/co60_linear.png)

---

## Conclusion
The enhanced simulation provides a high-fidelity representation of nuclear physics principles:
1. **Beta particles** exhibit a complex absorption curve due to their continuous energy spectrum and heavy Coulomb interaction.
2. **Gamma rays** follow the exponential attenuation law $I = I_0 e^{-\mu x}$ clearly when using high-density, high-$Z$ materials like Lead.
3. The inverse relationship between **incident energy** and **absorption coefficient** is clearly demonstrated across the gamma sources.

## Code Availability
The simulation framework and analysis scripts are available at:  
[GitHub: Daksh-Pandey/Absorption-Coefficient-Geant4](https://github.com/Daksh-Pandey/Absorption-Coefficient-Geant4)
