#include "DPEventAction.h"
#include "DPMCRawEvent.h"
#include "DPSimConfig.h"

#include <iostream>

DPEventAction::DPEventAction()
{
    p_IOmamnger = DPIOManager::instance();
    printFreq = DPSimConfig::instance()->printFreq;
}

DPEventAction::~DPEventAction() {}

void DPEventAction::BeginOfEventAction(const G4Event* theEvent)
{
    int eventID = theEvent->GetEventID();
    if(eventID % printFreq == 0)
    {
        std::cout << " ############ Processing event " << eventID << " ############" << std::endl;
    }
}

void DPEventAction::EndOfEventAction(const G4Event* theEvent)
{
    int eventID = theEvent->GetEventID();
    if(eventID % printFreq == 0)
    {
        std::cout << " ############ Finishing  event " << eventID << " ############" << std::endl;
        std::cout << " ############ " << timer.CpuTime() << " seconds for the past " << printFreq << " events." << std::endl;
        timer.Start();
    }
    p_IOmamnger->fillOneEvent(theEvent);
    p_IOmamnger->reset();
}
