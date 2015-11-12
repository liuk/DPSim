#include "DPPrimaryGeneratorAction.h"

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TVector3.h>
#include <TLorentzVector.h>

// global parameters
//const double pi = TMath::Pi();
//const double twopi = 2.*pi;

// masses
const double mp = 0.93827;
const double mmu = 0.10566;
const double mjpsi = 3.097;
const double mpsip = 3.686;

// 4-vectors
const double ebeam = 120.;
const TLorentzVector p_beam(0., 0., TMath::Sqrt(ebeam*ebeam - mp*mp), ebeam);
const TLorentzVector p_target(0., 0., 0., mp);
const TLorentzVector p_cms = p_beam + p_target;
const double scms = p_cms.M2();
const double sqrts = p_cms.M();

//distribution-wise constants
const double pT0DY = 2.8;
const double pTpowDY = 1./(6. - 1.);
const double pT0JPsi = 3.0;
const double pTpowJPsi = 1./(6. - 1.);

DPPrimaryGeneratorAction::DPPrimaryGeneratorAction()
{
    p_config = DPSimConfig::instance();
    p_IOmamnger = DPIOManager::instance();

    particleGun = new G4ParticleGun(1);
    particleDict = G4ParticleTable::GetParticleTable();

    pdf = LHAPDF::mkPDF("CT10nlo", 0);

    //Pythia8::RndmEngine* randomEng = new GeantRandom();
    //ppGen.setRndmEnginPtr(randomEng);
    ppGen.readString("PDF:pSet = 7");
    ppGen.readString("PhaseSpace:mHatMin = 0");
    ppGen.readString("ParticleDecays:limitTau = on");
    ppGen.readString("ParticleDecays:limitTau0 = on");
    ppGen.readString("SoftQCD:all = on");
    ppGen.readString("HardQCD:hardccbar = on");

    //pnGen.setRndmEnginPtr(randomEng);
    //pnGen.readString("PDF:pSet = 7");
    pnGen.readString("PhaseSpace:mHatMin = 0");
    pnGen.readString("ParticleDecays:limitTau = on");
    pnGen.readString("ParticleDecays:limitTau0 = on");
    pnGen.readString("SoftQCD:all = on");
    pnGen.readString("HardQCD:hardccbar = on");

    ppGen.init(2212, 2212, 120., 0.);
    pnGen.init(2212, 2112, 120., 0.);

    generatorType = 0;
    if(p_config->generator == "DrellYan") generatorType |= DrellYanGen;
    if(p_config->generator == "JPsi")     generatorType |= JPsiGen;
    if(p_config->generator == "Psip")     generatorType |= PsipGen;
    if(p_config->generator == "DarkPhoton")   generatorType |= DarkPhotonGen;
    if(p_config->generator == "Custom")       generatorType |= CustomGen;
    if(p_config->generator == "PythiaSingle") generatorType |= PythiaSingleGen;

    //initialization part of custom generator
    if(generatorSelected(CustomGen))
    {
        customInputFile = new TFile(p_config->customInput.Data(), "READ");
        customInputTree = (TTree*)customInputFile->Get("save");

        customPositions = new TClonesArray("TVector3");
        customMomentums = new TClonesArray("TVector3");

        customInputTree->SetBranchAddress("pdg", customParticlePDGs);
        customInputTree->SetBranchAddress("pos", customPositions);
        customInputTree->SetBranchAddress("mom", customMomentums);
    }
}

DPPrimaryGeneratorAction::~DPPrimaryGeneratorAction()
{
    delete particleGun;
}

void DPPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    theEvent = anEvent;
    if(generatorSelected(DrellYanGen))
    {
        generateDrellYan();
    }
    else if(generatorSelected(JPsiGen))
    {
        generateJPsi();
    }
    else if(generatorSelected(PsipGen))
    {
        generatePsip();
    }
    else if(generatorSelected(DarkPhotonGen))
    {
        generateDarkPhoton();
    }
    else if(generatorSelected(CustomGen))
    {
        generateCustom();
    }
    else if(generatorSelected(PythiaSingleGen))
    {
        generatePythiaSingle();
    }
}

void DPPrimaryGeneratorAction::generateDrellYan()
{

}

void DPPrimaryGeneratorAction::generateJPsi()
{

}

void DPPrimaryGeneratorAction::generatePsip()
{

}

void DPPrimaryGeneratorAction::generateDarkPhoton()
{

}

void DPPrimaryGeneratorAction::generatePythiaSingle()
{

}

void DPPrimaryGeneratorAction::generatePhaseSpace()
{
    
}

void DPPrimaryGeneratorAction::generateCustom()
{
    int eventID = theEvent->GetEventID();
    customInputTree->GetEntry(eventID);

    for(int i = 0; i < customPositions->GetEntries(); ++i)
    {
        TVector3 pos = *((TVector3*)customPositions->At(i));
        TVector3 mom = *((TVector3*)customMomentums->At(i));

        particleGun->SetParticleDefinition(particleDict->FindParticle(customParticlePDGs[i]));
        particleGun->SetParticlePosition(G4ThreeVector(pos.X()*cm, pos.Y()*cm, pos.Z()*cm));
        particleGun->SetParticleMomentum(G4ThreeVector(mom.X()*GeV, mom.Y()*GeV, mom.Z()*GeV));
        particleGun->GeneratePrimaryVertex(theEvent);
    }
}

bool DPPrimaryGeneratorAction::generateDimuon(double mass, double xF, DPMCDimuon& dimuon)
{
    double pz = xF*(sqrts - mass*mass/sqrts)/2.;

    double pTmaxSq = (scms*scms*(1. - xF*xF) - 2.*scms*mass*mass + mass*mass*mass*mass)/scms/4.;
    if(pTmaxSq < 0.) return false;

    double pTmax = sqrt(pTmaxSq);
    double pT = 10.;
    if(pTmax < 0.3)
    {
        pT = pTmax*sqrt(G4UniformRand());
    }
    else if(generatorSelected(DrellYanGen))
    {
        while(pT > pTmax) pT = pT0DY*sqrt(1./pow(G4UniformRand(), pTpowDY) - 1.);
    }
    else
    {
        while(pT > pTmax) pT = pT0JPsi*sqrt(1./pow(G4UniformRand(), pTpowJPsi) - 1.);
    }

    double phi = G4UniformRand()*twopi;
    double px = pT*TMath::Cos(phi);
    double py = pT*TMath::Sin(phi);

    //configure phase space generator
    TLorentzVector p_dimuon;
    p_dimuon.SetXYZM(px, py, pz, mass);
    double masses[2] = {mmu, mmu};
    phaseGen.SetDecay(p_dimuon, 2, masses);

    phaseGen.Generate();
    dimuon.fPosMomentum = *(phaseGen.GetDecay(0));
    dimuon.fNegMomentum = *(phaseGen.GetDecay(1));

    dimuon.calcVariables();
    if(dimuon.fx1 < p_config->x1Min || dimuon.fx1 > p_config->x1Max) return false;
    if(dimuon.fx2 < p_config->x2Min || dimuon.fx2 > p_config->x2Max) return false;

    return true;
}
