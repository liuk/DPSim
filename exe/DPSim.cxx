#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "Randomize.hh"

#include <TROOT.h>

#include "DPDetectorConstruction.h"
#include "DPPrimaryGeneratorAction.h"
#include "DPRunAction.h"
#include "DPEventAction.h"
#include "DPTrackingAction.h"
#include "DPSteppingAction.h"
#include "DPSimConfig.h"

using namespace std;

int main(int argc, char* argv[])
{
    //Initialize the configuration
    DPSimConfig* p_config = DPSimConfig::instance();
    p_config->init(argv[1]);

    //set random seed
    CLHEP::HepRandom::setTheSeed(p_config->seed);

    //General Initialization
    G4RunManager* runManager = new G4RunManager;
    runManager->SetUserInitialization(new DPDetectorConstruction);

    G4PhysListFactory factory;
    runManager->SetUserInitialization(factory.GetReferencePhysList(p_config->physicsList.Data()));

    //User actions
    DPPrimaryGeneratorAction* primaryGenerator = new DPPrimaryGeneratorAction;
    runManager->SetUserAction(primaryGenerator);

    runManager->SetUserAction(new DPRunAction);
    runManager->SetUserAction(new DPEventAction);
    runManager->SetUserAction(new DPTrackingAction);
    runManager->SetUserAction(new DPSteppingAction);

    runManager->SetRunIDCounter(p_config->seed);   //use seed as the runID
    runManager->Initialize();

#ifdef DEBUG_TR
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/tracking/verbose 1");
#endif

    runManager->BeamOn(p_config->nEvents);

    delete runManager;
    return EXIT_SUCCESS;
}
