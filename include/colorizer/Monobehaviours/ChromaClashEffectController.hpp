#pragma once


#include "UnityEngine/ParticleSystem.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/SaberType.hpp"

#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"


DECLARE_CLASS_CODEGEN(Chroma, ChromaClashEffectController, UnityEngine::MonoBehaviour,
  private:
          DECLARE_INSTANCE_FIELD(UnityEngine::ParticleSystem*, _sparkleParticleSystem);
          DECLARE_INSTANCE_FIELD(UnityEngine::ParticleSystem*, _glowParticleSystem);
          std::vector<UnityEngine::Color> _colors;

          DECLARE_METHOD(void, OnSaberColorChanged, int saberType, UnityEngine::Color color);
          public:
          DECLARE_METHOD(void, Init, UnityEngine::ParticleSystem* sparkleParticleSystem, UnityEngine::ParticleSystem* glowParticleSystem, GlobalNamespace::ColorManager* colorManager);
          DECLARE_METHOD(void, OnDestroy);

          DECLARE_SIMPLE_DTOR();
          DECLARE_DEFAULT_CTOR();

          REGISTER_FUNCTION(
              REGISTER_METHOD(Init);
              REGISTER_METHOD(OnDestroy);
              REGISTER_METHOD(OnSaberColorChanged);
              REGISTER_DEFAULT_CTOR();
              REGISTER_SIMPLE_DTOR();
          )
)