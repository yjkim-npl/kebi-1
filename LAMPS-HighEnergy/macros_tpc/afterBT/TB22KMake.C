#include "TB22K.h"

void TB22KMake(const char* infile = "g4_TB22Kmc_afterBT.root")
{
	const int detID[] = {1100,1101,1200,1201,1600};
	const char* tdet[] = {"SC1a","SC1b","BDC1","BDC2","BTOF"};
	const int ndet = sizeof(detID)/sizeof(detID[0]);

	TFile* F = new TFile(infile,"read");
	if(!F || F -> IsZombie()) {cout << Form("Cannot open %s",infile) << endl; return;}
	TTree* T = (TTree*) F -> Get("event");

	TClonesArray* fMCTrack = new TClonesArray("KBMCTrack");
	T -> SetBranchAddress("MCTrack",&fMCTrack);
	TClonesArray* fMCStep[ndet];
	for(int a=0; a<ndet; a++)
	{
		fMCStep[a] = new TClonesArray("KBMCStep");
		T -> SetBranchAddress(Form("MCStep%d",detID[a]),&fMCStep[a]);
	}

	TFile* G = new TFile(Form("out_%s",infile),"recreate");
	TH1F* H1_T = new TH1F("H1_T","",1.2e5,0,1.2e5);
	TH2F* H2_T_E[ndet][6];	// 6:pID
	for(int a=0; a<ndet; a++)
	for(int b=0; b<6; b++)
	{
		H2_T_E[a][b] = new TH2F(Form("H2_T_E_%d_%d",a,b),"",1.3e5,0,1.3e5,150,0,15);
	}

	const int nEvt = T -> GetEntriesFast();
	for(int a=0; a<nEvt; a++)
	{
		T -> GetEntry(a);
		const int nMCTrack = fMCTrack -> GetEntriesFast();

		vector<int> TrkID;
		map<int,int> TrkToPID;
		map<int,double> TrkToE[ndet];
		map<int,double> TrkToT[ndet];

		for(int b=0; b<nMCTrack; b++)
		{
			KBMCTrack* track = (KBMCTrack*) fMCTrack -> At(b);
			const int fTrkID = track -> GetTrackID();
			const int fPDG = track -> GetPDG();
			const int fParentID = track -> GetParentID();
			const double fT = track -> GetTime();
			if(fParentID ==0)	H1_T -> Fill(fT);

			const int PDGidx = PDGtoIndex(fPDG);
			TrkID.push_back(fTrkID);
			TrkToPID.insert(pair<int,int>(fTrkID,PDGidx));
		}

		for(int b=0; b<ndet; b++)
		{
			const int nMCStep = fMCStep[b] -> GetEntriesFast();
			for(int c=0; c<nMCStep; c++)
			{
				KBMCStep* step = (KBMCStep*) fMCStep[b] -> At(c);
				const int fTrkID = step -> GetTrackID();
				const double fEdep = step -> GetEdep();
				const double fTime = step -> GetTime();
				if(TrkToE[b][fTrkID] ==0) TrkToE[b][fTrkID] = fEdep;
				if(TrkToT[b][fTrkID] ==0) TrkToT[b][fTrkID] = fEdep*fTime;
				TrkToE[b][fTrkID] += fEdep;
				TrkToT[b][fTrkID] += fEdep*fTime;
			}
		}

		const int ntrack = TrkID.size();
		for(int b=0; b<ntrack; b++)
		{
			const int trkID = TrkID[b];
			const int Pidx = TrkToPID[trkID];
			for(int c=0; c<ndet; c++)
			{
				const double E = TrkToE[c][trkID];
				if(E ==0) continue;
				const double T = TrkToT[c][trkID]/E;
				H2_T_E[c][Pidx] -> Fill(T,E);
			}
		}
	}
	cout << "done" << endl;
	G -> Write();
	F -> Close();
	return;
}
