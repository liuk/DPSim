#include "DPTrackingAction.h"
#include "DPMCRawEvent.h"

#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

DPTrackingAction::DPTrackingAction()
{
    p_IOmanager = DPIOManager::instance();
}

DPTrackingAction::~DPTrackingAction() {}

void DPTrackingAction::PreUserTrackingAction(const G4Track* theTrack)
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " " << theTrack->GetTrackID() << "  " << theTrack->GetDefinition()->GetPDGEncoding() << std::endl;
#endif

    DPMCTrack mcTrack;
    mcTrack.fTrackID = theTrack->GetTrackID();
    mcTrack.fParentID = theTrack->GetParentID();
    mcTrack.fCharge = theTrack->GetDefinition()->GetPDGCharge();
    mcTrack.fPDGCode = theTrack->GetDefinition()->GetPDGEncoding();
    mcTrack.fOriginVol = theTrack->GetVolume()->GetName();

    G4ThreeVector pos_i = theTrack->GetPosition();
    G4ThreeVector mom_i = theTrack->GetMomentum();
    mcTrack.fInitialPos.SetXYZ(pos_i[0]/cm, pos_i[1]/cm, pos_i[2]/cm);
    mcTrack.fInitialMom.SetXYZT(mom_i[0]/GeV, mom_i[1]/GeV, mom_i[2]/GeV, theTrack->GetVertexKineticEnergy()/GeV);

    if(theTrack->GetCreatorProcess())
    {
        const G4VProcess* origin = theTrack->GetCreatorProcess();
        mcTrack.fProcess = origin->GetProcessName();
    }
    else
    {
        mcTrack.fProcess = "generator";
    }

    for(int i = 0; i < 4; ++i)
    {
        mcTrack.fInHodoAcc[i] = false;
        mcTrack.fInChamberAcc[i] = false;
    }
    mcTrack.fHitIDs.clear();

    p_IOmanager->fillOneTrack(mcTrack);
}

void DPTrackingAction::PostUserTrackingAction(const G4Track* theTrack)
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " " << theTrack->GetTrackID() << "  " << theTrack->GetDefinition()->GetPDGEncoding() << std::endl;
#endif
    p_IOmanager->updateOneTrack(theTrack->GetTrackID(), theTrack->GetPosition(), theTrack->GetMomentum());
}
