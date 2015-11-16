#include "DPDetectorConstruction.h"
#include "DPMagField.h"
#include "DPSensitiveDetector.h"
#include "DPSimConfig.h"

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

#include "G4SDManager.hh"

#include <iostream>

DPDetectorConstruction::DPDetectorConstruction() {}
DPDetectorConstruction::~DPDetectorConstruction() {}

G4VPhysicalVolume* DPDetectorConstruction::Construct()
{
    DPSimConfig* p_config = DPSimConfig::instance();
    std::cout << "Initializing global geometry from GDML file " << p_config->geometryGDMLInput.Data() << std::endl;

    gdmlParser.Read(p_config->geometryGDMLInput.Data());
    physicalWorld = gdmlParser.GetWorldVolume();

    return physicalWorld;
}

void DPDetectorConstruction::ConstructSDandField()
{
    std::cout << "Initializing sensitive detectors ..." << std::endl;

    //Construct sensitive detectors
    DPSensitiveDetector* sensDet= new DPSensitiveDetector("SensDet", "sensDetHitCol");
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);

    const G4GDMLAuxMapType* auxMap = gdmlParser.GetAuxMap();
    for(G4GDMLAuxMapType::const_iterator iter = auxMap->begin(); iter != auxMap->end(); ++iter)
    {
        for(G4GDMLAuxListType::const_iterator jter = iter->second.begin(); jter != iter->second.end(); ++jter)
        {
            if(jter->type == "SensDet")
            {
                iter->first->SetSensitiveDetector(sensDet);
            }
        }
    }

    //construct magnetif field
    globalField = new DPMagField();
    G4FieldManager* fieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldManager->SetDetectorField(globalField);

    G4Mag_UsualEqRhs* equations = new G4Mag_UsualEqRhs(globalField);
    G4MagIntegratorStepper* stepper = new G4ClassicalRK4(equations);
    G4ChordFinder* chordFinder = new G4ChordFinder(globalField, 0.1*mm, stepper);
    fieldManager->SetChordFinder(chordFinder);
}
