#include "StRHICfSimRHICfPoint.h"

ClassImp(StRHICfSimRHICfPoint)

StRHICfSimRHICfPoint::StRHICfSimRHICfPoint()
{
    Clear();
}

StRHICfSimRHICfPoint::~StRHICfSimRHICfPoint()
{
}

void StRHICfSimRHICfPoint::Clear(Option_t *option)
{
    mTowerIdx = -1;
    mPID = -1;
    fill_n(&mPosRHICfXY[0], rXYNum, -999.);
    fill_n(&mEnergy[0], 2, -999.);
}

void StRHICfSimRHICfPoint::SetTowerIdx(int idx){mTowerIdx = idx;}
void StRHICfSimRHICfPoint::SetPID(int pid){mPID = pid;}
void StRHICfSimRHICfPoint::SetPosXY(int xy, float val){mPosRHICfXY[xy] = val;}
void StRHICfSimRHICfPoint::SetEnergy(int order, float val){mEnergy[order] = val;}

Int_t StRHICfSimRHICfPoint::GetTowerIdx(){return mTowerIdx;}
Int_t StRHICfSimRHICfPoint::GetPID(){return mPID;}
Float_t StRHICfSimRHICfPoint::GetPosXY(int xy){return mPosRHICfXY[xy];}
Float_t StRHICfSimRHICfPoint::GetEnergy(int order){return mEnergy[order];}
