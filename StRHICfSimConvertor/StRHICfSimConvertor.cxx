
#include "StRHICfSimConvertor.h"

// STAR headers
#include "StChain/StChain.h"
#include "StChain/StChainOpt.h"
#include "StEvent/StBbcTriggerDetector.h"

// StMuEvents headers
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTypes.hh"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuMcTrack.h"
#include "StMuDSTMaker/COMMON/StMuMcVertex.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"

// StarGenEvent
#include "StarGenerator/EVENT/StarGenPPEvent.h"
#include "StarGenerator/EVENT/StarGenEvent.h"
#include "StarGenerator/EVENT/StarGenParticle.h"

// StRHICfSimDst 
#include "StRHICfPool/StRHICfSimDst/StRHICfSimDst.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimEvent.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimTrack.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimBBC.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimBTof.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimRHICfHit.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimRHICfPoint.h"
#include "StRHICfPool/StRHICfSimDst/StRHICfSimZDC.h"

StRHICfSimConvertor::StRHICfSimConvertor(int convertFlag, const char* fileName, const Char_t* name) 
: StMaker(name), mConvertFlag(convertFlag), mInputFile(fileName), mRHICfRunType(-1)
{
    InitGePid2PDG();
}

StRHICfSimConvertor::~StRHICfSimConvertor()
{
}

Int_t StRHICfSimConvertor::Init()
{
    if(mConvertFlag == kMuDst2SimDst){InitMuDst2SimDst();}
    if(mConvertFlag == kSimDst2MuDst){InitSimDst2MuDst();}

    return kStOk;
}

Int_t StRHICfSimConvertor::Make()
{
    LOG_INFO << "StRHICfSimConvertor::Make()" << endm;

    if(mConvertFlag == kMuDst2SimDst){ConvertMuDst2SimDst();}
    if(mConvertFlag == kSimDst2MuDst){ConvertSimDst2MuDst();}
                    
    return kStOk;
}

Int_t StRHICfSimConvertor::Finish()
{
    mSimDstFile -> cd();
    mSimDstTree -> Write();
    mSimDstFile -> Close();

    return kStOk;
}

Int_t StRHICfSimConvertor::clear()
{
    return kStOk;
}

Int_t StRHICfSimConvertor::InitGePid2PDG()
{
    mGePid2PDGMap.insert({1, 22}); // gamma
    mGePid2PDGMap.insert({2, -11}); // positron
    mGePid2PDGMap.insert({3, 11}); // electron
    mGePid2PDGMap.insert({4, 12}); // neutrino
    mGePid2PDGMap.insert({5, -13}); // mu+
    mGePid2PDGMap.insert({6, 13}); // mu-
    mGePid2PDGMap.insert({7, 111}); // pi0
    mGePid2PDGMap.insert({8, 211}); // pi+
    mGePid2PDGMap.insert({9, -211}); // pi-
    mGePid2PDGMap.insert({10, 130}); // K0 Long
    mGePid2PDGMap.insert({11, 321}); // K+
    mGePid2PDGMap.insert({12, -321}); // K-
    mGePid2PDGMap.insert({13, 2112}); // neutron
    mGePid2PDGMap.insert({14, 2212}); // proton
    mGePid2PDGMap.insert({15, -2212}); // anti proton
    mGePid2PDGMap.insert({16, 310}); // K0 Short
    mGePid2PDGMap.insert({17, 221}); // eta
    mGePid2PDGMap.insert({18, 3122}); // lambda
    mGePid2PDGMap.insert({19, 3222}); // sigma+
    mGePid2PDGMap.insert({20, 3212}); // sigma0
    mGePid2PDGMap.insert({21, 3112}); // sigma-
    mGePid2PDGMap.insert({22, 3322}); // xi0
    mGePid2PDGMap.insert({23, 3312}); // xi-
    mGePid2PDGMap.insert({24, 3334}); // omega
    mGePid2PDGMap.insert({25, -2112}); // anti neutron
    mGePid2PDGMap.insert({26, -3122}); // anti lambda
    mGePid2PDGMap.insert({27, -3112}); // anti sigma-
    mGePid2PDGMap.insert({28, -3212}); // anti sigma0
    mGePid2PDGMap.insert({29, -3222}); // anti sigma+
    mGePid2PDGMap.insert({30,-3322}); // anti xi0
    mGePid2PDGMap.insert({31, -3312}); // anti xi+
    mGePid2PDGMap.insert({32, -3334}); // anti omega+
    mGePid2PDGMap.insert({33, -15}); // tau+
    mGePid2PDGMap.insert({34, 15}); // tau-
    mGePid2PDGMap.insert({35, 441}); // D+
    mGePid2PDGMap.insert({36, -441}); // D-
    mGePid2PDGMap.insert({37, 421}); // D0
    mGePid2PDGMap.insert({38, -421}); // anti D0
    mGePid2PDGMap.insert({39, 431}); // Ds+ or f+ ??
    mGePid2PDGMap.insert({40, -431}); // Ds- or f- ??
    mGePid2PDGMap.insert({41, 4212}); // lambda c+
    mGePid2PDGMap.insert({42, 24}); // W+
    mGePid2PDGMap.insert({43, -24}); // W-
    mGePid2PDGMap.insert({44, 23}); // Z0
    mGePid2PDGMap.insert({45, 700201}); // deuteron
    mGePid2PDGMap.insert({46, 700301}); // tritium
    mGePid2PDGMap.insert({47, 700202}); // alpha
    mGePid2PDGMap.insert({48, 0}); // genatino

    return kStOk;
}

