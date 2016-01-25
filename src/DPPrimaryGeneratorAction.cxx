#include "DPPrimaryGeneratorAction.h"

#include <fstream>
#include <string>

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TVector3.h>
#include <TLorentzVector.h>

namespace DPGEN
{
    // global parameters
    const double pi = TMath::Pi();
    const double twopi = 2.*pi;
    const double sqrt2pi = TMath::Sqrt(twopi);

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
    const double s = p_cms.M2();
    const double sqrts = p_cms.M();

    //distribution-wise constants
    const double pT0DY = 2.8;
    const double pTpowDY = 1./(6. - 1.);
    const double pT0JPsi = 3.0;
    const double pTpowJPsi = 1./(6. - 1.);

    //charmonium generation constants  Ref: Schub et al Phys Rev D 52, 1307 (1995)
    const double sigmajpsi = 0.2398;    //Jpsi xf gaussian width
    const double brjpsi = 0.0594;       //Br(Jpsi -> mumu)
    const double ajpsi = 0.001464*TMath::Exp(-16.66*mjpsi/sqrts);
    const double bjpsi = 2.*sigmajpsi*sigmajpsi;

    const double psipscale = 0.019;     //psip relative to jpsi
}

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

    //TODO: need to find a way to pass the random number seed to pythia as well

    //initilize all kinds of generators
    if(p_config->generatorType == "dimuon")
    {
        if(p_config->generatorEng == "legacyDY")
        {
            std::cout << " Using legacy Drell-Yan generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generateDrellYan;
        }
        else if(p_config->generatorEng == "legacyJPsi")
        {
            std::cout << " Using legacy JPsi generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generateJPsi;
        }
        else if(p_config->generatorEng == "legacyPsip")
        {
            std::cout << " Using Psip generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generatePsip;
        }
        else if(p_config->generatorEng == "PHSP")
        {
            std::cout << " Using phase space generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generatePhaseSpace;
        }
        else if(p_config->generatorEng == "pythia")
        {
            std::cout << " Using pythia pythia generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generatePythiaDimuon;

            ppGen.readFile(p_config->pythiaConfig.Data());
            pnGen.readFile(p_config->pythiaConfig.Data());

            ppGen.init(2212, 2212, 120., 0.);
            pnGen.init(2212, 2112, 120., 0.);
        }
        else if(p_config->generatorEng == "DarkPhotonFromEta")
        {
            std::cout << " Using dark photon generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generateDarkPhotonFromEta;

            ppGen.readFile(p_config->pythiaConfig.Data());
            pnGen.readFile(p_config->pythiaConfig.Data());

            ppGen.init(2212, 2212, 120., 0.);
            pnGen.init(2212, 2112, 120., 0.);
        }
        else if(p_config->generatorEng == "custom")
        {
            std::cout << " Using custom LUT dimuon generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generateCustomDimuon;

            //read and parse the lookup table
            std::ifstream fin(p_config->customLUT.Data());
            std::cout << " Initializing custom dimuon cross section from LUT " << p_config->customLUT << std::endl;

            //Load the range and number of bins in each dimension
            std::string line;
            int n;
            double m_min, m_max, xF_min, xF_max;
            double m_bin, xF_bin;

            getline(fin, line);
            stringstream ss(line);
            ss >> n >> m_min >> m_max >> m_bin >> xF_min >> xF_max >> xF_bin;

            //test if the range is acceptable
            if(p_config->massMin < m_min || p_config->massMax > m_max || p_config->xfMin < xF_min || p_config->xfMax > xF_max)
            {
                std::cout << " ERROR: the specified phase space limits are larger than LUT limits!" << std::endl;
                exit(EXIT_FAILURE);
            }

            lut = new TGraph2D(n);
            int nPoints = 0;
            while(getline(fin, line))
            {
                double mass, xF, xsec;

                stringstream ss(line);
                ss >> mass >> xF >> xsec;
                xsec *= (m_bin*xF_bin);

                lut->SetPoint(nPoints++, mass, xF, xsec);
            }
        }
        else
        {
            std::cout << "ERROR: Generator engine is not set or ncd /seaot supported in dimuon mode" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if(p_config->generatorType == "single")
    {
        if(p_config->generatorEng == "pythia")
        {
            std::cout << " Using pythia single generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generatePythiaSingle;

            ppGen.readFile(p_config->pythiaConfig.Data());
            pnGen.readFile(p_config->pythiaConfig.Data());

            ppGen.init(2212, 2212, 120., 0.);
            pnGen.init(2212, 2112, 120., 0.);
        }
        else if(p_config->generatorEng == "geant")
        {
            std::cout << " Using geant4 single generator ..." << std::endl;
            p_generator = &DPPrimaryGeneratorAction::generateGeant4Single;
        }
        else
        {
            std::cout << "ERROR: Generator engine is not set or not supported in single mode" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if(p_config->generatorType == "external")
    {
        std::cout << " Using external generator ..." << std::endl;
        p_generator = &DPPrimaryGeneratorAction::generateExternal;

        externalInputFile = new TFile(p_config->externalInput.Data(), "READ");
        externalInputTree = (TTree*)externalInputFile->Get("save");

        externalPositions = new TClonesArray("TVector3");
        externalMomentums = new TClonesArray("TVector3");

        externalInputTree->SetBranchAddress("n", &nExternalParticles);
        externalInputTree->SetBranchAddress("pdg", externalParticlePDGs);
        externalInputTree->SetBranchAddress("pos", externalPositions);
        externalInputTree->SetBranchAddress("mom", externalMomentums);
    }
    else if(p_config->generatorType == "Debug")
    {
        std::cout << " Using simple debug generator ..." << std::endl;
        p_generator = &DPPrimaryGeneratorAction::generateDebug;
    }
    else
    {
        std::cout << "ERROR: Generator type not recognized! Will exit.";
        exit(EXIT_FAILURE);
    }
}

DPPrimaryGeneratorAction::~DPPrimaryGeneratorAction()
{
    delete pdf;
    delete particleGun;
    if(p_config->generatorType == "custom") delete lut;
}

void DPPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    p_config->nEventsThrown++;

    theEvent = anEvent;
    (this->*p_generator)();
}

void DPPrimaryGeneratorAction::generateDrellYan()
{
    DPMCDimuon dimuon;
    double mass = G4UniformRand()*(p_config->massMax - p_config->massMin) + p_config->massMin;
    double xF = G4UniformRand()*(p_config->xfMax - p_config->xfMin) + p_config->xfMin;
    if(!generateDimuon(mass, xF, dimuon)) return;
    p_vertexGen->generateVertex(dimuon);

    p_config->nEventsPhysics++;

    particleGun->SetParticleDefinition(mup);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fPosMomentum.X()*GeV, dimuon.fPosMomentum.Y()*GeV, dimuon.fPosMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    particleGun->SetParticleDefinition(mum);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fNegMomentum.X()*GeV, dimuon.fNegMomentum.Y()*GeV, dimuon.fNegMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    //calculate the cross section
    //PDF-related
    double zOverA = p_vertexGen->getPARatio();
    double nOverA = 1. - zOverA;

    double dbar1 = pdf->xfxQ(-1, dimuon.fx1, dimuon.fMass)/dimuon.fx1;
    double ubar1 = pdf->xfxQ(-2, dimuon.fx1, dimuon.fMass)/dimuon.fx1;
    double d1 = pdf->xfxQ(1, dimuon.fx1, dimuon.fMass)/dimuon.fx1;
    double u1 = pdf->xfxQ(2, dimuon.fx1, dimuon.fMass)/dimuon.fx1;
    double s1 = pdf->xfxQ(3, dimuon.fx1, dimuon.fMass)/dimuon.fx1;
    double c1 = pdf->xfxQ(4, dimuon.fx1, dimuon.fMass)/dimuon.fx1;

    double dbar2 = pdf->xfxQ(-1, dimuon.fx2, dimuon.fMass)/dimuon.fx2;
    double ubar2 = pdf->xfxQ(-2, dimuon.fx2, dimuon.fMass)/dimuon.fx2;
    double d2 = pdf->xfxQ(1, dimuon.fx2, dimuon.fMass)/dimuon.fx2;
    double u2 = pdf->xfxQ(2, dimuon.fx2, dimuon.fMass)/dimuon.fx2;
    double s2 = pdf->xfxQ(3, dimuon.fx2, dimuon.fMass)/dimuon.fx2;
    double c2 = pdf->xfxQ(4, dimuon.fx2, dimuon.fMass)/dimuon.fx2;

    double xsec_pdf = 4./9.*(u1*(zOverA*ubar2 + nOverA*dbar2) + ubar1*(zOverA*u2 + nOverA*d2) + 2*c1*c2) +
                  1./9.*(d1*(zOverA*dbar2 + nOverA*ubar2) + dbar1*(zOverA*d2 + nOverA*u2) + 2*s1*s2);

    //KFactor
    double xsec_kfactor = 1.;
    if(dimuon.fMass < 2.5)
    {
        xsec_kfactor = 1.25;
    }
    else if(dimuon.fMass < 7.5)
    {
        xsec_kfactor = 1.25 + (1.82 - 1.25)*(dimuon.fMass - 2.5)/5.;
    }
    else
    {
        xsec_kfactor = 1.82;
    }

    //phase space
    double xsec_phsp = dimuon.fx1*dimuon.fx2/(dimuon.fx1 + dimuon.fx2)/dimuon.fMass/dimuon.fMass/dimuon.fMass;

    //generation limitation
    double xsec_limit = (p_config->massMax - p_config->massMin)*(p_config->xfMax - p_config->xfMin)*
                        (p_config->cosThetaMax*p_config->cosThetaMax*p_config->cosThetaMax/3. + p_config->cosThetaMax
                         - p_config->cosThetaMin*p_config->cosThetaMin*p_config->cosThetaMin/3. - p_config->cosThetaMin)*4./3.;

    double xsec = xsec_pdf*xsec_kfactor*xsec_phsp*xsec_limit*p_vertexGen->getLuminosity();

    dimuon.fPosTrackID = 1;
    dimuon.fNegTrackID = 2;
    p_IOmamnger->fillOneDimuon(xsec, dimuon);
}

void DPPrimaryGeneratorAction::generateJPsi()
{
    DPMCDimuon dimuon;
    double xF = G4UniformRand()*(p_config->xfMax - p_config->xfMin) + p_config->xfMin;
    if(!generateDimuon(DPGEN::mjpsi, xF, dimuon)) return;
    p_vertexGen->generateVertex(dimuon);

    p_config->nEventsPhysics++;

    particleGun->SetParticleDefinition(mup);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fPosMomentum.X()*GeV, dimuon.fPosMomentum.Y()*GeV, dimuon.fPosMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    particleGun->SetParticleDefinition(mum);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fNegMomentum.X()*GeV, dimuon.fNegMomentum.Y()*GeV, dimuon.fNegMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    //calculate the cross section
    //xf distribution
    double xsec_xf = DPGEN::ajpsi*TMath::Exp(-dimuon.fxF*dimuon.fxF/DPGEN::bjpsi)/(DPGEN::sigmajpsi*DPGEN::sqrt2pi);

    //generation limitation
    double xsec_limit = p_config->xfMax - p_config->xfMin;

    double xsec = DPGEN::brjpsi*xsec_xf*xsec_limit*p_vertexGen->getLuminosity();

    dimuon.fPosTrackID = 1;
    dimuon.fNegTrackID = 2;
    p_IOmamnger->fillOneDimuon(xsec, dimuon);
}

void DPPrimaryGeneratorAction::generatePsip()
{
    DPMCDimuon dimuon;
    double xF = G4UniformRand()*(p_config->xfMax - p_config->xfMin) + p_config->xfMin;
    if(!generateDimuon(DPGEN::mpsip, xF, dimuon)) return;
    p_vertexGen->generateVertex(dimuon);

    p_config->nEventsPhysics++;

    particleGun->SetParticleDefinition(mup);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fPosMomentum.X()*GeV, dimuon.fPosMomentum.Y()*GeV, dimuon.fPosMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    particleGun->SetParticleDefinition(mum);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fNegMomentum.X()*GeV, dimuon.fNegMomentum.Y()*GeV, dimuon.fNegMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    //calculate the cross section
    //xf distribution
    double xsec_xf = DPGEN::ajpsi*TMath::Exp(-dimuon.fxF*dimuon.fxF/DPGEN::bjpsi)/(DPGEN::sigmajpsi*DPGEN::sqrt2pi);

    //generation limitation
    double xsec_limit = p_config->xfMax - p_config->xfMin;

    double xsec = DPGEN::psipscale*DPGEN::brjpsi*xsec_xf*xsec_limit*p_vertexGen->getLuminosity();

    dimuon.fPosTrackID = 1;
    dimuon.fNegTrackID = 2;
    p_IOmamnger->fillOneDimuon(xsec, dimuon);
}

void DPPrimaryGeneratorAction::generateDarkPhotonFromEta()
{
    p_vertexGen->generateVertex();
    double pARatio = p_vertexGen->getPARatio();

    Pythia8::Pythia* p_pythia = G4UniformRand() < pARatio ? &ppGen : &pnGen;
    while(!p_pythia->next()) {}

    int nEtas = 0;
    Pythia8::Event& particles = p_pythia->event;
    for(int i = 1; i < particles.size(); ++i)
    {
        if(particles[i].id() == 221)
        {
            DPMCDimuon dimuon;
            dimuon.fVertex.SetXYZ(G4RandGauss::shoot(0., 1.5), G4RandGauss::shoot(0., 1.5), G4UniformRand()*(620. - 300.) + 300.);

            //eta -> gamma A'
            TLorentzVector p_eta(particles[i].px(), particles[i].py(), particles[i].pz(), particles[i].e());

            double mass_eta_decays[2] = {G4UniformRand()*(p_eta.M() - 2.*DPGEN::mmu) + 2.*DPGEN::mmu, 0.};
            phaseGen.SetDecay(p_eta, 2, mass_eta_decays);
            phaseGen.Generate();
            TLorentzVector p_AP = *(phaseGen.GetDecay(0));

            //A' -> mumu
            double mass_AP_decays[2] = {DPGEN::mmu, DPGEN::mmu};
            phaseGen.SetDecay(p_AP, 2, mass_AP_decays);

            phaseGen.Generate();
            dimuon.fPosMomentum = *(phaseGen.GetDecay(0));
            dimuon.fNegMomentum = *(phaseGen.GetDecay(1));
            dimuon.calcVariables();

            particleGun->SetParticleDefinition(mup);
            particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
            particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fPosMomentum.X()*GeV, dimuon.fPosMomentum.Y()*GeV, dimuon.fPosMomentum.Z()*GeV));
            particleGun->GeneratePrimaryVertex(theEvent);

            particleGun->SetParticleDefinition(mum);
            particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
            particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fNegMomentum.X()*GeV, dimuon.fNegMomentum.Y()*GeV, dimuon.fNegMomentum.Z()*GeV));
            particleGun->GeneratePrimaryVertex(theEvent);

            //add to the IO stream
            dimuon.fPosTrackID = nEtas*2 - 1;
            dimuon.fNegTrackID = nEtas*2;
            p_IOmamnger->fillOneDimuon(1., dimuon);

            ++nEtas;
        }
    }
}

