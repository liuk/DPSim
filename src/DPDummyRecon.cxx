#include "DPDummyRecon.h"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

#include <TVector3.h>
#include <TLorentzVector.h>

#include "DPDetectorConstruction.h"

DPDummyRecon::DPDummyRecon()
{
    //Initial propagator manager and all related stuff
    g4eManager = G4ErrorPropagatorManager::GetErrorPropagatorManager();
    g4eData = G4ErrorPropagatorData::GetErrorPropagatorData();

    G4VPhysicalVolume* world = ((DPDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetWorldPtr();
    g4eManager->SetUserInitialization(world);
    g4eManager->InitGeant4e();

    G4UImanager::GetUIpointer()->ApplyCommand("/geant4e/limits/stepLength 10 mm");

    //Initialize the particle lut
    particleDict = G4ParticleTable::GetParticleTable();
}

void DPDummyRecon::reconstruct(DPMCRawEvent* rawEvent)
{
    int nDimuons = rawEvent->getNDimuons();
    for(int i = 0; i < nDimuons; ++i)
    {
        DPMCDimuon dimuon = rawEvent->getDimuon(i);

        //swim mu+
        DPMCTrack mup = rawEvent->getTrack(dimuon.fPosTrackID);
        DPMCHit phit = rawEvent->getHit(mup.fHitIDs[0]);

        setParticle(mup.fPDGCode, G4ThreeVector(phit.fPosition.X()*cm, phit.fPosition.Y()*cm, phit.fPosition.Z()*cm), G4ThreeVector(phit.fMomentum.Px()*GeV, phit.fMomentum.Py()*GeV, phit.fMomentum.Pz()*GeV));
        if(!swimTo(dimuon.fVertex.Z()*cm)) break;
        TVector3 ppos(finalPos.x()/cm, finalPos.y()/cm, finalPos.z()/cm);
        dimuon.fPosMomentum.SetXYZM(finalMom.x()/GeV, finalMom.y()/GeV, finalMom.z()/GeV, 0.10566);

        //swim mu+
        DPMCTrack mum = rawEvent->getTrack(dimuon.fNegTrackID);
        DPMCHit mhit = rawEvent->getHit(mum.fHitIDs[0]);

        setParticle(mum.fPDGCode, G4ThreeVector(mhit.fPosition.X()*cm, mhit.fPosition.Y()*cm, mhit.fPosition.Z()*cm), G4ThreeVector(mhit.fMomentum.Px()*GeV, mhit.fMomentum.Py()*GeV, mhit.fMomentum.Pz()*GeV));
        if(!swimTo(dimuon.fVertex.Z()*cm)) break;
        TVector3 mpos(finalPos.x()/cm, finalPos.y()/cm, finalPos.z()/cm);
        dimuon.fNegMomentum.SetXYZM(finalMom.x()/GeV, finalMom.y()/GeV, finalMom.z()/GeV, 0.10566);

        //update dimuon info
        dimuon.fVertex = 0.5*(ppos + mpos);
        dimuon.calcVariables();

        rawEvent->addRecDimuon(dimuon);
    }
}

void DPDummyRecon::setParticle(int pdgCode, G4ThreeVector pos, G4ThreeVector mom)
{
    particleName = particleDict->FindParticle(pdgCode)->GetParticleName();
    initPos = pos;
    initMom = mom;
}

bool DPDummyRecon::swimTo(double z)
{
    //set propagator mode
    g4eMode = z > initPos.z() ? G4ErrorMode_PropForwards : G4ErrorMode_PropBackwards;

    //create initial state and final target plane
    g4eState = new G4ErrorFreeTrajState(particleName, initPos, initMom);
    g4eTarget = new G4ErrorPlaneSurfaceTarget(0., 0., 1., -z);
    g4eData->SetTarget(g4eTarget);

    int err = 0;
    if(g4eMode == G4ErrorMode_PropBackwards)
    {
        g4eState->SetMomentum(-g4eState->GetMomentum());
        err = g4eManager->Propagate(g4eState, g4eTarget, g4eMode);
        g4eState->SetMomentum(-g4eState->GetMomentum());
    }
    else
    {
        err = g4eManager->Propagate(g4eState, g4eTarget, g4eMode);
    }

    //get the final results
    finalPos = g4eState->GetPosition();
    finalMom = g4eState->GetMomentum();

    //cleanup
    delete g4eTarget;
    delete g4eState;

    return err == 0;
}
