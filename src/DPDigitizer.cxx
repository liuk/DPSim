#include "DPDigitizer.h"
#include "DPSimConfig.h"

#include <iomanip>
#include <cmath>

#include "G4SystemOfUnits.hh"

#include <TMath.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include <boost/lexical_cast.hpp>

void DPDigiPlane::preCalculation()
{
    sinth = TMath::Sin(angleFromVert + rZ);
    costh = TMath::Cos(angleFromVert + rZ);
    tanth = TMath::Tan(angleFromVert + rZ);

    wc = getW(xc, yc);

    rotM[0][0] = TMath::Cos(rZ)*TMath::Cos(rY);
    rotM[0][1] = TMath::Cos(rZ)*TMath::Sin(rX)*TMath::Sin(rY) - TMath::Cos(rX)*TMath::Sin(rZ);
    rotM[0][2] = TMath::Cos(rX)*TMath::Cos(rZ)*TMath::Sin(rY) + TMath::Sin(rX)*TMath::Sin(rZ);
    rotM[1][0] = TMath::Sin(rZ)*TMath::Cos(rY);
    rotM[1][1] = TMath::Sin(rZ)*TMath::Sin(rX)*TMath::Sin(rY) + TMath::Cos(rZ)*TMath::Cos(rX);
    rotM[1][2] = TMath::Sin(rZ)*TMath::Sin(rY)*TMath::Cos(rX) - TMath::Cos(rZ)*TMath::Sin(rX);
    rotM[2][0] = -TMath::Sin(rY);
    rotM[2][1] = TMath::Cos(rY)*TMath::Sin(rX);
    rotM[2][2] = TMath::Cos(rY)*TMath::Cos(rX);

    uVec[0] = TMath::Cos(angleFromVert);
    uVec[1] = TMath::Sin(angleFromVert);
    uVec[2] = 0.;

    vVec[0] = -TMath::Sin(angleFromVert);
    vVec[1] = TMath::Cos(angleFromVert);
    vVec[2] = 0.;

    nVec[0] = 0.;
    nVec[1] = 0.;
    nVec[2] = 1.;

    xVec[0] = 1.;
    xVec[1] = 0.;
    xVec[2] = 0.;

    yVec[0] = 0.;
    yVec[1] = 1.;
    yVec[2] = 0.;

    //rotate u/v vector by the rotation matrix
    double temp[3];
    for(int i = 0; i < 3; ++i) temp[i] = uVec[i];
    for(int i = 0; i < 3; ++i)
    {
        uVec[i] = 0.;
        for(int j = 0; j < 3; ++j) uVec[i] += rotM[i][j]*temp[j];
    }

    for(int i = 0; i < 3; ++i) temp[i] = vVec[i];
    for(int i = 0; i < 3; ++i)
    {
        vVec[i] = 0.;
        for(int j = 0; j < 3; ++j) vVec[i] += rotM[i][j]*temp[j];
    }

    for(int i = 0; i < 3; ++i) temp[i] = xVec[i];
    for(int i = 0; i < 3; ++i)
    {
        xVec[i] = 0.;
        for(int j = 0; j < 3; ++j) xVec[i] += rotM[i][j]*temp[j];
    }

    for(int i = 0; i < 3; ++i) temp[i] = yVec[i];
    for(int i = 0; i < 3; ++i)
    {
        yVec[i] = 0.;
        for(int j = 0; j < 3; ++j) yVec[i] += rotM[i][j]*temp[j];
    }

    //n vector is the cross product of u and v
    nVec[0] = uVec[1]*vVec[2] - vVec[1]*uVec[2];
    nVec[1] = uVec[2]*vVec[0] - vVec[2]*uVec[0];
    nVec[2] = uVec[0]*vVec[1] - vVec[0]*uVec[1];
}