void DPPrimaryGeneratorAction::generateCustomDimuon()
{
    DPMCDimuon dimuon;
    double mass = G4UniformRand()*(p_config->massMax - p_config->massMin) + p_config->massMin;
    double xF = G4UniformRand()*(p_config->xfMax - p_config->xfMin) + p_config->xfMin;
    if(!generateDimuon(mass, xF, dimuon)) return;
    p_vertexGen->generateVertex(dimuon);

    p_config->nEventsPhysics++;

    particleGun->SetParticleDefinition(mup);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fPosMomentum.X()*GeV, dimuon.fPosMomentum.Y()*GeV, dimuon.fPosMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    particleGun->SetParticleDefinition(mum);
    particleGun->SetParticlePosition(G4ThreeVector(dimuon.fVertex.X()*cm, dimuon.fVertex.Y()*cm, dimuon.fVertex.Z()*cm));
    particleGun->SetParticleMomentum(G4ThreeVector(dimuon.fNegMomentum.X()*GeV, dimuon.fNegMomentum.Y()*GeV, dimuon.fNegMomentum.Z()*GeV));
    particleGun->GeneratePrimaryVertex(theEvent);

    //calculate the cross section
    double xsec = lut->Interpolate(mass, xF)*p_vertexGen->getLuminosity();

    dimuon.fPosTrackID = 1;
    dimuon.fNegTrackID = 2;
    p_IOmamnger->fillOneDimuon(xsec, dimuon);
}

