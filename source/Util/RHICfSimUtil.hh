#ifndef RHICfSimUtil_hh
#define RHICfSimUtil_hh

#include <time.h>
#include <limits.h>
#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TRandom3.h"

#include "RHICfSimOptions.hh"

class RHICfSimUtil
{
    public:
        static RHICfSimUtil* GetRHICfSimUtil(int num = 0,char** par = 0);

        RHICfSimUtil(int num = 0,char** par = 0);
        ~RHICfSimUtil();

        RHICfSimOptions* GetOptions(){return mSimOptions;}
        
        Long_t GenSeed();
        TString GetGDMLFile();
        Bool_t IsStarSimMode();
        TString GetProcessName(int procId);

    private:
        static RHICfSimUtil* mInstance;

        TString mInput;
        RHICfSimOptions* mSimOptions = nullptr;


};

#endif
