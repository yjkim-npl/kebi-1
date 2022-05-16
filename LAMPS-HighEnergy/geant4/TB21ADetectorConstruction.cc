#include "KBParameterContainer.hh"
#include "TB21ADetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4UniformMagField.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4SubtractionSolid.hh"

TB21ADetectorConstruction::TB21ADetectorConstruction()
: G4VUserDetectorConstruction()
{
}

TB21ADetectorConstruction::~TB21ADetectorConstruction()
{
}

G4VPhysicalVolume *TB21ADetectorConstruction::Construct()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();

  auto par = runManager -> GetParameterContainer();

  // environment set
  G4NistManager *nist = G4NistManager::Instance();
  G4double STPTemperature = 273.15;
  G4double labTemperature = STPTemperature + 20.*kelvin;
  
	G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
	G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);
	G4Element *elementO = new G4Element("elementO", "O", 8., 16.000*g/mole);

  G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);

    G4Material *matPE = new G4Material("Polyester", 1.40*g/cm3, 3, kStateSolid, labTemperature);
	G4Material *matAcrylic = new G4Material("Acrylic", 1.19*g/cm3, 3, kStateSolid, labTemperature);
	matAcrylic->AddElement(elementC,5);
	matAcrylic->AddElement(elementH,8);
	matAcrylic->AddElement(elementO,2);
	matPE -> AddElement(elementC,10);
	matPE -> AddElement(elementH,8);
	matPE -> AddElement(elementO,4);

	G4Material *matB = nist->FindOrBuildMaterial("G4_B");
	G4Material *matAl = nist->FindOrBuildMaterial("G4_Al");
	G4Material *matCu = nist->FindOrBuildMaterial("G4_Cu");
	G4Material *matFe = nist->FindOrBuildMaterial("G4_Fe");
	G4Material *matSn = nist->FindOrBuildMaterial("G4_Sn");
	G4Material *matAu = nist->FindOrBuildMaterial("G4_Au");
	G4Material *matSi = nist->FindOrBuildMaterial("G4_Si");
	G4Material *matSC = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
	//G4Material *matXYLENE = nist->FindOrBuildMaterial("G4_XYLENE");
	G4Material *matCH2 = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
	G4Material *matAir = nist->FindOrBuildMaterial("G4_AIR");
	G4Material *matVac = nist->FindOrBuildMaterial("G4_Galactic");
 
  G4double densityMethane = 0.717e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matMethaneGas = new G4Material("matMethaneGas ", densityMethane, 2, kStateGas, labTemperature);
  matMethaneGas -> AddElement(elementH, 4);
  matMethaneGas -> AddElement(elementC, 1);

	G4double densityBP = 0.3*matB->GetDensity() + 0.7*matCH2->GetDensity(); 
	G4Material *matBP = new G4Material("BoratedPolyethylene", densityBP, 2);
	matBP->AddMaterial(matB, 0.3);
	matBP->AddMaterial(matCH2, 0.7);

  TString gasPar = "p20";
  if (par -> CheckPar("TPCgasPar")) {
    gasPar = par -> GetParString("TPCgasPar");
    gasPar.ToLower();
         if (gasPar.Index("p10")>=0) gasPar = "p10";
    else if (gasPar.Index("p20")>=0) gasPar = "p20";
    else gasPar = "p20";
  }

  G4Material *matGas = nullptr;
  if (gasPar == "p10") {
    G4double densityGas = .9*densityArGas + .1*densityMethane;
    matGas = new G4Material("matP10", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.9*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.1*densityMethane/densityGas);
  }
  else if (gasPar == "p20") {
    G4double densityGas = .8*densityArGas + .2*densityMethane;
    matGas = new G4Material("matP20", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.8*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.2*densityMethane/densityGas);
  }

