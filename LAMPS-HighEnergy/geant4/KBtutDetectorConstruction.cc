#include "KBtutDetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "KBParameterContainer.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <iostream>
#include <cmath>
using namespace std;

//Constructor
KBtutDetectorConstruction::KBtutDetectorConstruction() : G4VUserDetectorConstruction()
{
	fMaterials = KBtutMaterials::GetInstance();
//	DefineMaterials();
}

//Destructor
KBtutDetectorConstruction::~KBtutDetectorConstruction()
{
//	G4AutoDelete::Register(fNist);
}

void KBtutDetectorConstruction::DefineMaterials()
{
	fMaterials = KBtutMaterials::GetInstance();
}

//=======================================================
G4VPhysicalVolume* KBtutDetectorConstruction::Construct()
{

	auto fRun = (KBG4RunManager*)G4RunManager::GetRunManager();
	auto fPar = fRun->GetParameterContainer();

	auto fVisFrame = new G4VisAttributes();
	fVisFrame->SetColor(G4Color::Gray());
	fVisFrame->SetForceWireframe(true);

	G4ThreeVector fZero(0, 0, 0);
	G4double fSTPTemp = 273.15 * kelvin;
	G4double fLabTemp = fSTPTemp + 20 * kelvin;

	//World volume
	//--------------------------------------------------------------------

	string worldMatStr;
	if      (fPar->GetParInt("worldOpt") == 0) worldMatStr = "G4_Galactic";
	else if (fPar->GetParInt("worldOpt") == 1) worldMatStr = "G4_AIR";
	else { cout <<"\nWorld material???\n"; assert(false); }

	G4int    worldID = fPar->GetParInt("worldID");
	G4double worldDX = fPar->GetParDouble("worlddX");
	G4double worldDY = fPar->GetParDouble("worlddY");
	G4double worldDZ = fPar->GetParDouble("worlddZ");

	G4Box*             WorldSol = new G4Box("WorldSolid", worldDX, worldDY, worldDZ);
	G4LogicalVolume*   WorldLog = new G4LogicalVolume(WorldSol, FindMaterial("G4_AIR"), "World");
	G4VPhysicalVolume* WorldPhy = new G4PVPlacement(0, fZero, WorldLog, "WorldPhys", 0, false, worldID, true);

	auto fVisWorld = new G4VisAttributes();
	fVisWorld->SetColor(G4Color::White());
	fVisWorld->SetForceWireframe(true);
	WorldLog->SetVisAttributes(fVisWorld);


//	G4NistManager* nist = G4NistManager::Instance();
//	nist -> FindOrBuildMaterial("G4_B");

	// Polystyrene BoX
	//--------------------------------------------------------------------

	if (fPar->GetParBool("BoxIn"))
	{
		G4int    BoxID    = fPar->GetParInt("BoxID");
		G4double BoxDimX  = fPar->GetParDouble("BoxDimX");
		G4double BoxDimY  = fPar->GetParDouble("BoxDimY");
		G4double BoxDimZ  = fPar->GetParDouble("BoxDimZ");
		G4double BoxPosZOff = fPar->GetParDouble("BoxPosZOff");

		// three steps to construct detector (solidVolume -> logicalVolume -> PhysicalVolume)
		G4Box*           solBox = new G4Box("SolidBox", BoxDimX/2, BoxDimY/2, BoxDimZ/2);
//		G4LogicalVolume* logBox = new G4LogicalVolume(solBox, FindMaterial("G4_Pb"), "logicBox");
		G4LogicalVolume* logBox = new G4LogicalVolume(solBox, FindMaterial("G4_PLASTIC_SC_VINYLTOLUENE"), "logicBox");
//		G4LogicalVolume* logBox = new G4LogicalVolume(solBox, nist -> FindOrBuildMaterial("G4_Pb"), "logicBox");

		G4ThreeVector BoxPos(0, 0, BoxPosZOff + BoxDimZ/2.); // starting point set at BoxPosZOff
		G4VPhysicalVolume* phyBox = new G4PVPlacement(0, BoxPos, logBox, "physicalBox", WorldLog, false, BoxID, true);

		fRun->SetSensitiveDetector(phyBox);

		auto fVisBox = new G4VisAttributes();
		fVisBox->SetColor(G4Color::Cyan());
		fVisBox->SetForceWireframe(true);
		logBox->SetVisAttributes(fVisBox);
	}//Box

	// others 
	//--------------------------------------------------------------------
//	if (fPar -> GetParBool("a"))
//	{
//	}

	return WorldPhy;
}//Construct
