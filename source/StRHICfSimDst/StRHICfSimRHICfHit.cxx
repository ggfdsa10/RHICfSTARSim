#include "StRHICfSimRHICfHit.h"

ClassImp(StRHICfSimRHICfHit)

StRHICfSimRHICfHit::StRHICfSimRHICfHit()
{
    Clear();
}

StRHICfSimRHICfHit::~StRHICfSimRHICfHit()
{
}

void StRHICfSimRHICfHit::Clear(Option_t *option)
{
    fill_n(&mFCdE[0], rTowerNum, -999.);
    fill_n(&mPlatedE[0][0], rTowerNum * rPlateNum, -999.);
    fill_n(&mSmallGSOBardE[0][0][0], rLayerNum*rXYNum*rSmallBarNum, -999.);
    fill_n(&mLargeGSOBardE[0][0][0], rLayerNum*rXYNum*rLargeBarNum, -999.);

    fill_n(&mPeakNum[0][0][0], rTowerNum*rLayerNum*rXYNum, -999);
    fill_n(&mMaxPeakBarIdx[0][0][0], rTowerNum*rLayerNum*rXYNum, -999);
    fill_n(&mSingleHitResult[0][0][0][0], rTowerNum*rLayerNum*rXYNum*3, -999);
    fill_n(&mMultiHitResult[0][0][0][0][0], rTowerNum*rLayerNum*rXYNum*2*2, -999);
    fill_n(&mMultiFitChi2[0][0][0], rTowerNum*rLayerNum*rXYNum, -999);

    for(int i=0; i<rTowerNum; i++){
        mSimTrkId[i].clear();
    }
}

void StRHICfSimRHICfHit::SetFCdE(int tower, float val){mFCdE[tower] = val;}
void StRHICfSimRHICfHit::SetPlatedE(int tower, int plate, float val){mPlatedE[tower][plate] = val;}

void StRHICfSimRHICfHit::SetGSOBardE(int tower, int layer, int xy, int bar, float val)
{
    if(tower == rTS){mSmallGSOBardE[layer][xy][bar] = val;}
    if(tower == rTL){mLargeGSOBardE[layer][xy][bar] = val;}
}

void StRHICfSimRHICfHit::SetPeakNum(int tower, int layer, int xy, int num){mPeakNum[tower][layer][xy] = num;}
void StRHICfSimRHICfHit::SetMaxPeakBarIdx(int tower, int layer, int xy, int bar){mMaxPeakBarIdx[tower][layer][xy] = bar;}

void StRHICfSimRHICfHit::SetSingleHitPos(int tower, int layer, int xy, float val){mSingleHitResult[tower][layer][xy][0] = val;}
void StRHICfSimRHICfHit::SetSinglePeakHeight(int tower, int layer, int xy, float val){mSingleHitResult[tower][layer][xy][1] = val;}
void StRHICfSimRHICfHit::SetSingleFitChi2(int tower, int layer, int xy, float val){mSingleHitResult[tower][layer][xy][2] = val;}

void StRHICfSimRHICfHit::SetMultiHitPos(int tower, int layer, int xy, int order, float val){mMultiHitResult[tower][layer][xy][order][0] = val;}
void StRHICfSimRHICfHit::SetMultiPeakHeight(int tower, int layer, int xy, int order, float val){mMultiHitResult[tower][layer][xy][order][1] = val;}
void StRHICfSimRHICfHit::SetMultiFitChi2(int tower, int layer, int xy, float val){mMultiFitChi2[tower][layer][xy] = val;}

void StRHICfSimRHICfHit::SetSimTrkId(int tower, int id){mSimTrkId[tower].push_back(id);}

Float_t StRHICfSimRHICfHit::GetFCdE(int tower){return mFCdE[tower];}
Float_t StRHICfSimRHICfHit::GetPlatedE(int tower, int plate){return mPlatedE[tower][plate];}

Float_t StRHICfSimRHICfHit::GetGSOBardE(int tower, int layer, int xy, int bar)
{
    if(tower == rTS){return mSmallGSOBardE[layer][xy][bar];}
    if(tower == rTL){return mLargeGSOBardE[layer][xy][bar];}
    return -999.;
}

Int_t StRHICfSimRHICfHit::GetPeakNum(int tower, int layer, int xy){return mPeakNum[tower][layer][xy];}
Int_t StRHICfSimRHICfHit::GetMaxPeakBarIdx(int tower, int layer, int xy){return mMaxPeakBarIdx[tower][layer][xy];}

Float_t StRHICfSimRHICfHit::GetSingleHitPos(int tower, int layer, int xy){return mSingleHitResult[tower][layer][xy][0];}
Float_t StRHICfSimRHICfHit::GetSinglePeakHeight(int tower, int layer, int xy){return mSingleHitResult[tower][layer][xy][1];}
Float_t StRHICfSimRHICfHit::GetSingleFitChi2(int tower, int layer, int xy){return mSingleHitResult[tower][layer][xy][2];}

Float_t StRHICfSimRHICfHit::GetMultiHitPos(int tower, int layer, int xy, int order){return mMultiHitResult[tower][layer][xy][order][0];}
Float_t StRHICfSimRHICfHit::GetMultiPeakHeight(int tower, int layer, int xy, int order){return mMultiHitResult[tower][layer][xy][order][1];}
Float_t StRHICfSimRHICfHit::GetMultiFitChi2(int tower, int layer, int xy){return mMultiFitChi2[tower][layer][xy];}

Int_t StRHICfSimRHICfHit::GetSimTrkNum(int tower){return mSimTrkId[tower].size();}
Int_t StRHICfSimRHICfHit::GetSimTrkId(int tower, int idx){return mSimTrkId[tower][idx];}