// Detector set

	//world
  G4double worlddX = par -> GetParDouble("worlddX");
  G4double worlddY = par -> GetParDouble("worlddY");
  G4double worlddZ = par -> GetParDouble("worlddZ");
  G4double worlddR = par -> GetParDouble("worlddR");

  G4Orb *OrbWorld = new G4Orb("World", worlddR);
  G4Box *BoxWorld = new G4Box("World", worlddX, worlddY, worlddZ);
  G4int worldShape = par -> GetParInt("worldShape");
  G4int worldOpt = par -> GetParInt("worldOpt");

  G4LogicalVolume *logicWorld;
		  if ( worldOpt== 0 && worldShape== 0 ){
		logicWorld = new G4LogicalVolume(BoxWorld, matVac, "World");
	}else if ( worldOpt== 0 && worldShape== 1 ){
		logicWorld = new G4LogicalVolume(OrbWorld, matAir, "World");
	}else if ( worldOpt== 1 && worldShape== 0 ){
		logicWorld = new G4LogicalVolume(BoxWorld, matAir, "World");
	}else if ( worldOpt== 1 && worldShape== 1 ){
		logicWorld = new G4LogicalVolume(OrbWorld, matAir, "World");
	}else{
		logicWorld = new G4LogicalVolume(BoxWorld, matVac, "World");
	}
  G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, par -> GetParInt("worldID"), true);

	// Collimator 
	if( par -> GetParBool("CollimatorIn") )
	{
		G4double Collx = par -> GetParDouble("Collimatorx");
		G4double Colly = par -> GetParDouble("Collimatory");
		G4double Collz = par -> GetParDouble("Collimatorz");
		G4double CollzOffset = par -> GetParDouble("CollimatorzOffset");

		G4Box *solidColl = new G4Box("Collimator", Collx/2.0, Colly/2.0, Collz/2.0);
		G4Tubs *solidHole = new G4Tubs("Hole", 0, 60/2, Collz, 0, 2*M_PI);
		G4SubtractionSolid *solidSubC = new G4SubtractionSolid("SubC", solidColl, solidHole, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubC = new G4LogicalVolume(solidSubC, matSC, "SubC");
		{
			G4VisAttributes * attSubC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attSubC -> SetVisibility(false);
			attSubC -> SetForceWireframe(true);
			logicSubC -> SetVisAttributes(attSubC);
		}
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,0,CollzOffset+Collz/2), logicSubC, "Collimator", logicWorld, false, par -> GetParInt("CollimatorID"), true);
//		runManager -> SetSensitiveDetector(pvp1);
	}


	// Acrylic Slit
	if ( par -> GetParBool("SlitIn") )
	{
		G4double Slitx = par -> GetParDouble("Slitx");
		G4double Slity = par -> GetParDouble("Slity");
		G4double Slitz = par -> GetParDouble("Slitz");
		G4double SlitzOffset = par -> GetParDouble("SlitzOffset");

		G4double Holex = par -> GetParDouble("Holex");
		G4double Holey = par -> GetParDouble("Holey");

		G4Box *solidSlit1 = new G4Box("Slit1", Slitx/2.0, Slity/2.0, Slitz/2.0);
		G4Box *solidHole1 = new G4Box("Hole1", Slitx/2.0, Holey/2.0, Slitz/2.0);
		G4SubtractionSolid *solidSubS1 = new G4SubtractionSolid("SubS1", solidSlit1, solidHole1, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubS1 ;
		
		if ( par -> GetParInt("SlitOpt")==0 ){
			logicSubS1 = new G4LogicalVolume(solidSubS1, matAl, "SubS1");
		}else if ( par -> GetParInt("SlitOpt")==1 ){
			logicSubS1 = new G4LogicalVolume(solidSubS1, matCu, "SubS1");
		}else if ( par -> GetParInt("SlitOpt")==2 ){
			logicSubS1 = new G4LogicalVolume(solidSubS1, matFe, "SubS1");
		}else if ( par -> GetParInt("SlitOpt")==3 ){
			logicSubS1 = new G4LogicalVolume(solidSubS1, matAcrylic, "SubS1");
		}else{
			logicSubS1 = new G4LogicalVolume(solidSubS1, matAl, "SubS1");
		}
		

		{
			G4VisAttributes * attSubS1 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubS1 -> SetVisAttributes(attSubS1);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,SlitzOffset+Slitz/2), logicSubS1, "SubS1", logicWorld, false, par -> GetParInt("SlitID"), true);

		G4Box *solidSlit2 = new G4Box("Slit2", Slitx/2.0, Slity/2.0, Slitz/2.0);
		G4Box *solidHole2 = new G4Box("Hole2", Holex/2.0, Slity/2.0, Slitz/2.0);
		G4SubtractionSolid *solidSubS2 = new G4SubtractionSolid("SubS2", solidSlit2, solidHole2, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubS2;
		if ( par -> GetParInt("SlitOpt")==0 ){
			logicSubS2 = new G4LogicalVolume(solidSubS2, matAl, "SubS2");
		}else if ( par -> GetParInt("SlitOpt")==1 ){
			logicSubS2 = new G4LogicalVolume(solidSubS2, matCu, "SubS2");
		}else if ( par -> GetParInt("SlitOpt")==2 ){
			logicSubS2 = new G4LogicalVolume(solidSubS2, matFe, "SubS2");
		}else if ( par -> GetParInt("SlitOpt")==3 ){
			logicSubS2 = new G4LogicalVolume(solidSubS2, matAcrylic, "SubS2");
		}else{
			logicSubS2 = new G4LogicalVolume(solidSubS2, matAl, "SubS2");
		}

		{
			G4VisAttributes * attSubS2 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubS2 -> SetVisAttributes(attSubS2);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,SlitzOffset+3*Slitz/2), logicSubS2, "SubS2", logicWorld, false, par -> GetParInt("SlitID"), true);
	}

	//Boron Shield
	if ( par -> GetParBool("ShieldIn") )
	{
		G4double Shieldx = par -> GetParDouble("Shieldx");	// 200
		G4double Shieldy = par -> GetParDouble("Shieldy");  // 100
		G4double Shieldz = par -> GetParDouble("Shieldz");  // 55

		G4double ShieldzOffset = par -> GetParDouble("ShieldzOffset");

		G4double ShieldHolex = par -> GetParDouble("ShieldHolex");
		G4double ShieldHoley = par -> GetParDouble("ShieldHoley");

		G4Box *solidShield1 = new G4Box("Shield1", Shieldx/2.0, Shieldz/2.0, Shieldy/2.0); // 1st Boron
		G4Box *solidShield2 = new G4Box("Shield2", Shieldx/2.0, Shieldy/2.0, Shieldz/2.0); // 2nd Boron
		G4Box *solidShield3 = new G4Box("Shield3", Shieldy/2.0,Shieldx/2.0,Shieldz/2.0);	// old Boron

		G4LogicalVolume *logicShield1;
		G4LogicalVolume *logicShield2;
		G4LogicalVolume *logicShield3;
		if ( par -> GetParInt("ShieldOpt")==0 ){
			logicShield1 = new G4LogicalVolume(solidShield1, matB, "Shield1");
			logicShield2 = new G4LogicalVolume(solidShield2, matB, "Shield2");
			logicShield3 = new G4LogicalVolume(solidShield3, matB, "Shield3");
		}else if ( par -> GetParInt("ShieldOpt")==1 ){
			logicShield1 = new G4LogicalVolume(solidShield1, matBP, "Shield1");
			logicShield2 = new G4LogicalVolume(solidShield2, matBP, "Shield2");
			logicShield3 = new G4LogicalVolume(solidShield3, matBP, "Shield3");
		}else{
			logicShield1 = new G4LogicalVolume(solidShield1, matB, "Shield1");
			logicShield2 = new G4LogicalVolume(solidShield2, matB, "Shield2");
			logicShield3 = new G4LogicalVolume(solidShield3, matB, "Shield3");
		}
		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Brown()));
			attSubS -> SetForceWireframe(true);
			logicShield1 -> SetVisAttributes(attSubS);
			logicShield2 -> SetVisAttributes(attSubS);
			logicShield3 -> SetVisAttributes(attSubS);
		}
#if 0	// old Boron setup
		{
			auto pvpu = new G4PVPlacement(0,G4ThreeVector(0,(Shieldy+ShieldHoley)/2,ShieldzOffset+Shieldz/2),logicShield2, "Shield2u", logicWorld, false, par->GetParInt("ShieldID"),true);
			auto pvpd = new G4PVPlacement(0,G4ThreeVector(0,-(Shieldy+ShieldHoley)/2,ShieldzOffset+Shieldz/2),logicShield2, "Shield2d", logicWorld, false, par->GetParInt("ShieldID"),true);
			auto pvpl = new G4PVPlacement(0,G4ThreeVector(-(Shieldy+ShieldHolex)/2,0,ShieldzOffset+Shieldz*3/2),logicShield3, "Shield2l", logicWorld, false, par->GetParInt("ShieldID"),true);
			auto pvpr = new G4PVPlacement(0,G4ThreeVector((Shieldy+ShieldHolex)/2,0,ShieldzOffset+Shieldz*3/2),logicShield3, "Shield2r", logicWorld, false, par->GetParInt("ShieldID"),true);
		}
#endif			
#if	1	// 1st Boron Set
		for(int a=0; a<7; a++) // y axis
		for(int b=0; b<2; b++) // x axis
		{
			if( a == 4)
			{
				new G4PVPlacement(0, G4ThreeVector(b==0?-105:105,-267+55*a+Shieldz/2,ShieldzOffset+Shieldy/2), logicShield1, Form("Shield1_%d_%d",a,b), logicWorld, false, par -> GetParInt("ShieldID"),true);
				continue;
			}
			new G4PVPlacement(0, G4ThreeVector(b==0?-100:100,-267+55*a+Shieldz/2,ShieldzOffset+Shieldy/2), logicShield1, Form("Shield1_%d_%d",a,b), logicWorld, false, par -> GetParInt("ShieldID"),true);
		}
