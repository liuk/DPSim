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
    virtual void EndOfEventAction(const G4Event*);

private:
    DPIOManager* p_IOmamnger;

    //print frequency
    int printFreq;
};

#endif