Int_t StRHICfSimConvertor::InitMuDst2SimDst()
{
    mChain = new TChain("genevents");

    TObjArray *tokens;

    TString generatorFile = "";
    if(mInputFile.Length() == 0){
        LOG_ERROR << "Input file is not a existing ... " << endm;
        return kStErr;
    }
    else{
        int fileNum = 0;
        TString muDstFile = "";
        if( mInputFile.Index(".list") != -1 || mInputFile.Index(".lis") != -1 ){
            std::ifstream inputStream( mInputFile.Data() );
            if(!inputStream) {LOG_ERROR << "ERROR: Cannot open list file " << mInputFile << endm;}

            std::string file;
            size_t pos;
            while(getline(inputStream, file)){
                pos = file.find_first_of(" ");
                if (pos != std::string::npos ) file.erase(pos,file.length()-pos);
                if(file.find("MuDst") != std::string::npos){
                    muDstFile = file;
                    fileNum++;
                }
            }
            if(fileNum != 1){
                LOG_ERROR << "StRHICfSimConvertor::InitMuDst2SimDst() -- Input file must be one MuDst file" << endm;
                return kStErr;
            }
        }
        if(muDstFile == ""){muDstFile = mInputFile;}
        if(muDstFile.Index("MuDst") != -1) {
            mOutputFile = muDstFile;
            tokens = mOutputFile.Tokenize("/");
            mOutputFile = ((TObjString *) tokens -> At(tokens->GetEntries()-1)) -> GetString();
            mOutputFile.ReplaceAll(".MuDst.root",".RHICfSimDst.root");

            generatorFile = muDstFile;
            generatorFile.ReplaceAll(".MuDst.root", ".Pythia8.root");
            int valid = mChain -> Add(generatorFile);
            if(valid == 0){
                LOG_ERROR << "StRHICfSimConvertor::InitMuDst2SimDst() -- There is no Generator ROOT file !!! " << endm;
                return kStErr;
            }
            LOG_INFO << Form("StRHICfSimConvertor::InitMuDst2SimDst() -- Find a Event generator file: %s",generatorFile.Data()) << endm;
        }
    }

    LOG_INFO << Form("StRHICfSimConvertor::InitMuDst2SimDst() -- Output file name: %s",mOutputFile.Data()) << endm;
    
    mSimDstFile = new TFile(mOutputFile.Data(), "recreate");
    mSimDstTree = new TTree("StRHICfSimDst","StRHICfSimDst");

    mSimDst = new StRHICfSimDst();
    mSimDst -> CreateDstArray(mSimDstTree);

    mGenPPEvent = new StarGenPPEvent("Pythia8");
    mGenEvent = new StarGenEvent("primaryEvent");

    mChain -> SetBranchAddress("Pythia8", &mGenPPEvent);
    mChain -> SetBranchAddress("primaryEvent", &mGenEvent);
    mTmpChainEvent = 0;

    InitRHICfGeometry();

    return kStOk;
}

Int_t StRHICfSimConvertor::InitSimDst2MuDst()
{

    return kStOk;
}

