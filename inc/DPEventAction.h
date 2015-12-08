#ifndef DPEventAction_H
#define DPEventAction_H

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include <TStopwatch.h>

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

    //Total number of events to process
    int nEventsAll;

    //Total time so far
    double timeElapsed;

    //Timer for performance benchmark
    TStopwatch timer;
};

#endif
