#include "DPDummyRecon.h"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

#include <TVector3.h>
#include <TLorentzVector.h>

#include "DPDetectorConstruction.h"
#include "DPSimConfig.h"

#define FMAG_LENGTH 502.92

DPDummyRecon::DPDummyRecon()
{
    //pt kick
    ptkick = 2.909*DPSimConfig::instance()->fMagMultiplier/FMAG_LENGTH;

    //Initialize the particle lut
    particleDict = G4ParticleTable::GetParticleTable();
}

void DPDummyRecon::reconstruct(DPMCRawEvent* rawEvent)
{
    int nDimuons = rawEvent->getNDimuons();
    for(int i = 0; i < nDimuons; ++i)
    {
        DPMCDimuon dimuon = rawEvent->getDimuon(i);
        if(!dimuon.fAccepted) continue;

        //swim mu+
        DPMCTrack mup = rawEvent->getTrack(dimuon.fPosTrackID);
        DPMCHit phit = rawEvent->getHit(mup.fHitIDs[0]);

        setParticle(mup.fPDGCode, G4ThreeVector(phit.fPosition.X()*cm, phit.fPosition.Y()*cm, phit.fPosition.Z()*cm), G4ThreeVector(phit.fMomentum.Px()*GeV, phit.fMomentum.Py()*GeV, phit.fMomentum.Pz()*GeV));
        if(!swimTo(dimuon.fVertex.Z())) break;
        TVector3 ppos(finalPos.x()/cm, finalPos.y()/cm, finalPos.z()/cm);
        dimuon.fPosMomentum.SetXYZM(finalMom.x()/GeV, finalMom.y()/GeV, finalMom.z()/GeV, 0.10566);

        //swim mu-
        DPMCTrack mum = rawEvent->getTrack(dimuon.fNegTrackID);
        DPMCHit mhit = rawEvent->getHit(mum.fHitIDs[0]);

        setParticle(mum.fPDGCode, G4ThreeVector(mhit.fPosition.X()*cm, mhit.fPosition.Y()*cm, mhit.fPosition.Z()*cm), G4ThreeVector(mhit.fMomentum.Px()*GeV, mhit.fMomentum.Py()*GeV, mhit.fMomentum.Pz()*GeV));
        if(!swimTo(dimuon.fVertex.Z())) break;
        TVector3 mpos(finalPos.x()/cm, finalPos.y()/cm, finalPos.z()/cm);
        dimuon.fNegMomentum.SetXYZM(finalMom.x()/GeV, finalMom.y()/GeV, finalMom.z()/GeV, 0.10566);

        //update dimuon info
        dimuon.fVertex = 0.5*(ppos + mpos);
        dimuon.calcVariables();

        rawEvent->addRecDimuon(dimuon, dimuon.fDimuonID);
    }
}

void DPDummyRecon::setParticle(int pdgCode, G4ThreeVector pos, G4ThreeVector mom)
{
    particleName = particleDict->FindParticle(pdgCode)->GetParticleName();
    initPos = pos;
    initMom = mom;
}

