#include "DPEventAction.h"
#include "DPMCRawEvent.h"
#include "DPSimConfig.h"

#include <iostream>

DPEventAction::DPEventAction()
{
    p_IOmamnger = DPIOManager::instance();
    printFreq = DPSimConfig::instance()->printFreq;
    nEventsAll = DPSimConfig::instance()->nEvents;
    timeElapsed = 0.;
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
    if(eventID % printFreq == 0 && eventID > 0)
    {
        double deltaT = timer.CpuTime();
        timeElapsed += deltaT;

        std::cout << " ############ Finishing  event " << eventID << " ############" << std::endl;
        std::cout << " ############ " << deltaT << " seconds for the past " << printFreq << " events." << std::endl;
        std::cout << " ############ Estimated time remaining: " << timeElapsed/eventID*(nEventsAll - eventID)/60. << " minutes. " << std::endl;
        timer.Start();
    }
    p_IOmamnger->fillOneEvent(theEvent);
    p_IOmamnger->reset();
}
