#include "TRestGeant4ParticleSourceSingleProcess.h"

#include "TFile.h"

using namespace std;

ClassImp(TRestGeant4ParticleSourceSingleProcess);

double TRestGeant4ParticleSourceSingleProcess::electron_mass_c2 = 510.998910;

void TRestGeant4ParticleSourceSingleProcess::Initialize() { SetLibraryVersion(LIBRARY_VERSION); }
void TRestGeant4ParticleSourceSingleProcess::PrintParticleSource() {
    RESTMetadata << "---------------------------------------" << RESTendl;
    RESTMetadata << "MegRest version : " << GetLibraryVersion() << RESTendl;
    RESTMetadata << "Particle Source: " << GetName() << RESTendl;
    RESTMetadata << "Cross section type: " << CrossSectionTypeToString(fCrossSectionType) << RESTendl;
    RESTMetadata << "Energy distribution type: " << EnergyDistributionTypesToString(fEnergyDistType)
                 << RESTendl;
    if (GetEnergyDistributionRangeMin() == GetEnergyDistributionRangeMax()) {
        RESTMetadata << "Energy distribution energy: " << GetEnergy() << " keV" << RESTendl;
    } else {
        RESTMetadata << "Energy distribution range (keV): (" << GetEnergyDistributionRangeMin() << ", "
                     << GetEnergyDistributionRangeMax() << ")" << RESTendl;
    }
    switch (fEnergyDistType) {
        case EnergyDistributionTypes::TH1D: {
            RESTMetadata << "Energy distribution filename: "
                         << TRestTools::GetPureFileName((string)GetEnergyDistributionFilename()) << RESTendl;
            RESTMetadata << "Energy distribution histogram name: " << GetEnergyDistributionNameInFile()
                         << RESTendl;
            break;
        }
        case EnergyDistributionTypes::FORMULA: {
            RESTMetadata << "Energy distribution random sampling grid size: "
                         << GetEnergyDistributionFormulaNPoints() << RESTendl;
            break;
        }
    }
    RESTMetadata << "Angular distribution type: " << AngularDistributionTypesToString(fAngularDistType)
                 << RESTendl;
    switch (fAngularDistType) {
        case AngularDistributionTypes::FLUX: {
            RESTMetadata << "Angular distribution direction: (" << GetDirection().X() << ","
                         << GetDirection().Y() << "," << GetDirection().Z() << ")" << RESTendl;
            break;
        }
    }
}

// Use existing parameters defined in TRestGeant4ParticleSource as much as possible
// Newly defined parameter: cross-section
void TRestGeant4ParticleSourceSingleProcess::InitFromConfigFile() {
    // Cross section
    fCrossSectionType = StringToCrossSectionType(GetParameter("cross-section"));

    // Energy distribution
    fEnergyDistType = StringToEnergyDistributionTypes(GetEnergyDistributionType().Data());
    switch (fEnergyDistType) {
        case EnergyDistributionTypes::TH1D: {
            TFile file(fEnergyDistributionFilename);
            auto distribution = (TH1D*)file.Get(fEnergyDistributionNameInFile);

            if (!distribution) {
                RESTError << "Error when trying to find energy spectrum" << RESTendl;
                RESTError << "File: " << fEnergyDistributionFilename << RESTendl;
                RESTError << "Spectrum name: " << fEnergyDistributionNameInFile << RESTendl;
                exit(1);
            }
            SetEnergyDistributionHistogram(distribution);
            break;
        }
        case EnergyDistributionTypes::FORMULA2:
            RESTError << "Unsupported Energy Distribution: FORMULA2." << RESTendl;
            exit(1);
    }

    // Angular distribution
    fAngularDistType = StringToAngularDistributionTypes(GetAngularDistributionType().Data());
    switch (fAngularDistType) {
        case AngularDistributionTypes::TH1D:
        case AngularDistributionTypes::FORMULA:
        case AngularDistributionTypes::FORMULA2:
        case AngularDistributionTypes::BACK_TO_BACK:
            RESTError << "Unsupported Angular Distribution. Only FLUX (uni-direction), ISOTROPIC are "
                         "implemented currently."
                      << RESTendl;
            exit(1);
    }

    // Read the type of incidence direction
    // Overrides the user customization
    // it has to be "Mono" and "Flux" so that the electron's direction and energy is fetched from
    // this class in PrimaryGenerator (restG4)
    fEnergyDistributionType = "Mono";
    fAngularDistributionType = "Flux";
}

void TRestGeant4ParticleSourceSingleProcess::Update() {
    // 0. clean up
    RemoveParticles();

    // 1. sample incident energy
    double gamEnergy0 = SampleEnergy();

    // 2. sample incident direction
    TVector3 gamDirection0 = SampleDirection();

    // 3. generate outgoing particles
    GenerateParticles(gamEnergy0, gamDirection0);
}

