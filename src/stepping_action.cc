#include "stepping_action.hh"
#include "run_action.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

stepping_action::stepping_action(run_action* ra) : G4UserSteppingAction(), fRunAction(ra) {}
stepping_action::~stepping_action() {}

void stepping_action::UserSteppingAction(const G4Step* step) {
  G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  if (!volume) return;
  if (volume->GetName() == "detector") {
    // first step in volume
    if (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) {
      fRunAction->AddCount();
    }
  }
}
