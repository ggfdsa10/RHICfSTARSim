#include "RHICfEventAction.hh"


RHICfEventAction::RHICfEventAction()
{
    fSimUtil = RHICfSimUtil::GetRHICfSimUtil();
    fSimOpt = fSimUtil -> GetOptions();

    fRHICfReco = new RHICfReconstruction();
    fRHICfReco -> Init();
}

RHICfEventAction::~RHICfEventAction()
{
}

void RHICfEventAction::BeginOfEventAction(const G4Event* evt)
{
    G4cout << "RHICfEventAction::BeginOfEventAction() -- Event: " << evt->GetEventID() << " progressing..." << G4endl;
}

void RHICfEventAction::EndOfEventAction(const G4Event* evt)
{
    fHitCollThisEvent = evt->GetHCofThisEvent();
    if(!fHitCollThisEvent) return;

    fRunManager = G4RunManager::GetRunManager();
    fSDManager = G4SDManager::GetSDMpointer();

    fGenAction = (RHICfPrimaryGeneratorAction*)fRunManager->GetUserPrimaryGeneratorAction();

    // For finding the number of incident particle
    vector<int> parSimTrkIdxArr = fGenAction -> GetParSimTrkIdxArray();
    fRHICfPrimaryNum[0].clear();
    fRHICfPrimaryNum[1].clear();
    fRHICfPrimaryNum[0].resize(parSimTrkIdxArr.size());
    fRHICfPrimaryNum[1].resize(parSimTrkIdxArr.size());

    fZDCPrimaryNum.clear();
    fZDCPrimaryNum.resize(parSimTrkIdxArr.size());

    // Assign the StRHICfSimEvent
    fSimEvent = fSimDst -> GetSimEvent();

    // ================ RHICf Sensitive detectors ================
    fSimRHICfHit = fSimDst -> GetSimRHICfHit();
    fSimRHICfHit -> Clear();

    // RHICf Forward Counter 
    G4int idRHICfFC = fSDManager->GetCollectionID("FC");
    fRHICfFCHitColl = (FCHitsCollection*)fHitCollThisEvent->GetHC(idRHICfFC);
    for(unsigned int i=0; i<fRHICfFCHitColl->GetSize(); i++){
        int towerIdx = (*fRHICfFCHitColl)[i]->GetTower();
        double edep = (*fRHICfFCHitColl)[i]->GetEdep();

        fSimRHICfHit -> SetFCdE(towerIdx, edep);

        vector<int> primaryTrkId = (*fRHICfFCHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par];
            if(trkId < fRHICfPrimaryNum[towerIdx].size() && fRHICfPrimaryNum[towerIdx].size() != 0){
                fRHICfPrimaryNum[towerIdx][trkId-1] += 1;
            }
        }
    }

    // RHICf GSO Plate
    G4int idRHICfGSOPlate = fSDManager->GetCollectionID("GSOplate");
    fRHICfGSOPlateHitColl = (GSOplateHitsCollection*)fHitCollThisEvent->GetHC(idRHICfGSOPlate);
    for(unsigned int i=0; i<fRHICfGSOPlateHitColl->GetSize(); i++){
        int towerIdx = (*fRHICfGSOPlateHitColl)[i]->GetTower();
        int plateIdx = (*fRHICfGSOPlateHitColl)[i]->GetPlate();
        double edep = (*fRHICfGSOPlateHitColl)[i]->GetEdep();

        fSimRHICfHit -> SetPlatedE(towerIdx, plateIdx, edep);

        vector<int> primaryTrkId = (*fRHICfGSOPlateHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par];
            if(trkId < fRHICfPrimaryNum[towerIdx].size() && fRHICfPrimaryNum[towerIdx].size() != 0){
                fRHICfPrimaryNum[towerIdx][trkId-1] += 1;
            }
        }
    }

    // RHICf GSO Bar
    G4int idRHICfGSOBar=fSDManager->GetCollectionID("GSObar");
    fRHICfGSOBarHitColl = (GSObarHitsCollection*)fHitCollThisEvent->GetHC(idRHICfGSOBar);
    for(unsigned int i=0; i<fRHICfGSOBarHitColl->GetSize(); i++){
        int towerIdx = (*fRHICfGSOBarHitColl)[i]->GetTower();
        int layerIdx = (*fRHICfGSOBarHitColl)[i]->GetBelt();
        int xyIdx = (*fRHICfGSOBarHitColl)[i]->GetXY();
        int barIdx = (*fRHICfGSOBarHitColl)[i]->GetBar();
        double edep = (*fRHICfGSOBarHitColl)[i]->GetEdep();

        fSimRHICfHit -> SetGSOBardE(towerIdx, layerIdx, xyIdx, barIdx, edep);

        vector<int> primaryTrkId = (*fRHICfGSOBarHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par];
            if(trkId < fRHICfPrimaryNum[towerIdx].size() && fRHICfPrimaryNum[towerIdx].size() != 0){
                fRHICfPrimaryNum[towerIdx][trkId-1] += 1;
            }
        }
    }

    // RHICf truth incident particle
    for(int tower=0; tower<ntower; tower++){
        for(int i=0; i<fRHICfPrimaryNum[tower].size(); i++){
            int numIncidentPar = fRHICfPrimaryNum[tower][i];
            if(numIncidentPar == 0){continue;}

            int simTrkId = parSimTrkIdxArr[i];
            fSimRHICfHit -> SetSimTrkId(tower, simTrkId);
        }
    }

    // ================ ZDC Sensitive detectors ================
    fSimZDC = fSimDst -> GetSimZDC();
    fSimZDC -> Clear();

    // ZDC Modules (PMT)
    G4int idZDCModudle=fSDManager->GetCollectionID("ZDC");
    fZDCPMTHitColl = (ZDCHitsCollection*)fHitCollThisEvent->GetHC(idZDCModudle);
    for(unsigned int i=0; i<fZDCPMTHitColl->GetSize(); i++) {
        int moduleIdx = (*fZDCPMTHitColl)[i]->GetModule();
        int photonNum = (*fZDCPMTHitColl)[i]->GetNphoton();
        double edep = (*fZDCPMTHitColl)[i]->GetEdep();

        fSimZDC -> SetPmtPhotonNum(moduleIdx, photonNum);
        fSimZDC -> SetPmtdE(moduleIdx, edep);

        vector<int> primaryTrkId = (*fZDCPMTHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par];
            if(trkId < fZDCPrimaryNum.size() && fZDCPrimaryNum.size() != 0){
                fZDCPrimaryNum[trkId-1] += 1;
            }
        }
    }

    // ZDC SMD
    G4int idZDCSMD=fSDManager->GetCollectionID("SMD");
    fZDCSMDHitColl = (SMDHitsCollection*)fHitCollThisEvent->GetHC(idZDCSMD);
    for(unsigned int i=0; i<fZDCSMDHitColl->GetSize(); i++) {
        int xyIdx = (*fZDCSMDHitColl)[i]->GetXY();
        int smdIdx = (*fZDCSMDHitColl)[i]->GetSMD();
        double edep = (*fZDCSMDHitColl)[i]->GetEdep();

        fSimZDC -> SetSMDdE(xyIdx, smdIdx, edep);

        vector<int> primaryTrkId = (*fZDCSMDHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par];
            if(trkId < fZDCPrimaryNum.size() && fZDCPrimaryNum.size() != 0){
                fZDCPrimaryNum[trkId-1] += 1;
            }
        }
    }

    // ZDC truth incident particle
    for(int i=0; i<fZDCPrimaryNum.size(); i++){
        int numIncidentPar = fZDCPrimaryNum[i];
        if(numIncidentPar == 0){continue;}

        int simTrkId = parSimTrkIdxArr[i];
        fSimZDC -> SetSimTrkId(simTrkId);
    }

    // ================ Reconstruction ================
    fRHICfReco -> SetSimDst(fSimDst);
    fRHICfReco -> Clear();
    fRHICfReco -> MakeResponse();
    fRHICfReco -> Reconstruct();

    // test !!!!! ====================
    cout << " ============================= " << endl;

    double sumdE = 0.;
    for(int it=0; it<ntower; it++){
        for(int ip=0; ip<nplate; ip++){
            sumdE += fSimRHICfHit -> GetPlatedE(it, ip);
        }
    } 
    cout << " Plate Sum Energy : " << sumdE << endl;
    for(int i=0; i<parSimTrkIdxArr.size(); i++){
        int simTrkId = parSimTrkIdxArr[i];
        fSimTrack = fSimDst -> GetSimTrack(simTrkId);
        int pid = fSimTrack -> GetPid();
        double e = fSimTrack -> GetE();
        double vxEnd = fSimTrack -> GetVxEnd();
        double vyEnd = fSimTrack -> GetVyEnd();
        double vzEnd = fSimTrack -> GetVzEnd();

        cout << "pid: " << pid << ", energy: " << e << " | vertex: (" << vxEnd << ", " << vyEnd << ", " << vzEnd<< ") " << endl;
    }
    

    // Fill the Output SimDst Tree
    fOutputTree -> Fill();

    // Print the event information
    EventPrint();
}

void RHICfEventAction::EventPrint()
{
    cout << "RHICfEventAction::EventPrint() -- Event: " << fSimEvent -> GetEventNumber() << endl;
    TString procName = fSimUtil -> GetProcessName(fSimEvent -> GetProcessId());
    cout << "    StRHICfSimEvent -- Process: " << procName << endl;

    cout << "    StRHICfSimTrack -- Primary Trk Num: " << fSimEvent -> GetPrimaryTrkNum() << ", ";
    cout << "Propagated Trk Num: " << (fGenAction -> GetParSimTrkIdxArray()).size() << endl;

    cout << "    RHICf deposited particle Num -- in TS: " << fSimRHICfHit -> GetSimTrkNum(0) << ", ";
    cout << "TL: " << fSimRHICfHit -> GetSimTrkNum(1) << endl;

    cout << "    ZDC deposited particle Num: " << fSimZDC -> GetSimTrkNum() << endl;

}
