#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"

#include <unordered_map>

#include "colorizer/LightColorizer.hpp"



using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;

// TODO: Is this the proper equivalent of HashSet<LSEColorManager>?
static std::vector<Chroma::LSEColorManager *> lseColorManagers;

DEFINE_CLASS(Chroma::LSEColorManager);

void LightColorizer::Reset(MonoBehaviour *behaviour) {
    LSEColorManager* manager = Chroma::LSEColorManager::GetLSEColorManager(behaviour);

    if (manager != nullptr) {
        manager->Reset();
    }
}

void LightColorizer::ResetAllLightingColors() {
    for (auto manager : lseColorManagers) {
        manager->Reset();
    }
}

void LightColorizer::SetLightingColors(MonoBehaviour *monoBehaviour, std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                       std::optional<UnityEngine::Color> Color0Boost, std::optional<UnityEngine::Color> Color1Boost) {
    auto manager = Chroma::LSEColorManager::GetLSEColorManager(monoBehaviour);

    if (manager != nullptr) {
        manager->SetLightingColors(Color0, Color1, Color0Boost, Color1Boost);
    }
}

void LightColorizer::SetLightingColors(BeatmapEventType *beatmapEventType, std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                       std::optional<UnityEngine::Color> Color0Boost, std::optional<UnityEngine::Color> Color1Boost) {
    auto manager = Chroma::LSEColorManager::GetLSEColorManager(beatmapEventType);

    for (auto l : LSEColorManager::GetLSEColorManager(beatmapEventType)) {
        l->SetLightingColors(Color0, Color1, Color0Boost, Color1Boost);
    }
}

void LightColorizer::SetAllLightingColors(std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                          std::optional<UnityEngine::Color> Color0Boost, std::optional<UnityEngine::Color> Color1Boost) {
    for (auto manager : lseColorManagers) {
        manager->SetLightingColors(Color0, Color1, Color0Boost, Color1Boost);
    }
}

void LightColorizer::SetActiveColors(BeatmapEventType *lse) {
    for (auto l : LSEColorManager::GetLSEColorManager(lse)) {
        l->SetActiveColors();
    }
}

void LightColorizer::SetAllActiveColors() {
    for (LSEColorManager* lseColorManager : lseColorManagers)
    {
        lseColorManager->SetActiveColors();
    }
}

void LightColorizer::ClearLSEColorManagers() {
    lseColorManagers.clear();
}

void LightColorizer::SetLastValue(MonoBehaviour *monoBehaviour, int val) {
    auto manager = LSEColorManager::GetLSEColorManager(monoBehaviour);

    if (manager != nullptr) {
        manager->SetLastValue(val);
    }
}

std::vector<ILightWithId *> LightColorizer::GetLights(LightSwitchEventEffect *lse) {
    auto manager = LSEColorManager::GetLSEColorManager(lse);

    if (manager != nullptr) {
        return manager->Lights;
    }

    return {};
}

std::unordered_map<int, std::vector<ILightWithId *>> LightColorizer::GetLightsPropagationGrouped(LightSwitchEventEffect *lse) {
    auto manager = LSEColorManager::GetLSEColorManager(lse);

    if (manager != nullptr) {
        return manager->LightsPropagationGrouped;
    }

    return {};

}

void LightColorizer::LSEStart(MonoBehaviour *monoBehaviour, BeatmapEventType *beatmapEventType) {
    LSEColorManager::CreateLSEColorManager(monoBehaviour, beatmapEventType);
}




