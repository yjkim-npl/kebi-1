#include "KBtutMaterials.hh"

#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"

#include <iostream>
#include <algorithm>
#include <cmath>

KBtutMaterials* KBtutMaterials::fInstance = 0;

KBtutMaterials::KBtutMaterials(){
	fNistMan = G4NistManager::Instance();
	CreateMaterials();
}
KBtutMaterials::~KBtutMaterials()
{
}

KBtutMaterials* KBtutMaterials::GetInstance(){
	if (fInstance==0) fInstance = new KBtutMaterials();
	return fInstance;
}

G4Material* KBtutMaterials::GetMaterial(const G4String matName){
	G4Material* mat = fNistMan->FindOrBuildMaterial(matName);

	if(!mat) mat = G4Material::GetMaterial(matName);
	if(!mat){
		std::ostringstream o;
		o << "Material " << matName << " not found!";
		G4Exception("KBtutMaterials::GetMaterial","",FatalException,o.str().c_str());
	}
	return mat;
}

void KBtutMaterials::CreateMaterials(){
	fNistMan -> FindOrBuildMaterial("G4_Galactic");
	fNistMan -> FindOrBuildMaterial("G4_AIR");

	G4String symbol;
	G4double a, z, density;
	G4int ncomp, natoms;
	G4Element* H = new G4Element("Hydrogen"	,symbol="H"	, z=1., a=1.01*g/mole);
	G4Element* C = new G4Element("Carbon"	,symbol="C" , z=6., a=12.01*g/mole);
	G4Element* N = new G4Element("Nitrogen"	,symbol="N"	, z=7., a=14.01*g/mole);
	G4Element* O = new G4Element("Oxygen"	,symbol="O"	, z=8., a=16.00*g/mole);
	fVac = G4Material::GetMaterial("G4_Galactic");
	fAir = G4Material::GetMaterial("G4_AIR");
	fPS = new G4Material("Polystyrene", density=1.05*g/cm3, ncomp=2);
	fPS -> AddElement(C, natoms=8);
	fPS -> AddElement(H, natoms=8);

//	fUser = new G4Material();
//	fUser -> AddElement();
}
//KBtutMaterials
