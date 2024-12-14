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

    // For finding the number of incident particle which are deposited in detector medium
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

        if(fRHICfPrimaryNum[towerIdx].size() == 0){continue;}
        vector<int> primaryTrkId = (*fRHICfFCHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par] -1;

            if(trkId < fRHICfPrimaryNum[towerIdx].size()){
                fRHICfPrimaryNum[towerIdx][trkId] += 1;
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

        if(fRHICfPrimaryNum[towerIdx].size() == 0){continue;}
        vector<int> primaryTrkId = (*fRHICfGSOPlateHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par] -1;

            if(trkId < fRHICfPrimaryNum[towerIdx].size()){
                fRHICfPrimaryNum[towerIdx][trkId] += 1;
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

        if(fRHICfPrimaryNum[towerIdx].size() == 0){continue;}
        vector<int> primaryTrkId = (*fRHICfGSOBarHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par] -1;

            if(trkId < fRHICfPrimaryNum[towerIdx].size()){
                fRHICfPrimaryNum[towerIdx][trkId] += 1;
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

        if(fZDCPrimaryNum.size() == 0){continue;}
        vector<int> primaryTrkId = (*fZDCPMTHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par] -1;

            if(trkId < fZDCPrimaryNum.size()){
                fZDCPrimaryNum[trkId] += 1;
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

        if(fZDCPrimaryNum.size() == 0){continue;}
        vector<int> primaryTrkId = (*fZDCSMDHitColl)[i]->GetPrimaryTrackId();
        for(int par=0; par<primaryTrkId.size(); par++){
            int trkId = primaryTrkId[par] -1;

            if(trkId < fZDCPrimaryNum.size()){
                fZDCPrimaryNum[trkId] += 1;
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
    fRHICfReco -> MakeResponse();
    fRHICfReco -> Reconstruct();
    

    // Fill the Output SimDst Tree
    fOutputTree -> Fill();

    // Print the event information
    EventPrint();
}

void RHICfEventAction::EventPrint()
{
    cout << "=======================================================================================" << endl;
    cout << "RHICfEventAction::EventPrint() -- Event: " << fSimEvent -> GetEventNumber() -1 << endl;
    TString procName = fSimUtil -> GetProcessName(fSimEvent -> GetProcessId());
    cout << "    StRHICfSimEvent -- Process: " << procName << endl;

    cout << "    StRHICfSimTrack -- Primary Trk Num: " << fSimEvent -> GetPrimaryTrkNum() << ", ";
    cout << "Propagated Trk Num: " << (fGenAction -> GetParSimTrkIdxArray()).size() << endl;

    cout << "    RHICf deposited particle Num -- in TS: " << fSimRHICfHit -> GetSimTrkNum(0) << ", ";
    cout << "TL: " << fSimRHICfHit -> GetSimTrkNum(1) << endl;

    cout << "    ZDC deposited particle Num: " << fSimZDC -> GetSimTrkNum() << endl;

    // RHICf Reconstruction information 
    int gsoBarMaxLayer[2];
    double gsoBarMaxE[2];
    double plateSumE[2];
    memset(plateSumE, 0., sizeof(plateSumE));
    for(int itower=0; itower<ntower; itower++){
        double tmpValue = 0.;
        int tmpLayerIdx = -1;
        for(int ibelt=0; ibelt<nbelt; ibelt++){
            double sumE = 0.;
            for(int ixy=0; ixy<nxy; ixy++){
                for(int ibar=0; ibar<nbar[itower]; ibar++){
                    sumE += fSimRHICfHit -> GetGSOBardE(itower, ibelt, ixy, ibar);
                }
            }
            if(tmpValue < sumE){
                tmpValue = sumE;
                tmpLayerIdx = ibelt;
            }
        }
        gsoBarMaxLayer[itower] = tmpLayerIdx;
        gsoBarMaxE[itower] = tmpValue;

        for(int iplate=0; iplate<nplate; iplate++){
            plateSumE[itower] += fSimRHICfHit -> GetPlatedE(itower, iplate);
        }
    }

    cout << "    RHICfReconstruction -- Plate Sum of Energy -- TS: " << plateSumE[0] << ", ";
    cout << "TL: " << plateSumE[1] << endl;
    cout << "    RHICfReconstruction -- GSOBar Max layer -- TS: " << gsoBarMaxLayer[0] << ", ";
    cout << "TL: " << gsoBarMaxLayer[1] << endl;
    cout << "    RHICfReconstruction -- GSOBar Max layer Energy -- TS: " <<  gsoBarMaxE[0] << ", ";
    cout << "TL: " << gsoBarMaxE[1] << endl;

    int tsPeakNumX = fSimRHICfHit->GetPeakNum(0, gsoBarMaxLayer[0], 0);
    int tsPeakNumY = fSimRHICfHit->GetPeakNum(0, gsoBarMaxLayer[0], 1);
    int tlPeakNumX = fSimRHICfHit->GetPeakNum(1, gsoBarMaxLayer[1], 0);
    int tlPeakNumY = fSimRHICfHit->GetPeakNum(1, gsoBarMaxLayer[1], 1);
    cout << "    RHICfReconstruction -- GSOBar Max layer PeakNum (x,y)  -- TS: (" <<  tsPeakNumX << ", " << tsPeakNumY << "), ";
    cout << "TL: (" << tlPeakNumX << ", " << tlPeakNumY << ")" << endl;

    double tsposX = (tsPeakNumX == 2)? fSimRHICfHit->GetMultiHitPos(0, gsoBarMaxLayer[0], 0, 0) : ((tsPeakNumX == 1)? fSimRHICfHit->GetSingleHitPos(0, gsoBarMaxLayer[0], 0) : -999.);
    double tsposY = (tsPeakNumY == 2)? fSimRHICfHit->GetMultiHitPos(0, gsoBarMaxLayer[0], 1, 0) : ((tsPeakNumY == 1)? fSimRHICfHit->GetSingleHitPos(0, gsoBarMaxLayer[0], 1) : -999.);
    double tlposX = (tlPeakNumX == 2)? fSimRHICfHit->GetMultiHitPos(1, gsoBarMaxLayer[1], 0, 0) : ((tlPeakNumX == 1)? fSimRHICfHit->GetSingleHitPos(1, gsoBarMaxLayer[1], 0) : -999.);
    double tlposY = (tlPeakNumY == 2)? fSimRHICfHit->GetMultiHitPos(1, gsoBarMaxLayer[1], 1, 0) : ((tlPeakNumY == 1)? fSimRHICfHit->GetSingleHitPos(1, gsoBarMaxLayer[1], 1) : -999.);
    cout << "    RHICfReconstruction -- GSOBar Max layer Position (x,y) -- TS: (" <<  tsposX << ", " << tsposY << "), ";
    cout << "TL: (" << tlposX << ", " << tlposY << ")" << endl;
    cout << "=======================================================================================" << endl;
}