Int_t StRHICfSimConvertor::ConvertMuDst2SimDst()
{
    mSimDst -> Clear();
    mRHICfGammaIdx.clear();
    mRHICfNeuIdx.clear();

    // =================== DST Set up ======================
    mMuDst = (StMuDst*) GetInputDS("MuDst"); // from DST
    if(!mMuDst) {LOG_ERROR << "StRHICfPi0Maker::Make() -- no StMuDst" << endm;  return kStErr;};

    mMuEvent = mMuDst->event(); // from muDST
    if(!mMuEvent) {LOG_ERROR << "StRHICfPi0Maker::Make() -- no StMuEvent" << endm;  return kStErr;};

    // Get the event info
    mSimEvent = mSimDst -> GetSimEvent();
    int eventNumber = mMuEvent -> eventNumber();
    mSimEvent -> SetEventNumber(eventNumber);
    mSimEvent -> SetRHICfRunType(mRHICfRunType);

    // ======================= Get MuMcTrack ============================
    mMcVtxArray = mMuDst -> mcArray(0);
    mMcTrkArray = mMuDst -> mcArray(1); 

    int primaryTrkNum = 0;
    int mcTrkNum = mMcTrkArray -> GetEntriesFast();
    for(int i=0; i<mcTrkNum; i++){
        mMcTrk = (StMuMcTrack*)mMcTrkArray -> UncheckedAt(i);
        int VtxIdStart = mMcTrk -> IdVx();
        int VtxIdEnd = mMcTrk -> IdVxEnd();
        bool isPrimary = (VtxIdStart == 1)? true : false;
        if(isPrimary){primaryTrkNum++;}

        double posStart[3];
        mMcVtx = (StMuMcVertex*)mMcVtxArray -> UncheckedAt(VtxIdStart-1);
        posStart[0] = mMcVtx->XyzV().x();
        posStart[1] = mMcVtx->XyzV().y();
        posStart[2] = mMcVtx->XyzV().z();

        // primary vertex 
        mMcVtx = (StMuMcVertex*)mMcVtxArray -> UncheckedAt(0);
        // double primaryPosZ = mMcVtx->XyzV().z();

        int parentTrkId = mMcVtx -> IdParTrk() - 1; // starting of parentTrk Id from 0 index
        int daughterNum = mMcVtx -> NoDaughters();

        double posEnd[3] = {-9999., -9999., -9999.};
        if(VtxIdEnd != 0){
            mMcVtx = (StMuMcVertex*)mMcVtxArray -> UncheckedAt(VtxIdEnd-1);
            posEnd[0] = mMcVtx->XyzV().x();
            posEnd[1] = mMcVtx->XyzV().y();
            posEnd[2] = mMcVtx->XyzV().z();
        }

        int trkId = mMcTrk -> Id() -1; // starting of track Id from 0 index
        int gePid = mMcTrk -> GePid();
        int pid = GetGePid2PDG(gePid);
        double energy = mMcTrk -> E();
        double mom[3];
        mom[0] = mMcTrk -> Pxyz().x();
        mom[1] = mMcTrk -> Pxyz().y();
        mom[2] = mMcTrk -> Pxyz().z();

        // Save track info into StRHICfSimTrack
        mSimTrk = mSimDst -> GetSimTrack(mSimDst->GetSimTrackNum());
        mSimTrk -> SetId(trkId);
        mSimTrk -> SetPid(pid);
        mSimTrk -> SetParentId(parentTrkId);
        mSimTrk -> SetDaughterNum(daughterNum);
        mSimTrk -> SetEnergy(energy);
        mSimTrk -> SetMomentum(mom[0], mom[1], mom[2]);   
        mSimTrk -> SetVertexStart(posStart[0], posStart[1], posStart[2]);
        mSimTrk -> SetVertexEnd(posEnd[0], posEnd[1], posEnd[2]);
        
        if(IsSimPropagate(mSimTrk)){mSimTrk->SetIsSimPropagate();}

        // find a RHICf gamma or neutron
        if(gePid == 1 || gePid == 13){
            int hit = GetRHICfGeoHit(posStart[0], posStart[1], posStart[2], mom[0], mom[1], mom[2], energy);
            if(hit != -1){
                if(gePid == 1){mRHICfGammaIdx.push_back(mSimDst->GetSimTrackNum()-1);} // gamma
                if(gePid == 13){mRHICfNeuIdx.push_back(mSimDst->GetSimTrackNum()-1);} // neutron
            }
            else{if(isPrimary){mSimTrk -> SetIsPrimary();}}
        }
        else{if(isPrimary){mSimTrk -> SetIsPrimary();}}
    }

    mSimEvent -> SetPrimaryTrkNum(primaryTrkNum);

    // ======================= Get BBC ============================
    mSimBBC = mSimDst -> GetSimBBC();
    StBbcTriggerDetector bbc = mMuEvent -> bbcTriggerDetector();
    bbc.setYear(2017);
    int bbcPMTNum = bbc.numberOfPMTs();
    for(int i=0; i<bbcPMTNum; i++){
        int adc = bbc.adc(i);
        int dir = (i < bbcPMTNum/2)? rEast : rWest;

        // small tile
        if((i < rBBCSmallPmtNum) || (bbcPMTNum/2 <= i && i < bbcPMTNum/2 + rBBCSmallPmtNum)){
            int pmtIdx = (dir == rEast)? i : i - bbcPMTNum/2;
            mSimBBC -> SetSmallADC(dir, pmtIdx, adc);
        }
        else{ // large tile
            int pmtIdx = (dir == rEast)? i - rBBCSmallPmtNum : i - (bbcPMTNum/2 + rBBCSmallPmtNum);
            mSimBBC -> SetLargeADC(dir, pmtIdx, adc);
        }
    }

    // ======================= Get B-TOF ============================
    Int_t bTofTrayMult = mMuEvent -> btofTrayMultiplicity();
    Int_t bTofMult = mMuDst -> GetNBTofHit();
    for (int i=0; i<bTofMult; ++i) {
        mMuBTofHit = (StMuBTofHit*)mMuDst -> btofHit(i);
        if (!mMuBTofHit) continue;
        if (mMuBTofHit->tray() > 120) continue; // BTof has only 120 trays, over the 120 index will be VPD hit info
        int trayId = mMuBTofHit -> tray();
        int moduleId = mMuBTofHit -> module();
        int cellId = mMuBTofHit -> cell();
        int idTruth = mMuBTofHit -> idTruth();

        mSimBTof = mSimDst -> GetSimBTof(mSimDst -> GetSimBTofNum());
        mSimBTof -> SetTray(trayId);
        mSimBTof -> SetModule(moduleId);
        mSimBTof -> SetCell(cellId);
        mSimBTof -> SetSimTrkId(idTruth);
    }

    GetGeneratorData();

    mSimDstTree -> Fill();

    return kStOk;
}

