#ifndef StRHICfSimPar_HH
#define StRHICfSimPar_HH

enum SimProcessPar{
    rSingleDiff = 1,
    rDoubleDiff = 2,
    rNonDiff = 3
};

enum SimRHICfRunTypePar{
    rTLtype = 0,
    rTStype = 1,
    rTOPtype = 2
};

enum SimDirection{
    rEast = 0,
    rWest = 1,
    rSideNum = 2
};

enum SimBTofPar{
    rBTofTrayNum = 120,
    rBTofModuleNum = 32,
    rBTofCellnum = 6
};

enum SimVPDPar{
    rVPDEastTray = 121,
    rVPDWestTray = 122,
    rVPDCellNum = 19
};

enum SimBBCPar{
    rBBCLargePmtNum = 8,
    rBBCSmallPmtNum = 16
};

enum SimRHICfPar{
    rTS = 0,
    rTL = 1,
    rTowerNum = 2,
    rPlateNum = 16,
    rLayerNum = 4,
    rXYNum = 2,
    rSmallBarNum = 20,
    rLargeBarNum = 40
};

enum SimZDCPar{
    rZDCPMTNum = 3,
    rSMDXNum = 8,
    rSMDYNum = 7
};

enum SimIOMode{
    rWrite = 1,
    rRead = 2
};

#endif