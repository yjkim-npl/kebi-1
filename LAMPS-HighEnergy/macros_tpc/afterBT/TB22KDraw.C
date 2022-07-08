#include "TB22K.h"

void TB22KDraw(const char* infile = "out_g4_TB22Kmc_afterBT.root")
{
	const int detID[] = {1100,1101,1200,1201,1600};
	const char* tdet[] = {"SC1a","SC1b","BDC1","BDC2","BTOF"};
	const int ndet = sizeof(detID)/sizeof(detID[0]);

	TFile* F = new TFile(infile,"read");
	if(!F || F -> IsZombie()) {cout << Form("Cannot open %s",infile) << endl; return;}

	TH1F* H1_T = (TH1F*) F -> Get("H1_T");
	TH2F* H2_T_E[ndet][6];
	for(int a=0; a<ndet; a++)
	for(int b=0; b<6; b++)
	{
		H2_T_E[a][b] = (TH2F*) F -> Get(Form("H2_T_E_%d_%d",a,b));
	}

	TCanvas* c1[ndet];
	for(int det=0; det<ndet; det++)
	{
		c1[det] = new TCanvas(Form("c1_%d",det),"",1.2*3*600,2*600);
		c1[det] -> Divide(3,2);
		for(int a=0; a<6;a++)
		{
			c1[det] -> cd(a+1);
			H2_T_E[det][a] -> Rebin2D(1e3,1);
			H2_T_E[det][a] -> SetStats(false);
			TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
			leg -> SetFillStyle(0);
			leg -> SetBorderSize(0);
			leg -> SetTextSize(0.04);
			leg -> AddEntry((TObject*)0,Form("%s,%s,%d",tdet[det],IndextoLabel(a).c_str(),(int)H2_T_E[det][a] -> GetEntries()),"h");
			H2_T_E[det][a] -> Draw("colz2");
			leg -> Draw("same");
		}
		c1[det] -> SaveAs(Form("c1_T_E_%s.pdf",tdet[det]));
	}

	TCanvas* c2 = new TCanvas("c2","",1.2*600,600);
	H1_T -> SetStats(false);
	H1_T -> Rebin(1e3);
	TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
	leg -> SetFillStyle(0);
	leg -> SetBorderSize(0);
	leg -> SetTextSize(0.04);
	leg -> AddEntry((TObject*)0,Form("primary p"),"h");
	H1_T -> Draw();
	leg -> Draw("same");
	c2 -> SaveAs("c2_T.pdf");
	cout << "done" << endl;
	return;
}
