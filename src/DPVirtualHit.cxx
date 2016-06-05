#include "DPVirtualHit.h"
#include "G4SystemOfUnits.hh"
#include <iomanip>

G4Allocator<DPVirtualHit> DPVHitAllocator;

DPVirtualHit::DPVirtualHit() {}

DPVirtualHit::~DPVirtualHit() {}

bool DPVirtualHit::operator < (const DPVirtualHit& elem) const
{
    if(particleID != elem.particleID) return particleID < elem.particleID;
    if(detectorGroupName != elem.detectorGroupName) return detectorGroupName < elem.detectorGroupName;

    return edep >= elem.edep;
}

bool DPVirtualHit::operator == (const DPVirtualHit& elem) const
{
    return (particleID == elem.particleID) && (detectorGroupName == elem.detectorGroupName);
}

std::ostream& operator << (std::ostream& os, const DPVirtualHit& hit)
{
    os << "Hit comes from track " << hit.particleID << ", with PDG = " << hit.particlePDG << "\n"
       << "   on detector group " << hit.detectorGroupName << ", energy deposited = " << hit.edep/GeV << " GeV\n"
       << "   real position = (" << hit.pos[0]/cm << ", " << hit.pos[1]/cm << ", " << hit.pos[2]/cm << ") cm\n"
       << "   real momentum = (" << hit.mom[0]/GeV << ", " << hit.mom[1]/GeV << ", " << hit.mom[2]/GeV << ") GeV\n"
       << "   has " << hit.digiHits.size() << " digi hits attached. ";
    return os;
}
