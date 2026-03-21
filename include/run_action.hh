#ifndef run_action_hh
#define run_action_hh

#include "G4UserRunAction.hh"
#include "globals.hh"

class run_action : public G4UserRunAction
{
  public:
    run_action();
    virtual ~run_action();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void AddCount() { fCounts++; }

  private:
    G4int fCounts;
};

#endif
