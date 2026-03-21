#include "detector_construction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4RunManager.hh"

detector_construction::detector_construction()
: G4VUserDetectorConstruction(), fThickness(0.0*mm), fMaterialName("G4_Al"), fAbsorberLog(nullptr)
{
  fMessenger = new G4GenericMessenger(this, "/experiment/", "Experiment Control");
  fMessenger->DeclareMethod("thickness", &detector_construction::SetAbsorberThickness, "Set absorber thickness");
  fMessenger->DeclareMethod("material", &detector_construction::SetAbsorberMaterial, "Set absorber material");
}

detector_construction::~detector_construction() {
  delete fMessenger;
}

G4VPhysicalVolume* detector_construction::Construct() {
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* absorber_mat = nist->FindOrBuildMaterial(fMaterialName);
  G4Material* det_mat = nist->FindOrBuildMaterial("G4_Ar");

  // World
  G4Box* world_box = new G4Box("world", 10*cm, 10*cm, 15*cm);
  G4LogicalVolume* world_log = new G4LogicalVolume(world_box, world_mat, "world");
  G4VPhysicalVolume* world_phys = new G4PVPlacement(0, G4ThreeVector(), world_log, "world", 0, false, 0);

  // Absorber
  // Place at z = 5 cm
  G4double absorber_z = 5.0*cm;
  if (fThickness > 0) {
    G4Box* absorber_box = new G4Box("absorber", 5*cm, 5*cm, fThickness/2.0);
    fAbsorberLog = new G4LogicalVolume(absorber_box, absorber_mat, "absorber");
    new G4PVPlacement(0, G4ThreeVector(0, 0, absorber_z), fAbsorberLog, "absorber", world_log, false, 0);
  }

  // Detector (GM Counter window area)
  // Place at z = 10 cm
  G4Tubs* det_tubs = new G4Tubs("detector", 0, 1.5*cm, 1*cm, 0, 360*deg);
  G4LogicalVolume* det_log = new G4LogicalVolume(det_tubs, det_mat, "detector");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 10*cm), det_log, "detector", world_log, false, 0);

  // Visual Attributes
  G4VisAttributes* worldVis = new G4VisAttributes(false);
  world_log->SetVisAttributes(worldVis);

  G4VisAttributes* absorberVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.5));
  if (fAbsorberLog) fAbsorberLog->SetVisAttributes(absorberVis);

  G4VisAttributes* detVis = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.3));
  det_log->SetVisAttributes(detVis);

  return world_phys;
}

void detector_construction::ConstructSDandField() {}

void detector_construction::SetAbsorberThickness(G4double thickness) {
  fThickness = thickness;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void detector_construction::SetAbsorberMaterial(G4String material) {
  fMaterialName = material;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
