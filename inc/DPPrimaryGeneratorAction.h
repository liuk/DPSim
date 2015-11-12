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

#include "DPSimConfig.h"
#include "DPIOManager.h"

#define GenBit(n) (1 << (n))

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
    void generatePhaseSpace();
    void generateCustom();

    //Dimuon phase space generator
    bool generateDimuon(double mass, double xF, DPMCDimuon& dimuon);

    //test which generator is selected
    bool generatorSelected(int generator) { return (generatorType & generator) != 0; }

    //Generator type
    enum GenType
    {
        DrellYanGen   = GenBit(0),
        JPsiGen       = GenBit(1),
        PsipGen       = GenBit(2),
        DarkPhotonGen = GenBit(3),
        CustomGen     = GenBit(4),
        PhaseSpace    = GenBit(5),
        PythiaSingleGen = GenBit(6),
    };
    int generatorType;

private:
    //pointer to the configuration
    DPSimConfig* p_config;

    //pointer to the IO manager
    DPIOManager* p_IOmamnger;

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

    //Used for custom input
    TFile* customInputFile;
    TTree* customInputTree;
    int customParticlePDGs[10000];
    TClonesArray* customPositions;
    TClonesArray* customMomentums;
};

#endif