void DPPrimaryGeneratorAction::generatePythiaDimuon()
{
    p_config->nEventsPhysics++;

    DPMCDimuon dimuon;
    double zvtx = p_vertexGen->generateVertex();
    double pARatio = p_vertexGen->getPARatio();

    Pythia8::Pythia* p_pythia = G4UniformRand() < pARatio ? &ppGen : &pnGen;
    while(!p_pythia->next()) {}

    int pParID = 0;
    for(int i = 1; i < p_pythia->event.size(); ++i)
    {
        Pythia8::Particle par = p_pythia->event[i];
        if(par.status() > 0 && par.id() != 22)
        {
            particleGun->SetParticleDefinition(particleDict->FindParticle(par.id()));
            particleGun->SetParticlePosition(G4ThreeVector(par.xProd()*mm, par.yProd()*mm, par.zProd()*mm + zvtx*cm));
            particleGun->SetParticleMomentum(G4ThreeVector(par.px()*GeV, par.py()*GeV, par.pz()*GeV));
            particleGun->GeneratePrimaryVertex(theEvent);

            ++pParID;
            if(par.id() == -13)
            {
                dimuon.fPosTrackID = pParID;
                dimuon.fPosMomentum.SetXYZM(par.px(), par.py(), par.pz(), DPGEN::mmu);
                dimuon.fVertex.SetXYZ(par.xProd()/10., par.yProd()/10., par.zProd()/10. + zvtx);
            }
            else if(par.id() == 13)
            {
                dimuon.fNegTrackID = pParID;
                dimuon.fNegMomentum.SetXYZM(par.px(), par.py(), par.pz(), DPGEN::mmu);
                dimuon.fVertex.SetXYZ(par.xProd()/10., par.yProd()/10., par.zProd()/10. + zvtx);
            }
        }
    }

    p_IOmamnger->fillOneDimuon(1., dimuon);
}