Int_t StRHICfSimConvertor::GetGeneratorData()
{
    int simDstEvent = mSimEvent -> GetEventNumber();
    for(int event=mTmpChainEvent; event<mChain->GetEntriesFast(); event++){
        mChain -> GetEntry(event);
        if(mGenEvent -> GetFilterResult() != 1){continue;}
        if(simDstEvent == mGenEvent->GetEventNumber()){
            mTmpChainEvent = event;

            // Set the Event gernerator information
            int processId = mGenPPEvent -> process;
            double xParton1 = mGenPPEvent -> xParton1;
            double xParton2 = mGenPPEvent -> xParton2;
            double xPdf1 =  mGenPPEvent -> xPdf1;
            double xPdf2 =  mGenPPEvent -> xPdf2;
            double Q2Fac =  mGenPPEvent -> Q2fac;
            double Q2Ren =  mGenPPEvent -> Q2ren;
            double sHat =  mGenPPEvent -> sHat;
            double tHat =  mGenPPEvent -> tHat;
            double uHat =  mGenPPEvent -> uHat;
            double ptHat =  mGenPPEvent -> ptHat;

            int finalParNum = 0;
            int finalChargedParNum = 0;

            mSimEvent -> SetProcessId(processId);
            mSimEvent -> SetXParton1(xParton1);
            mSimEvent -> SetXParton2(xParton2);
            mSimEvent -> SetXPdf1(xPdf1);
            mSimEvent -> SetXPdf2(xPdf2);
            mSimEvent -> SetQ2Fac(Q2Fac);
            mSimEvent -> SetQ2Renorm(Q2Ren);
            mSimEvent -> SetsHat(sHat);
            mSimEvent -> SettHat(tHat);
            mSimEvent -> SetuHat(uHat);
            mSimEvent -> SetPtHat(ptHat);

            // Find a RHICf particle mothers
            vector<pair<int, int>> pairRHICfParticles;

            int RHICfGammaNum = mRHICfGammaIdx.size();
            int RHICfNeuNum = mRHICfNeuIdx.size();
            int totalRHICfParNum  = RHICfGammaNum + RHICfNeuNum;

            int parNum = mGenEvent -> GetNumberOfParticles();
            for(int par=0; par<parNum; par++){
                mGenParticle = (*mGenEvent)[par];
                int status = mGenParticle -> GetStatus();
                int pid = mGenParticle -> GetId();

                // Diffraction informations
                if(status == 15){
                    double diffMass = mGenParticle -> GetMass();
                    double px = mGenParticle -> GetPx();
                    double py = mGenParticle -> GetPy();
                    double pz = mGenParticle -> GetPz();
                    double p = sqrt(px*px + py*py + pz*pz);
                    double cosTheta = (p == 0.)? 1. : pz/p;
                    double diffEta = -0.5*TMath::Log((1. - cosTheta)/(1. + cosTheta));
                    if(par == 3){
                        mSimEvent -> SetDiffractionAMass(diffMass);
                        mSimEvent -> SetDiffractionAEta(diffEta);
                    }
                    if(par == 4){
                        mSimEvent -> SetDiffractionBMass(diffMass);
                        mSimEvent -> SetDiffractionBEta(diffEta);
                    }
                    if(par == 5){
                        mSimEvent -> SetDiffractionCMass(diffMass);
                        mSimEvent -> SetDiffractionCEta(diffEta);
                    }
                }
                // Get the number of final state particle 
                if(status == 1){
                    finalParNum++;
                    if(abs(pid) == 11 || abs(pid) == 13 || pid == 22 || abs(pid) == 211 || abs(pid) == 321 || abs(pid) == 2212){
                        finalChargedParNum++;
                    }
                }

                if(pid != 22 && pid != 2112){continue;}

                int genPtInt = int(mGenParticle -> pt()*10000);
                int genEnergyInt = int(mGenParticle -> GetEnergy()*10000);

                for(int r=0; r<totalRHICfParNum; r++){
                    int simTrkIdx = (r < RHICfGammaNum)? mRHICfGammaIdx[r] : mRHICfNeuIdx[r];
                    mSimTrk = mSimDst -> GetSimTrack(simTrkIdx);

                    double px = mSimTrk -> GetPx();
                    double py = mSimTrk -> GetPy();
                    int simTrkPtInt = int(sqrt(px*px + py*py)*10000);
                    int simTrkEnergyInt = int(mSimTrk->GetE()*10000);

                    // Getting the same RHICf particle 
                    if(genEnergyInt == simTrkEnergyInt && genPtInt == simTrkPtInt){
                        pairRHICfParticles.push_back(make_pair(simTrkIdx, par));
                    }
                }
                if(int(pairRHICfParticles.size()) == totalRHICfParNum){break;}
            }

            // Set the generator level number of particles
            mSimEvent -> SetGenFinalParNum(finalParNum);
            mSimEvent -> SetGenFinalChargedParNum(finalChargedParNum);

            if(pairRHICfParticles.size() == 0){break;}

            bool isRHICfNeuAndPi0 = false;
            for(int i=0; i<totalRHICfParNum; i++){
                int simTrkIdx1 = pairRHICfParticles[i].first;
                int genParIdx1 = pairRHICfParticles[i].second;

                mGenParticle = (*mGenEvent)[genParIdx1];
                int motherIdx1 = mGenParticle -> GetFirstMother();
                int pid = mGenParticle -> GetId();
                if(pid == 22){
                    bool findGammaPair = false;
                    for(int j=i+1; j<totalRHICfParNum; j++){
                        int simTrkIdx2 = pairRHICfParticles[j].first;
                        int genParIdx2 = pairRHICfParticles[j].second;

                        mGenParticle = (*mGenEvent)[genParIdx2];
                        int motherIdx2 = mGenParticle -> GetFirstMother();   
                        if(motherIdx1 != motherIdx2){continue;}
                        findGammaPair = true;

                        // find a generator level RHICf Pi0
                        mGenParticle = (*mGenEvent)[motherIdx1];
                        int motherPid = mGenParticle -> GetId();
                        int pi0MotherIdx = mGenParticle -> GetFirstMother();
                        int pi0MotherIdxLast = mGenParticle -> GetLastMother();

                        if(motherPid == 111){
                            bool isDirect = true;
                            if(pi0MotherIdxLast == 0){isDirect = false;} // if pi0 has two mother, it is directed pi0

                            // Gamma from pi0 start point
                            mSimTrk = mSimDst -> GetSimTrack(simTrkIdx1);
                            double gammaStartVx = mSimTrk->GetVxEnd();
                            double gammaStartVy = mSimTrk->GetVyEnd();
                            double gammaStartVz = mSimTrk->GetVzEnd();

                            // Set the pi0 infomation
                            mSimTrk = mSimDst -> GetSimTrack(mSimDst->GetSimTrackNum());
                            int pi0SimTrkId = mSimDst->GetSimTrackNum()-1;
                            mSimTrk -> SetId(pi0SimTrkId);
                            mSimTrk -> SetPid(mGenParticle->GetId());  // pdg encoding
                            mSimTrk -> SetDaughterNum(2);
                            mSimTrk -> SetEnergy(mGenParticle->GetEnergy());
                            mSimTrk -> SetMomentum(mGenParticle->GetPx(), mGenParticle->GetPy(), mGenParticle->GetPz());

                            double pi0StartVx = mGenParticle->GetVx();
                            double pi0StartVy = mGenParticle->GetVy();
                            double pi0StartVz = mGenParticle->GetVz();
                            mSimTrk -> SetVertexStart(pi0StartVx, pi0StartVy, pi0StartVz);
                            mSimTrk -> SetVertexEnd(gammaStartVx, gammaStartVy, gammaStartVz);

                            if(isDirect){
                                mSimTrk -> SetIsPrimary();
                                mSimTrk -> SetParentId(-999); // direct
                            }
                            else{
                                // RHICf pi0's mother particle information
                                mGenParticle = (*mGenEvent)[pi0MotherIdx];
                                int motherIdxLast = mGenParticle -> GetLastMother();
                                isDirect = true;
                                if(motherIdxLast == 0){isDirect = false;}

                                mSimTrk = mSimDst -> GetSimTrack(mSimDst->GetSimTrackNum());
                                int pi0MotherSimTrkId = mSimDst->GetSimTrackNum()-1;
                                mSimTrk -> SetId(pi0MotherSimTrkId);
                                mSimTrk -> SetPid(mGenParticle->GetId());  // pdg encoding
                                mSimTrk -> SetDaughterNum(2);
                                mSimTrk -> SetEnergy(mGenParticle->GetEnergy());
                                mSimTrk -> SetMomentum(mGenParticle->GetPx(), mGenParticle->GetPy(), mGenParticle->GetPz());
                                mSimTrk -> SetVertexStart(mGenParticle->GetVx(), mGenParticle->GetVy(), mGenParticle->GetVz());
                                mSimTrk -> SetVertexEnd(pi0StartVx, pi0StartVy, pi0StartVz);
                                if(isDirect){mSimTrk->SetIsPrimary();}

                                // Pi0 mother's another daughter particle information
                                int pi0Mothers_AnotherDaughterIdx = 0;
                                if(mGenParticle -> GetFirstDaughter() == motherIdx1){pi0Mothers_AnotherDaughterIdx = mGenParticle -> GetLastDaughter();}
                                else{pi0Mothers_AnotherDaughterIdx = mGenParticle -> GetFirstDaughter();}

                                mGenParticle = (*mGenEvent)[pi0Mothers_AnotherDaughterIdx];
                                
                                // Find a RHICf Neutron
                                if(mGenParticle -> GetId() == 2112){
                                    for(int i=0; i<totalRHICfParNum-1; i++){
                                        int genParIdx = pairRHICfParticles[i].second;  
                                        if(pi0Mothers_AnotherDaughterIdx == genParIdx){
                                            cout << "RHICf Neutron came form RHICf pi0' mother particle" << endl;
                                            isRHICfNeuAndPi0 = true;
                                            break;
                                        }
                                    }
                                }
                                if(isRHICfNeuAndPi0){continue;}

                                mSimTrk = mSimDst -> GetSimTrack(mSimDst->GetSimTrackNum());
                                mSimTrk -> SetId(mSimDst->GetSimTrackNum()-1);
                                mSimTrk -> SetParentId(pi0MotherSimTrkId);
                                mSimTrk -> SetPid(mGenParticle->GetId()); // pdg encoding
                                mSimTrk -> SetEnergy(mGenParticle->GetEnergy());
                                mSimTrk -> SetMomentum(mGenParticle->GetPx(), mGenParticle->GetPy(), mGenParticle->GetPz());
                                mSimTrk -> SetVertexStart(mGenParticle->GetVx(), mGenParticle->GetVy(), mGenParticle->GetVz());

                                // Set the Pi0's mother trk Id
                                mSimTrk = mSimDst -> GetSimTrack(pi0SimTrkId);
                                mSimTrk -> SetParentId(pi0MotherSimTrkId);
                            }

                            // Set the Gamma's mother (Pi0) trk Id
                            mSimTrk = mSimDst -> GetSimTrack(simTrkIdx1);
                            mSimTrk -> SetParentId(pi0SimTrkId);
                            mSimTrk = mSimDst -> GetSimTrack(simTrkIdx2);
                            mSimTrk -> SetParentId(pi0SimTrkId);
                        }
                    }
                    // standalone RHICf gamma
                    if(!findGammaPair){ 
                        mGenParticle = (*mGenEvent)[genParIdx1];
                        int motherIdx = mGenParticle -> GetFirstMother();
                        int motherIdxLast = mGenParticle -> GetLastMother();
                        bool isDirect = true;
                        if(motherIdxLast == 0){isDirect = false;}
                        if(isDirect){
                            mSimTrk = mSimDst -> GetSimTrack(simTrkIdx1);
                            mSimTrk -> SetIsPrimary();
                        }
                        else{
                            double gammaStartVx = mGenParticle->GetVx();
                            double gammaStartVy = mGenParticle->GetVy();
                            double gammaStartVz = mGenParticle->GetVz();

                            mGenParticle = (*mGenEvent)[motherIdx];
                            motherIdx = mGenParticle -> GetFirstMother();
                            motherIdxLast = mGenParticle -> GetLastMother();
                            isDirect = true;
                            if(motherIdxLast == 0){isDirect = false;}

                            mSimTrk = mSimDst -> GetSimTrack(mSimDst->GetSimTrackNum());
                            mSimTrk -> SetId(mSimDst->GetSimTrackNum()-1);
                            mSimTrk -> SetPid(mGenParticle->GetId());  // pdg encoding
                            mSimTrk -> SetEnergy(mGenParticle->GetEnergy());
                            mSimTrk -> SetMomentum(mGenParticle->GetPx(), mGenParticle->GetPy(), mGenParticle->GetPz());
                            mSimTrk -> SetVertexStart(mGenParticle->GetVx(), mGenParticle->GetVy(), mGenParticle->GetVz());
                            mSimTrk -> SetVertexEnd(gammaStartVx, gammaStartVy, gammaStartVz);
                            if(isDirect){mSimTrk->SetIsPrimary();}
                        }
                    }
                }
                else if(pid == 2112){
                    mGenParticle = (*mGenEvent)[genParIdx1];
                    int motherIdx = mGenParticle -> GetFirstMother();
                    int motherIdxLast = mGenParticle -> GetLastMother();
                    bool isDirect = true;
                    if(motherIdxLast == 0){isDirect = false;}
                    if(isDirect){
                        mSimTrk = mSimDst -> GetSimTrack(simTrkIdx1);
                        mSimTrk -> SetIsPrimary();
                        continue;
                    }
                    if(isRHICfNeuAndPi0){continue;}

                    // Find whether Neutron's brother is RHICf pi0
                    mGenParticle = (*mGenEvent)[motherIdx];
                    int neuMothers_AnotherDaughterIdx = 0;
                    if(mGenParticle -> GetFirstDaughter() == genParIdx1){neuMothers_AnotherDaughterIdx = mGenParticle -> GetLastDaughter();}
                    else{neuMothers_AnotherDaughterIdx = mGenParticle -> GetFirstDaughter();}

                    mGenParticle = (*mGenEvent)[neuMothers_AnotherDaughterIdx];

                    if(mGenParticle -> GetId() == 111){
                        bool isFindRHICfPi0 = false;
                        int genPi0Idx = mGenParticle -> GetIndex();
                        for(int j=i+1; j<totalRHICfParNum; j++){
                            int genParIdx = pairRHICfParticles[j].second;
                            if(genPi0Idx == genParIdx){
                                isFindRHICfPi0 = true;
                                break;
                            }
                        }
                        isRHICfNeuAndPi0 = true;
                        if(isFindRHICfPi0){continue;}
                    }

                    // Set the Neutron's mother information
                    mGenParticle = (*mGenEvent)[motherIdx];
                    motherIdx = mGenParticle -> GetFirstMother();
                    motherIdxLast = mGenParticle -> GetLastMother();
                    isDirect = true;
                    if(motherIdxLast == 0){isDirect = false;}
                    mSimTrk = mSimDst -> GetSimTrack(mSimDst->GetSimTrackNum());
                    int neutronSimTrkId = mSimDst->GetSimTrackNum()-1;
                    mSimTrk -> SetId(neutronSimTrkId);
                    mSimTrk -> SetDaughterNum(2);
                    mSimTrk -> SetPid(mGenParticle->GetId()); // pdg encoding
                    mSimTrk -> SetEnergy(mGenParticle->GetEnergy());
                    mSimTrk -> SetMomentum(mGenParticle->GetPx(), mGenParticle->GetPy(), mGenParticle->GetPz());
                    mSimTrk -> SetVertexStart(mGenParticle->GetVx(), mGenParticle->GetVy(), mGenParticle->GetVz());
                    if(isDirect){mSimTrk->SetIsPrimary();}

                    mSimTrk =  mSimDst -> GetSimTrack(simTrkIdx1);
                    mSimTrk -> SetParentId(neutronSimTrkId);
                }
            }
            break;
        }
    }
    return kStOk;
}

