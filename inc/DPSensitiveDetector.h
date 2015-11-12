#ifndef DPSensitiveDetector_H
#define DPSensitiveDetector_H

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4String.hh"

#include "DPVirtualHit.h"

class DPSensitiveDetector: public G4VSensitiveDetector
{
public:
    explicit DPSensitiveDetector(const G4String& name, const G4String& hitCollectionName);
    virtual ~DPSensitiveDetector();

    void Initialize(G4HCofThisEvent* HCE);
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* hist);
    void EndOfEvent(G4HCofThisEvent* HCE);

private:
    int hcID;
    DPVHitCollection* theHC;
};

#endif
