#include "DPMCRawEvent.h"

#include <TMath.h>

ClassImp(DPMCDimuon)
ClassImp(DPMCHeader)
ClassImp(DPMCTrack)
ClassImp(DPMCHit)
ClassImp(DPMCRawEvent)

void DPMCDimuon::calcVariables()
{
    Double_t mp = 0.938;
    Double_t ebeam = 120.;

    TLorentzVector p_beam(0., 0., sqrt(ebeam*ebeam - mp*mp), ebeam);
    TLorentzVector p_target(0., 0., 0., mp);

    TLorentzVector p_cms = p_beam + p_target;
    TLorentzVector p_sum = fPosMomentum + fNegMomentum;

    fMass = p_sum.M();
    fpT = p_sum.Perp();

    fx1 = (p_target*p_sum)/(p_target*p_cms);
    fx2 = (p_beam*p_sum)/(p_beam*p_cms);

    Double_t s = p_cms.M2();
    Double_t sqrts = p_cms.M();
    TVector3 bv_cms = p_cms.BoostVector();
    p_sum.Boost(-bv_cms);

    fxF = 2.*p_sum.Pz()/sqrts/(1. - fMass*fMass/s);
    fCosTh = 2.*(fNegMomentum.E()*fPosMomentum.Pz() - fPosMomentum.E()*fNegMomentum.Pz())/fMass/TMath::Sqrt(fMass*fMass + fpT*fpT);
    fPhi = TMath::ATan(2.*TMath::Sqrt(fMass*fMass + fpT*fpT)/fMass*(fNegMomentum.Px()*fPosMomentum.Py() - fPosMomentum.Px()*fNegMomentum.Py())/(fPosMomentum.Px()*fPosMomentum.Px() - fNegMomentum.Px()*fNegMomentum.Px() + fPosMomentum.Py()*fPosMomentum.Py() - fNegMomentum.Py()*fNegMomentum.Py()));
}

DPMCTrack::DPMCTrack()
{
    for(int i = 0; i < 4; ++i)
    {
        fInHodoAcc[i] = false;
        fInChamberAcc[i] = false;
    }

    fHitIDs.clear();
}

std::ostream& operator << (std::ostream& os, const DPMCHit& hit)
{
    os << "Hit " << hit.fHitID << " comes from track " << hit.fTrackID << "\n"
       << "   on detector " << hit.fDetectorID << " element " << hit.fElementID << " with distance to center = " << hit.fDriftDistance << "\n"
       << "   real position = (" << hit.fPosition.X() << ", " << hit.fPosition.Y() << ", " << hit.fPosition.Z() << ") cm\n"
       << "   real momentum = (" << hit.fMomentum.X() << ", " << hit.fMomentum.Y() << ", " << hit.fMomentum.Z() << ") cm";
    return os;
}

DPMCRawEvent::DPMCRawEvent()
{
    fDimuons = new TClonesArray("DPMCDimuon");
    fNDimuons = 0;

    fTracks = new TClonesArray("DPMCTrack");
    fNTracks = 0;

    fHits = new TClonesArray("DPMCHit");
    fNHits = 0;

    fEvtHeader.fRunID = -1;
    fEvtHeader.fSpillID = -1;
    fEvtHeader.fEventID = -1;
}

DPMCRawEvent::~DPMCRawEvent()
{
    clear();
    delete fDimuons;
    delete fTracks;
    delete fHits;
}

void DPMCRawEvent::clear()
{
    fNDimuons = 0;
    fNTracks = 0;
    fNHits = 0;

    fDimuons->Clear();
    fTracks->Clear();
    fHits->Clear();
}

UInt_t DPMCRawEvent::addDimuon(DPMCDimuon dimuon, Int_t index)
{
    dimuon.fDimuonID = index > 0 ? index : fNDimuons;

    TClonesArray& Dimuons = *fDimuons;
    new(Dimuons[fNDimuons++]) DPMCDimuon(dimuon);

    return dimuon.fDimuonID;
}

UInt_t DPMCRawEvent::addTrack(DPMCTrack track, Int_t index)
{
    track.fTrackID = index > 0 ? index : fNTracks;

    TClonesArray& Tracks = *fTracks;
    new(Tracks[fNTracks++]) DPMCTrack(track);

    return track.fTrackID;
}

UInt_t DPMCRawEvent::addHit(DPMCHit hit, Int_t trackID, Int_t index)
{
    hit.fHitID = index > 0 ? index : fNHits;

    TClonesArray& Hits = *fHits;
    new(Hits[fNHits++]) DPMCHit(hit);

    if(trackID >= 0 && trackID < fNTracks)
    {
        DPMCTrack* track = (DPMCTrack*)fTracks->At(trackID);
        track->fHitIDs.push_back(hit.fHitID);
    }

    return hit.fHitID;
}