#endif

#if 1 	// 2nd Boron set
		for(int a=0; a<4; a++) // y axis
		for(int b=-1; b<2; b++) // x axis
		{
			if( a<2 )
			{
			new G4PVPlacement(0,G4ThreeVector(200*b,-289+100*a+50,ShieldzOffset+Shieldy+Shieldz/2), logicShield2, Form("Shield2_%d_%d",a,b), logicWorld, false, par -> GetParInt("ShieldID"),true);
			}
			if( a==2 )
			{
				if( b>=1 ) continue;
				new G4PVPlacement(0,G4ThreeVector(b==0?-105:105,-289+100*a+50,ShieldzOffset+Shieldy+Shieldz/2), logicShield2, Form("Shield2_%d_%d",a,b), logicWorld, false, par -> GetParInt("ShieldID"),true);
			}
			if( a==3 )
			{
				if(b>=0) continue;
				new G4PVPlacement(0,G4ThreeVector(0,-289+100*a+50,ShieldzOffset+Shieldy+Shieldz/2), logicShield2, Form("Shield2_%d_%d",a,b), logicWorld, false, par -> GetParInt("ShieldID"),true);
			}
		}
#endif		

	//Target1
	if ( par -> GetParBool("Target1In") )
	{
		G4double Target1x = par -> GetParDouble("Target1x");
		G4double Target1y = par -> GetParDouble("Target1y");
		G4double Target1z = par -> GetParDouble("Target1z");
		G4double Target1zOffset = par -> GetParDouble("Target1zOffset");

		G4Box *solidTarget1 = new G4Box("Target1", Target1x/2.0, Target1y/2.0, Target1z/2.0);
		G4LogicalVolume *logicTarget1 = new G4LogicalVolume(solidTarget1, matCH2, "Traget1");
		{
			G4VisAttributes * attTarget1 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget1 -> SetVisAttributes(attTarget1);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target1zOffset+Target1z/2), logicTarget1, "Target1", logicWorld, false,par->GetParInt("TG1ID"), true);
		runManager -> SetSensitiveDetector(pvp);
	}

	}
#if 1	//Radiation damage checking
	if( par -> GetParBool("MPPCIn"))
	{
		G4double PEx = par -> GetParDouble("PEx");
		G4double PEy = par -> GetParDouble("PEy");
		G4double PEz = par -> GetParDouble("PEz");

		G4double CHx = par -> GetParDouble("CHx");
		G4double CHy = par -> GetParDouble("CHy");
		G4double CHz = par -> GetParDouble("CHz");

		G4double MPPCx = par -> GetParDouble("MPPCx");
		G4double MPPCy = par -> GetParDouble("MPPCy");
		G4double MPPCz = par -> GetParDouble("MPPCz");
		G4double MPPCz_off = par -> GetParDouble("MPPCz_off");

		G4Box *solidPE = new G4Box("PE",PEx/2,PEy/2,PEz/2);
		G4Box *solidCH = new G4Box("CH",CHx/2,CHy/2,CHz/2);
		G4Box *solidMPPC = new G4Box("MPPC",MPPCx/2,MPPCy/2,MPPCz/2);

		G4LogicalVolume *logicPE = new G4LogicalVolume(solidPE ,matPE ,"PE" );
		G4LogicalVolume *logicCH = new G4LogicalVolume(solidCH ,matCH2 ,"CH" );
		G4LogicalVolume *logicMPPC = new G4LogicalVolume(solidMPPC ,matSi ,"MPPC" );
		{
			G4VisAttributes *attPE  = new G4VisAttributes(G4Colour(G4Colour::Green()));
			G4VisAttributes *attCH  = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			G4VisAttributes *attMPPC  = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attPE -> SetForceWireframe(true);
			attCH -> SetForceWireframe(true);
			attMPPC -> SetForceWireframe(true);
			logicPE -> SetVisAttributes(attPE);
			logicCH -> SetVisAttributes(attCH);
			logicMPPC -> SetVisAttributes(attMPPC);
		}
		auto pvpP1 = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+PEz/2),logicPE,"PE1",logicWorld,false,par->GetParInt("PE1ID"),true);
		auto pvpP2 = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+3*PEz/2+CHz),logicPE,"PE2",logicWorld,false,par->GetParInt("PE2ID"),true);
		auto pvpP3 = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+5*PEz/2+CHz),logicPE,"PE3",logicWorld,false,par->GetParInt("PE3ID"),true);
		auto pvpP4 = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+7*PEz/2+2*CHz),logicPE,"PE4",logicWorld,false,par->GetParInt("PE4ID"),true);
		auto pvpC1 = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+3*PEz/2),logicCH,"CH1",logicWorld,false,par->GetParInt("CH1ID"),true);
		auto pvpC2 = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+7*PEz/2+CHz),logicCH,"CH2",logicWorld,false,par->GetParInt("CH2ID"),true);
		auto pvpM = new G4PVPlacement(0,G4ThreeVector(0,50,MPPCz_off+9*PEz/2+2*CHz+MPPCz/2),logicMPPC,"MPPC",logicWorld,false,par->GetParInt("MPPCID"),true);
		runManager -> SetSensitiveDetector(pvpP1);
		runManager -> SetSensitiveDetector(pvpP2);
		runManager -> SetSensitiveDetector(pvpP3);
		runManager -> SetSensitiveDetector(pvpP4);
		runManager -> SetSensitiveDetector(pvpC1);
		runManager -> SetSensitiveDetector(pvpC2);
		runManager -> SetSensitiveDetector(pvpM);
	}//MPPC
