#ifndef DPMCEvent_H
#define DPMCEvent_H

#include <vector>

#include <TROOT.h>
#include <TObject.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>

class DPMCDimuon: public TObject
{
public:
    UInt_t fDimuonID;
    UInt_t fPosTrackID;
    UInt_t fNegTrackID;

    TVector3 fVertex;
    TLorentzVector fPosMomentum;
    TLorentzVector fNegMomentum;

public:   //derived variables
    void calcVariables();
    Double_t fMass, fpT, fxF, fx1, fx2, fCosTh, fPhi;

    ClassDef(DPMCDimuon, 1)
};

class DPMCHeader: public TObject
{
public:
    Int_t fRunID;
    UInt_t fSpillID;
    UInt_t fEventID;

    Double_t fSigWeight;

    ClassDef(DPMCHeader, 1)
};

class DPMCTrack: public TObject
{
public:
    DPMCTrack();

public:
    UInt_t fTrackID;
    Short_t fCharge;
    Int_t fPDGCode;

    bool fInHodoAcc[4];
    bool fInChamberAcc[4];

    TVector3 fInitialPos;
    TLorentzVector fInitialMom;

    TVector3 fFinalPos;
    TLorentzVector fFinalMom;

    UInt_t fParentID;
    TString fProcess;

    std::vector<UInt_t> fHitIDs;

    ClassDef(DPMCTrack, 1)
};

class DPMCHit: public TObject
{
public:
    //digitized info
    UInt_t fHitID;
    UInt_t fTrackID;
    Short_t fDetectorID;
    Short_t fElementID;
    Double_t fDriftDistance;

    //truth info
    TVector3 fMomentum;
    TVector3 fPosition;
    //Double_t fDepEnergy;

    ClassDef(DPMCHit, 1)
};

class DPMCRawEvent: public TObject
{
public:
    DPMCRawEvent();
    virtual ~DPMCRawEvent();

    //re-initialize all the IDs and container
    void clear();

    DPMCHeader& eventHeader() { return fEvtHeader; }

    UInt_t getNDimuons() { return fNDimuons; }
    TClonesArray* getDimuons() { return fDimuons; }
    DPMCDimuon getDimuon(Int_t i) { return *(DPMCDimuon*)fDimuons->At(i); }

    UInt_t getNTracks() { return fNTracks; }
    TClonesArray* getTracks() { return fTracks; }
    DPMCTrack getTrack(Int_t i) { return *(DPMCTrack*)fTracks->At(i); }

    UInt_t getNHits() { return fNHits; }
    TClonesArray* getHits() { return fHits; }
    DPMCHit getHit(Int_t i) { return *(DPMCHit*)fHits->At(i); }

    UInt_t addDimuon(DPMCDimuon dimuon, Int_t index = -1);
    UInt_t addTrack(DPMCTrack track, Int_t index = -1);
    UInt_t addHit(DPMCHit hit, Int_t trackID = -1, Int_t index = -1);

private:
    DPMCHeader fEvtHeader;

    UInt_t fNDimuons;
    TClonesArray* fDimuons;

    UInt_t fNTracks;
    TClonesArray* fTracks;

    UInt_t fNHits;
    TClonesArray* fHits;

    ClassDef(DPMCRawEvent, 1)
};

#endif
