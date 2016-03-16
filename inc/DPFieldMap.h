#ifndef DPFieldMap_H
#define DPFieldMap_H

#include <TString.h>
#include <TH3D.h>

#include "DPSimConfig.h"

class DPFieldMap
{
public:
    DPFieldMap(TString name, TString fieldMapFile, double strength, double z_center);
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

#endif
