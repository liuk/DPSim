#ifndef DPDetectorConstruction_H
#define DPDetectorConstruction_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4VPhysicalVolume.hh"
#include "G4GDMLParser.hh"

#include "DPMagField.h"

class DPDetectorConstruction: public G4VUserDetectorConstruction
{
public:
    DPDetectorConstruction();
    ~DPDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    //helper function to extract world volume pointer and access geometry 
    const G4VPhysicalVolume* GetWorldPtr() { return physicalWorld; }

private:
    //pointer to the physical volume
    G4VPhysicalVolume* physicalWorld;

    //pointer to the global magnetic field
    DPMagField* globalField;

    //GDML parser
    G4GDMLParser gdmlParser;

};

#endif
