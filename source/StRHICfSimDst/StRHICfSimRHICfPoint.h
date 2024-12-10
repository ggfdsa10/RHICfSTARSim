#ifndef StRHICfSimRHICfPoint_HH
#define StRHICfSimRHICfPoint_HH

#include <algorithm>
#include "TObject.h"
#include "StRHICfSimPar.h"

using namespace std;

class StRHICfSimRHICfPoint : public TObject
{
    public: 
        StRHICfSimRHICfPoint();
        ~StRHICfSimRHICfPoint();

        void Clear(Option_t *option = "");

        void SetTowerIdx(int idx);
        void SetPID(int pid);
        void SetPosXY(int xy, float val);
        void SetEnergy(int order, float val);

        Int_t GetTowerIdx();
        Int_t GetPID();
        Float_t GetPosXY(int xy);
        Float_t GetEnergy(int order);

    private:
        Char_t mTowerIdx;
        Char_t mPID;
        Float_t mPosRHICfXY[rXYNum]; // [x, y]
        Float_t mEnergy[2]; // [photon, hadron]

    ClassDef(StRHICfSimRHICfPoint,1)
};

#endif
