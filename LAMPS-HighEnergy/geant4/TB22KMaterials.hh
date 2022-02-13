#ifndef TB22KMATERIALS_HH
#define TB22KMATERIALS_HH

#include "G4Material.hh"
#include "G4NistManager.hh"

class TB22KMaterials{
	public:
		TB22KMaterials();
		virtual ~TB22KMaterials();
		static TB22KMaterials* GetInstance();
		G4Material* GetMaterial(const G4String);

	private:
		void CreateMaterials();

		static TB22KMaterials* fInstance;
		G4NistManager* fNistMan;
		G4Material* fVac;
		G4Material* fAir;
//		G4Material* fArgon;
//		G4Material* fCH4;
		G4Material* fAcryl;
		G4Material* fBP;
		G4Material* fp10;	//TPCmaterial
		G4Material* fPSV;	//SCmaterial
		G4Material* fMylar;	//BDC
		G4Material* fAluOxi;
		G4Material* fC2H4;	//TGmaterial
		G4Material* fHL;	// liquid hydrogen
		G4Material* fHS;	// solid hydrogen
		G4Material* fGrp;	// graphite
		G4Material* fPS;
//		G4Material* fUser;
};

#endif
