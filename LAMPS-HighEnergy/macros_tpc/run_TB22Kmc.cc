#include "globals.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4GenericPhysicsList.hh"
#include "G4StepLimiterPhysics.hh"

#include "KBCompiled.h"
#include "KBEventAction.hh"
#include "KBG4RunManager.hh"
#include "KBParameterContainer.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "KBSteppingAction.hh"
#include "KBTrackingAction.hh"

#include "TB22KDetectorConstruction.hh"

int main(int argc, char** argv)
{
//	cout << argc << endl;
//	cout << argv[0] << endl;
//	cout << argv[1] << endl;
	std::string physListStr;

	G4PhysListFactory* physListFac = new G4PhysListFactory();
	G4VModularPhysicsList* physList = nullptr;

	if      (argc == 2) // usage : ./run.g4sim <par>
	{
		std::vector<G4String>* phys_vec = new std::vector<G4String>;

		phys_vec -> push_back("G4StepLimiterPhysics");

		phys_vec -> push_back("G4EmStandardPhysics");		
		phys_vec -> push_back("G4EmExtraPhysics");
		phys_vec -> push_back("G4DecayPhysics");
		phys_vec -> push_back("G4HadronElasticPhysics");
		phys_vec -> push_back("G4HadronPhysicsQGSP_BERT");
		phys_vec -> push_back("G4StoppingPhysics");
		phys_vec -> push_back("G4IonPhysics");
		phys_vec -> push_back("G4NeutronTrackingCut");	// default : QGSP_BERT

//		phys_vec -> push_back("G4IonElasticPhysics");	// in QGSP_BIC

//		phys_vec -> push_back("G4RadioactiveDecayPhysics"); // for HP option

		physList = new G4GenericPhysicsList(phys_vec);
	}
	else if (argc == 3)	// usage : ./run.g4sim <par> <PL>
	{
//		physListStr = "QGSP_BERT";
		physListStr = argv[2];
		if(!physListFac -> IsReferencePhysList(physListStr))
		{
			cout << physListStr << " is not available" << endl;
			assert(false);
		}
		physList = physListFac -> GetReferencePhysList(physListStr.c_str());
		physList->RegisterPhysics(new G4StepLimiterPhysics());
	}
	else
	{
		cout <<"Usage: ./run_Tb22Kmc.g4sim blah.conf G4PhysList\n";
		assert(false);
	}

	auto runManager = new KBG4RunManager();
	runManager->SetParameterContainer(argv[1]);
	runManager->SetUserInitialization(physList);
	runManager->SetUserInitialization(new TB22KDetectorConstruction());
	runManager->Initialize();
	runManager->Run(argc, argv);
	delete runManager;
	return 0;
}//Main
