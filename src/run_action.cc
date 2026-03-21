#include "run_action.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

run_action::run_action() : G4UserRunAction(), fCounts(0) {}
run_action::~run_action() {}

void run_action::BeginOfRunAction(const G4Run*) {
  fCounts = 0;
}

void run_action::EndOfRunAction(const G4Run* run) {
  G4int events = run->GetNumberOfEvent();
  if (events == 0) return;
  G4cout << "--- absorption results ---" << G4endl;
  G4cout << "counts: " << fCounts << G4endl;
  G4cout << "events: " << events << G4endl;
  G4cout << "rate: " << (G4double)fCounts/events << G4endl;
}
