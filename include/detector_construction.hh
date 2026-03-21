#ifndef detector_construction_hh
#define detector_construction_hh

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4GenericMessenger.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class detector_construction : public G4VUserDetectorConstruction
{
  public:
    detector_construction();
    virtual ~detector_construction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    void SetAbsorberThickness(G4double thickness);
    void SetAbsorberMaterial(G4String material);

  private:
    G4GenericMessenger* fMessenger;
    G4double fThickness;
    G4String fMaterialName;

    G4LogicalVolume* fAbsorberLog;
};

#endif
