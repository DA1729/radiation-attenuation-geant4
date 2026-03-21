#include "primary_generator_action.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomTools.hh"

primary_generator_action::primary_generator_action()
: G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr), fSource("Ra")
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  fMessenger = new G4GenericMessenger(this, "/experiment/", "Experiment Control");
  fMessenger->DeclareMethod("source", &primary_generator_action::SetSource, "Set radioactive source (Ra, Sr90, Cs137, Co60)");

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
    // Radium-226: Use representative beta/gamma
    // For simplicity, let's use a 1 MeV electron or gamma
    fParticleGun->SetParticleDefinition(table->FindParticle("gamma"));
    fParticleGun->SetParticleEnergy(1.0*MeV);
  } else if (fSource == "Sr90") {
    // Strontium-90 / Yttrium-90: Beta emitter
    fParticleGun->SetParticleDefinition(table->FindParticle("e-"));
    fParticleGun->SetParticleEnergy(1.0*MeV); // Representative energy
  } else if (fSource == "Cs137") {
    // Cesium-137: Gamma 662 keV
    fParticleGun->SetParticleDefinition(table->FindParticle("gamma"));
    fParticleGun->SetParticleEnergy(0.662*MeV);
  } else if (fSource == "Co60") {
    // Cobalt-60: Gamma 1.25 MeV (average)
    fParticleGun->SetParticleDefinition(table->FindParticle("gamma"));
    fParticleGun->SetParticleEnergy(1.25*MeV);
  }
}

void primary_generator_action::GeneratePrimaries(G4Event* event) {
  // Point source at z=0, emitting into a cone towards the absorber
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));

  // Isotropic emission within a small cone towards z+
  // For simplicity, we can use a fixed direction or random within cone
  G4ThreeVector dir = G4RandomDirection();
  if (dir.z() < 0) dir.setZ(-dir.z()); // Half-sphere
  // Even better, limit to a small cone to improve efficiency
  // But let's keep it simple for now: fixed direction towards detector
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));

  fParticleGun->GeneratePrimaryVertex(event);
}
