#ifndef StRHICfSimZDC_HH
#define StRHICfSimZDC_HH

#include <algorithm>
#include <vector>
#include "TObject.h"
#include "StRHICfSimPar.h"

using namespace std;

class StRHICfSimZDC : public TObject
{
    public: 
        StRHICfSimZDC();
        ~StRHICfSimZDC();

        void Clear(Option_t *option = "");

        void SetPmtPhotonNum(int idx, int val);
        void SetPmtdE(int idx, float val);
        void SetSMDdE(int xy, int idx, float val);

        void SetPmtADC(int idx, int val);
        void SetSMDADC(int xy, int idx, int val);

        void SetSimTrkId(int id);

        Int_t GetPmtPhotonNum(int idx);
        Float_t GetPmtdE(int idx);
        Float_t GetSMDdE(int xy, int idx);

        Int_t GetPmtADC(int idx);
        Int_t GetSMDADC(int xy, int idx);

        Int_t GetSimTrkNum();
        Int_t GetSImTrkId(int idx);

    private:
        Int_t mPhotonNum[rZDCPMTNum];
        Float_t mPmtdE[rZDCPMTNum];
        Float_t mSMDXdE[rSMDXNum]; 
        Float_t mSMDYdE[rSMDYNum];

        Int_t mPmtADC[rZDCPMTNum];
        Int_t mSMDXADC[rSMDXNum];
        Int_t mSMDYADC[rSMDYNum];

        vector<int> mSimTrkId;

    ClassDef(StRHICfSimZDC,1)
};

#endif
