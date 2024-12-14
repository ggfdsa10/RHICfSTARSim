#ifndef RHICFRECONSTRUCITON_HH
#define RHICFRECONSTRUCITON_HH

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"

#include "RHICfSimPar.hh"
#include "RHICfSimUtil.hh"

#include "StRHICfSimDst.h"
#include "StRHICfSimRHICfHit.h"
#include "StRHICfSimZDC.h"

class RHICfReconstruction
{
public:
    RHICfReconstruction();
    ~RHICfReconstruction();

    void Init();

    void OnlyResponseMode(){fRecoMode = false;}
    void SetSimDst(StRHICfSimDst* simDst){fSimDst = simDst;}
    
    void MakeResponse();
    void Reconstruct();

    private:

        void CrossTalk();
        void PedestalFluctuation();

        void CrossTalkCorrection();
        void PeakSearch();
        void LateralFitting();
        void SingleHitFitting(int atower, int abelt, int axy);
        void MultiHitFitting(int atower, int abelt, int axy);

        Double_t GetLorenzianSingle(Double_t* x, Double_t *par);
        Double_t GetLorenzianMulti(Double_t* x, Double_t *par);

        bool fRecoMode;
        TRandom3* fRandom;

        //RHICfSimUtil
        RHICfSimUtil* fSimUtil;
        RHICfSimOptions* fSimOpt;

        // SimDst
        StRHICfSimDst* fSimDst;
        StRHICfSimRHICfHit* fSimRHICfHit;
        StRHICfSimZDC* fSimZDC;

        // Data Tables
        TGraphErrors* fGSOBarXTalk[ntower][nbelt][nxy][nbarTL];
        TMatrixD* fCrossTalkCorrMatrix[nbelt][nxy];
        TVectorD* fGSOBarMatrix[nbelt][nxy];

        // Reconstruction tool
        TSpectrum* fSpectrum;
        TH1F* fTmpGSOBar[ntower];
        TGraphErrors* fGSOBarGraph;
        TF1* fSingleFit;
        TF1* fMultiFit;

        double fGSOBarError[ntower][nbelt][nxy][nbarTL];
        double fPeaks[ntower][nbelt][nxy][2][2]; // [tower][layer][xy][prime, sub][pos, height]
        int fPeakNum[ntower][nbelt][nxy];

        double fFitResultsSingle[ntower][nbelt][nxy][5]; // test

        // Define the fitting parameters
        const Double_t fParWidth1 = 1.0;
        const Double_t fParWidth2 = 10.0;
        const Double_t fParRatio = 0.6;
        const Double_t fParBaseLine = 0.00001;

        //Define the fitting parameter Limits
        const Double_t fParWidth1Min = 0.01;
        const Double_t fParWidth1Max = 5.0;
        const Double_t fParWidth2Min = 5.0;
        const Double_t fParWidth2Max = 40.0;
        const Double_t fParRatioMin = 0.0;
        const Double_t fParRatioMax = 1.0;
};

#endif
