#include "DPSimConfig.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <wordexp.h> //to expand environmentals

#include <boost/algorithm/string.hpp> //for strip
#include <boost/lexical_cast.hpp>

ClassImp(DPSimConfig)

using namespace std;

DPSimConfig* DPSimConfig::p_config = NULL;
DPSimConfig* DPSimConfig::instance()
{
    if(p_config == NULL) p_config = new DPSimConfig;
    return p_config;
}

DPSimConfig::DPSimConfig() {}

void DPSimConfig::init(TString configFile)
{
    parseConfig(configFile);

    //Fill the content
    seed = pInt("seed", 0);
    nEvents = pInt("nEvents", 100);
    printFreq = pInt("printFreq", 10);
    version = pString("version");

    bucket_size = pInt("bucket_size", 40000);
    beamMomentum = pDouble("beamMomentum", 120.);
    beamCurrent = pDouble("beamCurrent", 1.E12);

    geometryGDMLInput = pString("geometryGDMLInput");
    geometrySchema = pString("geometrySchema");
    mysqlServer = pString("mysqlServer");
    mysqlPort = pInt("mysqlPort", 3306);
    login = pString("login");
    password = pString("password");

    fMagMap = pString("fMagMap");
    kMagMap = pString("kMagMap");
    fMagMultiplier = pDouble("fMagMultiplier", 1.);
    kMagMultiplier = pDouble("kMagMultiplier", 1.);

    generator = pString("generator");
    eventPos = pString("eventPos");

    configFileName = configFile;
    outputFileName = pString("outputFileName");
    customInput = pString("customInput");
    outputMode = pString("outputMode");

    x1Min = pDouble("x1Min", 0.);
    x1Max = pDouble("x1Max", 1.);
    x2Min = pDouble("x2Min", 0.);
    x2Max = pDouble("x2Max", 1.);
    xfMin = pDouble("xfMin", -1.);
    xfMax = pDouble("xfMax", 1.);
    massMin = pDouble("massMin", 0.22);
    massMax = pDouble("massMax", 10.);
    cosThetaMin = pDouble("cosThetaMin", -1.);
    cosThetaMax = pDouble("cosThetaMax", 1.);

    nEventsThrown = 0;
    nEventsPhysics = 0;
    nEventsAccepted = 0;
}

void DPSimConfig::parseConfig(TString configFile)
{
    symbols.clear();
    configFileName = expandEnv(configFile);

    ifstream fin(configFileName.Data());
    if(!fin)
    {
        cerr << "ERROR: configuration file " << configFileName << " not found!" << endl;
        throw 1;
    }

    string line, key, val;
    while(getline(fin, line))
    {
        boost::algorithm::trim(line);
        if(line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        ss >> key >> val;

        TString valExp = expandEnv(val);
        symbols.insert(map<TString, TString>::value_type(key, valExp));
    }
    fin.close();
}

TString DPSimConfig::expandEnv(const TString& input) const
{
    wordexp_t exp_result;
    if(wordexp(input.Data(), &exp_result, 0) != 0)
    {
        cerr << "ERROR: input " << input << " cannot be understood! " << endl;
        throw 1;
    }

    const TString output(exp_result.we_wordv[0]);
    return output;
}

TString DPSimConfig::pString(TString name)
{
    if(symbols.find(name) != symbols.end()) return symbols[name];
    return TString("");
}

bool DPSimConfig::pBool(TString name, bool default_val)
{
    TString val = pString(name);
    if((val == "yes") || (val == "Yes") || (val == "YES") || (val == "true") || (val == "True") || (val == "TRUE"))
    {
        return true;
    }

    return false;
}

Double_t DPSimConfig::pDouble(TString name, Double_t default_val)
{
    TString val = pString(name);
    if(val.Length() != 0) return atof(val.Data());
    return default_val;
}

Int_t DPSimConfig::pInt(TString name, Int_t default_val)
{
    TString val = pString(name);
    if(val.Length() != 0) return atoi(val.Data());
    return default_val;
}