bool StRHICfSimConvertor::IsSimPropagate(StRHICfSimTrack* simTrk)
{
    float startVx = simTrk->GetVxStart();
    float startVy = simTrk->GetVyStart();
    float startVz = simTrk->GetVzStart();
    float endVz = simTrk->GetVzEnd();

    // 1st cut: if track is not alive 
    if(endVz > -9990.){return false;} 

    // 2nd cut: if track eta < 5
    float eta = simTrk -> GetEta();
    if(eta < 5){return false;}

    // 3nd cut: startVz larger than 10m (DX magnet)
    if(startVz > 1000){return false;}

    float px = simTrk->GetPx();
    float py = simTrk->GetPy();
    float pz = simTrk->GetPz();
    float p = sqrt(px*px + py*py + pz*pz);

    float unitX = px/p;
    float unitY = py/p;
    float unitZ = pz/p;

    // 4th cut: if Z diraction is negative
    if(unitZ < 0.){return false;}

    float startR = sqrt(startVx*startVx + startVy*startVy);
    
    // 5th cut: if startR > DX magnet diameter and unitX and unitY are positive.
    if(startR > 30.){
        if(unitX > 0. && unitY > 0.){return false;}
    }

    return true;
}

Int_t StRHICfSimConvertor::GetGePid2PDG(int gepid)
{
    if(mGePid2PDGMap.find(gepid) != mGePid2PDGMap.end()) {
        return mGePid2PDGMap.find(gepid)->second;
    }
    return 0;
}

