#ifndef DPMagField_H
#define DPMagField_H

#include "G4MagneticField.hh"
#include "G4String.hh"

#include <vector>
#include <map>
#include <string>

#include "DPSimConfig.h"
#include "DPFieldMap.h"

class DPMagField: public G4MagneticField
{
public:
    explicit DPMagField();
    virtual ~DPMagField();

    //overide the get field method
    void GetFieldValue(const double Point[3], double* Bfield) const;

private:
    //Store the FMag and KMag and any future mags
    std::map<G4String, DPFieldMap*> fieldMaps;
};

#endif
