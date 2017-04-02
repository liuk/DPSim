#include <iostream>
#include <list>

#include <TFile.h>
#include <TTree.h>

#include "DPMCRawEvent.h"
#include "DPSimConfig.h"
#include "DPTriggerAnalyzer.h"

using namespace std;

int main(int argc, char* argv[])
{
    DPSimConfig* p_config = DPSimConfig::instance();
    p_config->init(argv[1]);

    DPTriggerAnalyzer* p_triggerAna = DPTriggerAnalyzer::instance();

    TFile* dataFile = new TFile(argv[2], "READ");
    TTree* dataTree = (TTree*)dataFile->Get("save");

    DPMCRawEvent* rawEvent = new DPMCRawEvent;
    dataTree->SetBranchAddress("rawEvent", &rawEvent);

    int nFired = 0;
    for(int i = 0; i < dataTree->GetEntries(); ++i)
    {
        dataTree->GetEntry(i);
        if(i % 1000 == 0) cout << i << "  " << nFired << endl;

        p_triggerAna->analyzeTrigger(rawEvent);
        list<DPTriggerRoad>& pRoads = p_triggerAna->getRoadsFound(0);
        list<DPTriggerRoad>& mRoads = p_triggerAna->getRoadsFound(1);
        if(pRoads.empty() || mRoads.empty()) continue;

        ++nFired;
        /*
        for(list<DPTriggerRoad>::iterator pRoad = pRoads.begin(); pRoad != pRoads.end(); ++pRoad)
        {
            for(list<DPTriggerRoad>::iterator mRoad = mRoads.begin(); mRoad != mRoads.end(); ++mRoad)
            {
            }
        }*/
    }

    cout << nFired << endl;
    return 0;
}
