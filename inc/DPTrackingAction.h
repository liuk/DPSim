#ifndef DPTrackingAction_H
#define DPTrackingAction_H

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"

#include "DPIOManager.h"

class DPTrackingAction: public G4UserTrackingAction
{
public:
    DPTrackingAction();
    virtual ~DPTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

private:
    DPIOManager* p_IOmanager;
};

#endif
