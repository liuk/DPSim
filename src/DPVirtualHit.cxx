#include "DPVirtualHit.h"
#include "G4SystemOfUnits.hh"
#include <iomanip>

G4Allocator<DPVirtualHit> DPVHitAllocator;

DPVirtualHit::DPVirtualHit()
{}

DPVirtualHit::~DPVirtualHit()
{}

std::ostream& operator << (std::ostream& os, const DPVirtualHit& hit)
{
    os << "Hit comes from track " << hit.particleID << ", with PDG = " << hit.particlePDG << "\n"
       << "   on detector group " << hit.detectorGroupName << ", energy deposited = " << hit.edep << "\n"
       << "   real position = (" << hit.pos[0]/cm << ", " << hit.pos[1]/cm << ", " << hit.pos[2]/cm << ") cm\n"
       << "   real momentum = (" << hit.mom[0]/GeV << ", " << hit.mom[1]/GeV << ", " << hit.mom[2]/GeV << ") GeV\n"
       << "   has " << hit.digiHits.size() << " digi hits attached. ";
    return os;
}
