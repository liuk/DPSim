#ifndef DPDetectorConstruction_H
#define DPDetectorConstruction_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4VPhysicalVolume.hh"

#include "DPMagField.h"

class DPDetectorConstruction: public G4VUserDetectorConstruction
{
public:
    DPDetectorConstruction();
    ~DPDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

private:
    //pointer to the physical volume
    G4VPhysicalVolume* physicalWorld;

    //pointer to the global magnetic field
    DPMagField* globalField;

};

#endif
