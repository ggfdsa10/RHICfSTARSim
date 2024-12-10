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

        void SetFCADC(int tower, int val);
        void SetPlateADC(int tower, int plate, int val);
        void SetGSOBarADC(int tower, int layer, int xy, int bar, int val);

        void SetSimTrkId(int tower, int id);

        Float_t GetFCdE(int tower);
        Float_t GetPlatedE(int tower, int plate);
        Float_t GetGSOBardE(int tower, int layer, int xy, int bar);

        Int_t GetFCADC(int tower);
        Int_t GetPlateADC(int tower, int plate);
        Int_t GetGSOBarADC(int tower, int layer, int xy, int bar);

        Int_t GetSimTrkNum(int tower);
        Int_t GetSimTrkId(int tower, int idx);

    private:
        // RHICf Raw Hits (geant level data and response data)
        Float_t mFCdE[rTowerNum];
        Float_t mPlatedE[rTowerNum][rPlateNum];
        Float_t mSmallGSOBardE[rLayerNum][rXYNum][rSmallBarNum]; // TS
        Float_t mLargeGSOBardE[rLayerNum][rXYNum][rLargeBarNum]; // TL

        Int_t mFCADC[rTowerNum];
        Int_t mPlateADC[rTowerNum][rPlateNum];
        Int_t mSmallGSOBarADC[rLayerNum][rXYNum][rSmallBarNum]; // TS
        Int_t mLargeGSOBarADC[rLayerNum][rXYNum][rLargeBarNum]; // TL

        vector<int> mSimTrkId[rTowerNum];

        // RHICf Hits (reconstruction level)
        // Float_t mTowerSumEnergy[2]; // [photon, hadron] // to be update
        
    ClassDef(StRHICfSimRHICfHit,1)
};

#endif