std::string TRestGeant4ParticleSourceSingleProcess::CrossSectionTypeToString(const CrossSectionType& type) {
    switch (type) {
        case CrossSectionType::ISOTROPIC:
            return "Isotropic";
        case CrossSectionType::STANDARD:
            return "Standard";
            // case CrossSectionType::POLARIZED:
            //     return "Polarized";
    }
    RESTError << "Unknown CrossSectionType" << RESTendl;
    exit(1);
}

TRestGeant4ParticleSourceSingleProcess::CrossSectionType
TRestGeant4ParticleSourceSingleProcess::StringToCrossSectionType(const std::string& type) {
    if (TString(type).EqualTo(CrossSectionTypeToString(CrossSectionType::ISOTROPIC),
                              TString::ECaseCompare::kIgnoreCase)) {
        return CrossSectionType::ISOTROPIC;
    } else if (TString(type).EqualTo(CrossSectionTypeToString(CrossSectionType::STANDARD),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return CrossSectionType::STANDARD;
        // } else if (TString(type).EqualTo(CrossSectionTypeToString(CrossSectionType::POLARIZED),
        //                                  TString::ECaseCompare::kIgnoreCase)) {
        //     return CrossSectionType::POLARIZED;
    } else {
        RESTError << "Unknown CrossSectionType: " << type << RESTendl;
        exit(1);
    }
}

double TRestGeant4ParticleSourceSingleProcess::SampleEnergy() {
    switch (fEnergyDistType) {
        case EnergyDistributionTypes::MONO:
            return fEnergy;
        case EnergyDistributionTypes::FLAT: {
            return (fEnergyDistributionRange.Y() - fEnergyDistributionRange.X()) * fRandomMethod() +
                   fEnergyDistributionRange.X();
        }
        case EnergyDistributionTypes::LOG: {
            return exp((log(fEnergyDistributionRange.Y()) - log(fEnergyDistributionRange.X())) *
                           fRandomMethod() +
                       log(fEnergyDistributionRange.X()));
        }
        case EnergyDistributionTypes::TH1D: {
            Double_t value = fRandomMethod() * fEnergyDistIntegral;
            Double_t sum = 0;
            Double_t deltaEnergy =
                fEnergyDistHistogram.GetBinCenter(2) - fEnergyDistHistogram.GetBinCenter(1);
            for (int bin = fEnergyStartBin; bin <= fEnergyStopBin; bin++) {
                sum += fEnergyDistHistogram.GetBinContent(bin);

                if (sum > value) {
                    return fEnergyFactor *
                           (fEnergyDistHistogram.GetBinCenter(bin) + deltaEnergy * (0.5 - fRandomMethod()));
                }
            }
        }
        default:
            RESTError << "Should not reach this point" << RESTendl;
            exit(1);
    }
}

TVector3 TRestGeant4ParticleSourceSingleProcess::SampleDirection() {
    switch (fAngularDistType) {
        case AngularDistributionTypes::FLUX:
            return fDirection.Unit();
        case AngularDistributionTypes::ISOTROPIC:
            return GetIsotropicVector();
        default:
            RESTError << "Should not reach this point" << RESTendl;
            exit(1);
    }
}

TVector3 TRestGeant4ParticleSourceSingleProcess::GetIsotropicVector() {
    double a, b, c;
    double n;
    do {
        a = (fRandomMethod() - 0.5) / 0.5;
        b = (fRandomMethod() - 0.5) / 0.5;
        c = (fRandomMethod() - 0.5) / 0.5;
        n = a * a + b * b + c * c;
    } while (n > 1 || n == 0.0);

    n = sqrt(n);
    a /= n;
    b /= n;
    c /= n;
    return {a, b, c};
}

void TRestGeant4ParticleSourceSingleProcess::SetEnergyDistributionHistogram(const TH1D* h) {
    auto xLabel = (TString)h->GetXaxis()->GetTitle();

    if (xLabel.Contains("MeV")) {
        fEnergyFactor = 1.e3;
    } else if (xLabel.Contains("GeV")) {
        fEnergyFactor = 1.e6;
    } else {
        fEnergyFactor = 1.;
    }

    fEnergyDistHistogram = *h;
    fEnergyDistIntegral = fEnergyDistHistogram.Integral();

    fEnergyStartBin = 1;
    fEnergyStopBin = fEnergyDistHistogram.GetNbinsX();

    double eMin = fEnergyDistributionRange.X();
    double eMax = fEnergyDistributionRange.Y();
    if (eMin > 0) {
        for (int i = fEnergyStartBin; i <= fEnergyStopBin; i++) {
            if (fEnergyDistHistogram.GetBinCenter(i) > eMin) {
                fEnergyStartBin = i;
                break;
            }
        }
    }

    if (eMax > 0) {
        for (int i = fEnergyStartBin; i <= fEnergyStopBin; i++) {
            if (fEnergyDistHistogram.GetBinCenter(i) > eMax) {
                fEnergyStopBin = i;
                break;
            }
        }
    }

    fEnergyDistIntegral = fEnergyDistHistogram.Integral(fEnergyStartBin, fEnergyStopBin);
}
