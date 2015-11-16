#include "DPMagField.h"
#include "DPSimConfig.h"

#include "G4SystemOfUnits.hh"

#include <fstream>
#include <string>

DPFieldMap::DPFieldMap(G4String name, G4String fieldMapFile, double strength, double zcenter)
{
    using namespace std;

    //load the ascii field map
    ifstream fin(fieldMapFile.c_str());
    if(!fin)
    {
        cerr << "Field map input " << fieldMapFile << " does not exist." << endl;
        throw 1;
    }
    cout << " Initializing magnetic field of " << name << " from field map " << fieldMapFile << endl;

    //Load the range and number of bins in each dimension
    string line;

    getline(fin, line);
    stringstream ssx(line);
    ssx >> nx >> xmin >> xmax;
    xmin = xmin*cm; xmax = xmax*cm;
    x0 = (xmax + xmin)/2.;

    getline(fin, line);
    stringstream ssy(line);
    ssy >> ny >> ymin >> ymax;
    ymin = ymin*cm; ymax = ymax*cm;
    y0 = (ymax + ymin)/2.;

    getline(fin, line);
    stringstream ssz(line);
    ssz >> nz >> zmin >> zmax;
    zmin = zmin*cm; zmax = zmax*cm;
    zmin = zmin + zcenter;
    zmax = zmax + zcenter;
    z0 = (zmax + zmin)/2. + zcenter;

    cout << "  its demensions: " << endl;
    cout << "    X: " << nx << " bins, " << xmin/cm << "cm -- " << xmax/cm << "cm." << endl;
    cout << "    Y: " << ny << " bins, " << ymin/cm << "cm -- " << ymax/cm << "cm." << endl;
    cout << "    Z: " << nz << " bins, " << zmin/cm << "cm -- " << zmax/cm << "cm." << endl;

    //Fill the grid
    for(int i = 0; i < 3; ++i)
    {
        grid[i] = new TH3D(Form("%s_%d", name.c_str(), i), Form("%s_%d", name.c_str(), i),
                           nx, xmin - 0.5*(xmax - xmin)/(nx-1), xmax + 0.5*(xmax - xmin)/(nx-1),
                           ny, ymin - 0.5*(ymax - ymin)/(ny-1), ymax + 0.5*(ymax - ymin)/(ny-1),
                           nz, zmin - 0.5*(zmax - zmin)/(nz-1), zmax + 0.5*(zmax - zmin)/(nz-1));
    }
    while(getline(fin, line))
    {
        double x, y, z, bx, by, bz;

        stringstream ss(line);
        ss >> x >> y >> z >> bx >> by >> bz;

        x = x*cm; y = y*cm; z = z*cm;
        bx = bx*tesla; by = by*tesla; bz = bz*tesla;

        grid[0]->Fill(x, y, z, bx*strength);
        grid[1]->Fill(x, y, z, by*strength);
        grid[2]->Fill(x, y, z, bz*strength);
    }
}

DPFieldMap::~DPFieldMap()
{
    for(int i = 0; i < 3; ++i) delete grid[i];
}

void DPFieldMap::GetFieldValue(const double Point[3], double* Bfield) const
{
    for(int i = 0; i < 3; ++i) Bfield[i] = 0.;
    if(Point[0] < xmin || Point[0] > xmax || Point[1] < ymin || Point[1] > ymax || Point[2] < zmin || Point[2] > zmax) return;

    for(int i = 0; i < 3; ++i) Bfield[i] = grid[i]->Interpolate(Point[0], Point[1], Point[2]);
}

DPMagField::DPMagField()
{
    DPSimConfig* p_config = DPSimConfig::instance();

    //Load KMag
    DPFieldMap* kMagMap = new DPFieldMap("KMag", p_config->kMagMap.Data(), p_config->kMagMultiplier, 1064.26*cm);
    fieldMaps.insert(std::map<TString, DPFieldMap*>::value_type("KMag", kMagMap));

    //Load FMag
    DPFieldMap* fMagMap = new DPFieldMap("FMag", p_config->fMagMap.Data(), p_config->fMagMultiplier, 0.*cm);
    fieldMaps.insert(std::map<TString, DPFieldMap*>::value_type("FMag", fMagMap));
}

DPMagField::~DPMagField()
{
    delete fieldMaps["KMag"];
    delete fieldMaps["FMag"];
}

void DPMagField::GetFieldValue(const double Point[3], double* Bfield) const
{
    double temp[3];
    fieldMaps.find("KMag")->second->GetFieldValue(Point, temp);
    fieldMaps.find("FMag")->second->GetFieldValue(Point, Bfield);

    for(int i = 0; i < 3; ++i) Bfield[i] += temp[i];
}
