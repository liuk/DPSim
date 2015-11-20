#ifndef DPVirtualHit_H
#define DPVirtualHit_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"

#include "DPMCRawEvent.h"

#include <iostream>
#include <vector>

class DPVirtualHit: public G4VHit
{
public:
    DPVirtualHit();
    ~DPVirtualHit();

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    bool operator < (const DPVirtualHit& elem) const;
    bool operator == (const DPVirtualHit& elem) const;

    friend ostream& operator << (std::ostream& os, const DPVirtualHit& hit);

public:
    //general virtual hit information
    unsigned int particleID;   // --- this always corresponds to the trackID in geant4 simulation
    int particlePDG;

    //at this detector
    G4String detectorGroupName;

    double edep;
    G4ThreeVector mom;
    G4ThreeVector pos;

    //digitized hits storage
    std::vector<DPMCHit> digiHits;
};

//vector collection of one type of hits
typedef G4THitsCollection<DPVirtualHit> DPVHitCollection;
extern G4Allocator<DPVirtualHit> DPVHitAllocator;

inline void* DPVirtualHit::operator new(size_t)
{
    void* aHit;
    aHit = (void*)DPVHitAllocator.MallocSingle();

    return aHit;
}

inline void DPVirtualHit::operator delete(void* aHit)
{
    DPVHitAllocator.FreeSingle((DPVirtualHit*)aHit);
}

#endif
