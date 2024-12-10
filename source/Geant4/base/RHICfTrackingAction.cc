#include "RHICfTrackingAction.hh"

RHICfTrackingAction::RHICfTrackingAction()
{
}

RHICfTrackingAction::~RHICfTrackingAction()
{
}

void RHICfTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
}

void RHICfTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
    int parentId = aTrack -> GetParentID();
    int trackId = aTrack -> GetTrackID();
    if(parentId != 0){return;}

    // Set the Propagated particle end vertex
    fRunManager = G4RunManager::GetRunManager();
    fGenAction = (RHICfPrimaryGeneratorAction*)fRunManager->GetUserPrimaryGeneratorAction();
    vector<int> parSimTrkId = fGenAction -> GetParSimTrkIdxArray();
    if(parSimTrkId.size() == 0){return;}

    int simTrkId = parSimTrkId[trackId-1];
    fSimTrack = fSimDst->GetSimTrack(simTrkId);

    double vxEnd = aTrack -> GetPosition().x()*0.1; // [cm]
    double vyEnd = aTrack -> GetPosition().y()*0.1; // [cm]
    double vzEnd = aTrack -> GetPosition().z()*0.1; // [cm]

    fSimTrack -> SetVertexEnd(vxEnd, vyEnd, vzEnd);
}
