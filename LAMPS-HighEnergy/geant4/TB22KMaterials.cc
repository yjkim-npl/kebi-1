#include "TB22KMaterials.hh"

#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"

#include <iostream>
#include <algorithm>
#include <cmath>

TB22KMaterials* TB22KMaterials::fInstance = 0;

TB22KMaterials::TB22KMaterials(){
	fNistMan = G4NistManager::Instance();
	CreateMaterials();
}
TB22KMaterials::~TB22KMaterials()
{
}

TB22KMaterials* TB22KMaterials::GetInstance(){
	if (fInstance==0) fInstance = new TB22KMaterials();
	return fInstance;
}

G4Material* TB22KMaterials::GetMaterial(const G4String matName){
	G4Material* mat = fNistMan->FindOrBuildMaterial(matName);

	if(!mat) mat = G4Material::GetMaterial(matName);
	if(!mat){
		std::ostringstream o;
		o << "Material " << matName << " not found!";
		G4Exception("TB22KMaterials::GetMaterial","",FatalException,o.str().c_str());
	}
	return mat;
}

void TB22KMaterials::CreateMaterials(){
	fNistMan -> FindOrBuildMaterial("G4_Galactic");
	fNistMan -> FindOrBuildMaterial("G4_AIR");
	fNistMan -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
	fNistMan -> FindOrBuildMaterial("G4_MYLAR");
	fNistMan -> FindOrBuildMaterial("G4_ALUMINUM_OXIDE");
	fNistMan -> FindOrBuildMaterial("G4_POLYETHYLENE");
	fNistMan -> FindOrBuildMaterial("G4_B");

	G4String symbol;
	G4double a, z, density;
	G4int ncomp, natoms;
	G4double fSTPTemp = 273.15 * kelvin;
	G4double fLabTemp = fSTPTemp + 20 * kelvin;
	G4Element* H = new G4Element("Hydrogen"	,symbol="H"	, z=1., a=1.01*g/mole);
	G4Element* C = new G4Element("Carbon"	,symbol="C" , z=6., a=12.01*g/mole);
	G4Element* N = new G4Element("Nitrogen"	,symbol="N"	, z=7., a=14.01*g/mole);
	G4Element* O = new G4Element("Oxygen"	,symbol="O"	, z=8., a=16.00*g/mole);

	fVac = G4Material::GetMaterial("G4_Galactic");
	fAir = G4Material::GetMaterial("G4_AIR");
	fPSV = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
	fMylar = G4Material::GetMaterial("G4_MYLAR");
	fAluOxi = G4Material::GetMaterial("G4_ALUMINUM_OXIDE");
	fC2H4 = G4Material::GetMaterial("G4_POLYETHYLENE");

	const G4double AcrylD = 1.19*g/cm3;
	G4Material* fAcryl = new G4Material("Acrylic",AcrylD,3,kStateSolid,fLabTemp);
	fAcryl -> AddElement(C,5);
	fAcryl -> AddElement(H,8);
	fAcryl -> AddElement(O,2);

	G4Material* fB = G4Material::GetMaterial("G4_B");
	const G4double BPD = 0.3*fB->GetDensity() + 0.7*fC2H4->GetDensity();
	G4Material* fBP = new G4Material("BoratedPolyethylene",BPD, 2);
	fBP -> AddMaterial(fB,0.3);
	fBP -> AddMaterial(fC2H4,0.7);

	const G4double ArD = 1.7836 * mg/cm3 * fSTPTemp/fLabTemp;
	G4Material* fArgon = new G4Material("ArGas", 18, 39.948*g/mole, ArD, kStateGas, fLabTemp);

	const G4double CH4D = 0.717e-3 * g/cm3 * fSTPTemp/fLabTemp;
	G4Material* fCH4 = new G4Material("CH4gas", CH4D, 2, kStateGas, fLabTemp);
	fCH4 -> AddElement(C,1);
	fCH4 -> AddElement(H,4);

	const G4double p10D = 0.9*ArD + 0.1* CH4D;
	fp10 = new G4Material("p10Gas",p10D,2,kStateGas,fLabTemp);
	fp10 -> AddMaterial(fArgon, 0.9 * ArD/p10D);
	fp10 -> AddMaterial(fCH4, 0.1 * CH4D/p10D);

	fHL = new G4Material("HLiquid",1,1.008*g/mole,70.85*mg/cm3);;
	fHS = new G4Material("HSolid",1,1.008*g/mole,44.0*mg/cm3);;
	fGrp = new G4Material("Graphite",12,12.011*g/mole,2.26*g/cm3,kStateSolid);
	
	fPS = new G4Material("Polystyrene", density=1.05*g/cm3, ncomp=2);
	fPS -> AddElement(C, natoms=8);
	fPS -> AddElement(H, natoms=8);

//	fUser = new G4Material();
//	fUser -> AddElement();
}
//TB22KMaterials
