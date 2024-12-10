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

    fill_n(&mFCADC[0], rTowerNum, -999);
    fill_n(&mPlateADC[0][0], rTowerNum * rPlateNum, -999);
    fill_n(&mSmallGSOBarADC[0][0][0], rLayerNum*rXYNum*rSmallBarNum, -999);
    fill_n(&mLargeGSOBarADC[0][0][0], rLayerNum*rXYNum*rLargeBarNum, -999);

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

void StRHICfSimRHICfHit::SetFCADC(int tower, int val){mFCADC[tower] = val;}
void StRHICfSimRHICfHit::SetPlateADC(int tower, int plate, int val){mPlateADC[tower][plate] = val;}

void StRHICfSimRHICfHit::SetGSOBarADC(int tower, int layer, int xy, int bar, int val)
{
    if(tower == rTS){mSmallGSOBarADC[layer][xy][bar] = val;}
    if(tower == rTL){mLargeGSOBarADC[layer][xy][bar] = val;}
}

void StRHICfSimRHICfHit::SetSimTrkId(int tower, int id){mSimTrkId[tower].push_back(id);}

Float_t StRHICfSimRHICfHit::GetFCdE(int tower){return mFCdE[tower];}
Float_t StRHICfSimRHICfHit::GetPlatedE(int tower, int plate){return mPlatedE[tower][plate];}

Float_t StRHICfSimRHICfHit::GetGSOBardE(int tower, int layer, int xy, int bar)
{
    if(tower == rTS){return mSmallGSOBardE[layer][xy][bar];}
    if(tower == rTL){return mLargeGSOBardE[layer][xy][bar];}
    return -999.;
}

Int_t StRHICfSimRHICfHit::GetFCADC(int tower){return mFCADC[tower];}
Int_t StRHICfSimRHICfHit::GetPlateADC(int tower, int plate){return mPlateADC[tower][plate];}

Int_t StRHICfSimRHICfHit::GetGSOBarADC(int tower, int layer, int xy, int bar)
{
    if(tower == rTS){return mSmallGSOBarADC[layer][xy][bar];}
    if(tower == rTL){return mLargeGSOBarADC[layer][xy][bar];}
    return -999;
}

Int_t StRHICfSimRHICfHit::GetSimTrkNum(int tower){return mSimTrkId[tower].size();}
Int_t StRHICfSimRHICfHit::GetSimTrkId(int tower, int idx){return mSimTrkId[tower][idx];}