bool DPDummyRecon::swimTo(double z)
{
    //define the geometrical limits of FMag
    double z_lo = 25.4;
    double z_hi = 502.94;
    double x_lo = -80.01;
    double x_hi = 80.01;
    double y_lo = -64.77;
    double y_hi = 64.77;

    int nSteps = 500;
    double zstep = (z_hi - z_lo)/nSteps/2.;

    //Project the initial position and momentum at z = z_hi
    double tx = initMom.x()/initMom.z();
    double ty = initMom.y()/initMom.z();
    TVector3 mom(initMom.x()/GeV, initMom.y()/GeV, initMom.z()/GeV);
    TVector3 pos(initPos.x()/cm + (z_hi - initPos.z()/cm)*tx, initPos.y()/cm + (z_hi - initPos.z()/cm)*ty, z_hi);

    TVector3 pos_bend(0., 0., 275.);
    double charge = particleDict->FindParticle(particleName)->GetPDGCharge();
    //std::cout << "Start propagator: target = " << z << ", charge = " << charge << std::endl;
    while(pos.Z() > z_lo && fabs(z - pos.Z()) > 0.1)
    {
        /*
        std::cout << "Pre step " << std::endl;
        std::cout << " pos: " << pos.X() << "  " << pos.Y() << "  " << pos.Z() << std::endl;
        std::cout << " mom: " << mom.X() << "  " << mom.Y() << "  " << mom.Z() << std::endl;
        */

        //check if it's still within FMag
        if(pos.X() < x_lo || pos.X() > x_hi || pos.Y() < y_lo || pos.Y() > y_hi) return false;

        //save the position at bend plane
        if(fabs(pos.Z() - pos_bend.Z()) < zstep)
        {
            double dz = pos_bend.Z() - pos.Z();
            pos_bend.SetX(pos.X() + mom.Px()/mom.Pz()*dz);
            pos_bend.SetY(pos.Y() + ty*dz);

            /*
            std::cout << "Bend plane found  " << std::endl;
            std::cout << " pos: " << pos_bend.X() << "  " << pos_bend.Y() << "  " << pos_bend.Z() << std::endl;
            */
        }

        //adjust the step size at the last step if destination is within reach
        if(z < pos.Z() && z > pos.Z() - 2.*zstep) zstep = (pos.Z() - z)/2.;
        //std::cout << " step size = " << zstep << std::endl;

        //Make pT kick at the center of slice, add energy loss at both first and last half-slice
        //Note that ty is the global class data member, which does not change during the entire swimming
        double tx_i = mom.Px()/mom.Pz();
        double tx_f = tx_i + 2.*charge*ptkick*zstep/sqrt(mom.Px()*mom.Px() + mom.Pz()*mom.Pz());

        TVector3 trajVec1(tx_i*zstep, ty*zstep, zstep);
        double p_tot_b = mom.Mag() + dedx(mom.Mag())*trajVec1.Mag();

        TVector3 trajVec2(tx_f*zstep, ty*zstep, zstep);
        double p_tot_f = p_tot_b + dedx(p_tot_b)*trajVec2.Mag();
        double pz_f = p_tot_f/sqrt(1. + tx_f*tx_f + ty*ty);

        mom.SetXYZ(pz_f*tx_f, pz_f*ty, pz_f);
        pos = pos - trajVec1 - trajVec2;

        /*
        std::cout << "Post step " << std::endl;
        std::cout << " pos: " << pos.X() << "  " << pos.Y() << "  " << pos.Z() << std::endl;
        std::cout << " mom: " << mom.X() << "  " << mom.Y() << "  " << mom.Z() << std::endl;
        */
    }

    double zstep_final = pos.Z() - z;
    TVector3 trajVec(mom.Px()/mom.Pz()*zstep_final, ty*zstep_final, zstep_final);
    pos = pos - trajVec;

    if(z < 100)
    {
        //re-tracking correction
        mom.SetX(mom.Pz()*pos_bend.X()/(pos_bend.Z() - z));
        mom.SetY(mom.Pz()*pos_bend.Y()/(pos_bend.Z() - z));
    }

    finalPos.set(pos.X()*cm, pos.Y()*cm, pos.Z()*cm);
    finalMom.set(mom.Px()*GeV, mom.Py()*GeV, mom.Pz()*GeV);

    return true;
}

double DPDummyRecon::dedx(double e)
{
    return (7.18274 + 0.0361447*e - 0.000718127*e*e + 7.97312e-06*e*e*e - 3.05481e-08*e*e*e*e)/FMAG_LENGTH;
}

/*
The following code will be re-visited in future
DPDummyRecon::DPDummyRecon()
{
    //Initial propagator manager and all related stuff
    g4eManager = G4ErrorPropagatorManager::GetErrorPropagatorManager();
    g4eData = G4ErrorPropagatorData::GetErrorPropagatorData();

    G4VPhysicalVolume* world = ((DPDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetWorldPtr();
    g4eManager->SetUserInitialization(world);
    g4eManager->InitGeant4e();

    G4UImanager::GetUIpointer()->ApplyCommand("/geant4e/limits/stepLength 10 mm");

    //Initialize the particle lut
    particleDict = G4ParticleTable::GetParticleTable();
}



bool DPDummyRecon::swimTo(double z)
{
    //set propagator mode
    g4eMode = z > initPos.z() ? G4ErrorMode_PropForwards : G4ErrorMode_PropBackwards;

    //create initial state and final target plane
    g4eState = new G4ErrorFreeTrajState(particleName, initPos, initMom);
    g4eTarget = new G4ErrorPlaneSurfaceTarget(0., 0., 1., -z);
    g4eData->SetTarget(g4eTarget);

    int err = 0;
    if(g4eMode == G4ErrorMode_PropBackwards)
    {
        g4eState->SetMomentum(-g4eState->GetMomentum());
        err = g4eManager->Propagate(g4eState, g4eTarget, g4eMode);
        g4eState->SetMomentum(-g4eState->GetMomentum());
    }
    else
    {
        err = g4eManager->Propagate(g4eState, g4eTarget, g4eMode);
    }

    //get the final results
    finalPos = g4eState->GetPosition();
    finalMom = g4eState->GetMomentum();

    //cleanup
    delete g4eTarget;
    delete g4eState;

    return err == 0;
}
*/
