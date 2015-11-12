#ifndef DPEventAction_H
#define DPEventAction_H

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "DPIOManager.h"

class DPEventAction: public G4UserEventAction
{
public:
    DPEventAction();
    virtual ~DPEventAction();

public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndofEventAction(const G4Event*);

private:
    DPIOManager* p_IOmamnger;
};

#endif