#endif

	// just test
	{
		G4double temp=1;
		G4Box* solidt = new G4Box("temp",temp/2,temp/2,temp/2);
		G4LogicalVolume* logict = new G4LogicalVolume(solidt, matVac, "t");
		auto pt = new G4PVPlacement(0, G4ThreeVector(0,0,0), logict,"t",logicWorld, false, 4, true);
		runManager -> SetSensitiveDetector(pt);
	}
	//Start counter A : 2
	if ( par -> GetParInt("StartCounterAIn") != 0 )
	{
		G4double SCAx = par -> GetParDouble("SCAx");
		G4double SCAy = par -> GetParDouble("SCAy");
		G4double SCAz = par -> GetParDouble("SCAz");
		G4double SCAzOffset_1 = par -> GetParDouble("SCAzOffset_1");
		G4double SCAzOffset_2 = par -> GetParDouble("SCAzOffset_2");

		G4Box *solidSCA = new G4Box("SCA", SCAx/2.0, SCAy/2.0, SCAz/2.0);
		G4LogicalVolume *logicSCA = new G4LogicalVolume(solidSCA, matSC, "SCA");
		{
			G4VisAttributes * attSCA = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attSCA -> SetForceWireframe(true);
			logicSCA -> SetVisAttributes(attSCA);
		}
		if( par -> GetParInt("StartCounterAIn") >= 1 )
		{
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,0,SCAzOffset_1), logicSCA, "SCA_1", logicWorld, false, par -> GetParInt("SCA1ID"), true);
		runManager -> SetSensitiveDetector(pvp1);
		}
		if( par -> GetParInt("StartCounterAIn") >= 2 )
		{
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,0,SCAzOffset_2), logicSCA, "SCA_2", logicWorld, false, par -> GetParInt("SCA2ID"), true);
		runManager -> SetSensitiveDetector(pvp2);
		}

		if(par -> GetParBool("SCAframeIn"))
		{
			G4double SCF1x = par -> GetParDouble("SCAframe1x");
			G4double SCF1y = par -> GetParDouble("SCAframe1y");
			G4double SCF1z = par -> GetParDouble("SCAframe1z");

			G4double SCF2x = par -> GetParDouble("SCAframe2x");
			G4double SCF2y = par -> GetParDouble("SCAframe2y");
			G4double SCF2z = par -> GetParDouble("SCAframe2z");

			G4Box* solidFrame1 = new G4Box("F1", SCF1x/2, SCF1y/2, SCF1z/2);
			G4LogicalVolume* logicFrame1 = new G4LogicalVolume(solidFrame1, matAl, "Frame1");
			G4Box* solidFrame2 = new G4Box("F2", SCF2x/2, SCF2y/2, SCF2z/2);
			G4LogicalVolume* logicFrame2 = new G4LogicalVolume(solidFrame2, matAl, "Frame2");
			{
				G4VisAttributes* attFrame1 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
				attFrame1 -> SetForceWireframe(true);
				logicFrame1 -> SetVisAttributes(attFrame1);
				G4VisAttributes* attFrame2 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
				attFrame2 -> SetForceWireframe(true);
				logicFrame2 -> SetVisAttributes(attFrame2);
			}
			if( par -> GetParInt("StartCounterAIn") >=1)
			{
				auto pvpf1u = new G4PVPlacement(0, G4ThreeVector(0,-(SCF2y+SCF1y)/2,SCAzOffset_1), logicFrame1, "SCAF1u", logicWorld, false, par -> GetParInt("SCAF1uID"), true);
				auto pvpf1d = new G4PVPlacement(0, G4ThreeVector(0,(SCF2y+SCF1y)/2,SCAzOffset_1), logicFrame1, "SCAF1d", logicWorld, false, par -> GetParInt("SCAF1dID"), true);
				auto pvpf2l = new G4PVPlacement(0, G4ThreeVector(-(SCF1x-SCF2x)/2,0,SCAzOffset_1), logicFrame2, "SCAF2l", logicWorld, false, par -> GetParInt("SCAF1lID"), true);
				auto pvpf2r = new G4PVPlacement(0, G4ThreeVector((SCF1x-SCF2x)/2,0,SCAzOffset_1), logicFrame2, "SCAF2r", logicWorld, false, par -> GetParInt("SCAF1rID"), true);
			}
			if( par -> GetParInt("StartCounterAIn") >=2)
			{
				auto pvpf1u = new G4PVPlacement(0, G4ThreeVector(0,-(SCF2y+SCF1y)/2,SCAzOffset_2), logicFrame1, "SCAF1u", logicWorld, false, par -> GetParInt("SCAF2uID"), true);
				auto pvpf1d = new G4PVPlacement(0, G4ThreeVector(0,(SCF2y+SCF1y)/2,SCAzOffset_2), logicFrame1, "SCAF1d", logicWorld, false, par -> GetParInt("SCAF2dID"), true);
				auto pvpf2l = new G4PVPlacement(0, G4ThreeVector(-(SCF1x-SCF2x)/2,0,SCAzOffset_2), logicFrame2, "SCAF2l", logicWorld, false, par -> GetParInt("SCAF2lID"), true);
				auto pvpf2r = new G4PVPlacement(0, G4ThreeVector((SCF1x-SCF2x)/2,0,SCAzOffset_2), logicFrame2, "SCAF2r", logicWorld, false, par -> GetParInt("SCAF2rID"), true);
			}
		}
		if(par -> GetParBool("SCAsensorIn"))
		{
			G4double SCAsx = par -> GetParDouble("SCAsx");
			G4double SCAsy = par -> GetParDouble("SCAsy");
			G4double SCAsz = par -> GetParDouble("SCAsz");

			G4Box* solids = new G4Box("SCAs", SCAsx/2, SCAsy/2, SCAsz/2);
			G4LogicalVolume* logics = new G4LogicalVolume(solids, matSi, "SCs");
			{
				G4VisAttributes* atts = new G4VisAttributes(G4Colour(G4Colour::Blue()));
				atts -> SetForceWireframe(true);
				logics -> SetVisAttributes(atts);
			}
			if( par -> GetParInt("StartCounterAIn") >=1)
			{
				auto pvp1l = new G4PVPlacement(0, G4ThreeVector(-SCAx/2-20,0,SCAzOffset_1+SCAz/2), logics, "SCs1l", logicWorld, false, par->GetParInt("SCAs1lID"), true);
				runManager -> SetSensitiveDetector(pvp1l);
				auto pvp2r = new G4PVPlacement(0, G4ThreeVector(SCAx/2+20,0,SCAzOffset_1+SCAz/2), logics, "SCs1r", logicWorld, false, par->GetParInt("SCAs1rID"), true);
				runManager -> SetSensitiveDetector(pvp2r);
			}
			if( par -> GetParInt("StartCounterAIn") >=2)
			{
				auto pvp1l = new G4PVPlacement(0, G4ThreeVector(-SCAx/2-20,0,SCAzOffset_2+SCAz/2), logics, "SCs2l", logicWorld, false, par->GetParInt("SCAs2lID"), true);
				runManager -> SetSensitiveDetector(pvp1l);
				auto pvp2r = new G4PVPlacement(0, G4ThreeVector(SCAx/2+20,0,SCAzOffset_2+SCAz/2), logics, "SCs2r", logicWorld, false, par->GetParInt("SCAs2rID"), true);
				runManager -> SetSensitiveDetector(pvp2r);
			}
		}
	}

	//Start counter B : 2
	if ( par -> GetParInt("StartCounterBIn") != 0 )
	{
		G4double SCBx = par -> GetParDouble("SCBx");
		G4double SCBy = par -> GetParDouble("SCBy");
		G4double SCBz = par -> GetParDouble("SCBz");
		G4double SCBzOffset_1 = par -> GetParDouble("SCBzOffset_1");
		G4double SCBzOffset_2 = par -> GetParDouble("SCBzOffset_2");

		G4Box *solidSCB = new G4Box("SCB", SCBx/2.0, SCBy/2.0, SCBz/2.0);
		G4LogicalVolume *logicSCB = new G4LogicalVolume(solidSCB, matSC, "SCB");
		{
			G4VisAttributes * attSCB = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attSCB -> SetForceWireframe(true);
			logicSCB -> SetVisAttributes(attSCB);
		}
		if( par -> GetParInt("StartCounterBIn") >=1)
		{
			auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,0,SCBzOffset_1+SCBz/2), logicSCB, "SCB_1", logicWorld, false, par -> GetParInt("SCB1ID"), true);
			runManager -> SetSensitiveDetector(pvp1);
		}
		if( par -> GetParInt("StartCounterBIn") >=2)
		{
			auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,0,SCBzOffset_2), logicSCB, "SCB_2", logicWorld, false, par -> GetParInt("SCB2ID"), true);
			runManager -> SetSensitiveDetector(pvp2);
		}
	}

	//Start counter C : 2
	if ( par -> GetParInt("StartCounterCIn") != 0 )
	{
		G4double SCCx = par -> GetParDouble("SCCx");
		G4double SCCy = par -> GetParDouble("SCCy");
		G4double SCCz = par -> GetParDouble("SCCz");
		G4double SCCzOffset_1 = par -> GetParDouble("SCCzOffset_1");
		G4double SCCzOffset_2 = par -> GetParDouble("SCCzOffset_2");

		G4Box *solidSCC = new G4Box("SCC", SCCx/2.0, SCCy/2.0, SCCz/2.0);
		G4LogicalVolume *logicSCC = new G4LogicalVolume(solidSCC, matSC, "SCC");
		{
			G4VisAttributes * attSCC = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attSCC -> SetForceWireframe(true);
			logicSCC -> SetVisAttributes(attSCC);
		}
		if( par -> GetParInt("StartCounterCIn") >= 1 )
		{
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,0,SCCzOffset_1+SCCz/2), logicSCC, "SCC_1", logicWorld, false, par -> GetParInt("SCC1ID"), true);
		runManager -> SetSensitiveDetector(pvp1);
		}
		if( par -> GetParInt("StartCounterCIn") >= 2 )
		{
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,0,SCCzOffset_2+SCCz/2), logicSCC, "SCC_2", logicWorld, false, par -> GetParInt("SCC2ID"), true);
		runManager -> SetSensitiveDetector(pvp2);
		}
		if(par -> GetParBool("SCCframeIn"))
		{
			G4double SCF1x = par -> GetParDouble("SCCframe1x");
			G4double SCF1y = par -> GetParDouble("SCCframe1y");
			G4double SCF1z = par -> GetParDouble("SCCframe1z");

			G4double SCF2x = par -> GetParDouble("SCCframe2x");
			G4double SCF2y = par -> GetParDouble("SCCframe2y");
			G4double SCF2z = par -> GetParDouble("SCCframe2z");

			G4Box* solidFrame1 = new G4Box("F1", SCF1x/2, SCF1y/2, SCF1z/2);
			G4LogicalVolume* logicFrame1 = new G4LogicalVolume(solidFrame1, matAl, "Frame1");
			G4Box* solidFrame2 = new G4Box("F2", SCF2x/2, SCF2y/2, SCF2z/2);
			G4LogicalVolume* logicFrame2 = new G4LogicalVolume(solidFrame2, matAl, "Frame2");
			{
				G4VisAttributes* attFrame1 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
				attFrame1 -> SetForceWireframe(true);
				logicFrame1 -> SetVisAttributes(attFrame1);
				G4VisAttributes* attFrame2 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
				attFrame2 -> SetForceWireframe(true);
				logicFrame2 -> SetVisAttributes(attFrame2);
			}
			if( par -> GetParInt("StartCounterCIn") >=1)
			{
				auto pvpf1u = new G4PVPlacement(0, G4ThreeVector(0,-(SCF2y+SCF1y)/2,SCCzOffset_1), logicFrame1, "SCCF1u", logicWorld, false, par -> GetParInt("SCCF1uID"), true);
				auto pvpf1d = new G4PVPlacement(0, G4ThreeVector(0,(SCF2y+SCF1y)/2,SCCzOffset_1), logicFrame1, "SCCF1d", logicWorld, false, par -> GetParInt("SCCF1dID"), true);
				auto pvpf1l = new G4PVPlacement(0, G4ThreeVector(-(SCF1x-SCF2x)/2,0,SCCzOffset_1), logicFrame2, "SCCF2l", logicWorld, false, par -> GetParInt("SCCF1lID"), true);
				auto pvpf1r = new G4PVPlacement(0, G4ThreeVector((SCF1x-SCF2x)/2,0,SCCzOffset_1), logicFrame2, "SCCF2r", logicWorld, false, par -> GetParInt("SCCF1rID"), true);
			}
			if( par -> GetParInt("StartCounterCIn") >=2)
			{
				auto pvpf2u = new G4PVPlacement(0, G4ThreeVector(0,-(SCF2y+SCF1y)/2,SCCzOffset_2), logicFrame1, "SCCF1u", logicWorld, false, par -> GetParInt("SCCF2uID"), true);
				auto pvpf2d = new G4PVPlacement(0, G4ThreeVector(0,(SCF2y+SCF1y)/2,SCCzOffset_2), logicFrame1, "SCCF1d", logicWorld, false, par -> GetParInt("SCCF2dID"), true);
				auto pvpf2l = new G4PVPlacement(0, G4ThreeVector(-(SCF1x-SCF2x)/2,0,SCCzOffset_2), logicFrame2, "SCCF2l", logicWorld, false, par -> GetParInt("SCCF2lID"), true);
				auto pvpf2r = new G4PVPlacement(0, G4ThreeVector((SCF1x-SCF2x)/2,0,SCCzOffset_2), logicFrame2, "SCCF2r", logicWorld, false, par -> GetParInt("SCCF2rID"), true);
			}
		}
		if(par -> GetParBool("SCCsensorIn"))
		{
			G4double SCCsx = par -> GetParDouble("SCCsx");
			G4double SCCsy = par -> GetParDouble("SCCsy");
			G4double SCCsz = par -> GetParDouble("SCCsz");

			G4Box* solids = new G4Box("SCCs", SCCsx/2, SCCsy/2, SCCsz/2);
			G4LogicalVolume* logics = new G4LogicalVolume(solids, matSi, "SCs");
			{
				G4VisAttributes* atts = new G4VisAttributes(G4Colour(G4Colour::Blue()));
				atts -> SetForceWireframe(true);
				logics -> SetVisAttributes(atts);
			}
			if( par -> GetParInt("StartCounterCIn") >=1)
			{
				auto pvp1l = new G4PVPlacement(0, G4ThreeVector(-SCCx/2-2,0,SCCzOffset_1+SCCz/2), logics, "SCs3l", logicWorld, false, par->GetParInt("SCCs1lID"), true);
				runManager -> SetSensitiveDetector(pvp1l);
				auto pvp2r = new G4PVPlacement(0, G4ThreeVector(SCCx/2+2,0,SCCzOffset_1+SCCz/2), logics, "SCs3r", logicWorld, false, par->GetParInt("SCCs1rID"), true);
				runManager -> SetSensitiveDetector(pvp2r);
			}
			if( par -> GetParInt("StartCounterCIn") >=2)
			{
				auto pvp1l = new G4PVPlacement(0, G4ThreeVector(-SCCx/2-2,0,SCCzOffset_2+SCCz/2), logics, "SCs4l", logicWorld, false, par->GetParInt("SCCs2lID"), true);
				runManager -> SetSensitiveDetector(pvp1l);
				auto pvp2r = new G4PVPlacement(0, G4ThreeVector(SCCx/2+2,0,SCCzOffset_2+SCCz/2), logics, "SCs4r", logicWorld, false, par->GetParInt("SCCs2rID"), true);
				runManager -> SetSensitiveDetector(pvp2r);
			}
		}
	}

	//Start counter D : 2
	if ( par -> GetParInt("StartCounterDIn") )
	{
		G4double SCDx = par -> GetParDouble("SCDx");
		G4double SCDy = par -> GetParDouble("SCDy");
		G4double SCDz = par -> GetParDouble("SCDz");
		G4double SCDzOffset_1 = par -> GetParDouble("SCDzOffset_1");
		G4double SCDzOffset_2 = par -> GetParDouble("SCDzOffset_2");
		G4double SCDr2 = par -> GetParDouble("SCDr2");
		G4double SCDrot = par -> GetParDouble("SCDrot");
		G4double SCDzadd2 = SCDr2 * cos(SCDrot*deg);
		G4double SCDxadd2 = SCDr2 * sin(SCDrot*deg);

		G4RotationMatrix* Rot = new G4RotationMatrix;
		Rot->rotateY(SCDrot*deg);

		G4Box *solidSCD = new G4Box("SCD", SCDx/2.0, SCDy/2.0, SCDz/2.0);
		G4LogicalVolume *logicSCD = new G4LogicalVolume(solidSCD, matSC, "SCD");
		{
			G4VisAttributes * attSCD = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attSCD -> SetForceWireframe(true);
			logicSCD -> SetVisAttributes(attSCD);
		}
		if( par -> GetParInt("StartCounterDIn") >= 1 )
		{
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,0,SCDzOffset_1+SCDz/2), logicSCD, "SCD_1", logicWorld, false, par -> GetParInt("SCD1ID"), true);
		runManager -> SetSensitiveDetector(pvp1);
		}

		if( par -> GetParInt("StartCounterDIn") >= 2 )
		{
		auto pvp2 = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(SCDxadd2,+0,SCDzOffset_2+SCDzadd2)), logicSCD, "SCD_2", logicWorld, false, par -> GetParInt("SCD2ID"), true);
		runManager -> SetSensitiveDetector(pvp2);
		}
