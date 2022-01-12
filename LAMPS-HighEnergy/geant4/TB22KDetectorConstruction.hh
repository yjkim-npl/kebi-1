#ifndef TB22KDETECTORCONSTRUCTION_HH
#define TB22KDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4NistManager;
class G4VisAttributes;

class TB22KDetectorConstruction : public G4VUserDetectorConstruction
{
	public:

		TB22KDetectorConstruction();
		virtual ~TB22KDetectorConstruction();

		virtual G4VPhysicalVolume* Construct();

	private:

		G4NistManager* fNist;
};
#endif
