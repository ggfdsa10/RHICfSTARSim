    #ifndef LHCFRUNACTION_H
    #define LHCFRUNACTION_H 1

    #include "G4UserRunAction.hh"
    #include "G4Timer.hh"
    #include "G4String.hh"
    #include "globals.hh"

    #include "TROOT.h"
    #include "TFile.h"
    #include "TTree.h"
    #include "TString.h"

    #include "StRHICfSimDst.h"
    #include "RHICfSimUtil.hh"

class G4Run;
class RHICfRunAction: public G4UserRunAction
{
    public:
        RHICfRunAction();
        ~RHICfRunAction();

        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);

        private:
        G4Timer* fTimer; 
        G4double fElapsedTime;

    	RHICfSimUtil* fSimUtil;
        RHICfSimOptions* fSimOpt;

        TFile* fInSimDstFile;
        TTree* fInSimDstTree;

        TFile* fOutSimDstFile;
        TTree* fOutSimDstTree;

        StRHICfSimDst* fSimDst;
    };

    #endif
