#include "TRestGeant4ParticleSourceCompton.h"

#include "TMath.h"

using namespace std;

ClassImp(TRestGeant4ParticleSourceCompton);

void TRestGeant4ParticleSourceCompton::GenerateParticles(double e0, TVector3 dir0) {
    TVector3 dir1;
    double epsilon, onecost;
    double E0_m = e0 / electron_mass_c2;

    switch (fCrossSectionType) {
        case CrossSectionType::ISOTROPIC: {
            dir1 = GetIsotropicVector();
            onecost = 1. - dir1.CosTheta();
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

            dir1 = TVector3(sinTeta * cos(Phi), sinTeta * sin(Phi), cosTeta);
            break;
        }
    }

    dir1.RotateUz(dir0);

    // 4. calculate dynamics
    double e1 = epsilon * e0;
    double eKinEnergy = e0 - e1;
    TVector3 eDirection = e0 * dir0 - e1 * dir1;
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
    gamma.SetEnergy(e1);
    gamma.SetDirection(dir1);
    AddParticle(gamma);
}