void DPPrimaryGeneratorAction::generatePythiaSingle()
{
    p_config->nEventsPhysics++;

    double zvtx = p_vertexGen->generateVertex();
    double pARatio = p_vertexGen->getPARatio();

    Pythia8::Pythia* p_pythia = G4UniformRand() < pARatio ? &ppGen : &pnGen;
    while(!p_pythia->next()) {}

    for(int j = 1; j < p_pythia->event.size(); ++j)
    {
        Pythia8::Particle par = p_pythia->event[j];
        if(par.status() > 0 && par.id() != 22)
        {
            particleGun->SetParticleDefinition(particleDict->FindParticle(par.id()));
            particleGun->SetParticlePosition(G4ThreeVector(par.xProd()*mm, par.yProd()*mm, par.zProd()*mm + zvtx*cm));
            particleGun->SetParticleMomentum(G4ThreeVector(par.px()*GeV, par.py()*GeV, par.pz()*GeV));
            particleGun->GeneratePrimaryVertex(theEvent);
        }
    }
}

void DPPrimaryGeneratorAction::generateGeant4Single()
{
    p_config->nEventsPhysics++;

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
    p_vertexGen->generateVertex(dimuon);

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
    dimuon.fNegTrackID = 2;
    p_IOmamnger->fillOneDimuon(1., dimuon);
}

