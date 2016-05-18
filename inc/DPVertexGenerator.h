#ifndef DPVertexGenerator_H
#define DPVertexGenerator_H

#include <vector>
#include <iostream>

#include "G4String.hh"
#include "G4Material.hh"

#include <TVector3.h>

#include "DPSimConfig.h"
#include "DPMCRawEvent.h"

class DPBeamLineObject
{
public:
    DPBeamLineObject();
    DPBeamLineObject(const G4Material* pMaterial);

    //get the expotential z distribution
    double getZ();

    bool operator < (const DPBeamLineObject& obj) const;
    friend std::ostream& operator << (std::ostream& os, const DPBeamLineObject& obj);

public:
    G4String name;

    //intialized by geometry
    double z_up, z_down, z0; //the z position of upstram/downstream face and center
    double length;           //length of the stuff

    //initialized by material property
    double nucIntLen;        //nuclear interaction length in cm
    double density;          //density in g/cm3
    double Z, A, N;          //number of protons, nucleons, neutrons
    double protonPerc;       //percentage of protons = Z/A;

    //intialized by its neighbours
    double attenuationSelf;  //beam attenuation percentage by itself
    double attenuation;      //beam attenuation factor by this object
    double prob;             //probability of having collision
    double accumulatedProb;  //sum of all the previous/upstream probs
};

class DPVertexGenerator
{
public:
    DPVertexGenerator();
    static DPVertexGenerator* instance();

    //initialize and read the beam line objects from database
    void init();

    //get the vertex generated
    void generateVertex(DPMCDimuon& dimuon);
    double generateVertex();

    //do the actual sampling
    void findInteractingPiece();

    //get the proton/neutron ratio of the piece, must be called after generateVertex
    double getPARatio() { return interactables[index].protonPerc; }

    //get the relative luminosity on this target
    double getLuminosity() { return p_config->biasVertexGen ? interactables[index].prob : probSum; }

    //get the reference to the chosen objects
    const DPBeamLineObject& getInteractable() { return interactables[index]; }

private:
    static DPVertexGenerator* p_vertexGen;

    //pointer to the configuration
    DPSimConfig* p_config;

    //vector of the interactable stuff
    std::vector<DPBeamLineObject> interactables;

    //array of the accumulated probability of each object
    unsigned int nPieces;
    double probSum;
    double accumulatedProbs[100];    //for now set no more than 100 objects

    //the index of the piece that is chosen
    int index;
};

#endif
