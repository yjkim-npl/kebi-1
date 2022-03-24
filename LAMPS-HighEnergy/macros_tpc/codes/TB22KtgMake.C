#include "TB22K.h"

void TB22KtgMake(const char* infile = "sample.root", const char* S = "sample")
{
	TStopwatch sw;
	sw.Start();

	// enum
	enum {EMP,SC1a, SC1b, SC2a, SC2b, ATTPCa, ATTPCb, FTOF, TG};
	const int detID[] = {0,1100,1101,1400,1401,1500,1501,1600,1300};
	const int zdet[] = {0,-900, -200, 290, 290, 410,410,2850,0};
	const char* SDID[] = {"0","1100","1101","1400","1401","1500","1501","1600","1300"};
	const int nSD = sizeof(SDID)/sizeof(SDID[0]) ; // reject target ID
	// Link Input
	//-----------------------------------------
	TFile* F = TFile::Open(infile);
	if (!F || F -> IsZombie()) { cout << Form("Cannot open %s: stop. \n",infile) << endl; return;}
	TTree* T = (TTree*) F -> Get("event");

	TClonesArray* fMCTrack 		= new TClonesArray("KBMCTrack");
//	TClonesArray* fMCPostTrack 	= new TClonesArray("KBMCTrack");
	T -> SetBranchAddress("MCTrack", &fMCTrack);
//	T -> SetBranchAddress("MCPostTrack", &fMCPostTrack);
	TClonesArray* fMCStep[nSD];
	for (int a=0; a<nSD; a++)
	{
		if(a==EMP || a==TG) continue;
		fMCStep[a] = new TClonesArray("KBMCTrack");
		T -> SetBranchAddress(Form("MCStep%s",SDID[a]),&fMCStep[a]);
	}


	// Output structure
	//-----------------------------------------
	TFile* G = new TFile(Form("out_%s.root",S),"recreate");
	TH3F* H3_Edep_SD_pID = new TH3F("H3_Edep_SD_pID","",400,0,20,9,0,9,7,0,7);
//	TH3F* H3_Time_SD_pID = new TH3F("H3_Time_SD_pID","",300,0,15,7,0,7,7,0,7);
	TH3F* H3_SD_detID_pID = new TH3F("H3_SD_detID_pID","",8,0,8,9,0,9,7,0,7);
	H3_Edep_SD_pID -> Sumw2();
//	H3_Time_SD_pID -> Sumw2();
	H3_SD_detID_pID -> Sumw2();

	// Loop over events
	//-----------------------------------------

	const int nEvt = T -> GetEntriesFast();
	for(int a=0; a<nEvt; a++)//event scan
	{
		if( a>0 && a%10==0 ) cout << "Processed : " << a << " th event" << endl;
		//yjkim marked
//		if( a>100 ) continue;
		fMCTrack -> Clear();
//		fMCPostTrack -> Clear();
		fMCStep[SC1a] -> Clear();
		fMCStep[SC1b] -> Clear();
		fMCStep[SC2a] -> Clear();
		fMCStep[SC2b] -> Clear();
		fMCStep[ATTPCa] -> Clear();
		fMCStep[ATTPCb] -> Clear();
		fMCStep[FTOF] -> Clear();

		T -> GetEntry(a);
		const int nMCTrack = fMCTrack -> GetEntriesFast();
//		const int nMCPostTrack = fMCPostTrack -> GetEntriesFast();
//		for(int b=0; b<nMCPostTrack; b++)
		vector<int> vec_track;
		map<int,int> TrkToDet;	// where does track created (MCTrack)
		map<int,int> TrkToPDG;	// what species of track
//		map<int,int> SDToTrk;	// where does track leave hits on det
		set<int> setSD[nSD];
		for(int b=0; b<nMCTrack; b++)	//MCTrack scan
		{
//			KBMCTrack* fTrack = (KBMCTrack*) fMCPostTrack -> At(b);
			KBMCTrack* fTrack = (KBMCTrack*) fMCTrack -> At(b);
			const int fTrkID = fTrack -> GetTrackID();
			const int fDetID = fTrack -> GetDetectorID();
			const int fPDG = fTrack -> GetPDG();
//			const double fvx = fTrack -> GetVX();
//			const double fvy = fTrack -> GetVY();
//			const double fvz = fTrack -> GetVZ();
//			const double fpx = fTrack -> GetPX();
//			const double fpy = fTrack -> GetPY();
//			const double fpz = fTrack -> GetPZ();

			TrkToPDG.insert(pair<int,int>(fTrkID,PDGtoIndex(fPDG)));
			TrkToDet.insert(pair<int,int>(fTrkID,DettoIndex(fDetID)));
//			cout << fTrkID << " " << DettoIndex(fDetID) << endl;
			vec_track.push_back(fTrkID);
		}
		double _Edep[nMCTrack+1][nSD+1];	// time, edep per track and each SD
//		vector<double> _time[nMCTrack+1][nSD];

		for(int b=0; b<nSD; b++)
		{
			if(b==EMP || b==TG) continue;
			const int nMCStep = fMCStep[b] -> GetEntriesFast();

			for(int c=0; c<nMCStep; c++)
			{
				KBMCStep* step = (KBMCStep*)fMCStep[b] -> At(c);
				const int fTrkID = step -> GetTrackID();
				const double fEdep = step -> GetEdep();
				_Edep[fTrkID][b] += fEdep;
				if(!Exist(setSD[b],fTrkID)) setSD[b].insert(fTrkID);
			}
		}


//		for(int b=0; b<nMCStepSC1a; b++)	//SC1a
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[SC1a]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][SC1a].push_back(fTime);
//			_Edep[fTrkID][SC1a] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,SC1a-0.5,TrkToPDG[fTrkID]+0.5);
//			if( TrkToDet[fTrkID] == SC1a ) continue; 
//			if(!Exist(setSD[SC1a],fTrkID)) setSD[SC1a].insert(fTrkID);
////			if(!SDToTrk[fTrkID][SC1a] ) SDToTrk[fTrkID].insert(pair<int,int>(SC1a,fTrkID));
//		}
//		for(int b=0; b<nMCStepSC1b; b++)	//SC1b
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[SC1b]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][SC1b].push_back(fTime);
//			_Edep[fTrkID][SC1b] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,SC1b+0.5,TrkToPDG[fTrkID]+0.5);
//			if( TrkToDet[fTrkID] == SC1b ) continue; 
//			if(!Exist(setSD[SC1b],fTrkID)) setSD[SC1b].insert(fTrkID);
//		}
//		for(int b=0; b<nMCStepSC2a; b++)	//SC2a
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[SC2a]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][SC2a].push_back(fTime);
//			_Edep[fTrkID][SC2a] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,SC2a+0.5,TrkToPDG[fTrkID]+0.5);
////			if( !TrkToSD[fTrkID] ) SDToTrk.insert(pair<int,int>(fTrkID,SC1a));
//			if( TrkToDet[fTrkID] == SC2a ) continue; 
//			if(!Exist(setSD[SC2a],fTrkID)) setSD[SC2a].insert(fTrkID);
//		}
//		for(int b=0; b<nMCStepSC2b; b++)	//SC2b
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[SC2b]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][S2ba].push_back(fTime);
//			_Edep[fTrkID][SC2b] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,SC2b+0.5,TrkToPDG[fTrkID]+0.5);
//			if( TrkToDet[fTrkID] == SC2b ) continue; 
//			if(!Exist(setSD[SC2b],fTrkID)) setSD[SC2b].insert(fTrkID);
////			if(!Exist(setSD[fTrkID],SC2b)) setSD[fTrkID].insert(SC2b);
//		}
////		cout << "yjkim marked" << endl;
////		return;
//		for(int b=0; b<nMCStepATTPCa; b++)	//ATTPCa
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[ATTPCa]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][S2ba].push_back(fTime);
//			_Edep[fTrkID][ATTPCa] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,ATTPCa+0.5,TrkToPDG[fTrkID]+0.5);
//			if( TrkToDet[fTrkID] == ATTPCa ) continue; 
//			if(!Exist(setSD[ATTPCa],fTrkID)) setSD[ATTPCa].insert(fTrkID);
//		}
//		for(int b=0; b<nMCStepATTPCb; b++)	//ATTPCb
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[ATTPCb]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][S2ba].push_back(fTime);
//			_Edep[fTrkID][ATTPCb] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,ATTPCb+0.5,TrkToPDG[fTrkID]+0.5);
//			if( TrkToDet[fTrkID] == ATTPCb ) continue; 
//			if(!Exist(setSD[ATTPCb],fTrkID)) setSD[ATTPCb].insert(fTrkID);
//		}
//		for(int b=0; b<nMCStepFTOF; b++)	//FTOF
//		{
//			KBMCStep* step = (KBMCStep*)fMCStep[FTOF]->At(b);
//			const int fTrkID = step -> GetTrackID();
//			const double fEdep = step -> GetEdep();
////			const double fTime = step -> GetTime();
////			_time[fTrkID][S2ba].push_back(fTime);
//			_Edep[fTrkID][FTOF] += fEdep;
////			H3_Time_SD_pID -> Fill(fTime,FTOF+0.5,TrkToPDG[fTrkID]+0.5);
//			if( TrkToDet[fTrkID] == FTOF ) continue; 
//			if(!Exist(setSD[FTOF],fTrkID)) setSD[FTOF].insert(fTrkID);
//		}

		// storing info track by track
		for(int c=0; c<nMCTrack; c++)	// for all MCTracks
		{
			const int ftrkID = vec_track.at(c);
//			cout << "Energy deposition : ";
//			bool bSD[nSD];
			for(int d=0; d<nSD; d++)		// for 4SC
			{
				if ( _Edep[ftrkID][d] < 0.1 && d==SC1a) continue;
				if ( _Edep[ftrkID][d] ==0) continue;
				H3_Edep_SD_pID -> Fill(_Edep[ftrkID][d],d+0.5,TrkToPDG[ftrkID]);

//				if(Exist(setSD[ftrkID],d) && bSD[d] == false)
				if(Exist(setSD[d],ftrkID))
				{
//					bSD[d] = true;
//					cout << d << " " << TrkToDet[ftrkID] << " " << TrkToPDG[ftrkID] << endl;
					H3_SD_detID_pID -> Fill(d+0.5,TrkToDet[ftrkID],TrkToPDG[ftrkID]);
				}
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
