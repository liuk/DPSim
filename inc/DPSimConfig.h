#ifndef DPSimConfig_H
#define DPSimConfig_H

#include <map>
#include <string>

#include <TROOT.h>
#include <TObject.h>
#include <TString.h>

class DPSimConfig: public TObject
{
public:
    static DPSimConfig* instance();
    DPSimConfig();

    //parse the config file and set the config
    void init(TString configFile);

public:
    //Random seed
    Int_t seed;

    //Number of events to run
    Int_t nEvents;
    Int_t printFreq;

    //MC version hash string
    TString version;

    //beam setup
    Int_t    bucket_size;        // How many protons per event in gun generator
    Double_t beamMomentum;
    Double_t beamCurrent;

    //Detector setup
    TString geometryGDMLInput;// The GDML file for geometry
    TString geometrySchema;   // The sql schema that GMC pulls the geometry information from
    TString mysqlServer;      // Address of the SQL Server, shouldn't need to modify
    Int_t   mysqlPort;          // The port number for MySQL
    TString login;            // The login for the SQL server
    TString password;         // The password for the SQL server

    //Magnetic field setup
    TString fMagMap;          // Name of the ascii text file that contains the fmag map
    TString kMagMap;          // Name of the ascii text file that contains the kmag map
    Double_t fMagMultiplier;  // Multiplies the strength of FMAG's field
    Double_t kMagMultiplier;  // Multiplies the strength of KMAG's field

    //Event generation setup
    TString generator;        // The type of event generator running, i.e. gun or dimuon
    TString eventPos;         // Where the event is generated, target, dump or both.  Doesn't affect gun generator
    
    //I/O setup
    TString configFileName;   // name of configuration file
    TString outputFileName;   // The database name that output goes to
    TString customInput;      // Input ROOT file containing the generator info

    //phase space constrain
    Double_t x1Min;
    Double_t x1Max;
    Double_t x2Min;
    Double_t x2Max;
    Double_t xfMin;
    Double_t xfMax;
    Double_t massMin;
    Double_t massMax;
    Double_t cosThetaMin;
    Double_t cosThetaMax;

    //Run-accumulated variables
    Int_t nEventsThrown;
    Int_t nEventsPhysics;
    Int_t nEventsAccepted;  //TODO: add this in the IO manager to keep track of in-acceptance dimuons

private: //used only for parsing
    //general parser
    void parseConfig(TString configFile);
    TString expandEnv(const TString& input) const;

    // get string/bool/float/int config entry
    TString  pString(TString name);
    bool     pBool(TString name, bool default_val);
    Double_t pDouble(TString name, Double_t default_val);
    Int_t    pInt(TString name, Int_t default_val);

    // config group symbol map
    std::map<TString, TString> symbols;

    //static pointer
    static DPSimConfig* p_config;

    ClassDef(DPSimConfig, 1)
};

#endif
