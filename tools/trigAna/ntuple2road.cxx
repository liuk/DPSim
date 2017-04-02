#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <TFile.h>
#include <TTree.h>

using namespace std;

void calc(vector<double>& data, double& mean, double& rms)
{
	mean = 0.;
	rms = 0.;

	for(unsigned int i = 0; i < data.size(); ++i)
	{
		mean += data[i];
	}
	mean = mean/data.size();

	for(unsigned int i = 0; i < data.size(); ++i)
	{
		rms = rms + (data[i] - mean)*(data[i] - mean);
	}
	rms = sqrt(rms/data.size());
}

int main(int argc, char* argv[])
{
	double zvtx;
    int quadrant;
    int y1, y2, y41, y42;
    int x1, x2, x3, x4;

    TFile* dataFile = new TFile(argv[1], "READ");
    TTree* dataTree = (TTree*)dataFile->Get("save");

    dataTree->SetBranchAddress("zvtx", &zvtx);
    dataTree->SetBranchAddress("quadrant", &quadrant);
    dataTree->SetBranchAddress("y1", &y1);
    dataTree->SetBranchAddress("y2", &y2);
    dataTree->SetBranchAddress("y41", &y41);
    dataTree->SetBranchAddress("y42", &y42);
    dataTree->SetBranchAddress("x1", &x1);
    dataTree->SetBranchAddress("x2", &x2);
    dataTree->SetBranchAddress("x3", &x3);
    dataTree->SetBranchAddress("x4", &x4);

    map<long, vector<double> > roads;
	int choice = atoi(argv[4]);
    for(int i = 0; i < dataTree->GetEntries(); ++i)
    {
    	dataTree->GetEntry(i);

    	long uniqueID = choice == 1 ? (y1%1000) + (y2%1000)*100 + (y41%1000)*10000 + quadrant*1000000
		 							: (x1%1000) + (x2%1000)*100 + (x3%1000)*10000 + (x4%1000)*1000000 + quadrant*100000000;
    	roads[uniqueID].push_back(zvtx);
    }

	TFile* saveFile = new TFile(argv[2], "recreate");
	TTree* saveTree = new TTree("save", "save");

	int nCounts;
	double mean, rms;

	saveTree->Branch("nCounts", &nCounts);
	saveTree->Branch("quadrant", &quadrant);
	saveTree->Branch("mean", &mean);
	saveTree->Branch("rms", &rms);
	saveTree->Branch("y1", &y1);
	saveTree->Branch("y2", &y2);
	saveTree->Branch("y41", &y41);
	saveTree->Branch("x1", &x1);
	saveTree->Branch("x2", &x2);
	saveTree->Branch("x3", &x3);
	saveTree->Branch("x4", &x4);

	fstream fout(argv[3], ios::out);
    for(map<long, vector<double> >::iterator iter = roads.begin(); iter != roads.end(); ++iter)
    {
    	calc(iter->second, mean, rms);
		nCounts = iter->second.size();

		long uID = iter->first;
		int tb;
		int charge;
		if(choice == 1)
		{
			quadrant = uID/1000000;
			y1 = (uID % 100) + (49 + quadrant)*1000;
			y2 = ((uID/100) % 100) + (53 + quadrant)*1000;
			y41 = ((uID/10000) % 100) + (35 + (quadrant == 0 || quadrant == 2 ? 0 : 1))*1000;
    		//cout << iter->first << "  " << iter->second.size() << "  " << mean << "  " << rms << endl;

			saveTree->Fill();

			//ascii out
			if(mean < 350) continue;
			charge = quadrant == 0 || quadrant == 2 ? -1 : 1;
			tb = quadrant <= 1 ? 1 : -1;
			fout << quadrant << "  " << tb*uID << "  " << y1 << "  " << y2 << "   " << y41 << "  " << mean << "  " << nCounts << "  " << rms << endl;
		}
		else
		{
			quadrant = uID/100000000;
			tb = quadrant <= 1 ? 1 : 0;
			x1 = (uID % 100) + (25 + tb)*1000;
			x2 = ((uID/100) % 100) + (31 + tb)*1000;
			x3 = ((uID/10000) % 100) + (33 + tb)*1000;
			x4 = ((uID/1000000) % 100) + (39 + tb)*1000;
			//cout << iter->first << "  " << iter->second.size() << "  " << mean << "  " << rms << endl;

			saveTree->Fill();

			//ascii out
			if(mean < 300) continue;
			charge = quadrant == 0 || quadrant == 2 ? -1 : 1;
			tb = tb > 0 ? -1 : 1;
			fout << quadrant << "  " << tb*uID << "  " << x1 << "  " << x2 << "   " << x3 << "  " << x4 << "  " << mean << "  " << nCounts << "  " << rms << endl;
		}
    }
	fout.close();

	saveFile->cd();
	saveTree->Write();
	saveFile->Close();

	return 0;
}
