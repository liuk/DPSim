#include "DPEventAction.h"
#include "DPMCRawEvent.h"

#include <iostream>

DPEventAction::DPEventAction()
{
    p_IOmamnger = DPIOManager::instance();
}

DPEventAction::~DPEventAction()
{}

void DPEventAction::BeginOfEventAction(const G4Event* theEvent)
{
    int eventID = theEvent->GetEventID();
    p_IOmamnger->reset(eventID);

    if(eventID % 1000 == 0)
    {
        std::cout << " ############ Processing event " << eventID << " ############" << std::endl;
    }
}

void DPEventAction::EndofEventAction(const G4Event* theEvent)
{
    p_IOmamnger->fillOneEvent(theEvent);
}
