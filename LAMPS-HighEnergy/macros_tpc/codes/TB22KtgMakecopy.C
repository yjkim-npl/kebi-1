#include "TB22K.h"

void TB22KtgMakecopy(const char* infile = "sample.root", const char* S = "sample")
{
	TStopwatch sw;
	sw.Start();

	// enum
	enum {SC1a, SC1b, SC2a, SC2b, ATTPCa, ATTPCb, FTOF, TG};
//	enum{SC1a};
//	const int detID[] = {0,1100,1101,1400,1401,1500,1501,1600,1300};
//	const char* SDID[] = {"0","1100","1101","1400","1401","1500","1501","1600","1300"};
	const int detID[] = {1100,1101,1400,1401,1500,1501,1600,1300};
	const char* tdet[] = {"SC1a","SC1b","SC2a","SC2b","ATTPCa","ATTPCb","FTOF","TG"};
	const int ndet = sizeof(detID)/sizeof(detID[0]);
	const char* SDID[] = {"1100","1101","1400","1401","1500","1501","1600","1300"};
	const int nSD = sizeof(SDID)/sizeof(SDID[0]) ; // reject target ID
//	const int nSD = 1 ; // temp for SC1a
	const char* tpID[] = {"p","n","gam","e-","e+","ELS"};
	const int npID = sizeof(tpID)/sizeof(tpID[0]);
	// Link Input
	//-----------------------------------------
	TFile* F = TFile::Open(infile);
	if (!F || F -> IsZombie()) { cout << Form("Cannot open %s: stop. \n",infile) << endl; return;}
	TTree* T = (TTree*) F -> Get("event");

	TClonesArray* fMCTrack 		= new TClonesArray("KBMCTrack");
	T -> SetBranchAddress("MCTrack", &fMCTrack);
	TClonesArray* fMCStep[nSD];
	for (int a=0; a<nSD; a++)
	{
		if(a==TG) continue;
		fMCStep[a] = new TClonesArray("KBMCTrack");
		T -> SetBranchAddress(Form("MCStep%s",SDID[a]),&fMCStep[a]);
	}


	// Output structure
	//-----------------------------------------
	TFile* G = new TFile(Form("out_%s.root",S),"recreate");
	TH1F* H1_Edep[ndet][npID];
	for(int det=0; det<ndet; det++)
	for(int n=0; n<npID; n++)
	{
		 H1_Edep[det][n] = new TH1F(Form("H1_Edep_%s_%s",tdet[det],tpID[n]),"",100,0,10);
		 H1_Edep[det][n] -> Sumw2();
	}
//	TH3F* H3_Edep_SD_pID = new TH3F("H3_Edep_SD_pID","",400,0,20,9,0,9,7,0,7);
//	TH3F* H3_Time_SD_pID = new TH3F("H3_Time_SD_pID","",300,0,15,7,0,7,7,0,7);
//	TH3F* H3_SD_detID_pID = new TH3F("H3_SD_detID_pID","",8,0,8,9,0,9,7,0,7);
//	H3_Edep_SD_pID -> Sumw2();
//	H3_Time_SD_pID -> Sumw2();
//	H3_SD_detID_pID -> Sumw2();

	// Loop over events
	//-----------------------------------------

	const int nEvt = T -> GetEntriesFast();
	for(int a=0; a<nEvt; a++)//event scan
	{
		if( a>0 && a%10==0 ) cout << "Processed : " << a << " th event" << endl;
		//yjkim marked
//		if( a>110 ) continue;
		fMCTrack -> Clear();
		for(int det=0; det<ndet; det++)
		{
			if(det==TG) continue;
			fMCStep[det] -> Clear();
		}
//		cout << "possible " << endl;
//		return;

		T -> GetEntry(a);
		const int nMCTrack = fMCTrack -> GetEntriesFast();
		vector<int> vec_track;
//		map<int,int> TrkToDet;	// where does track created (MCTrack)
//		map<int,int> TrkToSD;	// where does track created (MCTrack)
		map<int,int> TrkToPDG;	// what species of track
//		map<int,int> SDToTrk;	// where does track leave hits on det
		set<int> setSD[nSD];
		for(int b=0; b<nMCTrack; b++)	//MCTrack scan
		{
			KBMCTrack* fTrack = (KBMCTrack*) fMCTrack -> At(b);
			const int fTrkID = fTrack -> GetTrackID();
			const int fDetID = fTrack -> GetDetectorID();
			const int fPDG = fTrack -> GetPDG();
//			cout << fPDG << endl;

			TrkToPDG.insert(pair<int,int>(fTrkID,PDGtoIndex(fPDG)));
//			TrkToDet.insert(pair<int,int>(fTrkID,DettoIndex(fDetID)));
			vec_track.push_back(fTrkID);
		}

//		double _Edep[nMCTrack+1][nSD+1];	// time, edep per track and each SD
		double _Edep[nMCTrack][nSD]; memset(_Edep,0,sizeof(_Edep));//std::fill(_Edep[0][0],nMCTrack,0);
//		vector<double> _time[nMCTrack+1][nSD];

		for(int b=0; b<nSD; b++)
		{
			if(b==TG) continue;
//			if(b != SC1a) continue;
			const int nMCStep = fMCStep[b] -> GetEntriesFast();

			for(int c=0; c<nMCStep; c++)
			{
				KBMCStep* step = (KBMCStep*)fMCStep[b] -> At(c);
				const int fTrkID = step -> GetTrackID();
				const double fEdep = step -> GetEdep();
//				cout << fTrkID << endl;
//				if( b==SC1a ) 	cout << fEdep << endl;
				_Edep[fTrkID][b] += fEdep;
//				_Edep[fTrkID][b] += fEdep;
				if(!Exist(setSD[b],fTrkID)) setSD[b].insert(fTrkID);
//				if(!TrkToSD[fTrkID]) TrkToSD.insert(pair<int,int>(fTrkID,b));
			}
		}

//		for(int row=0; row<nMCTrack; row++)
//		for(int col=0; col<nSD-1; col++)
//		{
//			if( _Edep[row][col])	cout << _Edep[row][col] << endl;
//		}
		// storing info track by track
//		for(int det=0; det<nSD; det++)
		for(int c=0; c<nMCTrack; c++)	// for all MCTracks
		{
			const int ftrkID = vec_track.at(c);
			const int pdgIndex = TrkToPDG[ftrkID];
//			if( !TrkToSD[ftrkID]) continue;
			bool active[nSD-1]; fill_n(active,nSD-1,0);
			for(int d=0; d<nSD-1; d++)
			{
				if(_Edep[ftrkID][d] == 0) continue;
				if(Exist(setSD[d],ftrkID))
				{
					active[d] = true;
					H1_Edep[d][pdgIndex] -> Fill(_Edep[ftrkID][d]);
				}
			}
//			const int det = TrkToSD[ftrkID];
//			cout << det << endl
//			cout << pdgIndex << endl;
//			cout << _Edep[ftrkID] << endl;
//			cout << "Energy deposition : ";
//			bool bSD[nSD];
//			for(int d=0; d<nSD; d++)		// for 4SC
			{
//				if ( _Edep[ftrkID][d] < 0.1 && d==SC1a) continue;
//				if ( _Edep[ftrkID][d] ==0) continue;
//				cout << "108 : " << 
//					Form("Edep of %dth track : %f",ftrkID,_Edep[ftrkID]) << endl;
//				H3_Edep_SD_pID -> Fill(_Edep[ftrkID][d],d+0.5,TrkToPDG[ftrkID]);

//				if(Exist(setSD[ftrkID],d) && bSD[d] == false)
//				if(Exist(setSD[d],ftrkID))
//				{
//					bSD[d] = true;
//					cout << d << " " << TrkToDet[ftrkID] << " " << TrkToPDG[ftrkID] << endl;
//					H3_SD_detID_pID -> Fill(d+0.5,TrkToDet[ftrkID],TrkToPDG[ftrkID]);
//				}
			}
//			cout << endl;
//	cout << "done" << endl;
//	return;
		}
	}//event
	G->Write();
	F->Close();
	cout << "done" << endl;
	return;
}
