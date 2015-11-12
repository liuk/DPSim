#ifndef DPSteppingAction_H
#define DPSteppingAction_H

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

class DPSteppingAction: public G4UserSteppingAction
{
public:
    DPSteppingAction();
    virtual ~DPSteppingAction();

    virtual void UserSteppingAction(const G4Step*);
};

#endif
