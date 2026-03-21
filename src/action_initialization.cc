#include "action_initialization.hh"
#include "primary_generator_action.hh"
#include "run_action.hh"
#include "stepping_action.hh"
action_initialization::action_initialization() : G4VUserActionInitialization() {}
action_initialization::~action_initialization() {}
void action_initialization::BuildForMaster() const {
  SetUserAction(new run_action);
}
void action_initialization::Build() const {
  SetUserAction(new primary_generator_action);
  run_action* ra = new run_action;
  SetUserAction(ra);
  SetUserAction(new stepping_action(ra));
}
