#ifndef LHCFTRACKINGACTION_H
#define LHCFTRACKINGACTION_H 1

#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"

#include "G4RunManager.hh"
#include "RHICfPrimaryGeneratorAction.hh"
#include "StRHICfSimDst.h"
#include "StRHICfSimTrack.h"

class RHICfTrackingAction: public G4UserTrackingAction
{
    public:
        RHICfTrackingAction();
        ~RHICfTrackingAction();

        void PreUserTrackingAction(const G4Track*);
        void PostUserTrackingAction(const G4Track*);

        void SetSimDst(StRHICfSimDst* simDst){fSimDst = simDst;}

    private:
        G4RunManager* fRunManager;
        RHICfPrimaryGeneratorAction* fGenAction;
        StRHICfSimDst* fSimDst;
        StRHICfSimTrack* fSimTrack;
};

#endif
