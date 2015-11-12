#include "DPSteppingAction.h"

#include "G4SystemOfUnits.hh"
#include "G4TrackStatus.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"

DPSteppingAction::DPSteppingAction()
{}

DPSteppingAction::~DPSteppingAction()
{}

void DPSteppingAction::UserSteppingAction(const G4Step* theStep)
{
    G4Track* theTrack = theStep->GetTrack();

    if(theTrack->GetMomentumDirection()[2] < 0.)  //going backwards
    {
        theTrack->SetTrackStatus(fStopAndKill);
        return;
    }

    double z_pos = theTrack->GetPosition()[2]/cm;
    if(z_pos > 0. && z_pos < 500.) //in beam dump, but not enough energy
    {
        double minE = (500. - z_pos)*6./500.*GeV;
        if(theTrack->GetTotalEnergy() < minE)
        {
            theTrack->SetTrackStatus(fStopAndKill);
            return;
        }
    }

    if(theTrack->GetTotalEnergy() < 1.*MeV)
    {
        theTrack->SetTrackStatus(fStopAndKill);
        return;
    }
}
