#include "G4Run.hh"
#include "G4RunManager.hh"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

// #include "HepMC/IO_GenEvent.h"
// #include "HepMCG4AsciiReader.hh"

#include "RHICfRunAction.hh"
#include "RHICfPrimaryGeneratorAction.hh"
#include "RHICfEventAction.hh"
#include "RHICfTrackingAction.hh"

RHICfRunAction::RHICfRunAction():G4UserRunAction(), fTimer(0)
{
}

RHICfRunAction::~RHICfRunAction()
{

}

void RHICfRunAction::BeginOfRunAction(const G4Run* aRun)
{  
	G4cout << "RHICfRunAction::BeginOfRunAction() -- " << aRun->GetRunID() << " start." << G4endl;

	fSimUtil = RHICfSimUtil::GetRHICfSimUtil();
	fSimOpt = fSimUtil -> GetOptions();

	/// Timer start
	fTimer=new G4Timer();
	fTimer->Start();
	fElapsedTime=0.;

	G4RunManager* runManager = G4RunManager::GetRunManager();
	RHICfPrimaryGeneratorAction* genAction = (RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
	RHICfEventAction* eventAction = (RHICfEventAction*)runManager->GetUserEventAction();
	RHICfTrackingAction* trackingAction = (RHICfTrackingAction*)runManager->GetUserTrackingAction();

	TString inputFile = fSimOpt -> GetOptString("INPUT");
	if(fSimUtil->IsStarSimMode()){
		fInSimDstFile = new TFile(inputFile, "READ");
		if(!fInSimDstFile){
			cout << "RHICfRunAction::BeginOfRunAction() -- Error: " << inputFile << " can not opend! "<< endl;
			return;
		}
		cout << "RHICfRunAction::BeginOfRunAction() -- STARSim Mode has been detected.." << endl;

		fInSimDstTree = (TTree*)fInSimDstFile -> Get("StRHICfSimDst");
		fSimDst = new StRHICfSimDst();
		fSimDst -> ReadDstArray(fInSimDstTree);

		fOutSimDstFile = new TFile("/star/u/slee5/rhicfSim/rhicfsimv10test/rhicf/testRHICfSimDst.root", "RECREATE");
        fOutSimDstTree = fInSimDstTree -> CloneTree(0);

		genAction -> SetInputSimDstTree(fInSimDstTree);
		genAction -> SetSimDst(fSimDst);

		eventAction -> SetOutputSimDstTree(fOutSimDstTree);
		eventAction -> SetSimDst(fSimDst);

		trackingAction -> SetSimDst(fSimDst);
	}
	else{
		
	}


	// if(genname=="Pythia8.219") {
  //     Pythia8::Pythia pythia;

	// 	// Read in commands from external file.
	// 	std::stringstream sevent;
	// 	sevent << nevent;
	// 	for(int i=0; i<1; i++) {

	// 		cout << " test part! " << " " << sevent.str() << endl;
	// 		pythia.readString("Main:timesAllowErrors = 10");
	// 		pythia.readString("SoftQCD:all = on");
	// 		pythia.readString(("Main:numberOfEvents = "+sevent.str()).c_str());

	// 		pythia.readString("Beams:idA = 2212");
	// 		pythia.readString("Beams:idB = 2212");
	// 		pythia.readString("Beams:pzA = 255.");
	// 		pythia.readString("Beams:pzB = -255.");

	// 		pythia.readString("Random:setSeed = on");
	// 		TString SeedMacros = Form("Random:seed = %i", seed2);
	// 		pythia.readString(SeedMacros.Data());

	// 		int nEvent=pythia.mode("Main:numberOfEvents");
	// 		int nAbort=pythia.mode("Main:timesAllowErrors");

	// 		cout <<"nEvent " <<  nEvent << endl;

	// 		std::ofstream output(ftmp.c_str());
	// 		HepMC::IO_GenEvent ascii_io(output);

	// 		pythia.init();

	// 		int iAbort=0;
	// 		for(int iEvent=0; iEvent<nEvent; ++iEvent) {
	// 			if(!pythia.next()) {
	// 				if(pythia.info.atEndOfFile()) {
	// 					G4cout << " Aborted since reached end of Les Houches Event File" << G4endl;
	// 					break;
	// 				}
	// 				if(++iAbort < nAbort) continue;
	// 				G4cout << " Event generation aborted prematurely, owing to error!" << G4endl;
	// 				break;
	// 			}
	// 			HepMC::GenEvent* hepmcevt=new HepMC::GenEvent();
	// 			HepMC::Pythia8ToHepMC ToHepMC;
	// 			ToHepMC.fill_next_event(pythia, hepmcevt, iEvent);
	// 			ascii_io << hepmcevt;
	// 			//      ascii_io.print();

	// 			cout << "test << " << iEvent << endl;
	// 			delete hepmcevt;
	// 		}
	// 		pythia.stat();

	// 		if(i==0) sigEla=pythia.info.sigmaGen();
	// 		else     sigIne=pythia.info.sigmaGen();
	// 	}
	// 	sigTot=sigEla+sigIne;
  //   }


  //   flag_merged=flag_detector;
  // }

  // HepMCG4AsciiReader* hepMC=dynamic_cast<HepMCG4AsciiReader*>(genAction->GetGenerator());
  // hepMC->SetFileName(ftmp);
  // hepMC->Initialize();

}

void RHICfRunAction::EndOfRunAction(const G4Run*)
{
	cout << " RunAction done " << endl; // test

	fInSimDstFile -> Close();

	fOutSimDstFile -> cd();
	fOutSimDstTree -> Write();
	fOutSimDstFile -> Close();

	/// Timer stop
	fTimer->Stop();
	fElapsedTime+=fTimer->GetRealElapsed();
	G4cout << "Timer: " << fElapsedTime << " sec." << G4endl;

    cout << "RHICfRunAction::EndOfRunAction() -- Run has done" << endl;
}