#include "TB22K.h"

void test_Draw(const char* suffix = "3",
			   bool PRINT_C   = 0,
			   bool PRINT_PNG = 1)
{
	TString ItemToDraw = "1";
	const char* inFile = "out_sample.root";
	TString instr = inFile;
	instr.ReplaceAll("xx",suffix);

//	pair<int,int> tgdim = {25,25};

	// Link Input
	//----------------------------------------------------------------------
	TFile* F = TFile::Open(inFile);
	if (!F || F -> IsZombie()) { cout << Form("Cannot open %s : stop.\n",inFile) << endl; return;}
	cout << "inFile "<< inFile << " is loaded!" << endl;
	TH1F* H1_Edep_SC1a = (TH1F*)F -> Get("H1_Edep_SC1a");
//	TH3F* H3_Edep_SD_pID = (TH3F*)F->Get("H3_Edep_SD_pID");
//	cout << H3_Edep_SD_pID -> GetEntries() << endl;
//	TH3F* H3_SD_detID_pID = (TH3F*)F->Get("H3_SD_detID_pID");
//	TH3F* H3_Edep_SC_pID = (TH3F*)F->Get("H3_Edep_SC_pID");
//	TH3F* H3_Time_SC_pID = (TH3F*)F->Get("H3_Time_SC_pID");
	enum {EMP, SC1a, SC1b, SC2a, SC2b, ATTPCa, ATTPCb, FTOF, TG};
	const char* tDet[] = {"EMP","SC1a","SC1b","SC2a","SC2b","ATTPCa","ATTPCb","FTOF","TG"};
	const int detID[] = {0,1100,1101,1400,1401,1500,1501,1600,1300};
	const int ndet = 8;
	const int npID = 6;

	// 1 : Edep of SDs
	//----------------------------------------------------------------------
	if ( ItemToDraw.Contains("1") )
	{
		TCanvas* c1 = new TCanvas("c1","",800,600);
		c1 -> SetLogy();
		H1_Edep_SC1a -> Draw();
		H1_Edep_SC1a -> GetXaxis() -> SetRangeUser(0,5);
//		c1 -> Divide(4,2);	//temporary
//		TH3F* H3_E_S_p = (TH3F*)H3_Edep_SD_pID -> Clone();
//		cout << H3_E_S_p -> GetEntries() << endl;
//		TH1F* H1_E[ndet][npID];
//		for(int a=0;a<8;a++)
//		{
//			c1 -> cd(a);	//temp
//			if(a==0) continue;
//			TLegend* leg1 = new TLegend(0.7,0.5,0.9,0.9);
//			for(int b=0; b<npID; b++)
//			{
////				H1_E[a][b] = new TH1F(Form("H1_Edep_%s_%s",IndextoDet(a).c_str(),IndextoLabel(b).c_str()),100,0,100);
//				H1_E[a][b] = new TH1F(Form("H1_Edep_%d_%d",a,b),"",100,0,100);
//				H3_E_S_p -> GetZaxis() -> SetRangeUser(b,b+1);	// proton
//				H3_E_S_p -> GetYaxis() -> SetRangeUser(a,a+1);	// SC1a, SC1b, SC2a, SC2b
////				H3_E_S_p -> GetXaxis() -> SetRangeUser(0,3);
////				TH1F* H1_Edep = (TH1F*)H3_E_S_p -> Project3D("X");			// Edep per SC and proton
//				H1_E[a][b] = (TH1F*)H3_E_S_p -> Project3D("X");			// Edep per SC and proton
//					if( a<3)
//					{
//						H1_E[a][b] -> GetXaxis() -> SetRangeUser(0,5);
//					}
//				if(b==0)
//				{
//					H1_E[a][b] -> GetYaxis() -> SetRangeUser(0.1,1e10);
//					H1_E[a][b] -> GetXaxis() -> SetTitle("Edep[MeV]");
//					gPad -> SetLogy();
//				}
//				H1_E[a][b] -> SetStats(false);
////				H1_E[a][b] -> SetTitle("");
//				H1_E[a][b] -> SetTitle(Form("%s",IndextoDet(a).c_str()));
//				H1_E[a][b] -> SetLineColor(COLOR[b]);
//				H1_E[a][b] -> DrawCopy(b==0?"hist e":"hist e same");
//				const char* leg1info = Form("%s,%i",IndextoLabel(b).c_str(),(int)H1_E[a][b]->GetEntries());
//				leg1 -> AddEntry(H1_E[a][b] -> Clone(), leg1info,"lf");
//			}
//			leg1 -> Draw();
//		}
//
//			}
//			double y  = H1_Edep -> GetMean();
////			cout << tSC[a] << " : " << y << endl;
//			double ye = H1_Edep ->  GetStdDev();
//			H1_E -> SetBinContent(a+1,y);
//		}
//		for(int x=0;x<5;x++)
//		{
//			H1_E -> GetXaxis() -> SetBinLabel(x+1,x==4?"":tSC[x]);
//		}
//		H1_E -> DrawCopy("hist text0");
		if (PRINT_C) 	c1 -> SaveAs(Form("c1_H1_E_SC.C"));
		if (PRINT_PNG)  c1 -> Print(Form("c1_H1_E_SD_%s.png",suffix));

	}

	// 2 : correlation of tracks created and detected
	//----------------------------------------------------------------------
	if ( ItemToDraw.Contains("2") )
	{
//		TCanvas* c2 = new TCanvas("c2","",1600,900);
//		c2 -> Divide(3,2);
//		TH3F* H3_S_D_p = (TH3F*)H3_SD_detID_pID -> Clone();
//		TH2F* H2_S_D[npID];
//		for(int a=0; a<npID; a++)
//		{
//			c2 -> cd(a+1);
//			cout << a << endl;
//			H3_S_D_p -> GetZaxis() -> SetRangeUser(a+0.01,a+0.99);
//			H2_S_D[a] = (TH2F*)H3_S_D_p -> Project3D("YX");
//			H2_S_D[a] -> SetStats(false);
//			H2_S_D[a] -> SetTitle(Form("%s",IndextoLabel(a).c_str()));
//			const int nbinsx = H2_S_D[a] -> GetNbinsX();
//			const int nbinsy = H2_S_D[a] -> GetNbinsY();
//			for(int x=0; x<nbinsx; x++)
//			{
//				H2_S_D[a] -> GetXaxis() -> SetBinLabel(x+1,tDet[x]);
//			}
//			for(int y=0; y<nbinsy; y++)
//			{
//				H2_S_D[a] -> GetYaxis() -> SetBinLabel(y+1,tDet[y]);
//			}
////			H2_S_D[a]
//			H2_S_D[a] -> DrawCopy("text0 colz");
////			TLegend* leg2 = new TLegend(0.7,0.5,0.9,0.9);
//
//		}

//				H1_E[a][b] -> SetStats(false);
//		if (PRINT_C) 	c2 -> SaveAs(Form("c2_H1_E_SC.C"));
//		if (PRINT_PNG)  c2 -> Print(Form("c2_H2_SD_det_%s.png",suffix));
	}

	// 3 : Time profile at each SC per pID
//	//----------------------------------------------------------------------	
//	if ( ItemToDraw.Contains("3") )
//	{
//		TCanvas* c3 = new TCanvas("c3","",1600,900);
//		c3 -> Divide(2,2);
//		
//		TH3F* H3_T_S_p = (TH3F*)F -> Get("H3_Time_SC_pID");
//		TH1F* H1_T[4][6];	// 4 : SC, 6 : pID
//		for(int a=0; a < 4; a++)
//		{
//			c3 -> cd(a+1);
//			TLegend* leg1 = new TLegend(0.7,0.5,0.9,0.9);
//			for(int b=0;b<6;b++)//pID
//			{
//				H3_T_S_p -> GetXaxis() -> SetRangeUser(0,15);
//				H3_T_S_p -> GetYaxis() -> SetRangeUser(a,a+1);
//				H3_T_S_p -> GetZaxis() -> SetRangeUser(b,b+1);
//				H1_T[a][b] = (TH1F*) H3_T_S_p -> Project3D("X");
//				if(b==0)
//				{
//					H1_T[a][b] -> GetXaxis() -> SetRangeUser(0,15);
//					H1_T[a][b] -> GetYaxis() -> SetRangeUser(0.1,5e4);
//					H1_T[a][b] -> GetXaxis() -> SetTitle("Time");
//					gPad -> SetLogy();
//				}
//				H1_T[a][b] -> SetStats(false);
//				H1_T[a][b] -> SetTitle("");
//				H1_T[a][b] -> SetLineColor(COLOR[b]);
//				H1_T[a][b] -> Rebin(5);
//				H1_T[a][b] -> DrawCopy(b==0?"hist e":"hist e same");
//				const char* leg1info = Form("%s",IndextoLabel(b).c_str());
//				leg1 -> AddEntry(H1_T[a][b] -> Clone(), leg1info, "lf");
//			}
//			leg1 -> Draw();
//		}
//	if (PRINT_C)	c3 -> SaveAs(Form("c3_H1_T_SC.C"));
//	if (PRINT_PNG)	c3 -> Print(Form("c3_H1_T_SC.png"));
//	}
//
//	// 4 : composition from EppID
//	//----------------------------------------------------------------------
//	if ( ItemToDraw.Contains("4") )
//	{
//		TCanvas* c4 = new TCanvas("c4","",800,500);
//		c4 -> cd(1) -> SetGrid();
//		TH2F* H2_EppID = (TH2F*)F -> Get("EppID");
//		TH1F* H1_pID = (TH1F*)H2_EppID -> ProjectionY(Form("%s_pID",H2_EppID -> GetName()),0,101);
//		const int nbins = H1_pID -> GetNbinsX();
//		for(int x=0;x<nbins;x++)
//		{
//			H1_pID -> GetXaxis() -> SetBinLabel(x+1,IndextoLabel(x).c_str());
////			H1_pID -> SetFillColor(3);
////			H1_pID -> SetFillStyle(3021);
//		}
//		H1_pID -> SetTitle(Form("compo at z=%s;;#",suffix));
//		H1_pID -> GetXaxis() -> SetLabelSize(0.04);
//		
//		H1_pID -> DrawCopy("colz text0");
//		if (PRINT_C) 	c4 -> SaveAs(Form("c4_H1_compo_E.C"));
//		if (PRINT_PNG)  c4 -> Print(Form("c4_H1_compo_E_%s.png",suffix));
//	}
//	//----------------------------------------------------------------------
	return;
}
