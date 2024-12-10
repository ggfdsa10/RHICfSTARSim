#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "RHICfDetectorConstruction.hh"
#include "RHICfPhysicsList.hh"
#include "RHICfPrimaryGeneratorAction.hh"
#include "RHICfRunAction.hh"
#include "RHICfEventAction.hh"
#include "RHICfTrackingAction.hh"
#include "RHICfReconstruction.hh"

#include "RHICfSimPar.hh"
#include "RHICfSimUtil.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#define G4UI_USE_TCSH

#if defined(G4UI_USE_TCSH)
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#else
#include "G4UIterminal.hh"
#endif

int main(int argc,char** argv)
{
    if(argc==1) {
        G4cout << "No input file!" << G4endl;
        return 1;
    }

    // ==================================== RHICfSimUtil  ===============================================
    RHICfSimUtil* simUtil = RHICfSimUtil::GetRHICfSimUtil(argc, argv);
    RHICfSimOptions* simOpt = simUtil -> GetOptions();

    /// Set seed for Geant4
    Long64_t geantSeed = 0;
    if(!simOpt->CheckOpt("SEED1")){geantSeed = simUtil -> GenSeed();}
    else{geantSeed = simOpt->GetOptInt("SEED1");}
    G4Random::setTheSeed(geantSeed);

  // reco ========================================================

  /// Reconstruction mode: reconstruct and exit
  // std::string reco=vm["RECONSTRUCT"].as<std::string>();
  // if(reco=="TRUE") {
  //   G4cout << "Reconstruction mode" << G4endl;

  //   /// Set output
  //   fs::path foutput;
  //   foutput=vm["OUTPUTFILE"].as<fs::path>();
  //   /// Prepare output file
  //   if(foutput.branch_path()!="" && !fs::exists(foutput.branch_path())) {
  //     boost::system::error_code error;
  //     const bool result=fs::create_directories(foutput.branch_path(), error);
  //     if(!result || error) {
	// std::string mes="Could not create directory "+foutput.branch_path().string()+".";
	// G4Exception("rhicf","Invalid Directory",FatalException,mes.c_str());
  //     }
  //   }

    // /// Set input
    // fs::path finput;
    // finput=vm["INPUTFILE"].as<fs::path>();
    // TFile *fin=new TFile(finput.string().c_str());
    // if(!fin->IsZombie()) {
    //   TTree *tin=(TTree*)fin->Get("RunInfo");

    //   RunInfo* runInfo=new RunInfo();
    //   tin->SetBranchAddress("RunInfo",&runInfo);
    //   tin->GetEntry(0);
    // }else{
    //   G4cerr << finput.string() << " does not exist!" << G4endl;
    //   exit(1);
    // }

    // /// Set tables
    // fs::path ftables;
    // try{
    //   ftables=vm["TABLESDIR"].as<fs::path>();
    // }catch(const boost::bad_any_cast& ex) {
    //   G4cout << ex.what() << G4endl;
    //   exit(1);
    // }
    // if(!fs::exists(ftables)) {
    //   std::string mes="Directory "+ ftables.string() +" does not exist.";
    //   G4Exception("rhicf","Invalid Directory",FatalException,mes.c_str());
    // }

  //   G4cout << "Input:  " << finput.string()  << G4endl;
  //   G4cout << "Output: " << foutput.string() << G4endl;

  //   TFile *fout=new TFile(foutput.string().c_str(),"recreate");
  //   TTree *trun_out=new TTree("RunInfo", "RunInfo");
  //   TTree *tevent_out=new TTree("EventInfo", "EventInfo");
  //   RunInfo* runInfo_out=new RunInfo();
  //   trun_out->Branch("RunInfo", &runInfo_out);
  //   RHICfSimEvent* simEvent_out=new RHICfSimEvent();
  //   tevent_out->Branch("SimEvent", &simEvent_out);
  //   RHICfReconstruction* reconstruction=new RHICfReconstruction(new TFile(finput.string().c_str()),fout,ftables);

  //   trun_out->Print();
  //   tevent_out->Print();

  //   fout->cd();
  //   trun_out->SetDirectory(fout);
  //   trun_out->Write();
  //   tevent_out->SetDirectory(fout);
  //   tevent_out->Write();

  //   fout->Close();

  //   return 1;
  // }else if(reco=="FALSE") {
  // }else{
  //   G4Exception("rhicf","Invalid opttion",FatalException,"RECONSTRUCT option: choose TRUE or FALSE");
  // }
  // reco end =====================================

    // Construct the default run manager
    G4RunManager* runManager = new G4RunManager;

    RHICfDetectorConstruction* detector=new RHICfDetectorConstruction();
    runManager->SetUserInitialization(detector);

    G4VUserPhysicsList* physics=new RHICfPhysicsList("QGSP_BERT");
    runManager->SetUserInitialization(physics);

    G4UserRunAction* runAction=new RHICfRunAction();
    runManager->SetUserAction(runAction);
    RHICfPrimaryGeneratorAction* genAction=new RHICfPrimaryGeneratorAction();
    runManager->SetUserAction(genAction);
    G4UserEventAction* eventAction=new RHICfEventAction();
    runManager->SetUserAction(eventAction);
    G4UserTrackingAction* trackAction=new RHICfTrackingAction();
    runManager->SetUserAction(trackAction);


    // Initialize G4 kernel
    runManager->Initialize();

    #ifdef G4VIS_USE
    // Initialize visualization
    G4VisManager* visManager=new G4VisExecutive;
    visManager->Initialize();
    #endif

    // Get the pointer to the User Interface manager
    G4UImanager* UI=G4UImanager::GetUIpointer();

    if(0) {
        // interactive mode : define visualization UI terminal
        G4UIsession* session = 0;
    #if defined(G4UI_USE_TCSH)
        session = new G4UIterminal(new G4UItcsh);
    #else
        session = new G4UIterminal();
    #endif
        session->SessionStart();
        delete session;
    }else{
        int eventNum = simOpt -> GetOptInt("EventNum");
        UI->ApplyCommand(Form("/run/beamOn %i", eventNum));
        return 0;
    }

    return 0;
}