Int_t StRHICfSimConvertor::ConvertSimDst2MuDst()
{
    return kStOk;
}

void StRHICfSimConvertor::InitRHICfGeometry()
{
    double mRHICfTowerBoundary[2][4][2]; // [TS, TL][bound square][x, y]
    double mRHICfTowerCenterPos[2]; // [TS, TL] y pos

    double tsDetSize = 20.; // [mm]
    double tlDetSize = 40.; // [mm]
    double detBoundCut = 2.; // [mm]
    double distTStoTL = 47.4; // [mm]
    double detBeamCenter = 0.;

    if(mRHICfRunType < rTStype || mRHICfRunType > rTOPtype){
        LOG_ERROR << "StRHICfSimConvertor::InitRHICfGeometry() warning!!! RHICf run type is not setted!!!" << endm;
    }

    if(mRHICfRunType == rTStype){detBeamCenter = 0.;} // TS
    if(mRHICfRunType == rTLtype){detBeamCenter = -47.4;} // TL
    if(mRHICfRunType == rTOPtype){detBeamCenter = 21.6;} // TOP

    mRHICfTowerBoundary[0][0][0] = sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 
    mRHICfTowerBoundary[0][0][1] = 0.;
    mRHICfTowerBoundary[0][1][0] = 0.; 
    mRHICfTowerBoundary[0][1][1] = sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 
    mRHICfTowerBoundary[0][2][0] = -1.*sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 
    mRHICfTowerBoundary[0][2][1] = 0.; 
    mRHICfTowerBoundary[0][3][0] = 0.; 
    mRHICfTowerBoundary[0][3][1] = -1.*sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 

    mRHICfTowerBoundary[1][0][0] = sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    mRHICfTowerBoundary[1][0][1] = 0.;
    mRHICfTowerBoundary[1][1][0] = 0.;
    mRHICfTowerBoundary[1][1][1] = sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    mRHICfTowerBoundary[1][2][0] = -1.*sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    mRHICfTowerBoundary[1][2][1] = 0.;
    mRHICfTowerBoundary[1][3][0] = 0.;
    mRHICfTowerBoundary[1][3][1] = -1.*sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    
    mRHICfTowerCenterPos[0] = detBeamCenter;
    mRHICfTowerCenterPos[1] = distTStoTL + detBeamCenter;

    mRHICfPoly = new TH2Poly();
    mRHICfPoly -> SetName("RHICfPoly");
    mRHICfPoly -> SetStats(0);

    double x[4];
    double y[4];

    for(int t=0; t<2; t++){
        for(int i=0; i<4; i++){
        double xPos = mRHICfTowerBoundary[t][i][0];
        double yPos = mRHICfTowerCenterPos[t] + mRHICfTowerBoundary[t][i][1];
        x[i] = xPos;
        y[i] = yPos;
        }
        mRHICfPoly -> AddBin(4, x, y);
    }
}

Int_t StRHICfSimConvertor::GetRHICfGeoHit(double posX, double posY, double posZ, double px, double py, double pz, double e)
{
  if(e < 10.){return -1;} // energy cut

  double momMag = sqrt(px*px + py*py + pz*pz);
  double unitVecX = px/momMag;
  double unitVecY = py/momMag;
  double unitVecZ = pz/momMag;

  if(unitVecZ < 0){return -1;} // opposite side cut

  double mRHICfDetZ = 17800.; // [mm]
  double z = mRHICfDetZ - posZ;
  if(z < 0.){return -1;} // create z-position cut

  double x = z * (unitVecX/unitVecZ) + posX;
  double y = z * (unitVecY/unitVecZ) + posY;

  int type = mRHICfPoly -> FindBin(x, y);
  if(type < 1 || type > 2){return -1;} // RHICf geometrical hit cut

  return type;
}

ClassImp(StRHICfSimConvertor)