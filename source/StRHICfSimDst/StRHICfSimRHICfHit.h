#ifndef StRHICfSimRHICfHit_HH
#define StRHICfSimRHICfHit_HH

#include <algorithm>
#include <vector>
#include "TObject.h"
#include "StRHICfSimPar.h"

using namespace std;

class StRHICfSimRHICfHit : public TObject
{
    public: 
        StRHICfSimRHICfHit();
        ~StRHICfSimRHICfHit();

        void Clear(Option_t *option = "");

        void SetFCdE(int tower, float val);
        void SetPlatedE(int tower, int plate, float val);
        void SetGSOBardE(int tower, int layer, int xy, int bar, float val);

        void SetPeakNum(int tower, int layer, int xy, int num);
        void SetMaxPeakBarIdx(int tower, int layer, int xy, int bar);

        void SetSingleHitPos(int tower, int layer, int xy, float val);
        void SetSinglePeakHeight(int tower, int layer, int xy, float val);
        void SetSingleFitChi2(int tower, int layer, int xy, float val);

        void SetMultiHitPos(int tower, int layer, int xy, int order, float val);
        void SetMultiPeakHeight(int tower, int layer, int xy, int order, float val);
        void SetMultiFitChi2(int tower, int layer, int xy, float val);

        void SetSimTrkId(int tower, int id);

        Float_t GetFCdE(int tower);
        Float_t GetPlatedE(int tower, int plate);
        Float_t GetGSOBardE(int tower, int layer, int xy, int bar);

        Int_t GetPeakNum(int tower, int layer, int xy);
        Int_t GetMaxPeakBarIdx(int tower, int layer, int xy);

        Float_t GetSingleHitPos(int tower, int layer, int xy);
        Float_t GetSinglePeakHeight(int tower, int layer, int xy);
        Float_t GetSingleFitChi2(int tower, int layer, int xy);

        Float_t GetMultiHitPos(int tower, int layer, int xy, int order);
        Float_t GetMultiPeakHeight(int tower, int layer, int xy, int order);
        Float_t GetMultiFitChi2(int tower, int layer, int xy);

        Int_t GetSimTrkNum(int tower);
        Int_t GetSimTrkId(int tower, int idx);

    private:
        // RHICf Raw Hits (geant level data and response data)
        Float_t mFCdE[rTowerNum];
        Float_t mPlatedE[rTowerNum][rPlateNum];
        Float_t mSmallGSOBardE[rLayerNum][rXYNum][rSmallBarNum]; // TS
        Float_t mLargeGSOBardE[rLayerNum][rXYNum][rLargeBarNum]; // TL

        // RHICf Reco Hits (reconstruction)
        Int_t mPeakNum[rTowerNum][rLayerNum][rXYNum];
        Int_t mMaxPeakBarIdx[rTowerNum][rLayerNum][rXYNum];
        Float_t mSingleHitResult[rTowerNum][rLayerNum][rXYNum][3]; // [pos, height, chi2]
        Float_t mMultiHitResult[rTowerNum][rLayerNum][rXYNum][2][2]; // [prime, sub][pos, height]
        Float_t mMultiFitChi2[rTowerNum][rLayerNum][rXYNum]; 

        vector<int> mSimTrkId[rTowerNum];


    ClassDef(StRHICfSimRHICfHit,1)
};

#endif
