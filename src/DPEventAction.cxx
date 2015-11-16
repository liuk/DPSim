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
    p_IOmamnger->reset(eventID);

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
        std::cout << " ############ Finishing event " << eventID << " ############" << std::endl;
    }
    p_IOmamnger->fillOneEvent(theEvent);
}
