#include "DPMagField.h"

#include "G4SystemOfUnits.hh"

#include <fstream>
#include <string>

DPMagField::DPMagField()
{
    DPSimConfig* p_config = DPSimConfig::instance();

    //Load KMag
    DPFieldMap* kMagMap = new DPFieldMap("KMag", p_config->kMagMap.Data(), p_config->kMagMultiplier, 1064.26);
    fieldMaps.insert(std::map<G4String, DPFieldMap*>::value_type("KMag", kMagMap));

    //Load FMag
    DPFieldMap* fMagMap = new DPFieldMap("FMag", p_config->fMagMap.Data(), p_config->fMagMultiplier, 0.);
    fieldMaps.insert(std::map<G4String, DPFieldMap*>::value_type("FMag", fMagMap));
}

DPMagField::~DPMagField()
{
    delete fieldMaps["KMag"];
    delete fieldMaps["FMag"];
}

void DPMagField::GetFieldValue(const double Point[3], double* Bfield) const
{
    double pos[3];
    for(int i = 0; i < 3; ++i) pos[i] = Point[i]/cm;

    double temp[3];
    fieldMaps.find("KMag")->second->GetFieldValue(pos, temp);
    fieldMaps.find("FMag")->second->GetFieldValue(pos, Bfield);

    for(int i = 0; i < 3; ++i) Bfield[i] = (Bfield[i] + temp[i])*tesla;
}
