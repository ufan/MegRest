#ifndef TRestGeant4ParticleSourceSingleCompton_Class
#define TRestGeant4ParticleSourceSingleCompton_Class

#include "TRestGeant4ParticleSourceSingleProcess.h"

class TRestGeant4ParticleSourceCompton : public TRestGeant4ParticleSourceSingleProcess {
   public:
    TRestGeant4ParticleSourceCompton() = default;
    ~TRestGeant4ParticleSourceCompton() = default;

   protected:
    virtual void GenerateParticles(double e0, TVector3 dir0) override;

    ClassDefOverride(TRestGeant4ParticleSourceCompton, 1);
};
#endif
