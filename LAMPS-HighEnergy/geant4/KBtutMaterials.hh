#ifndef KBTUTMATERIALS_HH
#define KBTUTMATERIALS_HH

#include "G4Material.hh"
#include "G4NistManager.hh"

class KBtutMaterials{
	public:
		KBtutMaterials();
		virtual ~KBtutMaterials();
		static KBtutMaterials* GetInstance();
		G4Material* GetMaterial(const G4String);

	private:
		void CreateMaterials();

		static KBtutMaterials* fInstance;
		G4NistManager* fNistMan;
		G4Material* fVac;
		G4Material* fAir;
		G4Material* fPS;
		G4Material* fB;
		G4Material* fPb;
		G4Material* fUser;
};

#endif
