#include "DPVertexGenerator.h"

#include <algorithm>

#include "Randomize.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4Tubs.hh"

#include <TMath.h>
#include <TString.h>

#include "DPDetectorConstruction.h"

DPBeamLineObject::DPBeamLineObject() {}

DPBeamLineObject::DPBeamLineObject(const G4Material* pMaterial)
{
    nucIntLen = pMaterial->GetNuclearInterLength()/cm;
    density = pMaterial->GetDensity()/(g/cm3);

    unsigned int nElements = pMaterial->GetNumberOfElements();
    const G4ElementVector elemArr = *(pMaterial->GetElementVector());
    const double* fracArr = pMaterial->GetFractionVector();
    const double* abdArr = pMaterial->GetVecNbOfAtomsPerVolume();

    Z = 0.; A = 0.; N = 0.;
    for(unsigned int i = 0; i < nElements; ++i)
    {
        A += (fracArr[i]*elemArr[i]->GetA()/(g/mole));
        Z += (abdArr[i]*elemArr[i]->GetZ());
        N += (abdArr[i]*elemArr[i]->GetN());
    }

    protonPerc = Z/N;  //N here for now stands for total nucleons
    Z = A*protonPerc;
    N = A - Z;
}

bool DPBeamLineObject::operator < (const DPBeamLineObject& obj) const
{
    return z0 < obj.z0;    //NOTE: this is only valid if there is no overlap
}

std::ostream& operator << (std::ostream& os, const DPBeamLineObject& obj)
{
    os << "Beamline object name " << obj.name << " at " << obj.z_up << " <-- " << obj.z0 << " --> " << obj.z_down << "\n"
       << "   Z = " << obj.Z << ", A = " << obj.A << ", N = " << obj.N << "\n"
       << "   Nuclear inc. len. = " << obj.nucIntLen << ", density = " << obj.density << "\n"
       << "   " << obj.length/obj.nucIntLen*100. << "% interaction length, " << " upstream attenuation = " << 1. - obj.attenuation/obj.attenuationSelf << "\n"
       << "   Attenuation by itself = " << obj.attenuationSelf << ", " << " real attenuation = " << obj.attenuation << "\n"
       << "   Collision prob = " << obj.prob << ", accumulated prob = " << obj.accumulatedProb;

    return os;
}

double DPBeamLineObject::getZ()
{
    return z_up - nucIntLen*TMath::Log(1. - attenuationSelf*G4UniformRand());
}

DPVertexGenerator* DPVertexGenerator::p_vertexGen = NULL;
DPVertexGenerator* DPVertexGenerator::instance()
{
    if(p_vertexGen == NULL) p_vertexGen = new DPVertexGenerator;
    return p_vertexGen;
}

DPVertexGenerator::DPVertexGenerator()
{
    p_config = DPSimConfig::instance();
}