//		auto pvp2 = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(SCDxadd2,+0,SCDzOffset_1+SCDzadd2+SCDz/2)), logicSCD, "SCD_2", logicWorld, false, par -> GetParInt("SCD2ID"), true);
//		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,0,SCDzOffset_2+SCDz/2), logicSCD, "SCD_2", logicWorld, false, par -> GetParInt("SCD2ID"), true);
		}
	
#if 1

	//Veto counter
	if( par -> GetParBool("VetoIn") )
	{

		G4double Vetox = par -> GetParDouble("Vetox");
		G4double Vetoy = par -> GetParDouble("Vetoy");
		G4double Vetoz = par -> GetParDouble("Vetoz");
		G4double VetozOffset = par -> GetParDouble("VetozOffset");

		G4Box *solidVeto = new G4Box("Veto", Vetox/2.0, Vetoy/2.0, Vetoz/2.0);
		G4LogicalVolume *logicVeto = new G4LogicalVolume(solidVeto, matSC, "Veto");
		{
			G4VisAttributes * attVeto = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			logicVeto -> SetVisAttributes(attVeto);
		}
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,(Vetoy+10.0)/2,VetozOffset+Vetoz/2), logicVeto, "Veto1", logicWorld, false, 3, true);
		runManager -> SetSensitiveDetector(pvp1);

		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,-(Vetoy+10.0)/2,VetozOffset+Vetoz/2), logicVeto, "Veto2", logicWorld, false, 4, true);
		runManager -> SetSensitiveDetector(pvp2);
	}

	//BDC1
	if( par -> GetParBool("BDC1In") )
	{
		G4double BDC1x = par -> GetParDouble("BDC1x");
		G4double BDC1y = par -> GetParDouble("BDC1y");
		G4double BDC1z = par -> GetParDouble("BDC1z");
		G4double BDC1framex = par -> GetParDouble("BDC1framex");
		G4double BDC1framey = par -> GetParDouble("BDC1framey");
		G4double BDC1framez = par -> GetParDouble("BDC1framez");
		G4double BDC1zOffset = par -> GetParDouble("BDC1zOffset");
		G4double BDC1rot = par -> GetParDouble("BDC1rot");
		G4double BDC1r = par -> GetParDouble("BDC1r");
		G4double BDC1r1 = par -> GetParDouble("BDC1r1");
		G4double BDC1r2 = par -> GetParDouble("BDC1r2");
		G4double BDC1zadd = BDC1r * cos(BDC1rot*deg);
		G4double BDC1zadd1 = BDC1r1 * cos(BDC1rot*deg);
		G4double BDC1zadd2 = BDC1r2 * cos(BDC1rot*deg);
		G4double BDC1xadd = BDC1r * sin(BDC1rot*deg);
		G4double BDC1xadd1 = BDC1r1 * sin(BDC1rot*deg);
		G4double BDC1xadd2 = BDC1r2 * sin(BDC1rot*deg);

		G4RotationMatrix* Rot = new G4RotationMatrix;
		Rot->rotateY(BDC1rot*deg);

		G4Box *solidBDC1 = new G4Box("BDC1", BDC1x/2.0, BDC1y/2.0, BDC1z/2.0);
		G4Box *solidframe = new G4Box("BDC1frame", BDC1framex/2.0, BDC1framey/2.0, BDC1framez/2.0);

		G4SubtractionSolid *solidSubframe = new G4SubtractionSolid("Subframe1", solidframe, solidBDC1, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicBDC1 = new G4LogicalVolume(solidBDC1, matGas, "BDC1");
		{
			G4VisAttributes * attBDC1 = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attBDC1 -> SetForceWireframe(true);
			logicBDC1 -> SetVisAttributes(attBDC1);
		}
//		auto pvp = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(x_off,+0,ATTPCzOffset+ATTPCz/2)), logicATTPC, "ATTPC", logicWorld, false, 12, true);
		auto pvp = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(BDC1xadd,0,BDC1zOffset+BDC1zadd)), logicBDC1, "BDC1", logicWorld, false, par -> GetParInt("BDC1ID"), true);
		runManager -> SetSensitiveDetector(pvp);

		G4LogicalVolume *logicSubframe = new G4LogicalVolume(solidSubframe, matAl, "Subframe1");
		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attSubS -> SetForceWireframe(true);
			logicSubframe -> SetVisAttributes(attSubS);
		}
		if( par -> GetParBool("BDC1frameIn"))
		{
			auto pvp1 = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(BDC1xadd1,0,BDC1zOffset+BDC1zadd1)), logicSubframe,"BDC1frame1",logicWorld,false,par->GetParInt("BDC1frame1ID"),true);
			runManager -> SetSensitiveDetector(pvp1);
			auto pvp2 = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(BDC1xadd2,0,BDC1zOffset+BDC1zadd2)), logicSubframe,"BDC1frame2",logicWorld,false,par->GetParInt("BDC1frame2ID"),true);
			runManager -> SetSensitiveDetector(pvp2);
		}
	}

	//BDC2
	if( par -> GetParBool("BDC2In") )
	{
		G4double BDC2x = par -> GetParDouble("BDC2x");
		G4double BDC2y = par -> GetParDouble("BDC2y");
		G4double BDC2z = par -> GetParDouble("BDC2z");
		G4double BDC2framex = par -> GetParDouble("BDC2framex");
		G4double BDC2framey = par -> GetParDouble("BDC2framey");
		G4double BDC2framez = par -> GetParDouble("BDC2framez");
		G4double BDC2zOffset = par -> GetParDouble("BDC2zOffset");
		G4double BDC2rot = par -> GetParDouble("BDC2rot");
		G4double BDC2r = par -> GetParDouble("BDC2r");
		G4double BDC2r1 = par -> GetParDouble("BDC2r1");
		G4double BDC2r2 = par -> GetParDouble("BDC2r2");
		G4double BDC2zadd = BDC2r * cos(BDC2rot*deg);
		G4double BDC2zadd1 = BDC2r1 * cos(BDC2rot*deg);
		G4double BDC2zadd2 = BDC2r2 * cos(BDC2rot*deg);
		G4double BDC2xadd = BDC2r * sin(BDC2rot*deg);
		G4double BDC2xadd1 = BDC2r1 * sin(BDC2rot*deg);
		G4double BDC2xadd2 = BDC2r2 * sin(BDC2rot*deg);

		G4RotationMatrix* Rot = new G4RotationMatrix;
		Rot->rotateY(BDC2rot*deg);

		G4Box *solidBDC2 = new G4Box("BDC2", BDC2x/2.0, BDC2y/2.0, BDC2z/2.0);
		G4Box *solidframe = new G4Box("BDC2frame",BDC2framex/2.0,BDC2framey/2.0,BDC2framez/2.0);
		G4LogicalVolume *logicBDC2 = new G4LogicalVolume(solidBDC2, matGas, "BDC2");

		G4SubtractionSolid *solidSubframe = new G4SubtractionSolid("Subframe2", solidframe, solidBDC2, 0, G4ThreeVector(0,0,0));
		G4LogicalVolume *logicSubframe = new G4LogicalVolume(solidSubframe, matAl, "Subframe2");
		{
			G4VisAttributes * attBDC2 = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attBDC2 -> SetForceWireframe(true);
			logicBDC2 -> SetVisAttributes(attBDC2);
		}
		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attSubS -> SetForceWireframe(true);
			logicSubframe -> SetVisAttributes(attSubS);
		}
		auto pvp = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(BDC2xadd,0,BDC2zOffset+BDC2zadd)), logicBDC2, "BDC2", logicWorld, false, par -> GetParInt("BDC2ID"), true);
		runManager -> SetSensitiveDetector(pvp);

		if( par -> GetParBool("BDC2frameIn"))
		{
		auto pvp1 = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(BDC2xadd1,0,BDC2zOffset+BDC2zadd1)), logicSubframe, "BDC2frame1", logicWorld, false, par -> GetParInt("BDC2frame1ID"), true);
		runManager -> SetSensitiveDetector(pvp1);

		auto pvp2 = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(BDC2xadd2,0,BDC2zOffset+BDC2zadd2)), logicSubframe, "BDC2frame2", logicWorld, false, par -> GetParInt("BDC2frame2ID"), true);
		runManager -> SetSensitiveDetector(pvp2);
		}
	}

	//ATTPCIn
	if ( par -> GetParBool("ATTPCIn") )
	{

		G4double ATTPCx = par -> GetParDouble("ATTPCx");
		G4double ATTPCy = par -> GetParDouble("ATTPCy");
		G4double ATTPCz = par -> GetParDouble("ATTPCz");

		G4double ATTPCzOffset = par -> GetParDouble("ATTPCzOffset");
		G4double ATTPCr = par -> GetParDouble("ATTPCr");
		G4double ATTPCtheta = par -> GetParDouble("ATTPCtheta");

		G4double ATTPCx_Off = ATTPCr * sin(ATTPCtheta*deg);
		G4double ATTPCz_Off = ATTPCzOffset + ATTPCr * cos(ATTPCtheta*deg);

		G4Box *solidATTPC = new G4Box("ATTPC", ATTPCx/2.0, ATTPCy/2.0, ATTPCz/2.0);
		G4LogicalVolume *logicATTPC = new G4LogicalVolume(solidATTPC, matGas, "ATTPC");

		{
			G4VisAttributes * attATTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attATTPC -> SetForceWireframe(true);
			logicATTPC -> SetVisAttributes(attATTPC);
		}

		G4RotationMatrix* Rot = new G4RotationMatrix;
		Rot->rotateY(ATTPCtheta*deg);
		//auto pvp = new G4PVPlacement(0, G4ThreeVector(+925,+0,ATTPCzOffset+ATTPCz/2), logicATTPC, "ATTPC", logicWorld, false, 10, true);
		auto pvp = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(ATTPCx_Off,+0,ATTPCzOffset+ATTPCz_Off)), logicATTPC, "ATTPC", logicWorld, false, par -> GetParInt("ATTPCID"), true);
		runManager -> SetSensitiveDetector(pvp);

	}

	//Target2
	if ( par -> GetParBool("Target2In") )
	{
		G4double Target2x = par -> GetParDouble("Target2x");
		G4double Target2y = par -> GetParDouble("Target2y");
		G4double Target2z = par -> GetParDouble("Target2z");
		G4double Target2zOffset = par -> GetParDouble("Target2zOffset");

		G4Box *solidTarget2 = new G4Box("Target2", Target2x/2.0, Target2y/2.0, Target2z/2.0);
		G4LogicalVolume *logicTarget2 = new G4LogicalVolume(solidTarget2, matCH2, "Traget2");
		{
			G4VisAttributes * attTarget2 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget2 -> SetVisAttributes(attTarget2);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target2zOffset+Target2z/2), logicTarget2, "Target2", logicWorld, false, 21, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//TPC
	if ( par -> GetParBool("TPCIn") )
	{
		G4double tpcInnerRadius = par -> GetParDouble("TPCrMin");
		G4double tpcOuterRadius = par -> GetParDouble("TPCrMax");
		G4double tpcLength = par -> GetParDouble("TPCLength");
		G4double tpcZOffset = par -> GetParDouble("TPCzOffset");

		G4Tubs *solidTPC = new G4Tubs("TPC", tpcInnerRadius, tpcOuterRadius, .5*tpcLength, 0., 360*deg);
		G4LogicalVolume *logicTPC = new G4LogicalVolume(solidTPC, matGas, "TPC");
		{
			G4VisAttributes * attTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attTPC -> SetForceWireframe(true);
			logicTPC -> SetVisAttributes(attTPC);
		}
		logicTPC -> SetUserLimits(new G4UserLimits(1.*mm));
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset+tpcLength/2), logicTPC, "TPC", logicWorld, false, 7, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//BTOF
	if ( par -> GetParBool("BTOFIn") )
	{

		G4double BTOFx = par -> GetParDouble("BTOFx");
		G4double BTOFy = par -> GetParDouble("BTOFy");
		G4double BTOFz = par -> GetParDouble("BTOFz");
		G4double BTOFzOffset = par -> GetParDouble("BTOFzOffset");

		G4Box *solidBTOF1 = new G4Box("BTOF1", BTOFx/2.0, BTOFy/2.0, BTOFz/2.0);
		G4LogicalVolume *logicBTOF1 = new G4LogicalVolume(solidBTOF1, matSC, "BTOF1");
		{
			G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOF -> SetForceWireframe(true);
			logicBTOF1 -> SetVisAttributes(attBTOF);
		}

		G4Box *solidBTOF2 = new G4Box("BTOF1", BTOFy/2.0, BTOFx/2.0, BTOFz/2.0);
		G4LogicalVolume *logicBTOF2 = new G4LogicalVolume(solidBTOF2, matSC, "BTOF2");
		{
			G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOF -> SetForceWireframe(true);
			logicBTOF2 -> SetVisAttributes(attBTOF);
		}

		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(+620,+BTOFy/2,BTOFzOffset+BTOFz/2), logicBTOF1, "BTOF1", logicWorld, false, 8, true);
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(+620,-BTOFy/2,BTOFzOffset+BTOFz/2), logicBTOF1, "BTOF2", logicWorld, false, 9, true);

		auto pvp3 = new G4PVPlacement(0, G4ThreeVector(+BTOFy/2,+520,BTOFzOffset+BTOFz/2), logicBTOF2, "BTOF3", logicWorld, false, 10, true);
		auto pvp4 = new G4PVPlacement(0, G4ThreeVector(-BTOFy/2,+520,BTOFzOffset+BTOFz/2), logicBTOF2, "BTOF4", logicWorld, false, 11, true);

		runManager -> SetSensitiveDetector(pvp1);
		runManager -> SetSensitiveDetector(pvp2);
		runManager -> SetSensitiveDetector(pvp3);
		runManager -> SetSensitiveDetector(pvp4);
	}

	//NeutronDetIn
	if ( par -> GetParBool("NeutronDetIn") )
	{

		G4double NeutronDetx = par -> GetParDouble("NeutronDetx");
		G4double NeutronDety = par -> GetParDouble("NeutronDety");
		G4double NeutronDetz = par -> GetParDouble("NeutronDetz");
		G4double NeutronDetzOffset = par -> GetParDouble("NeutronDetzOffset");

		G4Box *solidNeutronDet = new G4Box("NeutronDet", NeutronDetx/2.0, NeutronDety/2.0, NeutronDetz/2.0);
		G4LogicalVolume *logicNeutronDet = new G4LogicalVolume(solidNeutronDet, matSC, "NeutronDet");
		{
			G4VisAttributes * attNeutronDet = new G4VisAttributes(G4Colour(G4Colour::Green()));
			attNeutronDet -> SetForceWireframe(true);
			logicNeutronDet -> SetVisAttributes(attNeutronDet);
		}

		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(+1450,+NeutronDety/2,NeutronDetzOffset+NeutronDetz/2), logicNeutronDet, "NeutronDet", logicWorld, false, 12, true);
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(+1450,-NeutronDety/2,NeutronDetzOffset+NeutronDetz/2), logicNeutronDet, "NeutronDet", logicWorld, false, 12, true);
		runManager -> SetSensitiveDetector(pvp1);
		runManager -> SetSensitiveDetector(pvp2);

	}
