#ifndef TB21ADETECTORCONSTRUCTION_HH
#define TB21ADETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class TB21ADetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    TB21ADetectorConstruction();
    virtual ~TB21ADetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
