#include "DPDigitizer.h"
#include "DPSimConfig.h"
#include "DPMCRawEvent.h"

#include <cmath>

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
        for(int j = 0; i < 3; ++j) uVec[i] += rotM[i][j]*temp[j];
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

void DPDigiPlane::intercept(double tx, double ty, double x0, double y0, G4ThreeVector& pos, double& w)
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
                   "xPrimeOffset,x0+deltaX,y0+deltaY,z0+deltaZ,planeWidth,planeHeight,theta_x+rotateAboutX,"
                   "theta_y+rotateAboutY,theta_z+rotateAboutZ,detectorID FROM %s.Planes,%s.PlaneOffsets WHERE "
                   "Planes.detectorName=PlaneOffsets.detectorName and (Planes.detectorName LIKE 'H__' OR "
                   "Planes.detectorName LIKE 'H_____')", p_config->geometrySchema.Data(), p_config->geometrySchema.Data());
    TSQLServer* server = TSQLServer::Connect(Form("mysql://%s:%d", p_config->mysqlServer.Data(), p_config->mysqlPort), p_config->login, p_config->password);
    TSQLResult* res = server->Query(query);

    digiPlanes.clear();
    unsigned int nRows = res->GetRowCount();
    for(unsigned int i = 0; i < nRows; ++i)
    {
        TSQLRow* row = res->Next();

        DPDigiPlane digiPlane;
        digiPlane.detectorID = boost::lexical_cast<int>(row->GetField(15));
        digiPlane.detectorName = row->GetField(0);
        digiPlane.detectorGroupName = digiPlane.detectorName(0, 2);
        digiPlane.spacing = boost::lexical_cast<double>(row->GetField(1));
        digiPlane.cellWidth = boost::lexical_cast<double>(row->GetField(2));
        digiPlane.overlap = boost::lexical_cast<double>(row->GetField(3));
        digiPlane.nElements = boost::lexical_cast<int>(row->GetField(4));
        digiPlane.angleFromVert = boost::lexical_cast<double>(row->GetField(5));
        digiPlane.xPrimeOffset = boost::lexical_cast<double>(row->GetField(6));
        digiPlane.xc = boost::lexical_cast<double>(row->GetField(7));
        digiPlane.yc = boost::lexical_cast<double>(row->GetField(8));
        digiPlane.zc = boost::lexical_cast<double>(row->GetField(9));
        digiPlane.planeWidth = boost::lexical_cast<double>(row->GetField(10));
        digiPlane.planeHeight = boost::lexical_cast<double>(row->GetField(11));
        digiPlane.rX = boost::lexical_cast<double>(row->GetField(12));
        digiPlane.rY = boost::lexical_cast<double>(row->GetField(13));
        digiPlane.rZ = boost::lexical_cast<double>(row->GetField(14));

        digiPlane.preCalculation();
        digiPlanes[digiPlane.detectorGroupName].push_back(digiPlane);

        delete row;
    }

    delete res;
    delete server;
}

void DPDigitizer::digitize(DPVirtualHit& vHit)
{
    double tx = vHit.mom[0]/vHit.mom[2];
    double ty = vHit.mom[1]/vHit.mom[2];
    double x0 = (vHit.pos[0] - tx*vHit.pos[2])/cm;
    double y0 = (vHit.pos[1] - ty*vHit.pos[2])/cm;

    for(std::vector<DPDigiPlane>::iterator dp = digiPlanes[vHit.detectorGroupName].begin(); dp != digiPlanes[vHit.detectorGroupName].end(); ++dp)
    {
        double w;
        G4ThreeVector pos;
        dp->intercept(tx, ty, x0, y0, pos, w);

        //drop if this hit is out of the detector plane size
        if(fabs(dp->getH(pos[0], pos[1], pos[2])) > 0.5*dp->planeWidth || fabs(dp->getV(pos[0], pos[1], pos[2])) > 0.5*dp->planeHeight) continue;

        int elementID = TMath::Nint((dp->nElements + 1.0)/2.0 + (w - dp->xPrimeOffset)/dp->spacing) - 1;
        double driftDistance = w - dp->spacing*(elementID + 0.5 - dp->nElements/2.) - dp->xPrimeOffset;
        if(elementID > dp->nElements || fabs(driftDistance) > 0.5*dp->cellWidth) continue;

        DPMCHit digiHit;
        digiHit.fDetectorID = dp->detectorID;
        digiHit.fElementID = elementID;
        digiHit.fDriftDistance = driftDistance;
        digiHit.fMomentum.SetXYZ(vHit.mom[0]/GeV, vHit.mom[1]/GeV, vHit.mom[2]/GeV);
        digiHit.fPosition.SetXYZ(pos[0], pos[1], pos[2]);

        vHit.digiHits.push_back(digiHit);
    }
}
