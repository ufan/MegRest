#ifndef TRestGeant4ParticleSourceCompton_Class
#define TRestGeant4ParticleSourceCompton_Class

#include <string>

#include "TH1D.h"
#include "TRestGeant4ParticleSource.h"
#include "TRestGeant4PrimaryGeneratorInfo.h"

using namespace TRestGeant4PrimaryGeneratorTypes;

class TRestGeant4ParticleSourceCompton : public TRestGeant4ParticleSource {
   public:
    void Update() override;
    void InitFromConfigFile() override;
    void PrintParticleSource() override;

    TRestGeant4ParticleSourceCompton() = default;
    ~TRestGeant4ParticleSourceCompton() = default;

   protected:
    enum class CrossSectionType {
        ISOTROPIC,  // isotropic
        STANDARD,   // free-electron (G4KleinNishinaCompton)
        POLARIZED,  // [todo]: phi polarization to be implemented
    };

    std::string CrossSectionTypeToString(const CrossSectionType&);
    CrossSectionType StringToCrossSectionType(const std::string&);

    // sampling methods
    double SampleEnergy();       // incident energy
    TVector3 SampleDirection();  // incident direction
    TVector3 GetIsotropicVector();
    void SetEnergyDistributionHistogram(const TH1D* h);

   private:
    // attributes
    CrossSectionType fCrossSectionType = CrossSectionType::STANDARD;             // standard is free-electron
    EnergyDistributionTypes fEnergyDistType = EnergyDistributionTypes::MONO;     // mono is uni-energy
    AngularDistributionTypes fAngularDistType = AngularDistributionTypes::FLUX;  // flux is uni-direction
    // [improve] todo: thread-specific random generator may be needed (esp. in physical analysis)
    // calling interface from PrimaryGenerator needs to pass in the thread id
    TH1D fEnergyDistHistogram;
    Double_t fEnergyDistIntegral;
    Double_t fEnergyFactor;
    Int_t fEnergyStartBin;
    Int_t fEnergyStopBin;

    static double electron_mass_c2;

    ClassDefOverride(TRestGeant4ParticleSourceCompton, 1);
};
#endif