void DPVertexGenerator::init()
{
    //Traverse the geometry tree to get all the volumes and hence their materials
    const G4VPhysicalVolume* world = ((DPDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetWorldPtr();
    const G4LogicalVolume* worldLogical = world->GetLogicalVolume();

    int nDaughters = worldLogical->GetNoDaughters();
    for(int i = 0; i < nDaughters; ++i)
    {
        const G4VPhysicalVolume* pv = worldLogical->GetDaughter(i);
        G4ThreeVector pos = pv->GetObjectTranslation();
        if(fabs(pos.x()/cm) > 1. || fabs(pos.y()/cm) > 1.) continue;    //interactables must be in the beam line

        TString name = pv->GetName();
        if(!((p_config->targetInBeam && name.Contains("T_")) ||
             (p_config->dumpInBeam   && name.Contains("D_"))  ||
             (p_config->instruInBeam && name.Contains("I_")))) continue;

        DPBeamLineObject newObj(pv->GetLogicalVolume()->GetMaterial());
        newObj.name = name;
        newObj.z0 = pos.z()/cm;
        if(!name.Contains("D_"))
        {
            newObj.length = 2.*((G4Tubs*)(pv->GetLogicalVolume()->GetSolid()))->GetZHalfLength()/cm;
            newObj.z_down = newObj.z0 + 0.5*newObj.length;
            newObj.z_up = newObj.z0 - 0.5*newObj.length;
        }
        else  //special treatment for beam dump, make it box shaped and drill a hole
        {
            newObj.length = 2.*((G4Box*)(pv->GetLogicalVolume()->GetSolid()))->GetZHalfLength()/cm;
            newObj.z_down = newObj.z0 + 0.5*newObj.length;
            newObj.z_up = newObj.z0 - 0.5*newObj.length + 10.*2.54;
            newObj.length = newObj.length - 10.*2.54;
        }
        interactables.push_back(newObj);
    }

    //sort the vector by position
    std::sort(interactables.begin(), interactables.end());

    //detect if there are any gaps and fill it with air
    if(p_config->airInBeam)
    {
        //generate an air gap first
        DPBeamLineObject airgap(G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"));

        //insert the air gap where possible
        int nGaps = 0;
        unsigned int n_temp = interactables.size();
        for(unsigned int i = 1; i < n_temp; ++i)
        {
            if(fabs(interactables[i-1].z_down - interactables[i].z_up) < 0.1) continue;

            airgap.z_up = interactables[i-1].z_down;
            airgap.z_down = interactables[i].z_up;
            airgap.z0 = 0.5*(airgap.z_up + airgap.z_down);
            airgap.length = airgap.z_down - airgap.z_up;
            airgap.name = Form("AirGap_%d", nGaps++);

            interactables.push_back(airgap);
        }
        std::sort(interactables.begin(), interactables.end());
    }

    //set the quanties that rely on its neighbours
    double attenuationSum = 0.;
    for(unsigned int i = 0; i < interactables.size(); ++i)
    {
        interactables[i].attenuationSelf = 1. - TMath::Exp(-interactables[i].length/interactables[i].nucIntLen);
        interactables[i].attenuation = (1. - attenuationSum)*interactables[i].attenuationSelf;
        interactables[i].prob = interactables[i].attenuation*interactables[i].density*interactables[i].nucIntLen;

        attenuationSum += interactables[i].attenuation;
    }

    //set the accumulatedProbs
    nPieces = interactables.size();
    interactables[0].accumulatedProb = 0.;
    accumulatedProbs[0] = 0.;
    for(unsigned int i = 1; i < nPieces; ++i)
    {
        interactables[i].accumulatedProb = interactables[i-1].accumulatedProb + interactables[i-1].prob;
        accumulatedProbs[i] = p_config->biasVertexGen ? i : interactables[i].accumulatedProb;
    }
    accumulatedProbs[nPieces] = p_config->biasVertexGen ? nPieces : accumulatedProbs[nPieces-1] + interactables.back().prob;
    probSum = accumulatedProbs[nPieces];

    //Normalize the probs
    for(unsigned int i = 0; i < nPieces+1; ++i)
    {
        accumulatedProbs[i] = accumulatedProbs[i]/accumulatedProbs[nPieces];
    }

//#ifdef DEBUG_IN
    std::cout << "Following objects will interact with beam:" << std::endl;
    for(int i = 0; i < nPieces; ++i)
    {
        std::cout << i << " " << interactables[i] << std::endl;
    }
//#endif
}

double DPVertexGenerator::generateVertex()
{
    findInteractingPiece();
    double zOffset = p_config->zOffsetMin < p_config->zOffsetMax ? p_config->zOffsetMin + G4UniformRand()*(p_config->zOffsetMax - p_config->zOffsetMin) : 0.;
    return interactables[index].getZ() + zOffset;
}

void DPVertexGenerator::generateVertex(DPMCDimuon& dimuon)
{
    findInteractingPiece();

    dimuon.fVertex.SetX(G4RandGauss::shoot(0., 1.5));
    dimuon.fVertex.SetY(G4RandGauss::shoot(0., 1.5));
    dimuon.fVertex.SetZ(interactables[index].getZ());
    dimuon.fOriginVol = interactables[index].name;

    if(p_config->zOffsetMin < p_config->zOffsetMax)
    {
        double zOffset = p_config->zOffsetMin + G4UniformRand()*(p_config->zOffsetMax - p_config->zOffsetMin);
        dimuon.fVertex.SetZ(interactables[index].getZ() + zOffset);
    }
}

void DPVertexGenerator::findInteractingPiece()
{
    index = TMath::BinarySearch(nPieces+1, accumulatedProbs, G4UniformRand());
}
