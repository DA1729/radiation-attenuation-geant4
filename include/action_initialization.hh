#ifndef action_initialization_hh
#define action_initialization_hh
#include "G4VUserActionInitialization.hh"
class action_initialization : public G4VUserActionInitialization {
public:
  action_initialization();
  virtual ~action_initialization();
  virtual void BuildForMaster() const;
  virtual void Build() const;
};
#endif
