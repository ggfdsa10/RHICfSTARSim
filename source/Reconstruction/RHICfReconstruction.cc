#include "RHICfReconstruction.hh"

RHICfReconstruction::RHICfReconstruction()
: fRecoMode(true)
{
}

RHICfReconstruction::~RHICfReconstruction()
{
}

void RHICfReconstruction::Init()
{
    fSimUtil = RHICfSimUtil::GetRHICfSimUtil();
    fSimOpt = fSimUtil -> GetOptions();

    fRandom = new TRandom3(fSimUtil->GenSeed());

    // Read the CrossTalk table
    TString tablePath = fSimOpt -> GetOptString("TABLEDIR");
    TFile *CrossTalkFile = new TFile(Form("%s/GSObarXtalk.root", tablePath.Data()), "READ");

    for(int itower=0; itower<ntower; itower++){
        for(int ibelt=0; ibelt<nbelt; ibelt++){
            for(int ixy=0; ixy<nxy; ixy++){
                for(int ibar=0; ibar<nbar[itower]; ibar++){
                    TString name = Form("Xtalk_Tower%d_Lay%d_XY%d_Ch%02d", itower,ibelt,ixy,ibar);
                    fGSOBarXTalk[itower][ibelt][ixy][ibar] = (TGraphErrors*)CrossTalkFile -> Get(name);
                }
            }
        }
    }
    CrossTalkFile -> Close();

    if(fRecoMode){
        // Read the CrossTalk correction table
        TFile *CrossTalkCorrFile = new TFile(Form("%s/GSObarXtalk_Correction.root", tablePath.Data()), "READ");
        for(int ibelt=0; ibelt<nbelt; ibelt++) {
            for(int ixy=0; ixy<nxy; ixy++) {    
                TString name = Form("XtalkCorr_Lay%d_XY%d", ibelt,ixy);
                fCrossTalkCorrMatrix[ibelt][ixy] = (TMatrixD*)CrossTalkCorrFile -> Get(name);
                fGSOBarMatrix[ibelt][ixy] = new TVectorD(60);
            }
        }
        CrossTalkCorrFile -> Close();

        fSpectrum = new TSpectrum();
        fTmpGSOBar[0] = new TH1F("tmpGSOBarTS", "", nbar[0], 0, nbar[0]);
        fTmpGSOBar[1] = new TH1F("tmpGSOBarTL", "", nbar[1], 0, nbar[1]);
        fGSOBarGraph = new TGraphErrors();

        fSingleFit = new TF1("SingleFit", this, &RHICfReconstruction::GetLorenzianSingle, 0., double(nbarTL), 6);
        fMultiFit = new TF1("MultiFit", this, &RHICfReconstruction::GetLorenzianMulti, 0., double(nbarTL), 11);
    }
}

void RHICfReconstruction::Clear()
{
}

void RHICfReconstruction::MakeResponse()
{
    fSimRHICfHit = fSimDst -> GetSimRHICfHit();
    fSimZDC = fSimDst -> GetSimZDC();

    CrossTalk();
    PedestalFluctuation();
}

void RHICfReconstruction::Reconstruct()
{   
    if(!fSimRHICfHit){
        cout << "RHICfReconstruction::Reconstruct() -- Error: RHICfReconstruction::MakeResponse() is not work" << endl;
        return;
    }

    CrossTalkCorrection();
    PeakSearch();
    LateralFitting();
}