bool DPDigiPlane::intercept(double tx, double ty, double x0, double y0, G4ThreeVector& pos, double& w)
{
    //Refer to http://geomalgorithms.com/a05-_intersect-1.html
    //double u[3] = {tx, ty, 1};
    //double p0[3] = {x0, y0, 0};
    //double v0[3] = {xc, yc, zc};
    //double n[3] = nVec[3];
    //double w[3] = p0[3] - v0[3];

    double det = tx*nVec[0] + ty*nVec[1] + nVec[2];
    double dpos[3] = {x0 - xc, y0 - yc, -zc};
    double si = -(nVec[0]*dpos[0] + nVec[1]*dpos[1] + nVec[2]*dpos[2])/det;
    double vcp[3] = {vVec[1] - vVec[2]*ty, vVec[2]*tx - vVec[0], vVec[0]*ty - vVec[1]*tx};

    pos[0] = x0 + tx*si;
    pos[1] = y0 + ty*si;
    pos[2] = si;

    w = (vcp[0]*dpos[0] + vcp[1]*dpos[1] + vcp[2]*dpos[2])/det;

    return isInPlane(pos[0], pos[1], pos[2]);
}

std::ostream& operator << (std::ostream& os, const DPDigiPlane& plane)
{
    os << "DigiPlane ID = " << plane.detectorID << ", name = " << plane.detectorName << " belongs to group " << plane.detectorGroupName << "\n"
       << "   nElements = " << plane.nElements << ",  center x = " << plane.xc << ", y = " << plane.yc << ", z = " << plane.zc;
    return os;
}

DPDigitizer* DPDigitizer::p_digitizer = NULL;
DPDigitizer* DPDigitizer::instance()
{
    if(p_digitizer == NULL) p_digitizer = new DPDigitizer();
    return p_digitizer;
}

DPDigitizer::DPDigitizer()
{
    DPSimConfig* p_config = DPSimConfig::instance();

    //Load basic setup
    char query[500];
    sprintf(query, "SELECT Planes.detectorName,spacing,cellWidth,overlap,numElements,angleFromVert,"
                   "xPrimeOffset,x0+deltaX,y0+deltaY,z0+deltaZ,planeWidth,planeHeight,theta_x+rotX,"
                   "theta_y+rotY,theta_z+rotZ,Planes.detectorID,Planes.detectorGroupName,triggerLv "
                   "FROM %s.Planes,%s.Alignments WHERE Planes.detectorName=Alignments.detectorName",
                   p_config->geometrySchema.Data(), p_config->geometrySchema.Data());
    TSQLServer* server = TSQLServer::Connect(Form("mysql://%s:%d", p_config->mysqlServer.Data(), p_config->mysqlPort), p_config->login, p_config->password);
    TSQLResult* res = server->Query(query);

    map_groupID.clear();
    map_detectorID.clear();

    unsigned int nRows = res->GetRowCount();
    for(unsigned int i = 0; i < nRows; ++i)
    {
        TSQLRow* row = res->Next();
        int index = boost::lexical_cast<int>(row->GetField(15));

        assert(index <= NDETPLANES && "detectorID from database exceeds upper limit!");

        digiPlanes[index].detectorID = index;
        digiPlanes[index].detectorName = row->GetField(0);
        digiPlanes[index].detectorGroupName = row->GetField(16);
        digiPlanes[index].spacing = boost::lexical_cast<double>(row->GetField(1));
        digiPlanes[index].cellWidth = boost::lexical_cast<double>(row->GetField(2));
        digiPlanes[index].overlap = boost::lexical_cast<double>(row->GetField(3));
        digiPlanes[index].nElements = boost::lexical_cast<int>(row->GetField(4));
        digiPlanes[index].angleFromVert = boost::lexical_cast<double>(row->GetField(5));
        digiPlanes[index].xPrimeOffset = boost::lexical_cast<double>(row->GetField(6));
        digiPlanes[index].xc = boost::lexical_cast<double>(row->GetField(7));
        digiPlanes[index].yc = boost::lexical_cast<double>(row->GetField(8));
        digiPlanes[index].zc = boost::lexical_cast<double>(row->GetField(9));
        digiPlanes[index].planeWidth = boost::lexical_cast<double>(row->GetField(10));
        digiPlanes[index].planeHeight = boost::lexical_cast<double>(row->GetField(11));
        digiPlanes[index].rX = boost::lexical_cast<double>(row->GetField(12));
        digiPlanes[index].rY = boost::lexical_cast<double>(row->GetField(13));
        digiPlanes[index].rZ = boost::lexical_cast<double>(row->GetField(14));
        digiPlanes[index].triggerLv = boost::lexical_cast<int>(row->GetField(15));
        digiPlanes[index].preCalculation();

        map_groupID[digiPlanes[index].detectorGroupName].push_back(index);
        map_detectorID[digiPlanes[index].detectorName] = digiPlanes[index].detectorID;

#ifdef DEBUG_IN
        std::cout << digiPlanes[index] << std::endl;
#endif
        delete row;
    }

    delete res;
    delete server;
}

