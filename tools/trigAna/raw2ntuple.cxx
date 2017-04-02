#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>

#include "DPMCRawEvent.h"

using namespace std;

int main(int argc, char* argv[])
{
    TFile* dataFile = new TFile(argv[1], "READ");
    TTree* dataTree = (TTree*)dataFile->Get("save");

    DPMCRawEvent* rawEvent = new DPMCRawEvent;
    dataTree->SetBranchAddress("rawEvent", &rawEvent);

    double zvtx;
    int quadrant, quad1, quad2;
    int y1, y2, y41, y42;
    int x1, x2, x3, x4;
    double ry1, ry2, ry41, ry42;
    double rx1, rx2, rx3, rx4;

    TFile* saveFile = new TFile(argv[2], "recreate");
    TTree* saveTree = new TTree("save", "save");

    saveTree->Branch("zvtx", &zvtx);
    saveTree->Branch("quadrant", &quadrant);
    saveTree->Branch("y1", &y1);
    saveTree->Branch("y2", &y2);
    saveTree->Branch("y41", &y41);
    saveTree->Branch("y42", &y42);
    saveTree->Branch("x1", &x1);
    saveTree->Branch("x2", &x2);
    saveTree->Branch("x3", &x3);
    saveTree->Branch("x4", &x4);
    saveTree->Branch("ry1", &ry1);
    saveTree->Branch("ry2", &ry2);
    saveTree->Branch("ry41", &ry41);
    saveTree->Branch("ry42", &ry42);
    saveTree->Branch("rx1", &rx1);
    saveTree->Branch("rx2", &rx2);
    saveTree->Branch("rx3", &rx3);
    saveTree->Branch("rx4", &rx4);

    for(Int_t i = 0; i < dataTree->GetEntries(); ++i)
    {
        dataTree->GetEntry(i);
        if(i % 1000 == 0) cout << i << "  " << saveTree->GetEntries() << endl;

        for(Int_t j = 0; j < rawEvent->getNTracks(); ++j)
        {
            //get track by trackID
            DPMCTrack track = rawEvent->getTrack(j);
            if(abs(track.fPDGCode) != 13) continue;
            if(!track.isAccepted()) continue;

            //check if it hits triggering station as well
            zvtx = track.fInitialPos.Z();
            quadrant = -1;
            y1 = -1;
            y2 = -1;
            y41 = -1;
            y42 = -1;
            x1 = -1;
            x2 = -1;
            x3 = -1;
            x4 = -1;

            for(std::vector<UInt_t>::iterator iter = track.fHitIDs.begin(); iter != track.fHitIDs.end(); ++iter)
            {
            	DPMCHit hit = rawEvent->getHit(*iter);
            	//cout << hit.fDetectorID << endl;
            	if(hit.fDetectorID >= 49 && hit.fDetectorID <= 52)
            	{
            		y1 = hit.uniqueID();
                    ry1 = hit.fPosition.Y();
            		quad1 = hit.fDetectorID - 49;
            	}
            	else if(hit.fDetectorID >= 53 && hit.fDetectorID <= 56)
            	{
            		y2 = hit.uniqueID();
                    ry2 = hit.fPosition.Y();
            		quad2 = hit.fDetectorID - 53;
            	}
            	else if(hit.fDetectorID == 35 || hit.fDetectorID == 36)
            	{
            		y41 = hit.uniqueID();
                    ry41 = hit.fPosition.Y();
            	}
                else if(hit.fDetectorID == 37 || hit.fDetectorID == 38)
                {
                    y42 = hit.uniqueID();
                    ry42 = hit.fPosition.Y();
                }
                else if(hit.fDetectorID == 25 || hit.fDetectorID == 26)
                {
                    x1 = hit.uniqueID();
                    rx1 = hit.fPosition.Y();
                }
                else if(hit.fDetectorID == 31 || hit.fDetectorID == 32)
                {
                    x2 = hit.uniqueID();
                    rx2 = hit.fPosition.Y();
                }
                else if(hit.fDetectorID == 33 || hit.fDetectorID == 34)
                {
                    x3 = hit.uniqueID();
                    rx3 = hit.fPosition.Y();
                }
                else if(hit.fDetectorID == 39 || hit.fDetectorID == 40)
                {
                    x4 = hit.uniqueID();
                    rx4 = hit.fPosition.Y();
                }
            }
            quadrant = quad1;

            //cout << y1 << "  " << y2 << "  " << y4 << "  " << quar1 << "  " << quar2 << endl;
            if(y1 > 0 && y2 > 0 && y41 > 0 && y42 > 0 && quad1 == quad2) saveTree->Fill();
        }
    }

    saveFile->cd();
    saveTree->Write();
    saveFile->Close();

    return 0;
}