void RHICfReconstruction::CrossTalk()
{
    double gsobarRaw[ntower][nbelt][nxy][nbarTL];
    memset(gsobarRaw, 0., sizeof(gsobarRaw));

    // Make the CrossTalk effect
    for(int itower=0; itower<ntower; itower++){
        for(int ibelt=0; ibelt<nbelt; ibelt++){
            for(int ixy=0; ixy<nxy; ixy++){
                for(int ibar=0; ibar<nbar[itower]; ibar++){
                    double gsobardE = fSimRHICfHit -> GetGSOBardE(itower, ibelt, ixy, ibar);

                    for(int jtower=0; jtower<ntower; jtower++){
                        for(int jbar=0; jbar<nbar[jtower]; jbar++){
                            if(itower==jtower && ibar==jbar){
                                gsobarRaw[jtower][ibelt][ixy][jbar] += gsobardE;
                            }
                            else{
                                if(jtower==0){
                                    double table = fGSOBarXTalk[itower][ibelt][ixy][ibar]->GetY()[jbar];
                                    double tableErr = fGSOBarXTalk[itower][ibelt][ixy][ibar]->GetEY()[jbar];
                                    double fluc = fRandom -> Gaus(table, tableErr);
                                    gsobarRaw[jtower][ibelt][ixy][jbar] += (gsobardE * fluc);
                                }
                                else{
                                    double table = fGSOBarXTalk[itower][ibelt][ixy][ibar]->GetY()[jbar+nbar[0]];
                                    double tableErr = fGSOBarXTalk[itower][ibelt][ixy][ibar]->GetEY()[jbar+nbar[0]];
                                    double fluc = fRandom -> Gaus(table, tableErr);
                                    gsobarRaw[jtower][ibelt][ixy][jbar]+= (gsobardE * fluc);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    double mip = 0.746/5.; /// 1MIP=746keV/5p.e.
    for(int itower=0; itower<ntower; itower++) {
        for(int ibelt=0; ibelt<nbelt; ibelt++) {
            for(int ixy=0; ixy<nxy; ixy++) {
                for(int ibar=0; ibar<nbar[itower]; ibar++) {
                    double npe = gsobarRaw[itower][ibelt][ixy][ibar]/mip;
                    if(npe < 20.){
                        while(npe < 0){
                            npe = fRandom -> Poisson(npe);
                            npe = fRandom -> Gaus(npe, sqrt(npe)*0.3);
                        }
                    }
                    double raw = npe*mip;;
                    fSimRHICfHit -> SetGSOBardE(itower, ibelt, ixy, ibar, raw);
                }
            }
        }
    }
}

void RHICfReconstruction::PedestalFluctuation()
{
    double platePedResol = 15.;
    double gsoBarPedResol = 0.2;
    double fcPedResol = 0.2;
    double zdcPmtPedResol = 20.;
    double zdcSMDPedResol = 10.;

    // RHICf detector
    for(int itower=0; itower<ntower; itower++) {
        // Plate
        for(int iplate=0; iplate<nplate; iplate++) {
            double platedE = fSimRHICfHit -> GetPlatedE(itower, iplate);
            double plateFlucdE = fRandom -> Gaus(platedE, platePedResol);

            fSimRHICfHit -> SetPlatedE(itower, iplate, plateFlucdE);
        }
        // GSO bar
        for(int ibelt=0; ibelt<nbelt; ibelt++) {
            for(int ixy=0; ixy<nxy; ixy++) {
                for(int ibar=0; ibar<nbar[itower]; ibar++) {
                    double gsoBardE = fSimRHICfHit -> GetGSOBardE(itower, ibelt, ixy, ibar);
                    double gsoBarFlucdE = fRandom -> Gaus(gsoBardE, gsoBarPedResol);

                    fSimRHICfHit -> SetGSOBardE(itower, ibelt, ixy, ibar, gsoBarFlucdE);
                }
            }
        }
        // Forward Counter
        double fcdE = fSimRHICfHit -> GetFCdE(itower);
        double fcFlucdE = fRandom -> Gaus(fcdE, fcPedResol);

        fSimRHICfHit -> SetFCdE(itower, fcFlucdE);
    }

    // ZDC detector
    // ZDC pmt
    for(int izdc=0; izdc<nzdc; izdc++) {
        double zdcPmtPhoton = double(fSimZDC -> GetPmtPhotonNum(izdc));
        double zdcPmt = fRandom -> Gaus(zdcPmtPhoton, zdcPmtPedResol);

        fSimZDC -> SetPmtdE(izdc, zdcPmt);
    }
    // ZDC SMD
    for(int ixy=0; ixy<nxy; ixy++) {
        for(int ismd=0; ismd<nsmd[ixy]; ismd++) {
            double zdcSMDdE = fSimZDC -> GetSMDdE(ixy, ismd);
            double zdcSMDFlucdE = fRandom -> Gaus(zdcSMDdE, zdcSMDPedResol);

            fSimZDC -> SetSMDdE(ixy, ismd, zdcSMDFlucdE);
        }
    }
}

void RHICfReconstruction::CrossTalkCorrection()
{
    memset(fGSOBarError, 0., sizeof(fGSOBarError));
    double tmpGSOBar[64];
    for(int ibelt=0; ibelt<nbelt; ibelt++){
        for(int ixy=0; ixy<nxy; ixy++){
            for(int i=0; i<nbar[0]; i++){
                tmpGSOBar[i] = fSimRHICfHit -> GetGSOBardE(0, ibelt, ixy, i);
            }
            for(int i=0; i<nbar[1]; i++){
                tmpGSOBar[i+nbar[0]] =fSimRHICfHit -> GetGSOBardE(1, ibelt, ixy, i);
            }
            for(int i=0; i<nunused; i++){
                tmpGSOBar[i+nbar[0]+nbar[1]] = 0;
            }
            fGSOBarMatrix[ibelt][ixy] -> ResizeTo(64);
            fGSOBarMatrix[ibelt][ixy] -> SetElements(tmpGSOBar);
        }
    }

    double pedstalErr = 0.5;
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
        for(int ixy=0; ixy<nxy; ixy++) {

            // Get the CrossTalk error
            for(int ibar=0; ibar<64; ibar++) {
                double err2 = 0.;
                for(int jbar=0; jbar<64; jbar++) {
                    double err = 0.;
                    if(jbar < 20){
                        err=std::max((*fGSOBarMatrix[ibelt][ixy])[jbar], 0.) + pedstalErr*pedstalErr;
                    }
                    else if(jbar < 60){
                        err=std::max((*fGSOBarMatrix[ibelt][ixy])[jbar], 0.) + pedstalErr*pedstalErr;
                    }
                    else{
                        err=std::max((*fGSOBarMatrix[ibelt][ixy])[jbar], 0.) + pedstalErr*pedstalErr;
                    }
                    err2 += pow((*fCrossTalkCorrMatrix[ibelt][ixy])[ibar][jbar], 2.) * err;
                }
                if(ibar < 20){
                    fGSOBarError[0][ibelt][ixy][ibar] = sqrt(err2);
                }
                else if(ibar < 60){
                    fGSOBarError[1][ibelt][ixy][ibar-20] = sqrt(err2);
                }
            }
            
            // CrossTalk correction matrix multiplication
            (*fGSOBarMatrix[ibelt][ixy]) *= (*fCrossTalkCorrMatrix[ibelt][ixy]);

            // CrossTalk correction
            for(int itower=0; itower<ntower; itower++){
                for(int ibar=0; ibar<nbar[itower]; ibar++){
                    double gsoBarCorrdE = (*fGSOBarMatrix[ibelt][ixy])(nbar[0]*itower + ibar);
                    fSimRHICfHit -> SetGSOBardE(itower, ibelt, ixy, ibar, gsoBarCorrdE);
                }
            }
        }
    }
}

void RHICfReconstruction::PeakSearch()
{
    double maxBarThreshold = 10.;
    double spectrumSigma = 1.;
    double spectrumThreshold = 0.1;

    memset(fPeaks, 0., sizeof(fPeaks));
    memset(fPeakNum, 0, sizeof(fPeakNum));

    for(int itower = 0; itower < ntower; itower++) {
        for(int ibelt=0; ibelt<nbelt; ibelt++) {
            for(int ixy=0; ixy<nxy; ixy++) {

                // minimum cut
                double maxbar=0.;
                for(int ibar=0; ibar<nbar[itower];ibar++) {
                    double gsoBardE = fSimRHICfHit -> GetGSOBardE(itower, ibelt, ixy, ibar);
                    if(maxbar < gsoBardE){
                        maxbar = gsoBardE;
                    }
                }
                if(maxbar < maxBarThreshold){continue;}

                fSpectrum -> Clear("ICESM");
                fTmpGSOBar[itower] -> Clear("ICESM");

                for(int ibar=0; ibar<nbar[itower]; ibar++){
                    double gsoBardE = fSimRHICfHit -> GetGSOBardE(itower, ibelt, ixy, ibar);
                    fTmpGSOBar[itower] -> SetBinContent(ibar+1, gsoBardE);
                }

                const int nfound = fSpectrum -> Search(fTmpGSOBar[itower], spectrumSigma, "", spectrumThreshold);
                fSimRHICfHit ->SetPeakNum(itower, ibelt, ixy, nfound);

                if(nfound == 1){
                    double pos = (fSpectrum -> GetPositionX())[0];
                    int maxBin = fTmpGSOBar[itower] -> GetXaxis() -> FindBin(pos);
                    double height = fTmpGSOBar[itower] -> GetBinContent(maxBin);

                    fPeaks[itower][ibelt][ixy][0][0] = pos;
                    fPeaks[itower][ibelt][ixy][0][1] = height;
                    fPeakNum[itower][ibelt][ixy] = 1;

                    fSimRHICfHit -> SetMaxPeakBarIdx(itower, ibelt, ixy, maxBin);
                }
                if(nfound == 2){
                    double pos1 = (fSpectrum -> GetPositionX())[0];
                    int maxBin1 = fTmpGSOBar[itower] -> GetXaxis() -> FindBin(pos1);
                    double height1 = fTmpGSOBar[itower]  -> GetBinContent(maxBin1);

                    fSimRHICfHit -> SetMaxPeakBarIdx(itower, ibelt, ixy, maxBin1);

                    double pos2 = (fSpectrum -> GetPositionX())[1];
                    int maxBin2 = fTmpGSOBar[itower] -> GetXaxis() -> FindBin(pos2);
                    double height2 = fTmpGSOBar[itower]  -> GetBinContent(maxBin2);

                    if(height1 > height2){
                        fPeaks[itower][ibelt][ixy][0][0] = pos1;
                        fPeaks[itower][ibelt][ixy][0][1] = height1;
                        fPeakNum[itower][ibelt][ixy] += 1;

                        if(height2 < maxBarThreshold){continue;}
                        fPeaks[itower][ibelt][ixy][1][0] = pos2;
                        fPeaks[itower][ibelt][ixy][1][1] = height2;
                        fPeakNum[itower][ibelt][ixy] += 1;
                    }
                    else{
                        fPeaks[itower][ibelt][ixy][0][0] = pos2;
                        fPeaks[itower][ibelt][ixy][0][1] = height2;
                        fPeakNum[itower][ibelt][ixy] += 1;

                        if(height1 < maxBarThreshold){continue;}
                        fPeaks[itower][ibelt][ixy][1][0] = pos1;
                        fPeaks[itower][ibelt][ixy][1][1] = height1;
                        fPeakNum[itower][ibelt][ixy] += 1;
                    }
                }
            } // XY
        } // Belt
    } // Tower
}

void RHICfReconstruction::LateralFitting()
{
    for(int itower = 0; itower < ntower; itower++) {
        for(int ibelt=0; ibelt<nbelt; ibelt++) {
            for(int ixy=0; ixy<nxy; ixy++) {
                int peakNum = fPeakNum[itower][ibelt][ixy];
                if(peakNum == 0){continue;}

                fGSOBarGraph -> Set(0);
                if(peakNum == 1){SingleHitFitting(itower, ibelt, ixy);}
                if(peakNum == 2){MultiHitFitting(itower, ibelt, ixy);}
            }
        }
    }
}

void RHICfReconstruction::SingleHitFitting(int atower, int abelt, int axy)
{
    for(int ibar=0; ibar<nbar[atower]; ibar++) {
        double gsoBarPos = ibar+0.5;
        double gsoBardE = fSimRHICfHit -> GetGSOBardE(atower, abelt, axy, ibar);
        double gsoBardEErr = fGSOBarError[atower][abelt][axy][ibar];

        fGSOBarGraph -> SetPoint(ibar, gsoBarPos, gsoBardE);
        fGSOBarGraph -> SetPointError(ibar, 0., gsoBardEErr);
    }

    // Fit function initialization 
    double peakPos = fPeaks[atower][abelt][axy][0][0];
    double peakHieght = fPeaks[atower][abelt][axy][0][1];
    double posLowerLimit = (peakPos-2.5 < -1.)? -1. : peakPos-2.5;
    double posUpperLimit = (peakPos+2.5 > double(nbar[atower]+1))? double(nbar[atower]+1) : peakPos+2.5;
    double heightLowerLimit = peakHieght*0.25;
    double heightUppwerLimit = peakHieght*1.5;

    Double_t initParSingle[6] = {fParWidth1, peakPos, peakHieght, fParWidth2, fParRatio, fParBaseLine};
    fSingleFit -> SetParameters(initParSingle);
    fSingleFit -> SetParLimits(0, fParWidth1Min, fParWidth1Max);
    fSingleFit -> SetParLimits(1, posLowerLimit, posUpperLimit);
    fSingleFit -> SetParLimits(2, heightLowerLimit, heightUppwerLimit);
    fSingleFit -> SetParLimits(3, fParWidth2Min, fParWidth2Max);
    fSingleFit -> SetParLimits(4, fParRatioMin, fParRatioMax);
    fSingleFit -> SetRange(0., double(nbar[atower]));

    fGSOBarGraph -> Fit(fSingleFit, "QR");

    fSimRHICfHit -> SetSingleHitPos(atower, abelt, axy, fSingleFit->GetParameter(1));
    fSimRHICfHit -> SetSinglePeakHeight(atower, abelt, axy, fSingleFit->GetParameter(2));
    fSimRHICfHit -> SetSingleFitChi2(atower, abelt, axy, fSingleFit->GetChisquare());
}

void RHICfReconstruction::MultiHitFitting(int atower, int abelt, int axy)
{
    for(int ibar=0; ibar<nbar[atower]; ibar++) {
        double gsoBarPos = ibar+0.5;
        double gsoBardE = fSimRHICfHit -> GetGSOBardE(atower, abelt, axy, ibar);
        double gsoBardEErr = fGSOBarError[atower][abelt][axy][ibar];

        fGSOBarGraph -> SetPoint(ibar, gsoBarPos, gsoBardE);
        fGSOBarGraph -> SetPointError(ibar, 0., gsoBardEErr);
    }

    // Fit function initialization 
    double peakPos1 = fPeaks[atower][abelt][axy][0][0];
    double peakHieght1 = fPeaks[atower][abelt][axy][0][1];
    double posLowerLimit1 = (peakPos1-2. < -1.)? -1. : peakPos1-2.;
    double posUpperLimit1 = (peakPos1+2. > double(nbar[atower]+1))? double(nbar[atower]+1) : peakPos1+2.;
    double heightLowerLimit1 = peakHieght1*0.25;
    double heightUppwerLimit1 = peakHieght1*1.5;

    double peakPos2 = fPeaks[atower][abelt][axy][1][0];
    double peakHieght2 = fPeaks[atower][abelt][axy][1][1];
    double posLowerLimit2 = (peakPos2-2. < -1.)? -1. : peakPos2-2.;
    double posUpperLimit2 = (peakPos2+2. > double(nbar[atower]+1))? double(nbar[atower]+1) : peakPos2+2.;
    double heightLowerLimit2 = peakHieght2*0.25;
    double heightUppwerLimit2 = peakHieght2*1.5;

    Double_t initParMulti[11] = {fParWidth1, peakPos1, peakHieght1, fParWidth2, fParRatio, fParBaseLine, fParWidth1, peakPos2, peakHieght2, fParWidth2, fParRatio};
    fMultiFit -> SetParameters(initParMulti);
    fMultiFit -> SetParLimits(0, fParWidth1Min, fParWidth1Max);
    fMultiFit -> SetParLimits(1, posLowerLimit1, posUpperLimit1);
    fMultiFit -> SetParLimits(2, heightLowerLimit1, heightUppwerLimit1);
    fMultiFit -> SetParLimits(3, fParWidth2Min, fParWidth2Max);
    fMultiFit -> SetParLimits(4, fParRatioMin, fParRatioMax);
    fMultiFit -> SetParLimits(6, fParWidth1Min, fParWidth1Max);
    fMultiFit -> SetParLimits(7, posLowerLimit2, posUpperLimit2);
    fMultiFit -> SetParLimits(8, heightLowerLimit2, heightUppwerLimit2);
    fMultiFit -> SetParLimits(9, fParWidth2Min, fParWidth2Max);
    fMultiFit -> SetParLimits(10, fParRatioMin, fParRatioMax);
    fMultiFit -> SetRange(0., double(nbar[atower]));

    fGSOBarGraph -> Fit(fMultiFit, "QR");

    fSimRHICfHit -> SetMultiHitPos(atower, abelt, axy, 0, fMultiFit->GetParameter(1));
    fSimRHICfHit -> SetMultiPeakHeight(atower, abelt, axy, 0, fMultiFit->GetParameter(2));
    fSimRHICfHit -> SetMultiHitPos(atower, abelt, axy, 1, fMultiFit->GetParameter(6));
    fSimRHICfHit -> SetMultiPeakHeight(atower, abelt, axy, 1, fMultiFit->GetParameter(7));

    fSimRHICfHit -> SetMultiFitChi2(atower, abelt, axy, fMultiFit->GetChisquare());
}

Double_t RHICfReconstruction::GetLorenzianSingle(Double_t* x, Double_t *par)
{
    Double_t val = par[4]*par[0]/pow(((x[0]-par[1])*(x[0]-par[1])+par[0]), 1.5)/2.;
    Double_t val2 = (1-par[4])*(par[3]/2.)/pow(((x[0]-par[1])*(x[0]-par[1])+par[3]), 1.5);

    return par[2]*(val+val2) + par[5];
}

Double_t RHICfReconstruction::GetLorenzianMulti(Double_t* x, Double_t *par)
{
    Double_t p1val = par[4]*par[0]/pow(((x[0]-par[1])*(x[0]-par[1])+par[0]), 1.5)/2.;
    Double_t p1val2 = (1.-par[4])*par[3]/pow(((x[0]-par[1])*(x[0]-par[1])+par[3]), 1.5)/2.;
    Double_t peak1 = par[2]*(p1val+p1val2);

    Double_t p2val = par[10]*par[6]/pow(((x[0]-par[7])*(x[0]-par[7])+par[6]), 1.5)/2.;
    Double_t p2val2 = (1.-par[10])*par[9]/pow(((x[0]-par[7])*(x[0]-par[7])+par[9]), 1.5)/2.;
    Double_t peak2 = par[8]*(p2val+p2val2);

    return peak1 + peak2 + par[5];
}