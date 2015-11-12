#ifndef DPMagField_H
#define DPMagField_H

#include "G4MagneticField.hh"
#include "G4String.hh"

#include <vector>
#include <map>
#include <string>

#include <TH3D.h>

#include "DPSimConfig.h"

class DPFieldMap
{
public:
    DPFieldMap(G4String name, G4String fieldMapFile, double strength, double z_center);
    ~DPFieldMap();

    //triangular interpolation --- implemented by ROOT
    void GetFieldValue(const double Point[3], double* Bfield) const;

private:
    //3D field grid
    TH3D* grid[3];

    //demension of the grid
    int nx, ny, nz;

    //physical limits of the field
    double xmin, xmax, x0;
    double ymin, ymax, y0;
    double zmin, zmax, z0;
};

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