void DPPrimaryGeneratorAction::generateExternal()
{
    int eventID = theEvent->GetEventID();
    externalInputTree->GetEntry(eventID);

    for(int i = 0; i < nExternalParticles; ++i)
    {
        TVector3 pos = *((TVector3*)externalPositions->At(i));
        TVector3 mom = *((TVector3*)externalMomentums->At(i));

        particleGun->SetParticleDefinition(particleDict->FindParticle(externalParticlePDGs[i]));
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
    double pz = xF*(DPGEN::sqrts - mass*mass/DPGEN::sqrts)/2.;

    double pTmaxSq = (DPGEN::s*DPGEN::s*(1. - xF*xF) - 2.*DPGEN::s*mass*mass + mass*mass*mass*mass)/DPGEN::s/4.;
    if(pTmaxSq < 0.) return false;

    double pTmax = sqrt(pTmaxSq);
    double pT = 10.;
    if(pTmax < 0.3)
    {
        pT = pTmax*sqrt(G4UniformRand());
    }
    else if(p_config->drellyanMode)
    {
        while(pT > pTmax) pT = DPGEN::pT0DY*sqrt(1./pow(G4UniformRand(), DPGEN::pTpowDY) - 1.);
    }
    else
    {
        while(pT > pTmax) pT = DPGEN::pT0JPsi*sqrt(1./pow(G4UniformRand(), DPGEN::pTpowJPsi) - 1.);
    }

    double phi = G4UniformRand()*DPGEN::twopi;
    double px = pT*TMath::Cos(phi);
    double py = pT*TMath::Sin(phi);

    //configure phase space generator
    TLorentzVector p_dimuon;
    p_dimuon.SetXYZM(px, py, pz, mass);
    p_dimuon.Boost(DPGEN::bv_cms);
    double masses[2] = {DPGEN::mmu, DPGEN::mmu};
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
