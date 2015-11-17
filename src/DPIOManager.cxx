#include "DPIOManager.h"

#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

DPIOManager* DPIOManager::p_IOmamnger = NULL;
DPIOManager* DPIOManager::instance()
{
    if(p_IOmamnger == NULL) p_IOmamnger = new DPIOManager;
    return p_IOmamnger;
}

DPIOManager::DPIOManager(): saveFile(NULL), saveTree(NULL), rawEvent(NULL)
{
    p_config = DPSimConfig::instance();
    p_digitizer = DPDigitizer::instance();
    sensHitColID = -1;

    saveMode = HITSONLY;
    if(p_config->outputMode == "all")
    {
        saveMode = EVERYTHING;
    }
    else if(p_config->outputMode == "hits")
    {
        saveMode = HITSONLY;
    }
    else if(p_config->outputMode == "dimuon")
    {
        saveMode = INACCONLY;
    }

    //normally we expect less than 2000 tracks, 2000 hits
    tracks.reserve(2000);
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
    configTree = new TTree("config", "config");
    configTree->Branch("config", &p_config, 256000, 99);

    //in case there is an external input for the custom input, need to attach the raw input tree as well
}

void DPIOManager::reset()
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " reset IO manager for this event " << std::endl;
#endif
    rawEvent->clear();

    trackIDs.clear();
    tracks.clear();
    hits.clear();
}

void DPIOManager::fillOneDimuon(double weight, const DPMCDimuon& dimuon)
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " insert new dimuon with mass = " << dimuon.fMass << std::endl;
#endif
    rawEvent->eventHeader().fSigWeight = weight;
    rawEvent->addDimuon(dimuon);
}

void DPIOManager::fillOneEvent(const G4Event* theEvent)
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " fill all the event info for eventID = " << theEvent->GetEventID() << std::endl;
#endif
    //set event ID
    rawEvent->eventHeader().fEventID = theEvent->GetEventID();

    //extract all the virtual hits and digitize
    fillHitsVector(theEvent);

    //set the index of tracks and hits
    reIndex();

    //update the dimuon info if needed
    bool dimuonAccepted = false;
    if(rawEvent->getNDimuons() > 0)
    {
        for(int i = 0; i < rawEvent->getNDimuons(); ++i)
        {
            DPMCDimuon* dimuon = (DPMCDimuon*)rawEvent->getDimuons()->At(i);
            dimuon->fAccepted = tracks[trackIDs[dimuon->fPosTrackID]].first.isAccepted() && tracks[trackIDs[dimuon->fNegTrackID]].first.isAccepted();
            dimuon->fPosTrackID = tracks[trackIDs[dimuon->fPosTrackID]].first.fTrackID;
            dimuon->fNegTrackID = tracks[trackIDs[dimuon->fNegTrackID]].first.fTrackID;

            dimuonAccepted = dimuonAccepted || dimuon->fAccepted;
        }
    }
    if(saveMode == INACCONLY && !dimuonAccepted) return;

    //fill the tracks and hits into the event
    for(std::vector<std::pair<DPMCTrack, bool> >::iterator iter = tracks.begin(); iter != tracks.end(); ++iter)
    {
        //only save the tracks that have at least one hits in detector
#ifdef DEBUG_IO
        std::cout << __FILE__ << " " << __FUNCTION__ << " track with ID = " << iter->first.fTrackID << " PDG = " << iter->first.fPDGCode
                  << " has " << iter->first.fHitIDs.size() << " hits. " << std::endl;
#endif
        if(iter->second) rawEvent->addTrack(iter->first);
    }
    if(saveMode == HITSONLY && rawEvent->getNTracks() < 1) return;

    for(std::vector<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            rawEvent->addHit(*jter);
        }
    }

    //save the event
#ifdef DEBUG_IO
    rawEvent->print();
#endif
    saveTree->Fill();
    if(saveTree->GetEntries() % 1000 == 0) saveTree->AutoSave("SaveSelf");
}

