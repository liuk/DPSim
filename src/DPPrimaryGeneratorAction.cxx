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
const TVector3 bv_cms = p_cms.BoostVector();
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
    p_vertexGen = DPVertexGenerator::instance();

    particleGun = new G4ParticleGun(1);
    particleDict = G4ParticleTable::GetParticleTable();
    proton = particleDict->FindParticle(2212);
    mup = particleDict->FindParticle(-13);
    mum = particleDict->FindParticle(13);

    pdf = LHAPDF::mkPDF("CT10nlo", 0);

    //initilize all kinds of generators
    dimuonMode = false;
    if(p_config->generator == "DrellYan")
    {
        std::cout << " Using Drell-Yan generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generateDrellYan;
        dimuonMode = true;
    }
    else if(p_config->generator == "JPsi")
    {
        std::cout << " Using JPsi generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generateJPsi;
        dimuonMode = true;
    }
    else if(p_config->generator == "Psip")
    {
        std::cout << " Using Psip generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generatePsip;
        dimuonMode = true;
    }
    else if(p_config->generator == "DarkPhoton")
    {
        std::cout << " Using dark photon generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generateDarkPhoton;
        dimuonMode = true;
    }
    else if(p_config->generator == "PythiaSingle")
    {
        std::cout << " Using pythia single generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generatePythiaSingle;

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
    }
    else if(p_config->generator == "Geant4Single")
    {
        std::cout << " Using geant4 single generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generateGeant4Single;
    }
    else if(p_config->generator == "PhaseSpace")
    {
        std::cout << " Using phase space generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generatePhaseSpace;
        dimuonMode = true;
    }
    else if(p_config->generator == "Custom")
    {
        std::cout << " Using external custom generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generateCustom;

        customInputFile = new TFile(p_config->customInput.Data(), "READ");
        customInputTree = (TTree*)customInputFile->Get("save");

        customPositions = new TClonesArray("TVector3");
        customMomentums = new TClonesArray("TVector3");

        customInputTree->SetBranchAddress("pdg", customParticlePDGs);
        customInputTree->SetBranchAddress("pos", customPositions);
        customInputTree->SetBranchAddress("mom", customMomentums);
    }
    else if(p_config->generator == "Debug")
    {
        std::cout << " Using simple debug generator ..." << endl;
        p_generator = &DPPrimaryGeneratorAction::generateDebug;
    }
}

DPPrimaryGeneratorAction::~DPPrimaryGeneratorAction()
{
    delete pdf;
    delete particleGun;
}

void DPPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    p_config->nEventsThrown++;

    theEvent = anEvent;
    (this->*p_generator)();
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

void DPPrimaryGeneratorAction::generateGeant4Single()
{
    particleGun->SetParticleDefinition(proton);
    particleGun->SetParticlePosition(G4ThreeVector(0., 0., -600*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(0., 0., p_config->beamMomentum*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);
}

void DPPrimaryGeneratorAction::generatePhaseSpace()
{
    DPMCDimuon dimuon;
    double mass = G4UniformRand()*(p_config->massMax - p_config->massMin) + p_config->massMin;
    double xF = G4UniformRand()*(p_config->xfMax - p_config->xfMin) + p_config->xfMin;
    if(!generateDimuon(mass, xF, dimuon)) return;
    dimuon.fVertex = p_vertexGen->generateVertex();

    p_config->nEventsPhysics++;

    particleGun->SetParticleDefinition(mup);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fPosMomentum.X()*GeV, dimuon.fPosMomentum.Y()*GeV, dimuon.fPosMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    particleGun->SetParticleDefinition(mum);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fNegMomentum.X()*GeV, dimuon.fNegMomentum.Y()*GeV, dimuon.fNegMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    dimuon.fPosTrackID = 1;
    dimuon.fNegTrackID = -1;
    p_IOmamnger->fillOneDimuon(1., dimuon);
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

void DPPrimaryGeneratorAction::generateDebug()
{
    particleGun->SetParticleDefinition(mup);
    particleGun->SetParticlePosition(G4ThreeVector(0., 0., -500.*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(0., 0., 50.*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);
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
    else if(p_generator == &DPPrimaryGeneratorAction::generateDrellYan || p_generator == &DPPrimaryGeneratorAction::generatePhaseSpace)
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
    p_dimuon.Boost(bv_cms);
    double masses[2] = {mmu, mmu};
    phaseGen.SetDecay(p_dimuon, 2, masses);

    phaseGen.Generate();
    dimuon.fPosMomentum = *(phaseGen.GetDecay(0));
    dimuon.fNegMomentum = *(phaseGen.GetDecay(1));

    dimuon.calcVariables();
    if(dimuon.fx1 < p_config->x1Min || dimuon.fx1 > p_config->x1Max) return false;
    if(dimuon.fx2 < p_config->x2Min || dimuon.fx2 > p_config->x2Max) return false;
    if(dimuon.fCosTh < p_config->cosThetaMin || dimuon.fCosTh > p_config->cosThetaMax) return false;

    return true;
}
