#include "DPRunAction.h"
#include "DPIOManager.h"
#include "DPVertexGenerator.h"

#include <iostream>

DPRunAction::DPRunAction() {}

DPRunAction::~DPRunAction() {}

void DPRunAction::BeginOfRunAction(const G4Run* theRun)
{
    DPIOManager* p_IOmamnger = DPIOManager::instance();
    p_IOmamnger->initialize(theRun->GetRunID());

    DPVertexGenerator* p_vertexGen = DPVertexGenerator::instance();
    p_vertexGen->init();

    std::cout << " #### Starting Run " << theRun->GetRunID() << " #### " << std::endl;
    std::cout << " ######## Will process " << theRun->GetNumberOfEventToBeProcessed() << " events ######## " << std::endl;
}

void DPRunAction::EndOfRunAction(const G4Run* theRun)
{
    DPIOManager* p_IOmamnger = DPIOManager::instance();
    p_IOmamnger->finalize();

    std::cout << " #### Ending Run " << theRun->GetRunID() << " #### " << std::endl;
}
