#ifndef DPPrimaryGeneratorAction_H
#define DPPrimaryGeneratorAction_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4String.hh"

#include "LHAPDF/LHAPDF.h"
#include "Pythia8/Pythia.h"

#include <TTree.h>
#include <TFile.h>
#include <TClonesArray.h>
#include <TGenPhaseSpace.h>
#include <TVector3.h>

#include "DPSimConfig.h"
#include "DPIOManager.h"
#include "DPVertexGenerator.h"

class DPPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
    DPPrimaryGeneratorAction();
    ~DPPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* anEvent);

public:
    //Various generators
    void generateDrellYan();
    void generateJPsi();
    void generatePsip();
    void generateDarkPhoton();
    void generatePythiaSingle();
    void generateGeant4Single();
    void generatePhaseSpace();
    void generateCustom();
    void generateDebug();

    //Dimuon phase space generator
    bool generateDimuon(double mass, double xF, DPMCDimuon& dimuon);

    //test if in dimuon mode
    bool isInDimuonMode() { return dimuonMode; }

private:
    //pointer to the configuration
    DPSimConfig* p_config;

    //pointer to the IO manager
    DPIOManager* p_IOmamnger;

    //pointer to the vertex generator
    DPVertexGenerator* p_vertexGen;

    //pointer to the current G4Event
    G4Event* theEvent;

    //particle gun
    G4ParticleGun* particleGun;

    //Pointer to the particle table
    G4ParticleTable* particleDict;

    //ROOT phase space generator
    TGenPhaseSpace phaseGen;

    //PDFs
    LHAPDF::PDF*pdf;

    //Pythia generator
    Pythia8::Pythia ppGen;
    Pythia8::Pythia pnGen;

    //pointer to the real generator
    typedef void (DPPrimaryGeneratorAction::*GenPtr)();
    GenPtr p_generator;
    bool dimuonMode;

    //Used for custom input
    TFile* customInputFile;
    TTree* customInputTree;
    int customParticlePDGs[10000];
    TClonesArray* customPositions;
    TClonesArray* customMomentums;

    //Common particles to save time
    G4ParticleDefinition* proton;
    G4ParticleDefinition* mup;
    G4ParticleDefinition* mum;
};

#endif
