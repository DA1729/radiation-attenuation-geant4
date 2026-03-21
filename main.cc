#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "detector_construction.hh"
#include "action_initialization.hh"
int main(int argc, char** argv) {
  G4UIExecutive* ui = 0;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }
  G4RunManager* run_manager = new G4RunManager;
  run_manager->SetUserInitialization(new detector_construction());
  G4VModularPhysicsList* physics_list = new FTFP_BERT;
  physics_list->ReplacePhysics(new G4EmStandardPhysics_option4());
  run_manager->SetUserInitialization(physics_list);
  run_manager->SetUserInitialization(new action_initialization());
  G4VisManager* vis_manager = new G4VisExecutive;
  vis_manager->Initialize();
  G4UImanager* ui_manager = G4UImanager::GetUIpointer();
  if (!ui) {
    G4String command = "/control/execute ";
    G4String file_name = argv[1];
    ui_manager->ApplyCommand(command + file_name);
  } else {
    ui_manager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }
  delete vis_manager;
  delete run_manager;
  return 0;
}
