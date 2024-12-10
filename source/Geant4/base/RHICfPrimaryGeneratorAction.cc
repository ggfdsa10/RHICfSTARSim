#include "RHICfPrimaryGeneratorAction.hh"
// #include "HepMCG4AsciiReader.hh"


RHICfPrimaryGeneratorAction::RHICfPrimaryGeneratorAction()
{
    fParticleGun = new G4ParticleGun();
    fParTable = G4ParticleTable::GetParticleTable();
}

RHICfPrimaryGeneratorAction::~RHICfPrimaryGeneratorAction()
{
}

void RHICfPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    fRunManager = G4RunManager::GetRunManager();

    if(!fParticleGun){return;}

    int eventId = anEvent->GetEventID();
    fInputTree -> GetEntry(eventId);
    
    // for matching the incident particle information
    fParSimTrkIdx.clear();

    // start the propagating of particles
    int simTrackNum = fSimDst -> GetSimTrackNum();
    for(int i=0; i<simTrackNum; i++){
        fSimTrack = fSimDst -> GetSimTrack(i);
        bool isSimPropagate = fSimTrack -> IsSimPropagate();
        if(!isSimPropagate){continue;}

        int pid = fSimTrack -> GetPid();

        double startVx = fSimTrack -> GetVxStart() * 10.; // [mm]
        double startVy = fSimTrack -> GetVyStart() * 10.; // [mm]
        double startVz = fSimTrack -> GetVzStart() * 10.; // [mm]

        double px = fSimTrack -> GetPx();
        double py = fSimTrack -> GetPy();
        double pz = fSimTrack -> GetPz();
        double p = sqrt(px*px + py*py + pz*pz);
        double e = fSimTrack -> GetE();
        
        double unitX = px/p;
        double unitY = py/p;
        double unitZ = pz/p;

        fParticle = fParTable -> FindParticle(pid);
        fParticleGun -> SetParticleDefinition(fParticle);
        fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(unitX, unitY, unitZ));
        fParticleGun -> SetParticleEnergy(e *GeV);
        fParticleGun -> SetParticlePosition(G4ThreeVector(startVx *mm, startVy *mm, startVz *mm));
        fParticleGun -> GeneratePrimaryVertex(anEvent);

        fParSimTrkIdx.push_back(fSimTrack->GetId());
    }
}