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
#include "DPSimConfig.h"

using namespace std;
using namespace Pythia8;

//All tracks passed to this function are muons
bool keepTrack(double decayLength, double z0, TVector3 mom)
{
    double Lint = 20.42; //this is the pion interaction length in iron

    //See if the pion would be absorbed in beam dump
    double lengthInIron = z0 + decayLength - 25.4;   //there is a hole on the beam dump
    if(G4UniformRand() > TMath::Exp(-lengthInIron/Lint)) return false;

    //see if the muon has enough energy to penetrate beam dump
    double remainingLengthInIron = lengthInIron > 0 ? 502. - lengthInIron : 502.;
    if(mom.Mag()/remainingLengthInIron < 0.01) return false;

    //see if the opening angle is within acceptance
    if(fabs(mom.Px()/mom.Pz()) > 0.25 || fabs(mom.Py()/mom.Pz()) > 0.15) return false;

    return true;
}

int main(int argc, char* argv[])
{
    //Initialize the configuration
    DPSimConfig* p_config = DPSimConfig::instance();
    p_config->targetInBeam = true;
    p_config->dumpInBeam = true;
    p_config->instruInBeam = true;
    p_config->airInBeam = true;
    //p_config->init(argv[1]);

    //set random seed
    CLHEP::HepRandom::setTheSeed(p_config->seed);

    //General Initialization
    G4RunManager* runManager = new G4RunManager;
    runManager->SetUserInitialization(new DPDetectorConstruction);

    /* NOTE: let's see if this works
    G4PhysListFactory factory;
    runManager->SetUserInitialization(factory.GetReferencePhysList("FTFP_BERT_EMX"));

    //User actions
    DPPrimaryGeneratorAction* primaryGenerator = new DPPrimaryGeneratorAction;
    runManager->SetUserAction(primaryGenerator);

    runManager->SetUserAction(new DPRunAction);
    runManager->SetUserAction(new DPEventAction);
    runManager->SetUserAction(new DPTrackingAction);
    runManager->SetUserAction(new DPSteppingAction);

    runManager->SetRunIDCounter(p_config->seed);   //use seed as the runID
    */
    runManager->Initialize();

    //Initialize Output file
    int eventID;
    int n;
    int pdg[10000];
    TClonesArray* p_pos = new TClonesArray("TVector3");  p_pos->BypassStreamer(); TClonesArray& pos = *p_pos;
    TClonesArray* p_mom = new TClonesArray("TVector3");  p_mom->BypassStreamer(); TClonesArray& mom = *p_mom;

    TFile* saveFile = new TFile(argv[1], "recreate");
    TTree* saveTree = new TTree("save", "save");

    saveTree->Branch("eventID", &eventID, "eventID/I");
    saveTree->Branch("n", &n, "n/I");
    saveTree->Branch("pdg", &pdg, "pdg[n]/I");
    saveTree->Branch("pos", &pos, 256000, 99);
    saveTree->Branch("mom", &mom, 256000, 99);

    //Initialize pythia for pp and pn collisions
    Pythia ppGen;
    ppGen.readFile(argv[2]);
    ppGen.readString(Form("Random:seed = %d", atoi(argv[3])));
    ppGen.readString("Beams:idB = 2212");
    ppGen.init();

    Pythia pnGen;
    pnGen.readFile(argv[2]);
    pnGen.readString(Form("Random:seed = %d", atoi(argv[3])+1));
    pnGen.readString("Beams:idB = 2112");
    pnGen.init();

    //Initialize vertex generator
    DPVertexGenerator* p_vertexGen = DPVertexGenerator::instance();
    p_vertexGen->init();

    //Main loop
    int nEvents = atoi(argv[4]);
    for(int i = 0; i < nEvents; ++i)
    {
        double zvtx = p_vertexGen->generateVertex();
        double pARatio = p_vertexGen->getPARatio();

        Pythia* p_pythia = G4UniformRand() < pARatio ? &ppGen : &pnGen;
        Event& events = p_pythia->event;
        while(!p_pythia->next()) {}

        n = 0;
        eventID = i;
        int nParticles = 0;
        for(int j = 1; j < nParticles; ++j)
        {
            if(abs(events[j].id()) == 13)
            {
                if(!keepTrack(events[j].zProd()/10., zvtx, TVector3(events[j].px(), events[j].py(), events[j].pz()))) continue;

                //Fill muon
                pdg[n] = events[j].id();
                new(mom[n]) TVector3(events[j].px(), events[j].py(), events[j].pz());
                new(pos[n]) TVector3(events[j].xProd()/10., events[j].yProd()/10., events[j].zProd()/10. + zvtx);
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

        if(n > 0)
        {
            saveTree->Fill();
            mom.Clear();
            pos.Clear();
        }
    }

    //finalize
    saveFile->cd();
    saveTree->Write();
    saveFile->Close();

    return EXIT_SUCCESS;
}
