
#ifndef StRHICfSimConvertor_H
#define StRHICfSimConvertor_H

#include <map>

#include "TString.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TH2Poly.h"
#include "TObjString.h"

#include "StMaker.h"

class StEvent;
class StMuEvent;
class StMuDst;
class StMuDstMaker;
class StMuMcTrack;
class StMuMcVertex;
class StMuBTofHit;

class StarGenPPEvent;
class StarGenEvent;
class StarGenParticle;

class StRHICfSimDst;
class StRHICfSimEvent;
class StRHICfSimTrack;
class StRHICfSimBBC;
class StRHICfSimBTof;
class StRHICfSimZDC;

using namespace std;

class StRHICfSimConvertor : public StMaker
{
	enum kConvertFlag{
		kMuDst2SimDst = 0,
		kSimDst2MuDst = 1
	};

	public: 
		StRHICfSimConvertor(int convertFlag, const char* fileName, const Char_t* name="RHICfSimConvertor");
		~StRHICfSimConvertor();

		// void DoMuDst2SimDst(); // Make the RHICf MC input file
		// void DoSimDst2MuDst(); // for RHICf MC Reco
		void SetRHICfRunType(int type){mRHICfRunType = type;}

		Int_t Init();
		Int_t Make();
		Int_t Finish();
		Int_t clear();

	private:
		Int_t InitGePid2PDG();
		Int_t InitMuDst2SimDst();
		Int_t InitSimDst2MuDst();

		Int_t ConvertMuDst2SimDst();
		Int_t GetGeneratorData();

		bool IsSimPropagate(StRHICfSimTrack* simTrk);
		Int_t GetGePid2PDG(int gepid);

		Int_t ConvertSimDst2MuDst();

		void InitRHICfGeometry();
		Int_t GetRHICfGeoHit(double posX, double posY, double posZ, double px, double py, double pz, double e);

		int mConvertFlag;
		TString mInputFile;
		TString mOutputFile;
		Int_t mRHICfRunType;

		TH2Poly* mRHICfPoly;

		// SimDst Coll
		TFile* mSimDstFile;
		TTree* mSimDstTree;
		StRHICfSimDst* mSimDst;
		StRHICfSimEvent* mSimEvent;
		StRHICfSimTrack* mSimTrk;
		StRHICfSimBBC* mSimBBC;
		StRHICfSimBTof* mSimBTof;
		StRHICfSimZDC* mSimZDC;

		// MuDst Coll
		StMuDst* mMuDst;
		StMuEvent* mMuEvent;
		StEvent* mStEvent;
		Long64_t mEvent;

		TClonesArray* mMcVtxArray;
		TClonesArray* mMcTrkArray;
		StMuMcVertex* mMcVtx;
		StMuMcTrack* mMcTrk;
		StMuBTofHit* mMuBTofHit;

		// Generator-Level Coll
		TChain* mChain;
		UInt_t mTmpChainEvent;
		StarGenPPEvent* mGenPPEvent;
		StarGenEvent* mGenEvent;
		StarGenParticle* mGenParticle;

		vector<int> mRHICfGammaIdx;
		vector<int> mRHICfNeuIdx;

		// GePid - PDG encoding macthing
		map<int, int> mGePid2PDGMap;

	ClassDef(StRHICfSimConvertor,0);
};

#endif
