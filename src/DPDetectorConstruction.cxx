#include "DPDetectorConstruction.h"
#include "DPMagField.h"
#include "DPSensitiveDetector.h"

#include "G4SystemOfUnits.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"
#include "G4ClassicalRK4.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"

DPDetectorConstruction::DPDetectorConstruction() {}
DPDetectorConstruction::~DPDetectorConstruction() {}

G4VPhysicalVolume* DPDetectorConstruction::Construct()
{
    bool checkOverlaps = false;

    //world volume
    G4NistManager* nistDict = G4NistManager::Instance();
    G4Material* air = nistDict->FindOrBuildMaterial("Air");
    G4VSolid* worldSolid = new G4Box("worldBox", 800.*cm, 600.*cm, 12000.*cm);
    G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, air, "worldLogical");
    physicalWorld = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "worldPhysical", 0, false, 0, checkOverlaps);

    return physicalWorld;
}

void DPDetectorConstruction::ConstructSDandField()
{
    //construct magnetif field
    globalField = new DPMagField();
    G4FieldManager* fieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldManager->SetDetectorField(globalField);

    G4Mag_UsualEqRhs* equations = new G4Mag_UsualEqRhs(globalField);
    G4MagIntegratorStepper* stepper = new G4ClassicalRK4(equations);
    G4ChordFinder* chordFinder = new G4ChordFinder(globalField, 0.1*mm, stepper);
    fieldManager->SetChordFinder(chordFinder);

}
