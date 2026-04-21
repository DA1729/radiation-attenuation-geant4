#include "primary_generator_action.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomTools.hh"
#include "Randomize.hh"
#include <cmath>

namespace {
  // Dominant gamma lines from the Ra-226 decay chain in secular equilibrium.
  // Energies in MeV; weights are photon emission probabilities per parent decay
  // (Pb-214 and Bi-214 lines dominate the observed gamma spectrum of a sealed
  // Ra-226 source). Source: ENSDF / NNDC.
  struct GammaLine { G4double energy_MeV; G4double weight; };
  const GammaLine kRa226Lines[] = {
    {0.2420, 0.074},  // Pb-214
    {0.2952, 0.193},  // Pb-214
    {0.3519, 0.376},  // Pb-214
    {0.6093, 0.461},  // Bi-214
    {0.7684, 0.049},  // Bi-214
    {0.9341, 0.031},  // Bi-214
    {1.1203, 0.151},  // Bi-214
    {1.2381, 0.058},  // Bi-214
    {1.3780, 0.040},  // Bi-214
    {1.7645, 0.154},  // Bi-214
    {2.2041, 0.049}   // Bi-214
  };

  G4double SampleRa226GammaEnergy() {
    static const G4double total = []() {
      G4double s = 0.0;
      for (const auto& l : kRa226Lines) s += l.weight;
      return s;
    }();
    G4double u = G4UniformRand() * total;
    G4double cum = 0.0;
    for (const auto& l : kRa226Lines) {
      cum += l.weight;
      if (u <= cum) return l.energy_MeV * MeV;
    }
    return kRa226Lines[sizeof(kRa226Lines)/sizeof(kRa226Lines[0]) - 1].energy_MeV * MeV;
  }

  // Allowed beta-decay spectrum shape for Y-90 (Sr-90 daughter in equilibrium).
  // N(T) dT proportional to p (T+m_e) (T_max - T)^2 dT, with p = sqrt(T(T+2 m_e)).
  // Fermi function F(Z,T) is omitted here (Z=40 daughter Zr) for simplicity;
  // the correction is small near the endpoint and negligible for attenuation.
  G4double SampleY90BetaEnergy() {
    const G4double me   = 0.511 * MeV;
    const G4double Tmax = 2.28  * MeV;
    // Loose envelope for rejection sampling. The shape peaks near T ~ 0.4 Tmax;
    // evaluating there with a 50% safety factor gives an accept rate ~0.6.
    const G4double Tp   = 0.4 * Tmax;
    const G4double pp   = std::sqrt(Tp * (Tp + 2.0 * me));
    const G4double Nmax = 1.5 * pp * (Tp + me) * (Tmax - Tp) * (Tmax - Tp);

    G4double T, N;
    do {
      T = Tmax * G4UniformRand();
      const G4double p = std::sqrt(T * (T + 2.0 * me));
      N = p * (T + me) * (Tmax - T) * (Tmax - T);
    } while (G4UniformRand() * Nmax > N);
    return T;
  }
}

primary_generator_action::primary_generator_action()
: G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr), fSource("Ra")
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  fMessenger = new G4GenericMessenger(this, "/experiment/source/", "source control");
  fMessenger->DeclareMethod("set", &primary_generator_action::SetSource, "set radioactive source");

  SetSource("Ra");
}

primary_generator_action::~primary_generator_action() {
  delete fParticleGun;
  delete fMessenger;
}

void primary_generator_action::SetSource(G4String source) {
  fSource = source;
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();

  if (fSource == "Ra") {
    fParticleGun->SetParticleDefinition(table->FindParticle("gamma"));
    // Energy is sampled per event from the Ra-226 chain line spectrum.
  } else if (fSource == "Sr90") {
    fParticleGun->SetParticleDefinition(table->FindParticle("e-"));
    // Energy is sampled per event from the Y-90 beta spectrum.
  } else if (fSource == "Cs137") {
    fParticleGun->SetParticleDefinition(table->FindParticle("gamma"));
    fParticleGun->SetParticleEnergy(0.662*MeV);
  } else if (fSource == "Co60") {
    fParticleGun->SetParticleDefinition(table->FindParticle("gamma"));
    fParticleGun->SetParticleEnergy(1.25*MeV);
  }
}

void primary_generator_action::GeneratePrimaries(G4Event* event) {
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));

  if (fSource == "Sr90") {
    fParticleGun->SetParticleEnergy(SampleY90BetaEnergy());
  } else if (fSource == "Ra") {
    fParticleGun->SetParticleEnergy(SampleRa226GammaEnergy());
  }

  fParticleGun->GeneratePrimaryVertex(event);
}
