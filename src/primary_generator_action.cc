#include "primary_generator_action.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomTools.hh"
#include "Randomize.hh"

primary_generator_action::primary_generator_action()
: G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr), fSource("Ra")
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  fMessenger = new G4GenericMessenger(this, "/experiment/", "experiment control");
  fMessenger->DeclareMethod("source", &primary_generator_action::SetSource, "set radioactive source");

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
    fParticleGun->SetParticleEnergy(1.0*MeV);
  } else if (fSource == "Sr90") {
    fParticleGun->SetParticleDefinition(table->FindParticle("e-"));
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
    // sr-90 beta spectrum
    G4double energy = 2.28 * MeV * G4UniformRand(); 
    fParticleGun->SetParticleEnergy(energy);
  }

  fParticleGun->GeneratePrimaryVertex(event);
}
