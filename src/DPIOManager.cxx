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
    p_triggerAna = DPTriggerAnalyzer::instance();
    p_dummyRecon = p_config->enableDummyRecon ? new DPDummyRecon() : NULL;
    sensHitColID = -1;

    saveMode = HITSONLY;
    if(p_config->outputMode == "all")
    {
        saveMode = EVERYTHING;
    }
    else if(p_config->outputMode == "primary")
    {
        saveMode = PRIMARY;
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

    //turn off external control of single event buffer flushsing
    bufferState = INTERNAL;
}

DPIOManager::~DPIOManager()
{
    if(rawEvent != NULL) delete rawEvent;
    if(saveTree != NULL) delete saveTree;
    if(saveFile != NULL) delete saveFile;

    if(p_triggerAna != NULL) delete p_triggerAna;
    if(p_dummyRecon != NULL) delete p_dummyRecon;
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

    //Configuration tree
    configTree = new TTree("config", "config");
    configTree->Branch("config", &p_config, 256000, 99);

    //event buffer for the singles mode only
    if(p_config->dimuonMode || p_config->bucket_size == 1)
    {
        singleEvent = NULL;
        saveTree->Branch("rawEvent", &rawEvent, 256000, 99);
    }
    else
    {
        singleEvent = new DPMCRawEvent();
        singleEvent->eventHeader().fRunID = runID;
        singleEvent->eventHeader().fSpillID = 0;

        saveTree->Branch("rawEvent", &singleEvent, 256000, 99);
    }

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
    fillHitsList(theEvent);

    //set the index of tracks and hits
    reIndex();

    //update the dimuon info if needed
    bool dimuonAccepted = false;
    if(rawEvent->getNDimuons() > 0)
    {
        for(unsigned int i = 0; i < rawEvent->getNDimuons(); ++i)
        {
            DPMCDimuon* dimuon = (DPMCDimuon*)rawEvent->getDimuons()->At(i);
            dimuon->fAccepted = tracks[trackIDs[dimuon->fPosTrackID]].first.isAccepted() && tracks[trackIDs[dimuon->fNegTrackID]].first.isAccepted();

            dimuon->fPosTrackID = tracks[trackIDs[dimuon->fPosTrackID]].second ? tracks[trackIDs[dimuon->fPosTrackID]].first.fTrackID : -1;
            dimuon->fNegTrackID = tracks[trackIDs[dimuon->fNegTrackID]].second ? tracks[trackIDs[dimuon->fNegTrackID]].first.fTrackID : -1;

            dimuonAccepted = dimuonAccepted || dimuon->fAccepted;
            if(dimuon->fAccepted) p_config->nEventsAccepted++;
        }
    }
    if(saveMode == INACCONLY && !dimuonAccepted) return;

    //fill the tracks and hits into the event
    for(std::vector<std::pair<DPMCTrack, bool> >::iterator iter = tracks.begin(); iter != tracks.end(); ++iter)
    {
        //only save the tracks that have at least one hits in detector
#ifdef DEBUG_IO
        std::cout << __FILE__ << " " << __FUNCTION__ << " track with ID = " << iter->first.fTrackID << " PDG = " << iter->first.fPDGCode
                  << " has " << iter->first.fHitIDs.size() << " hits, " << (iter->second ? "will be saved" : "will be discarded.") << std::endl;
#endif
        if(iter->second) rawEvent->addTrack(iter->first);
    }
    if((saveMode == HITSONLY || saveMode == PRIMARY) && rawEvent->getNTracks() < 1) return;

    for(std::list<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            rawEvent->addHit(*jter);
        }
    }

    //In dimuon mode, fill the raw event once per event
    if(p_config->dimuonMode)
    {
        finalizeEvent();
    }
    else //in singles mode, merge the events before fill
    {
        //special case of bucket_size == 1
        if(p_config->bucket_size == 1)
        {
            finalizeEvent();
        }
        else
        {
            int eventID = theEvent->GetEventID();
            if(bufferState == CLEAN || (bufferState == INTERNAL && eventID % p_config->bucket_size == 0))
            {
#ifdef DEBUG_IO
                std::cout << __FILE__ << " " << __FUNCTION__ << " singles mode, eventID = " << eventID
                          << ", at the BOB" << std::endl;
#endif
                singleEvent->clear();
                singleEvent->eventHeader().fEventID = eventID/p_config->bucket_size;
                singleEvent->eventHeader().fSigWeight = 0.;

                bufferState = FILLED;
            }

            *singleEvent += (*rawEvent);

            if(bufferState == FLUSH || (bufferState == INTERNAL && ((eventID + 1) % p_config->bucket_size == 0 || eventID + 1 == p_config->nEvents)))
            {
#ifdef DEBUG_IO
                std::cout << __FILE__ << " " << __FUNCTION__ << " singles mode, eventID = " << eventID
                          << ", at the EOB" << std::endl;
#endif
                finalizeEvent();
            }
        }
    }

    //flush the buffer of TTree every 1000 entries
    if(saveTree->GetEntries() % 1000 == 0) saveTree->AutoSave("SaveSelf");
}

