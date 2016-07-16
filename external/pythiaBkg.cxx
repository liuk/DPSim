#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "Randomize.hh"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TClonesArray.h>

#include "Pythia8/Pythia.h"

#include "DPDetectorConstruction.h"
#include "DPPrimaryGeneratorAction.h"
#include "DPRunAction.h"
#include "DPEventAction.h"
#include "DPTrackingAction.h"
#include "DPSteppingAction.h"
#include "DPVertexGenerator.h"
#include "DPSimConfig.h"

using namespace std;
using namespace Pythia8;

//All tracks passed to this function are muons, and the mother
//particles are pions
bool keepTrack(double decayLength, double z0, int parentID, TVector3 mom)
{
    double PIint = 20.42;       //this is the pion interaction length in iron
    double Kint = 25.5;        //this is the Kaon interaction length in iron
    double dumpLength = 502.;  //this is the total length of beam dump
    double dumpHoleLen = 25.4; //this is the depth of the hole on dump face

    //See if the pion/kaon would be absorbed in beam dump
    double lengthInIron = z0 + decayLength - dumpHoleLen;   //there is a hole on the beam dump
    if(abs(parentID) == 211 || abs(parentID) == 321 || abs(parentID) == 130)
    {
        double Lint = abs(parentID) == 211 ? PIint : Kint;
        if(G4UniformRand() > TMath::Exp(-lengthInIron/Lint)) return false;
    }

    //see if the muon has enough energy to penetrate beam dump, conservative cut > 0.01GeV/cm
    double remainingLengthInIron = lengthInIron > 0 ? dumpLength - lengthInIron : dumpLength;
    if(mom.Mag()/remainingLengthInIron < 0.01) return false;

    //see if the opening angle is within acceptance
    if(fabs(mom.Px()/mom.Pz()) > 0.25 || fabs(mom.Py()/mom.Pz()) > 0.15) return false;

    return true;
}

int main(int argc, char* argv[])
{
    //Initialize the configuration
    DPSimConfig* p_config = DPSimConfig::instance();
    p_config->init(argv[1]);

    //Initialize vertex generator
    DPVertexGenerator* p_vertexGen = DPVertexGenerator::instance();
    p_vertexGen->init();

    //Initialize Output file
    int eventID;
    int n;
    int pdg[10000];
    TClonesArray* p_pos = new TClonesArray("TVector3");  p_pos->BypassStreamer(); TClonesArray& pos = *p_pos;
    TClonesArray* p_mom = new TClonesArray("TVector3");  p_mom->BypassStreamer(); TClonesArray& mom = *p_mom;

    TFile* saveFile = new TFile(p_config->outputFileName, "recreate");
    TTree* saveTree = new TTree("save", "save");

    saveTree->Branch("eventID", &eventID, "eventID/I");
    saveTree->Branch("n", &n, "n/I");
    saveTree->Branch("pdg", pdg, "pdg[n]/I");
    saveTree->Branch("pos", &p_pos, 256000, 99);
    saveTree->Branch("mom", &p_mom, 256000, 99);

    //Initialize pythia for pp and pn collisions
    Pythia ppGen;
    ppGen.readFile(p_config->pythiaConfig.Data());
    ppGen.readString(Form("Random:seed = %d", p_config->seed));
    ppGen.readString("Beams:idB = 2212");
    ppGen.init();

    Pythia pnGen;
    pnGen.readFile(p_config->pythiaConfig.Data());
    pnGen.readString(Form("Random:seed = %d", p_config->seed));
    pnGen.readString("Beams:idB = 2112");
    pnGen.init();

    //Main loop
    int nEvents = p_config->nEvents;
    for(int i = 0; i < nEvents; ++i)
    {
        TVector3 vtx = p_vertexGen->generateVertex();
        double pARatio = p_vertexGen->getPARatio();

        Pythia* p_pythia = G4UniformRand() < pARatio ? &ppGen : &pnGen;
        Event& events = p_pythia->event;
        while(!p_pythia->next()) {}

        n = 0;
        eventID = i;
        int nParticles = events.size();
        for(int j = 1; j < nParticles; ++j)
        {
            if(abs(events[j].id()) == 13)
            {
                if(!keepTrack(events[j].zProd()/10., vtx.Z(), events[events[j].mother1()].id(), TVector3(events[j].px(), events[j].py(), events[j].pz()))) continue;

                //Fill muon
                pdg[n] = events[j].id();
                new(mom[n]) TVector3(events[j].px(), events[j].py(), events[j].pz());
                new(pos[n]) TVector3(events[j].xProd()/10. + vtx.X(), events[j].yProd()/10. + vtx.Y(), events[j].zProd()/10. + vtx.Z());
                ++n;

                /*
                //If mother particle exists, fill mother as well
                int motherID = events[j].mother1();
                if(motherID < 1 || motherID >= nParticles) continue;
                if(abs(events[motherID].id()) < 10) continue;
                pdg[n] = events[motherID].id();
                new(mom[n]) TVector3(events[motherID].px(), events[motherID].py(), events[motherID].pz());
                new(pos[n]) TVector3(events[motherID].xProd()/10., events[motherID].yProd()/10., events[motherID].zProd()/10. + zvtx);
                ++n;*/
            }
        }

        if(n == 0) continue;

        saveTree->Fill();
        mom.Clear();
        pos.Clear();
    }

    //finalize
    saveFile->cd();
    saveTree->Write();
    saveFile->Close();

    return EXIT_SUCCESS;
}
