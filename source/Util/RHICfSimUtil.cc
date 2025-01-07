#include "RHICfSimUtil.hh"

RHICfSimUtil* RHICfSimUtil::mInstance = nullptr;

RHICfSimUtil* RHICfSimUtil::GetRHICfSimUtil(int num, char** par){
    if (mInstance != nullptr){return mInstance;}
    return new RHICfSimUtil(num, par);
}

RHICfSimUtil::RHICfSimUtil(int num, char** par) 
: mInput("")
{
    mInstance = this;

    if(!mSimOptions){
        mSimOptions = new RHICfSimOptions();
        mSimOptions -> SetInputOption(num, par);
    }
}

RHICfSimUtil::~RHICfSimUtil()
{
}

Long_t RHICfSimUtil::GenSeed()
{
    // generate the TRandom seed using local time in ms.
    struct timespec specific_time;
    clock_gettime(CLOCK_REALTIME, &specific_time);
    struct tm* now = localtime(&specific_time.tv_sec);
    int millsec = floor(specific_time.tv_nsec/1.0e6);
    TString correntTime = Form("%02d%02d%02d%02d%02d%02d%d",-100+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, millsec);

    // generate the seed from 0 to Long_t max value
    TRandom3* rndEngine = new TRandom3(correntTime.Atoi());
    return rndEngine -> Integer(LONG_MAX);
}

TString RHICfSimUtil::GetGDMLFile()
{
    if(!mSimOptions){return "";}
    TString path = mSimOptions->GetOptString("GEOMETRYDIR");
    TString runtype = mSimOptions->GetOptString("RUNTYPE");
    TString gdmlFile = "";
    if(runtype == "TS"){gdmlFile = path+"/full_ts.gdml";}
    else if(runtype == "TL"){gdmlFile = path+"/full_tl.gdml";}
    else if(runtype == "TOP"){gdmlFile = path+"/full_top.gdml";}
    else{
        cout << "RHICfSimUtil::GetGDMLFile() -- Error: Runtype = " << runtype << " is not validate" << endl;
        return "";
    }

    ifstream inFile(gdmlFile.Data());
    if(!(inFile.is_open())){return "";}

    vector<TString> lineVec;

    std::string linetmp;
    while(std::getline(inFile, linetmp)){
        TString line = linetmp;

        if(line.Index("<!ENTITY gwd") != -1){
            line = "<!ENTITY gwd \"" + path + "\">";
            lineVec.push_back(line);
        }
        else{
            lineVec.push_back(line);
        }
    }
    inFile.close();

    ofstream outFile(gdmlFile.Data());
    for(int i=0; i<lineVec.size(); i++){
        outFile << lineVec[i] << endl;
    }
    outFile.close();

    return gdmlFile;
}

Bool_t RHICfSimUtil::IsStarSimMode()
{
    if(!mSimOptions){return 0;}
    TString mode = mSimOptions -> GetOptString("MODE");
    mode.ToUpper();
    if(mode == "STARSIM"){return 1;}
    return 0;
}

TString RHICfSimUtil::GetProcessName(int procId)
{
    if(procId == 101){return "NonDiffraction";}
    if(procId == 103 || procId == 104){return "SingleDiffraction";}
    if(procId == 105){return "DoubleDiffraction";}
    if(procId == 106){return "CentralDiffraction";}
    if(procId == 102){return "Elestic";}
    return "Non";
}   

