#ifndef DPDigitizer_H
#define DPDigitizer_H

#include <vector>
#include <map>

#include "G4String.hh"

#include "DPVirtualHit.h"

class DPDigiPlane
{
public:
    //calculates all the derived variables
    void preCalculation();

    //intercepts with a 3-D straight line
    void intercept(double tx, double ty, double x0, double y0, G4ThreeVector& pos, double& w);

    //X, Y, U, V conversion
    double getX(double w, double y) const { return w/costh - y*tanth; }
    double getY(double w, double x) const { return w/sinth - x/tanth; }
    double getW(double x, double y) const { return x*costh + y*sinth; }

    //get the projection in horizontal/vertical direction
    double getW(double x, double y, double z) { return (x-xc)*uVec[0] + (y-yc)*uVec[1] + (z-zc)*uVec[2]; }
    double getH(double x, double y, double z) { return (x-xc)*xVec[0] + (y-yc)*xVec[1] + (z-zc)*xVec[2]; }
    double getV(double x, double y, double z) { return (x-xc)*yVec[0] + (y-yc)*yVec[1] + (z-zc)*yVec[2]; }

    //stream output
    friend std::ostream& operator << (std::ostream& os, const DPDigiPlane& plane);

public:
    int detectorID;
    G4String detectorGroupName;     //large detector group that this plane belongs to
    G4String detectorName;          //its own specific name

    //geometric specifications
    double spacing;
    double cellWidth;
    double angleFromVert;
    double xPrimeOffset;
    double planeWidth;
    double planeHeight;
    double overlap;
    int nElements;

    //3-D position and rotations
    double xc, yc, zc;
    double rX, rY, rZ;

    //pre-calculated variabels for projections
    double wc;        //center of the plane in its measuring direction
    double costh, sinth, tanth;
    double nVec[3];    //perpendicular to plane
    double uVec[3];    //measuring direction
    double vVec[3];    //perpendicular to measuring direction
    double xVec[3];    //X-axis, horizontal
    double yVec[3];    //Y-axis, vertical
    double rotM[3][3]; //rotation matrix
};

class DPDigitizer
{
public:
    DPDigitizer();
    static DPDigitizer* instance();

    //main external call, fill the digi hit vector
    void digitize(DPVirtualHit& vHit);

private:
    static DPDigitizer* p_digitizer;

    //map from geant sensitive detector name to the list of digi planes
    std::map<G4String, std::vector<DPDigiPlane> > digiPlanes;
};

#endif