void DPIOManager::finalizeEvent()
{
    //Pass the event through post-simulation analysis
    p_triggerAna->analyzeTrigger(rawEvent);
    if(p_config->enableDummyRecon) p_dummyRecon->reconstruct(rawEvent);

#ifdef DEBUG_IO
    if(singleEvent == NULL)
    {
        rawEvent->print();
    }
    else
    {
        singleEvent->print();
    }
#endif

    //save the event
    saveTree->Fill();

    //reset the single flush buffer if necessary
    if(bufferState != INTERNAL) bufferState = CLEAN;

    //flush the buffer of TTree every 1000 entries
    if(saveTree->GetEntries() % 1000 == 0) saveTree->AutoSave("SaveSelf");
}

void DPIOManager::fillOneTrack(const DPMCTrack& mcTrack, bool keep)
{
    assert(trackIDs.find(mcTrack.fTrackID) == trackIDs.end() && "WARNING! Duplicate track index!");

    tracks.push_back(std::make_pair(mcTrack, keep));
    trackIDs.insert(std::map<unsigned int, unsigned int>::value_type(mcTrack.fTrackID, tracks.size()-1));

#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " adding new track with ID = " << mcTrack.fTrackID
              << ", PDG = " << mcTrack.fPDGCode << ", loc = " << tracks.size()-1 << std::endl;
#endif
}

void DPIOManager::updateOneTrack(unsigned int trackID, G4ThreeVector pos, G4ThreeVector mom, bool keep)
{
    assert(trackIDs.find(trackID) != trackIDs.end() && "WARNING! trackID not found!");

#ifdef DEBUG_IO
    std::cout << __FILE__ << " " << __FUNCTION__ << " updating final pos/mom for trackID = " << trackID << std::endl;
#endif

    unsigned int loc = trackIDs[trackID];
    tracks[loc].first.fFinalPos.SetXYZ(pos[0]/cm, pos[1]/cm, pos[2]/cm);
    tracks[loc].first.fFinalMom.SetXYZM(mom[0]/GeV, mom[1]/GeV, mom[2]/GeV, tracks[loc].first.fInitialMom.M());
    tracks[loc].second = keep;
}

void DPIOManager::fillHitsList(const G4Event* theEvent)
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

    //remove duplicate virtual hits before digitization, merge all the energy deposite to the first of the group, then keep the first hit only
    hits.sort();
    DPVirtualHit* p_vHit = NULL;
    for(std::list<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        if(p_vHit == NULL || iter->detectorGroupName != p_vHit->detectorGroupName)  // there comes a new hit
        {
            p_vHit = &(*iter);
        }
        else
        {
            p_vHit->edep += iter->edep;
        }
    }
    hits.unique();

    for(std::list<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        p_digitizer->digitize(*iter);
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
    for(std::list<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            jter->fHitID = hitID++;
            tracks[trackIDs[iter->particleID]].first.addHit(*jter);
        }
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
        else if(saveMode == PRIMARY)
        {
            iter->second = (iter->first.fParentID == 0);
            continue;
        }

        iter->second = !(iter->first.fHitIDs.empty());
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

        if(iter->first.fParentID != 0)  //not a primary particle, at this point the parentID is still geant-assigned ID
        {
            iter->first.fParentPDGCode = tracks[trackIDs[iter->first.fParentID]].first.fPDGCode;
            iter->first.fParentID = tracks[trackIDs[iter->first.fParentID]].first.fTrackID;
        }
        else
        {
            iter->first.fParentPDGCode = 0;
            iter->first.fParentID = -1;
        }
    }

    //set the trackID in the digiHits vector inside each virtual hits
    for(std::list<DPVirtualHit>::iterator iter = hits.begin(); iter != hits.end(); ++iter)
    {
        for(std::vector<DPMCHit>::iterator jter = iter->digiHits.begin(); jter != iter->digiHits.end(); ++jter)
        {
            jter->fTrackID = tracks[trackIDs[iter->particleID]].first.fTrackID;
        }
    }
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
