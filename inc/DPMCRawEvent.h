#ifndef DPMCEvent_H
#define DPMCEvent_H

#include <iostream>
#include <vector>

#include <TROOT.h>
#include <TObject.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>

#define NDETPLANES 48

class DPMCHeader: public TObject
{
public:
    Int_t fRunID;
    UInt_t fSpillID;
    UInt_t fEventID;

    Double_t fSigWeight;

    Int_t fTriggerBit;
    std::vector<Int_t> fPosRoadIDs;
    std::vector<Int_t> fNegRoadIDs;

    ClassDef(DPMCHeader, 1)
};

class DPMCHit: public TObject
{
public:
    int uniqueID() { return fDetectorID*1000 + fElementID; }

    friend std::ostream& operator << (std::ostream& os, const DPMCHit& hit);

public:
    //!digitized info
    //@{
    UInt_t fHitID;
    UInt_t fTrackID;
    Short_t fDetectorID;
    Short_t fElementID;
    Double_t fDriftDistance;
    //@}

    //!truth info
    ///@{
    Int_t fPDGCode;
    TVector3 fMomentum;
    TVector3 fPosition;
    Double_t fDepEnergy;
    //@}

    ClassDef(DPMCHit, 2)
};

class DPMCTrack: public TObject
{
public:
    DPMCTrack();

    //!set the acceptance bit
    void addHit(DPMCHit& hit);

    //!test if is accepted by all hodos
    bool isAccepted() { return fInHodoAcc[0] && fInHodoAcc[1] && fInHodoAcc[2] && fInHodoAcc[3]; }

    friend std::ostream& operator << (std::ostream& os, const DPMCTrack& track);

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

    Int_t fParentID;
    Int_t fParentPDGCode;
    TString fProcess;
    TString fOriginVol;

    std::vector<UInt_t> fHitIDs;

    ClassDef(DPMCTrack, 1)
};

class DPMCDimuon: public TObject
{
public:
    UInt_t fDimuonID;
    Int_t fPosTrackID;
    Int_t fNegTrackID;

    TVector3 fVertex;
    TLorentzVector fPosMomentum;
    TLorentzVector fNegMomentum;
    TString fOriginVol;

    bool fAccepted;

public:
    //! calculate derived variables
    void calcVariables();
    Double_t fMass, fpT, fxF, fx1, fx2, fCosTh, fPhi;

public:
    friend std::ostream& operator << (std::ostream& os, const DPMCDimuon& dimuon);

    ClassDef(DPMCDimuon, 1)
};

class DPMCRawEvent: public TObject
{
public:
    DPMCRawEvent();
    virtual ~DPMCRawEvent();

    //!re-initialize all the IDs and container
    void clear(bool partial = false);

    DPMCHeader& eventHeader() { return fEvtHeader; }

    UInt_t getNDimuons() { return fNDimuons; }
    TClonesArray* getDimuons() { return fDimuons; }
    DPMCDimuon getDimuon(Int_t i) { return *(DPMCDimuon*)fDimuons->At(i); }

    UInt_t getNTracks() { return fNTracks; }
    TClonesArray* getTracks() { return fTracks; }
    DPMCTrack getTrack(Int_t i) { return *(DPMCTrack*)fTracks->At(i); }

    UInt_t getNHits(Int_t i = 0) { return fNHits[i]; }
    TClonesArray* getHits() { return fHits; }
    DPMCHit getHit(Int_t i) { return *(DPMCHit*)fHits->At(i); }

    UInt_t addDimuon(DPMCDimuon dimuon, Int_t index = -1);
    UInt_t addTrack(DPMCTrack track, Int_t index = -1);
    UInt_t addHit(DPMCHit hit, Int_t trackID = -1, Int_t index = -1);

    //!Dummy recon part
    //@{
    UInt_t getNRecDimuons() { return fNRecDimuons; }
    TClonesArray* getRecDimuons() { return fRecDimuons; }
    DPMCDimuon getRecDimuon(Int_t i) { return *(DPMCDimuon*)fRecDimuons->At(i); }

    UInt_t addRecDimuon(DPMCDimuon dimuon, Int_t index = -1);
    //@}

    //!combine multiple events
    DPMCRawEvent& operator += (const DPMCRawEvent& event);

    //!copy one event
    DPMCRawEvent& operator = (const DPMCRawEvent& event);

    //!debugging output
    void print() const;

private:
    DPMCHeader fEvtHeader;

    UInt_t fNDimuons;
    TClonesArray* fDimuons;

    UInt_t fNRecDimuons;
    TClonesArray* fRecDimuons;

    UInt_t fNTracks;
    TClonesArray* fTracks;

    //!id = 0 for all, and the rest is for each plane
    UInt_t fNHits[NDETPLANES+1];
    TClonesArray* fHits;

    ClassDef(DPMCRawEvent, 1)
};

#endif
