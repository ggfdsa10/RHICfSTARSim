#ifndef RHICfSimOptions_hh
#define RHICfSimOptions_hh

#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

using namespace std;

class RHICfSimOptions
{
    public:
        RHICfSimOptions();
        ~RHICfSimOptions();

        void Init();
        void SetInputOption(int num, char** par);

        void AddOpt(TString name, int value);
        void AddOpt(TString name, double value);
        void AddOpt(TString name, bool value);
        void AddOpt(TString name, TString value);
        // void AddOpt(TString name, const char* value){AddOpt(name, TString(value));}

        int GetOptInt(TString name);
        double GetOptDouble(TString name);
        bool GetOptBool(TString name);
        TString GetOptString(TString name); 
        
        bool CheckOpt(TString name);

    private:
        // void PrintOpt();
        void ParsingFile();
        void ParsingParm();
        void AddStringByType(TString name, TString val);
        TString GetDirPath(TString type);

        int mInputParNum;
        char** mInputPar;

        vector<pair<TString, TString>> mRequiredPar;
        vector<pair<TString, int>> mOptInt;
        vector<pair<TString, double>> mOptDouble;
        vector<pair<TString, bool>> mOptBool;
        vector<pair<TString, TString>> mOptString;
};

#endif
