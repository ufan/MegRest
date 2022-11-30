#include "TRestGeant4ParticleSourceCompton.h"

#include "TFile.h"
#include "TMath.h"

using namespace std;

ClassImp(TRestGeant4ParticleSourceCompton);

double TRestGeant4ParticleSourceCompton::electron_mass_c2 = 510.998910;

void TRestGeant4ParticleSourceCompton::PrintParticleSource() {
    RESTMetadata << "---------------------------------------" << RESTendl;
    RESTMetadata << "Particle Source Name: Compton Scattering (Simple)" << RESTendl;
    RESTMetadata << "Energy: " << GetEnergy() << " keV" << RESTendl;
}

// Use existing parameters defined in TRestGeant4ParticleSource as much as possible
// Newly defined parameter: cross-section
void TRestGeant4ParticleSourceCompton::InitFromConfigFile() {
    //
    fDirection = fDirection.Unit();

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

void TRestGeant4ParticleSourceCompton::Update() {
    RemoveParticles();

    // 1. sample incident energy
    double gamEnergy0 = SampleEnergy();

    // 2. sample incident direction
    TVector3 gamDirection0 = SampleDirection();

    // 3. sample outgoing direction
    TVector3 gamDirection1;
    double epsilon, onecost;
    double E0_m = gamEnergy0 / electron_mass_c2;
    switch (fCrossSectionType) {
        case CrossSectionType::ISOTROPIC: {
            gamDirection1 = GetIsotropicVector();
            onecost = 1. - gamDirection1.CosTheta();
            epsilon = 1. / (1. + E0_m * onecost);
            break;
        }
        case CrossSectionType::STANDARD: {
            double epsilonsq, sint2, greject;

            double eps0 = 1. / (1. + 2. * E0_m);
            double epsilon0sq = eps0 * eps0;
            double alpha1 = -TMath::Log(eps0);
            double alpha2 = alpha1 + 0.5 * (1. - epsilon0sq);

            static const int nlooplim = 1000;
            int nloop = 0;
            do {
                ++nloop;
                // false interaction if too many iterations
                if (nloop > nlooplim) {
                    return;
                }

                if (alpha1 > alpha2 * fRandomMethod()) {
                    epsilon = TMath::Exp(-alpha1 * fRandomMethod());  // eps0**r
                    epsilonsq = epsilon * epsilon;
                } else {
                    epsilonsq = epsilon0sq + (1. - epsilon0sq) * fRandomMethod();
                    epsilon = sqrt(epsilonsq);
                };

                onecost = (1. - epsilon) / (epsilon * E0_m);
                sint2 = onecost * (2. - onecost);
                greject = 1. - epsilon * sint2 / (1. + epsilonsq);
                // Loop checking, 03-Aug-2015, Vladimir Ivanchenko
            } while (greject < fRandomMethod());

            if (sint2 < 0.0) {
                sint2 = 0.0;
            }
            double cosTeta = 1. - onecost;
            double sinTeta = sqrt(sint2);
            double Phi = TMath::Pi() * fRandomMethod();

            gamDirection1 = TVector3(sinTeta * cos(Phi), sinTeta * sin(Phi), cosTeta);
            break;
        }
    }

    gamDirection1.RotateUz(gamDirection0);

    // 4. calculate dynamics
    double gamEnergy1 = epsilon * gamEnergy0;
    double eKinEnergy = gamEnergy0 - gamEnergy1;
    TVector3 eDirection = gamEnergy0 * gamDirection0 - gamEnergy1 * gamDirection1;
    eDirection = eDirection.Unit();

    // 5. create the electron and gamma
    TRestGeant4Particle electron;
    electron.SetParticleName("e-");
    electron.SetParticleCharge(-1);
    electron.SetExcitationLevel(0);
    electron.SetEnergy(eKinEnergy);
    electron.SetDirection(eDirection);
    AddParticle(electron);

    TRestGeant4Particle gamma;
    gamma.SetParticleName("gamma");
    gamma.SetParticleCharge(0);
    gamma.SetExcitationLevel(0);
    gamma.SetEnergy(gamEnergy1);
    gamma.SetDirection(gamDirection1);
    AddParticle(gamma);
}

std::string TRestGeant4ParticleSourceCompton::CrossSectionTypeToString(const CrossSectionType& type) {
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

TRestGeant4ParticleSourceCompton::CrossSectionType TRestGeant4ParticleSourceCompton::StringToCrossSectionType(
    const std::string& type) {
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

double TRestGeant4ParticleSourceCompton::SampleEnergy() {
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

TVector3 TRestGeant4ParticleSourceCompton::SampleDirection() {
    switch (fAngularDistType) {
        case AngularDistributionTypes::FLUX:
            return fDirection;
        case AngularDistributionTypes::ISOTROPIC:
            return GetIsotropicVector();
        default:
            RESTError << "Should not reach this point" << RESTendl;
            exit(1);
    }
}

TVector3 TRestGeant4ParticleSourceCompton::GetIsotropicVector() {
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

void TRestGeant4ParticleSourceCompton::SetEnergyDistributionHistogram(const TH1D* h) {
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