namespace Chroma {
    void LSEColorManager::ctor(MonoBehaviour *mono, BeatmapEventType *type) {
        _lse = mono;
        _type = type;
        InitializeSOs(mono, "_lightColor0", _lightColor0, _lightColor0_Original, _mLightColor0);
        InitializeSOs(mono, "_highlightColor0", _lightColor0, _lightColor0_Original, _mHighlightColor0);
        InitializeSOs(mono, "_lightColor1", _lightColor1, _lightColor1_Original, _mLightColor1);
        InitializeSOs(mono, "_highlightColor1", _lightColor1, _lightColor1_Original, _mHighlightColor1);


        auto klass = classof(LightSwitchEventEffect *);
        if (il2cpp_functions::class_is_assignable_from(mono->klass, klass)) {
            auto *lse = reinterpret_cast<LightSwitchEventEffect *>(mono);

            InitializeSOs(mono, "_lightColor0Boost", _lightColor0Boost, _lightColor0Boost_Original, _mLightColor0Boost);
            InitializeSOs(mono, "_highlightColor0Boost", _lightColor0Boost, _lightColor0Boost_Original,
                          _mHighlightColor0Boost);
            InitializeSOs(mono, "_lightColor1Boost", _lightColor1Boost, _lightColor1Boost_Original, _mLightColor1Boost);
            InitializeSOs(mono, "_highlightColor1Boost", _lightColor1Boost, _lightColor1Boost_Original,
                          _mHighlightColor1Boost);
            _supportBoostColor = true;

            auto lightList = lse->lightManager->lights->values[lse->lightsID];
            Array<ILightWithId *> *lightArray = lightList->items;
            ILightWithId **ptrArray = lightArray->values;

            Lights = std::vector<ILightWithId *>(ptrArray, ptrArray + ((int) lightList->get_Count()));

            std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroup;
            for (ILightWithId *light : Lights) {
                if (!light) continue;

                getLogger().debug("Doing light");

                auto object = reinterpret_cast<Il2CppObject *>(light);
                if (il2cpp_functions::class_is_assignable_from(object->klass, classof(MonoBehaviour *))) {
                    auto monoBehaviour = reinterpret_cast<MonoBehaviour *>(object);
                    int z = UnityEngine::Mathf::RoundToInt(monoBehaviour->get_transform()->get_position().z);

                    std::vector<ILightWithId *> list;

                    // Not found
                    if (lightsPreGroup.find(z) == lightsPreGroup.end()) {
                        list = {};
                    } else lightsPreGroup[z];

                    list.push_back(light);

                    lightsPreGroup[z] = list;
                }
            }

            LightsPropagationGrouped = lightsPreGroup;
        }
    }

    std::vector<LSEColorManager *> LSEColorManager::GetLSEColorManager(BeatmapEventType *type) {
        std::vector<LSEColorManager *> colorManagers;

        for (auto man : lseColorManagers) {
            if (man->_type == type)
                colorManagers.push_back(man);
        }

        return colorManagers;
    }

    LSEColorManager *LSEColorManager::GetLSEColorManager(MonoBehaviour *m) {
        for (auto man : lseColorManagers) {
            if (man->_lse == m)
                return man;
        }

        return nullptr;
    }

    LSEColorManager *
    LSEColorManager::CreateLSEColorManager(MonoBehaviour *lse, BeatmapEventType *type) {
        LSEColorManager * lsecm;
        lsecm = il2cpp_utils::New<LSEColorManager *>(lse, type).value();
        lseColorManagers.push_back(lsecm);
        return lsecm;
    }

    void LSEColorManager::Reset() {
        _lightColor0->SetColor(_lightColor0_Original);
        _lightColor1->SetColor(_lightColor1_Original);
        if (_supportBoostColor) {
            _lightColor0Boost->SetColor(_lightColor0Boost_Original);
            _lightColor1Boost->SetColor(_lightColor1Boost_Original);
        }
    }

    void LSEColorManager::SetLightingColors(std::optional<UnityEngine::Color> Color0,
                                            std::optional<UnityEngine::Color> Color1,
                                            std::optional<UnityEngine::Color> Color0Boost,
                                            std::optional<UnityEngine::Color> Color1Boost) {
        if (Color0) {
            _lightColor0->SetColor(Color0.value());
        }

        if (Color1) {
            _lightColor1->SetColor(Color1.value());
        }

        if (_supportBoostColor) {
            if (Color0Boost) {
                _lightColor0Boost->SetColor(Color0Boost.value());
            }

            if (Color1Boost) {
                _lightColor1Boost->SetColor(Color1Boost.value());
            }
        }
    }

    void LSEColorManager::SetLastValue(int value) {
        _lastValue = value;
    }

