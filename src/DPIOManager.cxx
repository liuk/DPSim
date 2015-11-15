#include "DPIOManager.h"

#include "G4SDManager.hh"

DPIOManager* DPIOManager::p_IOmamnger = NULL;
DPIOManager* DPIOManager::instance()
{
    if(p_IOmamnger == NULL) p_IOmamnger = new DPIOManager;
    return p_IOmamnger;
}

DPIOManager::DPIOManager(): saveFile(NULL), saveTree(NULL), rawEvent(NULL)
{
    p_config = DPSimConfig::instance();
    sensHitColID = -1;

    //normally we expect less than 500 tracks, 2000 hits
    tracks.reserve(500);
    hits.reserve(2000);
}

DPIOManager::~DPIOManager()
{
    if(rawEvent) delete rawEvent;
    if(saveTree) delete saveTree;
    if(saveFile) delete saveFile;
}

void DPIOManager::initialize(int runID)
{
    //Main output structure
    rawEvent = new DPMCRawEvent();
    rawEvent->eventHeader().fRunID = runID;
    rawEvent->eventHeader().fSpillID = 0;
    rawEvent->eventHeader().fSigWeight = 1.;

    saveFile = new TFile(p_config->outputFileName.Data(), "recreate");
    saveTree = new TTree("save", "save");

    saveTree->Branch("rawEvent", &rawEvent, 256000, 99);

    //Configuration tree
    TTree* configTree = new TTree("config", "config");
    configTree->Branch("config", &p_config, 256000, 99);

    configTree->Fill();
    configTree->Write();

    //in case there is an external input for the custom input
}

void DPIOManager::reset(int eventID)
{
    rawEvent->clear();
    rawEvent->eventHeader().fEventID = eventID;

    trackIDs.clear();
    tracks.clear();
    hits.clear();
}

void DPIOManager::fillOneDimuon(double weight, const DPMCDimuon& dimuon)
{
    rawEvent->eventHeader().fSigWeight = weight;
    rawEvent->addDimuon(dimuon);
}

void DPIOManager::fillOneEvent(const G4Event* theEvent)
{
    //extract all the virtual hits and digitize
    fillHitsVector(theEvent);

    //set the index of tracks and hits
    reIndex();

    //fill the tracks and hits into the event
    for(std::vector<DPMCTrack>::iterator iter = tracks.begin(); iter != tracks.end(); ++iter)
    {
        //only save the tracks that have at least one hits in detector
        if(!iter->fHitIDs.empty()) rawEvent->addTrack(*iter);
    }

    for(std::vector<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            rawEvent->addHit(*jter);
        }
    }

    //save the event
    saveTree->Fill();
    if(saveTree->GetEntries() % 1000 == 0) saveTree->AutoSave("SaveSelf");
}

void DPIOManager::fillOneTrack(const DPMCTrack& mcTrack)
{
    if(trackIDs.find(mcTrack.fTrackID) == trackIDs.end()) //new track
    {
        tracks.push_back(mcTrack);
        trackIDs.insert(std::map<unsigned int, unsigned int>::value_type(mcTrack.fTrackID, tracks.size()-1));
    }
    else
    {
        std::cout << "WARNING! Duplicate track index! " << std::endl;
    }
}

void DPIOManager::updateOneTrack(unsigned int trackID, G4ThreeVector pos, G4ThreeVector mom)
{
    std::map<unsigned int, unsigned int>::iterator index = trackIDs.find(trackID);
    if(index == trackIDs.end()) std::cout << "WARNING! Track with ID = " << trackID << " not found!" << std::endl;

    tracks[index->second].fFinalPos.SetXYZ(pos[0]/cm, pos[1]/cm, pos[2]/cm);
    tracks[index->second].fFinalMom.SetXYZM(mom[0]/GeV, mom[1]/GeV, mom[2]/GeV, tracks[index->second].fInitialMom.M());
}

void DPIOManager::fillHitsVector(const G4Event* theEvent)
{
    if(sensHitColID < 0) sensHitColID = G4SDManager::GetSDMpointer()->GetCollectionID("sensDetHitCol");

    G4HCofThisEvent* HCE = theEvent->GetHCofThisEvent();
    if(!HCE) return;

    DPVHitCollection* sensHC = (DPVHitCollection*)HCE->GetHC(sensHitColID);
    if(!sensHC) return;

    int nHits = sensHC->entries();
    for(int i = 0; i < nHits; ++i)
    {
        hits.push_back(*((*sensHC)[i]));
    }
}

void DPIOManager::reIndex()
{
    //Assign and fill the hitIDs to the tracks
    unsigned int hitID = 0;
    for(std::vector<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            jter->fHitID = hitID++;
            tracks[iter->particleID].fHitIDs.push_back(jter->fHitID);
        }
    }

    //set the trackID to all the tracks that have at least on hit
    unsigned int trackID = 0;
    for(std::vector<DPMCTrack>::iterator iter = tracks.begin(); iter != tracks.end(); ++iter)
    {
        if(iter->fHitIDs.empty()) continue;
        iter->fTrackID = trackID++;
    }

    //set the trackID in the digiHits vector inside each virtual hits
    for(std::vector<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            jter->fTrackID = tracks[iter->particleID].fTrackID;
        }
    }
}

void DPIOManager::finalize()
{
    saveFile->cd();
    saveTree->Write();
    saveFile->Close();
}
