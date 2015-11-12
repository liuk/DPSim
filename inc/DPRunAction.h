#ifndef DPRunAction_H
#define DPRunAction_H

#include "G4UserRunAction.hh"
#include "DPPrimaryGeneratorAction.h"

#include "G4Run.hh"

class DPRunAction: public G4UserRunAction
{
public:
    DPRunAction();
    virtual ~DPRunAction();

public:
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
};

#endif
