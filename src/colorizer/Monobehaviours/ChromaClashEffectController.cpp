#include "colorizer/Monobehaviours/ChromaClashEffectController.hpp"
#include "colorizer/SaberColorizer.hpp"

#include "UnityEngine/ParticleSystem_MainModule.hpp"
#include "UnityEngine/ParticleSystem_MinMaxGradient.hpp"

DEFINE_TYPE(Chroma::ChromaClashEffectController);

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;


void ChromaClashEffectController::Init(UnityEngine::ParticleSystem *sparkleParticleSystem,
                                       UnityEngine::ParticleSystem *glowParticleSystem,
                                       GlobalNamespace::ColorManager *colorManager) {
    _sparkleParticleSystem = sparkleParticleSystem;
    _glowParticleSystem = glowParticleSystem;
    _colors.push_back(colorManager->ColorForSaberType(SaberType::SaberA));
    _colors.push_back(colorManager->ColorForSaberType(SaberType::SaberB));
    SaberColorizer::SaberColorChanged += {&ChromaClashEffectController::OnSaberColorChanged, this};
}

void ChromaClashEffectController::OnSaberColorChanged(int saberType, UnityEngine::Color color) {
    _colors[(int)saberType] = color;

    Color average = Color::Lerp(_colors[0], _colors[1], 0.5f);
    auto sparkleMain = _sparkleParticleSystem->get_main();
    sparkleMain.set_startColor(ParticleSystem::MinMaxGradient(average));
    auto glowMain = _glowParticleSystem->get_main();
    glowMain.set_startColor(ParticleSystem::MinMaxGradient(average));
}

void ChromaClashEffectController::OnDestroy() {
    SaberColorizer::SaberColorChanged -= {&ChromaClashEffectController::OnSaberColorChanged, this};
}