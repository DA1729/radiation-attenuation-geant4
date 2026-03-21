#ifndef primary_generator_action_hh
#define primary_generator_action_hh

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4GenericMessenger.hh"

class primary_generator_action : public G4VUserPrimaryGeneratorAction
{
  public:
    primary_generator_action();
    virtual ~primary_generator_action();

    virtual void GeneratePrimaries(G4Event* event);

    void SetSource(G4String source);

  private:
    G4ParticleGun* fParticleGun;
    G4GenericMessenger* fMessenger;
    G4String fSource;
};

#endif
