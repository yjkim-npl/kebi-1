#ifndef KBTUTDETECTORCONSTRUCTION_HH
#define KBTUTDETECTORCONSTRUCTION_HH

#include "KBtutMaterials.hh"
#include "G4Material.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4NistManager;
class G4VisAttributes;

class KBtutDetectorConstruction : public G4VUserDetectorConstruction
{
	public:

		KBtutDetectorConstruction();
		virtual ~KBtutDetectorConstruction();

		virtual G4VPhysicalVolume* Construct();

	private:
		void DefineMaterials();
		KBtutMaterials* fMaterials;
		G4Material* FindMaterial(G4String matName) {return fMaterials -> GetMaterial(matName);}
//		G4NistManager* fNist;
};
#endif