void DPDigitizer::digitize(DPVirtualHit& vHit)
{
    // calculate the central position in each detector group, then linearly extrapolate the hits
    // to each individual plane, this is assuming there is no magnetic field in the detector, or
    // the bending is negligible
    double tx = vHit.mom[0]/vHit.mom[2];
    double ty = vHit.mom[1]/vHit.mom[2];
    double x0 = (vHit.pos[0] - tx*vHit.pos[2])/cm;
    double y0 = (vHit.pos[1] - ty*vHit.pos[2])/cm;

    //temporary variabels
    double w;
    G4ThreeVector pos;
    for(std::vector<int>::iterator dpid = map_groupID[vHit.detectorGroupName].begin(); dpid != map_groupID[vHit.detectorGroupName].end(); ++dpid)
    {
        //check if the track intercepts the plane
        if(!digiPlanes[*dpid].intercept(tx, ty, x0, y0, pos, w)) continue;

        int elementID = TMath::Nint((digiPlanes[*dpid].nElements + 1.0)/2.0 + (w - digiPlanes[*dpid].xPrimeOffset)/digiPlanes[*dpid].spacing);
        double driftDistance = w - digiPlanes[*dpid].spacing*(elementID - digiPlanes[*dpid].nElements/2. - 0.5) - digiPlanes[*dpid].xPrimeOffset;
        if(elementID < 1 || elementID > digiPlanes[*dpid].nElements || fabs(driftDistance) > 0.5*digiPlanes[*dpid].cellWidth) continue;

        DPMCHit digiHit;
        digiHit.fDetectorID = digiPlanes[*dpid].detectorID;
        digiHit.fElementID = elementID;
        digiHit.fDriftDistance = driftDistance;
        digiHit.fMomentum.SetXYZ(vHit.mom[0]/GeV, vHit.mom[1]/GeV, vHit.mom[2]/GeV);
        digiHit.fPosition.SetXYZ(pos[0], pos[1], pos[2]);

        vHit.digiHits.push_back(digiHit);

        //see if it also hits the next elements in the overlap region
        if(fabs(driftDistance) > 0.5*digiPlanes[*dpid].cellWidth - digiPlanes[*dpid].overlap)
        {
            if(driftDistance > 0. && elementID != digiPlanes[*dpid].nElements)
            {
                digiHit.fElementID = elementID + 1;
                digiHit.fDriftDistance = driftDistance - digiPlanes[*dpid].spacing;
                vHit.digiHits.push_back(digiHit);
            }
            else if(driftDistance < 0. && elementID != 1)
            {
                digiHit.fElementID = elementID - 1;
                digiHit.fDriftDistance = driftDistance + digiPlanes[*dpid].spacing;
                vHit.digiHits.push_back(digiHit);
            }
        }
    }
}