void DPIOManager::fillOneTrack(const DPMCTrack& mcTrack, bool keep)
{
    if(trackIDs.find(mcTrack.fTrackID) == trackIDs.end()) //new track
    {
        tracks.push_back(std::make_pair(mcTrack, keep));
        trackIDs.insert(std::map<unsigned int, unsigned int>::value_type(mcTrack.fTrackID, tracks.size()-1));
    }
    else
    {
        std::cout << "WARNING! Duplicate track index! " << std::endl;
    }
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " adding new track with ID = " << mcTrack.fTrackID
              << ", PDG = " << mcTrack.fPDGCode << ", loc = " << tracks.size()-1 << std::endl;
#endif
}

void DPIOManager::updateOneTrack(unsigned int trackID, G4ThreeVector pos, G4ThreeVector mom, bool keep)
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " updating final pos/mom for trackID = " << trackID << std::endl;
#endif
    std::map<unsigned int, unsigned int>::iterator index = trackIDs.find(trackID);
    if(index == trackIDs.end()) std::cout << "WARNING! Track with ID = " << trackID << " not found!" << std::endl;

    tracks[index->second].first.fFinalPos.SetXYZ(pos[0]/cm, pos[1]/cm, pos[2]/cm);
    tracks[index->second].first.fFinalMom.SetXYZM(mom[0]/GeV, mom[1]/GeV, mom[2]/GeV, tracks[index->second].first.fInitialMom.M());
    tracks[index->second].second = keep;
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
        p_digitizer->digitize(hits.back());
    }
}

void DPIOManager::reIndex()
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " now reset all the index." << std::endl;
#endif

    //NOTE: here in each virtual hit, the particleID refers to the real track ID assigned by geant
    //      and the index of this track in the track vector is stored in the map trackIDs, which maps
    //      the real geant trackID to the track vector index
    //      And the fTrackID stored in the digi hits (or DPMCHit), is the one has been re-assigned to
    //      eliminate the large trackID number by dropping the hitless tracks.
    //      In the re-index process, the trackID/particleID in virtual hits remains unchanged, only the
    //      ones in DPMCHit are updated after re-ordering the tracks

    //Assign and fill the hitIDs to the tracks
    unsigned int hitID = 0;
    for(std::vector<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            jter->fHitID = hitID++;
            tracks[trackIDs[iter->particleID]].first.addHit(*jter);
        }
        tracks[trackIDs[iter->particleID]].second = !iter->digiHits.empty(); //if hit list is non-empty, keep the corrsponding track
    }

    //label the tracks have at least one hit, or is the mother particle of the tracks with hits
    //the trackID of other tracks (hitless and childless) will be discarded
    for(std::vector<std::pair<DPMCTrack, bool> >::iterator iter = tracks.begin(); iter != tracks.end(); ++iter)
    {
        if(saveMode == EVERYTHING)
        {
            iter->second = true;
            continue;
        }

        if(iter->first.fParentID == 0) continue;
        if(!iter->second) continue;

        int parentID = iter->first.fParentID;
        while(parentID != 0)
        {
            tracks[trackIDs[parentID]].second = true;
            parentID = tracks[trackIDs[parentID]].first.fParentID;
        }
    }


    //set the trackID to all the tracks that should be kept
    unsigned int trackID = 0;
    for(std::vector<std::pair<DPMCTrack, bool> >::iterator iter = tracks.begin(); iter != tracks.end(); ++iter)
    {
        if(!iter->second) continue;
        iter->first.fTrackID = trackID++;

        if(iter->first.fParentID != 0)
        {
            iter->first.fParentPDGCode = tracks[trackIDs[iter->first.fParentID]].first.fPDGCode;
            iter->first.fParentID = tracks[trackIDs[iter->first.fParentID]].first.fTrackID;
        }
        else
        {
            iter->first.fParentPDGCode = 0;
        }
    }

    //set the trackID in the digiHits vector inside each virtual hits
    for(std::vector<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            jter->fTrackID = tracks[trackIDs[iter->particleID]].first.fTrackID;
        }
    }

    //set the trackID to the
}

void DPIOManager::finalize()
{
#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " closing up." << std::endl;
#endif
    saveFile->cd();
    saveTree->Write();

    configTree->Fill();
    configTree->Write();

    saveFile->Close();
}