#endif
	/*
  bool checkWall = par -> CheckPar("numNeutronWall");
  if (checkWall)
  {

    G4int numWall = par -> GetParInt("numNeutronWall");
    for (auto iwall = 0; iwall < numWall; ++iwall) {
      auto naStackAxis = par -> GetParAxis(Form("naStackAxis%d",iwall));
      auto naNumStack = par -> GetParInt(Form("naNumStack%d",iwall));
      auto nadX = par -> GetParDouble(Form("nadX%d",iwall));
      auto nadY = par -> GetParDouble(Form("nadY%d",iwall));
      auto nadZ = par -> GetParDouble(Form("nadZ%d",iwall));
      auto naXOffset = par -> GetParDouble(Form("naXOffset%d",iwall));
      auto naYOffset = par -> GetParDouble(Form("naYOffset%d",iwall));
      auto naZOffset = par -> GetParDouble(Form("naZOffset%d",iwall));

      G4Box* solidScint = new G4Box(Form("Scintillator_%d",iwall), 0.5*nadX, 0.5*nadY, 0.5*nadZ);
      G4LogicalVolume* logicScint = new G4LogicalVolume(solidScint, scint_mat, Form("Scintillator_%d",iwall));

      KBGeoBoxStack boxStack(naXOffset,naYOffset,naZOffset,nadX,nadY,nadZ,naNumStack,naStackAxis,KBVector3::kZ);

      for (auto copy = 0; copy < naNumStack; ++copy) {
        Int_t id = 10000+copy+iwall*100;
        G4String name = Form("Scintillator_%d_%d",iwall,copy);
        auto box = boxStack.GetBox(copy);
        auto pos = box.GetCenter();
        G4ThreeVector gpos(pos.X(),pos.Y(),pos.Z());
        auto cpvp = new G4PVPlacement(0, gpos, logicScint, name, logicWorld, false, id, true);
        runManager -> SetSensitiveDetector(cpvp);
      }
    }
  }
	*/

  //G4double bfieldx = par -> GetParDouble("bfieldx");
  //G4double bfieldy = par -> GetParDouble("bfieldy");
  //G4double bfieldz = par -> GetParDouble("bfieldz");
  //new G4GlobalMagFieldMessenger(G4ThreeVector(bfieldx*tesla, bfieldy*tesla, bfieldz*tesla));

  return physWorld;
}
