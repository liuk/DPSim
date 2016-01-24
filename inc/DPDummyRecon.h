#ifndef DPDummyRecon_H
#define DPDummyRecon_H

#include <vector>

#include "G4ErrorPropagator.hh"
#include "G4ErrorPropagatorData.hh"
#include "G4ErrorPropagatorManager.hh"
#include "G4ErrorPlaneSurfaceTarget.hh"
#include "G4ErrorSurfaceTrajState.hh"
#include "G4ErrorTrajErr.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleTable.hh"

#include "DPMCRawEvent.h"

class DPDummyRecon
{
public:
    DPDummyRecon();

    //Reconstruct one event
    std::vector<DPMCDimuon> reconstruct(DPMCRawEvent* rawEvent);

    //Set the initial particle and pos/mom
    void setParticle(int pdgCode, G4ThreeVector pos, G4ThreeVector mom);

    //Swim the track
    bool swimTo(double z);

    //Get the tracks after swimming
    G4ThreeVector getFinalPos() { return finalPos; }
    G4ThreeVector getFinalMom() { return finalMom; }

private:
    //Geant4e
    G4ErrorPropagatorManager* g4eManager;
    G4ErrorPropagatorData* g4eData;
    G4ErrorPlaneSurfaceTarget* g4eTarget;
    G4ErrorFreeTrajState* g4eState;
    G4ErrorMode g4eMode;

    //Pointer to the particle table
    G4ParticleTable* particleDict;

    //Particle name
    G4String particleName;

    //Initial state
    G4ThreeVector initPos;
    G4ThreeVector initMom;

    //Final state
    G4ThreeVector finalPos;
    G4ThreeVector finalMom;
};

#endif
