#include "TBox.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TExec.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLegend.h"
#include "TLine.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTree.h"
#include "TPad.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

// Constants, FIXED
//-------------------------------------------------------------------------

const float AMU = 931.5; // Atomic mass unit, a unit = 1.6605 x 10^-27 kg = 931.5 MeV
const double PI = M_PI;

const int pID_e = 11;		// electron
const int pID_g = 22;		// gamma
const int pID_n = 2112;		// neutron
const int pID_p = 2212;		// proton
const int pID_6Li = 1000030060;
const int pID_10B = 1000050100;
const int pID_11B = 1000050110;
const int pID_11C = 1000060110;
const int pID_12C = 1000060120;

vector <string> elTable = {"H","He","Li","Be","B","C","N","O"};
vector <int> COLOR = {1,2,4,6,7,8,9};

//-------------------------------------------------------------------------
bool Exist(set<int> List, int id)
{
	if( List.find(id) != List.end()) return true;
	else return false;
}

//-------------------------------------------------------------------------
int DettoIndex(const int detID)
{
	enum SDorder {EMP, SC1a, SC1b, SC2a, SC2b, ATTPCa, ATTPCb, FTOF, TG};
	int index;
	if		(detID == 1100) index = SC1a;
	else if	(detID == 1101) index = SC1b;
	else if	(detID == 1400) index = SC2a;
	else if	(detID == 1401) index = SC2b;
	else if	(detID == 1500) index = ATTPCa;
	else if	(detID == 1501) index = ATTPCb;
	else if	(detID == 1600) index = FTOF;
	else if	(detID == 1300) index = TG;
	return index;
}
//-------------------------------------------------------------------------
string IndextoDet(const int index)
{
	enum SDorder {EMP, SC1a, SC1b, SC2a, SC2b, ATTPCa, ATTPCb, FTOF, TG};
	string SDname[] =  {"EMP","SC1a", "SC1b", "SC2a", "SC2b", "ATTPCa", "ATTPCb", "FTOF", "TG"};
	string fdetname;
	if		(index == SC1a)	fdetname = SDname[1];
	else if	(index == SC1b)	fdetname = SDname[2];
	else if	(index == SC2a)	fdetname = SDname[3];
	else if	(index == SC2b)	fdetname = SDname[4];
	else if	(index == ATTPCa)	fdetname = SDname[5];
	else if	(index == ATTPCb)	fdetname = SDname[6];
	else if	(index == FTOF)	fdetname = SDname[7];
	else if	(index == TG)	fdetname = SDname[8];
	return fdetname;
}
//-------------------------------------------------------------------------
int PDGtoIndex(const int pID)
{
	enum pIDorder {pro,neu,gam,ele,pos,ELS};
	int index;
	if		(pID == pID_p) index = pro;
	else if (pID == pID_n) index = neu;
	else if (pID == pID_g) index = gam;
	else if (pID == pID_e) index = ele;
	else if (pID ==-pID_e) index = pos;
	else				   index = ELS;
	return index;
}

//-------------------------------------------------------------------------
pair<int,int> PDGtoAZ(const int pID)
{
	int numA;
	int numZ;
	if		(pID <  pID_n) { numZ = 0; numA = 0; }
	else if	(pID == pID_n) { numZ = 0; numA = 1; }
	else if (pID == pID_p) { numZ = 1; numA = 0; }
	else	// for ions
	{
		const int numZA = pID%1000000000;
		numZ = (numZA / 10000);
		numA = (numZA % 10000)/10;
	}
	return pair<int,int>{numA,numZ};
}

//-------------------------------------------------------------------------
float PDGtoMass(const int pID) //for e-,e+, p, n, gamma, ions
{
	float mass;
	if		(pID == pID_e) mass = 0.511;	// e- [MeV]
	else if (pID ==-pID_e) mass = 0.511;	// e+
	else if (pID == pID_g) mass = 0.000;	// gamma
	else if (pID == pID_p) mass = 938.272;	// proton
	else if (pID == pID_n) mass = 939.562;	// neutron
	else	// for ion (w/o binding energy, just mass of A + mass of Z)
	{
		pair<int,int> numAZ = PDGtoAZ(pID);
		const int numA = numAZ.first;
//		const int numZ = numAZ.second();
		mass = numA*AMU;
	}
	return mass;
}

//-------------------------------------------------------------------------
string PDGtoName(const int pID)
{
	string name;
	if		(pID == pID_e) { name = "e-";}
	else if (pID ==-pID_e) { name = "e+";}
	else if (pID == pID_g) { name = "#gamma";}
	else if (pID == pID_n) { name = "n";}
	else if (pID == pID_p) { name = "p";}
	else
	{
		pair<int,int> numAZ = PDGtoAZ(pID);
		int numA = numAZ.first;
		int numZ = numAZ.second;
		string el = elTable[numZ];
//		name = Form("%d%s",numA,el);
		name = to_string(numA) + el;
	}
	return name;
}

//-------------------------------------------------------------------------
string IndextoLabel(const int index)
{
	enum pIDorder {pro,neu,gam,ele,pos,ELS};
	string name;
	if		(index == pro) {name = "p";}
	else if (index == neu) {name = "n";}
	else if (index == ele) {name = "e-";}
	else if (index == pos) {name = "e+";}
	else if (index == gam) {name = "#gamma";}
	else				   {name = "ELS";}
	return name;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
