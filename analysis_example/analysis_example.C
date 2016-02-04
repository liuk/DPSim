#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>

//it's only here to suppress linter complains, comment out when actually running
//or writing your own analysis macro
//#include <DPMCRawEvent.h>

/*
This macro illustrates how to draw the dimuon mass/xF withing acceptance
*/
void acceptedMassXf()
{
    gSystem->Load("libMCRawEvent.so");

    TFile* dataFile = new TFile("output.root", "READ");
    TTree* dataTree = (TTree*)dataFile->Get("save");

    DPMCRawEvent* rawEvent = new DPMCRawEvent;
    dataTree->SetBranchAddress("rawEvent", &rawEvent);

    TH1D* hist1 = new TH1D("hist1", "hist1", 100, 0., 10.);   //hist for mass
    TH1D* hist2 = new TH1D("hist2", "hist2", 100, -1., 1.);   //hist for xF
    TH1D* hist1acc = new TH1D("hist1acc", "hist1acc", 100, 0., 10.);   //hist for accepted mass
    TH1D* hist2acc = new TH1D("hist2acc", "hist2acc", 100, -1., 1.);   //hist for accepted xF
    for(Int_t i = 0; i < dataTree->GetEntries(); ++i)
    {
        dataTree->GetEntry(i);

        //Normally there is only one dimuon per event
        //But pile-up of multiple dimuons is possible
        for(Int_t j = 0; j < rawEvent->getNDimuons(); ++j)
        {
            DPMCDimuon dimuon = rawEvent->getDimuon(j);

            double weight = rawEvent->eventHeader().fSigWeight;
            hist1->Fill(dimuon.fMass, weight);
            hist2->Fill(dimuon.fxF, weight);

            if(dimuon.fAccepted)
            {
                hist1acc->Fill(dimuon.fMass, weight);
                hist2acc->Fill(dimuon.fxF, weight);
            }
        }
    }

    TCanvas* c1 = new TCanvas();
    c1->Divide(2);

    c1->cd(1);
    hist1->Draw();
    hist1acc->Draw("same");

    c1->cd(2);
    hist2->Draw();
    hist2acc->Draw("same");
}


/*
This macro illustrates how to make 2D hit distribution on D2X of mu+ from a target dimuon within acceptance.
and have low mass (< 2GeV), and mu+ has initial momentum larger than 20 GeV
*/
void hitDistribution2D()
{
    gSystem->Load("libMCRawEvent.so");

    TFile* dataFile = new TFile("output.root", "READ");
    TTree* dataTree = (TTree*)dataFile->Get("save");

    DPMCRawEvent* rawEvent = new DPMCRawEvent;
    dataTree->SetBranchAddress("rawEvent", &rawEvent);

    TH2D* hist = new TH2D("hist", "hist", 50, -100., 100., 50, -100., 100.);
    for(Int_t i = 0; i < dataTree->GetEntries(); ++i)
    {
        dataTree->GetEntry(i);

        //Normally there is only one dimuon per event
        //But pile-up of multiple dimuons is possible
        for(Int_t j = 0; j < rawEvent->getNDimuons(); ++j)
        {
            DPMCDimuon dimuon = rawEvent->getDimuon(j);
            if(!dimuon.fAccepted) continue;
            if(dimuon.fMass > 2.) continue;

            //get track by trackID
            DPMCTrack track = rawEvent->getTrack(dimuon.fPosTrackID);
            if(track.fInitialMom.Vect().Mag() < 20.) continue;

            //loop over all the hits of this track by the hitIDs
            for(std::vector<UInt_t>::iterator iter = track.fHitIDs.begin(); iter != track.fHitIDs.end(); ++iter)
            {
                DPMCHit hit = rawEvent->getHit(*iter);
                if(hit.fDetectorID != 8) continue;

                hist->Fill(hit.fPosition.Y(), hit.fPosition.X());
            }
        }
    }

    hist->Draw();
}


/*
This macro illustrates how to plot the correlation of momentum between a pion-decayed muon
and its mother particle
*/
void pionDecay()
{
    gSystem->Load("libMCRawEvent.so");

    TFile* dataFile = new TFile("output.root", "READ");
    TTree* dataTree = (TTree*)dataFile->Get("save");

    DPMCRawEvent* rawEvent = new DPMCRawEvent;
    dataTree->SetBranchAddress("rawEvent", &rawEvent);

    TH2D* hist = new TH2D("hist", "hist", 50, 0., 100., 50, 0., 100.);
    for(Int_t i = 0; i < dataTree->GetEntries(); ++i)
    {
        dataTree->GetEntry(i);

        for(Int_t j = 0; j < rawEvent->getNTracks(); ++j)
        {
            //get track by trackID
            DPMCTrack track = rawEvent->getTrack(j);
            if(abs(track.fPDGCode) != 13) continue;
            if(abs(track.fParentPDGCode) != 211) continue;

            //get mother particle by parent track ID
            DPMCTrack mother = rawEvent->getTrack(track.fParentID);

            hist->Fill(mother.fInitialMom.Vect().Mag(), track.fInitialMom.Vect().Mag());
        }
    }

    hist->Draw();
}