    void LSEColorManager::SetActiveColors() {
        // Replace with ProcessLightSwitchEvent
        if (_lastValue == 0) {
            return;
        }

        bool warm;

        switch ((int) _lastValue) {
            case 1:
            case 2:
            case 3:
            default:
                warm = false;
                break;

            case 5:
            case 6:
            case 7:
                warm = true;
                break;
        }

        UnityEngine::Color c;
        switch ((int) _lastValue) {
            case 1:
            case 5:
            default:
                c = warm ? _mLightColor0->get_color() : _mLightColor1->get_color();
                break;

            case 2:
            case 6:
            case 3:
            case 7:
                c = warm ? _mHighlightColor0->get_color() : _mHighlightColor1->get_color();
                break;
        }

        if (_lse->get_enabled()) {
            if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(LightSwitchEventEffect *))) {
                auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                l1->highlightColor = c;
            } else if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(ParticleSystemEventEffect *))) {
                auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);

                p1->highlightColor = c;
            }

            if (_lastValue == 3 || _lastValue == 7) {
                if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(LightSwitchEventEffect *))) {
                    auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                    c.a = 0.0f;
                    l1->afterHighlightColor = c;
                } else if (il2cpp_functions::class_is_assignable_from(_lse->klass,
                                                                      classof(ParticleSystemEventEffect *))) {
                    auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);
                    c.a = 0.0f;
                    p1->highlightColor = c;
                }
            } else {
                if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(LightSwitchEventEffect *))) {
                    auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                    l1->afterHighlightColor = c;
                } else if (il2cpp_functions::class_is_assignable_from(_lse->klass,
                                                                      classof(ParticleSystemEventEffect *))) {
                    auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);

                    p1->afterHighlightColor = c;
                }
            }
        } else {
            if (_lastValue == 1 || _lastValue == 5 || _lastValue == 2 || _lastValue == 6) {
                if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(LightSwitchEventEffect *))) {
                    auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                    l1->SetColor(c);
                } else if (il2cpp_functions::class_is_assignable_from(_lse->klass,
                                                                      classof(ParticleSystemEventEffect *))) {
                    auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);

                    p1->particleColor = c;
                    p1->RefreshParticles();
                }
            }
        }

        if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(LightSwitchEventEffect *))) {
            auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
            c.a = 0.0f;
            l1->offColor = c;
        } else if (il2cpp_functions::class_is_assignable_from(_lse->klass, classof(ParticleSystemEventEffect *))) {
            auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);
            c.a = 0.0f;
            p1->offColor = c;
        }
    }


    void LSEColorManager::InitializeSOs(MonoBehaviour *lse, const std::string &id, SimpleColorSO *&sColorSO,
                                        UnityEngine::Color &originalColor, MultipliedColorSO *&mColorSO) {

        // Todo: Use codegen here
        MultipliedColorSO *lightMultSO = nullptr;
        if (il2cpp_functions::class_is_assignable_from(lse->klass, classof(LightSwitchEventEffect *))) {
            auto l1 = reinterpret_cast<LightSwitchEventEffect *>(lse);
            lightMultSO = reinterpret_cast<MultipliedColorSO *>(il2cpp_utils::GetFieldValue<ColorSO *>(l1,
                                                                                                       id).value()); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        } else if (il2cpp_functions::class_is_assignable_from(lse->klass, classof(ParticleSystemEventEffect *))) {
            auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(lse);
            lightMultSO = reinterpret_cast<MultipliedColorSO *>(il2cpp_utils::GetFieldValue<ColorSO *>(p1,
                                                                                                       id).value()); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        }

        UnityEngine::Color multiplierColor = lightMultSO->multiplierColor;
        SimpleColorSO *lightSO = lightMultSO->baseColor;
        originalColor = lightSO->color;

        if (mColorSO == nullptr) {
            mColorSO = ScriptableObject::CreateInstance<MultipliedColorSO *>();
            mColorSO->multiplierColor = multiplierColor; //.SetField("_multiplierColor", multiplierColor);

            if (sColorSO == nullptr) {
                sColorSO = ScriptableObject::CreateInstance<SimpleColorSO *>();
                sColorSO->SetColor(originalColor);
            }

            mColorSO->baseColor = sColorSO; //SetField("_baseColor", sColorSO);
        }

        getLogger().debug("Overwriting %s", id.c_str());
        if (il2cpp_functions::class_is_assignable_from(lse->klass, classof(LightSwitchEventEffect *))) {
            CRASH_UNLESS(mColorSO);
            auto l1 = reinterpret_cast<LightSwitchEventEffect *>(lse);

            il2cpp_utils::SetFieldValue<LightSwitchEventEffect *, ColorSO *>(l1, id,
                                                                             mColorSO); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        } else if (il2cpp_functions::class_is_assignable_from(lse->klass, classof(ParticleSystemEventEffect *))) {
            CRASH_UNLESS(mColorSO);
            auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(lse);

            il2cpp_utils::SetFieldValue<ParticleSystemEventEffect *, ColorSO *>(p1, id,
                                                                                mColorSO); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        }
    }
}