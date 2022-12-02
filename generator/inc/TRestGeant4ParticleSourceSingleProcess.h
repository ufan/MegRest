#ifndef TRestGeant4ParticleSourceSingleProcess_Class
#define TRestGeant4ParticleSourceSingleProcess_Class

#include <string>

#include "TH1D.h"
#include "TRestGeant4ParticleSource.h"
#include "TRestGeant4PrimaryGeneratorInfo.h"
#include "TVector3.h"

using namespace TRestGeant4PrimaryGeneratorTypes;

class TRestGeant4ParticleSourceSingleProcess : public TRestGeant4ParticleSource {
   public:
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void InitFromConfigFile() override;
    virtual void PrintParticleSource() override;

    TRestGeant4ParticleSourceSingleProcess() = default;
    ~TRestGeant4ParticleSourceSingleProcess() = default;

   protected:
    static double electron_mass_c2;  // keV
    enum class CrossSectionType {
        ISOTROPIC,  // isotropic
        STANDARD,   // free-electron (G4KleinNishinaCompton)
        POLARIZED,  // [todo]: phi polarization to be implemented
    };
    std::string CrossSectionTypeToString(const CrossSectionType&);
    CrossSectionType StringToCrossSectionType(const std::string&);

    //
    virtual void GenerateParticles(double e0, TVector3 dir0) = 0;  // production particles

    // sampling methods
    double SampleEnergy();       // incident energy
    TVector3 SampleDirection();  // incident direction
    TVector3 GetIsotropicVector();

    void SetEnergyDistributionHistogram(const TH1D* h);

    // attributes
    CrossSectionType fCrossSectionType = CrossSectionType::STANDARD;             // standard is free-electron
    EnergyDistributionTypes fEnergyDistType = EnergyDistributionTypes::MONO;     // mono is uni-energy
    AngularDistributionTypes fAngularDistType = AngularDistributionTypes::FLUX;  // flux is uni-direction
    TH1D fEnergyDistHistogram;
    Double_t fEnergyDistIntegral;
    Double_t fEnergyFactor;
    Int_t fEnergyStartBin;
    Int_t fEnergyStopBin;

    ClassDefOverride(TRestGeant4ParticleSourceSingleProcess, 1);
};
#endif
