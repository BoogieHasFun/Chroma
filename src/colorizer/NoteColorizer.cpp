#include "Chroma.hpp"

#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"

#include <unordered_map>
#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"
#include "ChromaObjectData.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;
//
//std::vector<std::optional<UnityEngine::Color>> NoteColorizer::SaberColorOverride = std::vector<std::optional<UnityEngine::Color>>(2);
//
//std::vector<SaberColorizer::BSMColorManager *> SaberColorizer::_bsmColorManagers;

std::unordered_map<int ,std::optional<UnityEngine::Color>> NoteColorizer::NoteColorOverride = {{0, std::nullopt}, {1, std::nullopt}};
std::unordered_map<int ,std::optional<UnityEngine::Color>> NoteColorizer::CNVColorManager::GlobalColor = {{0, std::nullopt}, {1, std::nullopt}};

void NoteColorizer::Reset(GlobalNamespace::NoteController *nc) {
    auto m = CNVColorManager::GetCNVColorManager(nc);

    if (m)
        m->Reset();
}

void NoteColorizer::ResetAllNotesColors() {
    CNVColorManager::ResetGlobal();

    for (auto cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager.second->Reset();
    }
}

void NoteColorizer::SetNoteColors(GlobalNamespace::NoteController *cnv, std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    auto m = CNVColorManager::GetCNVColorManager(cnv);

    if (m)
        m->SetNoteColors(color0, color1);
}

void
NoteColorizer::SetAllNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    CNVColorManager::SetGlobalNoteColors(color0, color1);

    for (auto cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager.second->Reset();
    }
}

void NoteColorizer::SetActiveColors(GlobalNamespace::NoteController *nc) {
    CNVColorManager::GetCNVColorManager(nc)->SetActiveColors();
}

void NoteColorizer::SetAllActiveColors() {
    for (auto cnvColorManager : _cnvColorManagers)
    {
        cnvColorManager.second->SetActiveColors();
    }
}

void NoteColorizer::ClearCNVColorManagers() {
    ResetAllNotesColors();
    _cnvColorManagers.clear();
}



//std::optional<UnityEngine::Color> getColor(rapidjson::Value& json) {
//    auto jsonArray = json.GetArray();
//
//    if (jsonArray.Size() < 3)
//        return std::nullopt;
//
//
//    return UnityEngine::Color(jsonArray[0].GetFloat(), jsonArray[1].GetFloat(), jsonArray[2].GetFloat(), jsonArray.Size() > 3 ? jsonArray[4].GetFloat() : 1.0f);
//}

void NoteColorizer::EnableNoteColorOverride(GlobalNamespace::NoteController *noteController) {
    auto chromaData = ChromaObjectDataManager::ChromaObjectDatas[noteController->noteData];

    auto noteChromaData = std::static_pointer_cast<ChromaNoteData>(chromaData);

    if (noteChromaData->Color0) {
        NoteColorOverride[0] = noteChromaData->Color0;
    } else {
        NoteColorOverride[0] = CNVColorManager::GlobalColor[0];
    }

    if (noteChromaData->Color1) {
        NoteColorOverride[1] = noteChromaData->Color1;
    } else {
        NoteColorOverride[1] = CNVColorManager::GlobalColor[1];
    }
}

void NoteColorizer::DisableNoteColorOverride() {
    NoteColorOverride[0] = std::nullopt;
    NoteColorOverride[1] = std::nullopt;
}

bool MatchesColorType(SaberType saberType, ColorType colorType){
    return (saberType.value == SaberType::SaberA && colorType.value == ColorType::ColorA)
    || (saberType.value == SaberType::SaberB && colorType.value == ColorType::ColorB);
}


void NoteColorizer::ColorizeSaber(GlobalNamespace::NoteController *noteController, GlobalNamespace::NoteCutInfo *noteCutInfo) {
    if (ChromaController::DoColorizerSabers())
        {
        NoteData* noteData = noteController->noteData;
        auto saberType = noteCutInfo->saberType;
        auto colorType = noteData->colorType;
        if (MatchesColorType(saberType, colorType))
        {
            UnityEngine::Color color = CNVColorManager::GetCNVColorManager(noteController)->ColorForCNVManager();

            SaberColorizer::SetSaberColor(saberType, color);
        }
    }
}

void NoteColorizer::CNVStart(GlobalNamespace::ColorNoteVisuals *cnv, GlobalNamespace::NoteController *nc) {
    ColorType noteType = nc->noteData->colorType;
    if (noteType == ColorType::ColorA || noteType == ColorType::ColorB)
    {
        CNVColorManager::CreateCNVColorManager(cnv, nc);
    }
}

std::optional<UnityEngine::Color> NoteColorizer::getNoteColorOverride(int color) {
    if (NoteColorizer::NoteColorOverride.size() < color + 1)
        return std::nullopt;

    return NoteColorizer::NoteColorOverride[color];
}

NoteColorizer::CNVColorManager::CNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv, GlobalNamespace::NoteController *nc) {
    _cnv = cnv;
    _nc = nc;
    _colorManager = cnv->colorManager;

    _chromaData = std::static_pointer_cast<ChromaNoteData>(ChromaObjectDataManager::ChromaObjectDatas[nc->noteData]);

    if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomNoteData*), nc->noteData->klass)) {
        _noteData = reinterpret_cast<CustomJSONData::CustomNoteData *>(nc->noteData);

        _noteData->customData->associatedData['C'] = new CustomData::NoteData {GlobalColor[0], GlobalColor[1]};
    }
}

NoteColorizer::CNVColorManager *
NoteColorizer::CNVColorManager::GetCNVColorManager(GlobalNamespace::NoteController *nc) {
    auto it = _cnvColorManagers.find(nc);

    if (it != _cnvColorManagers.end()) return it->second;

    return nullptr;
}

NoteColorizer::CNVColorManager *
NoteColorizer::CNVColorManager::CreateCNVColorManager(GlobalNamespace::ColorNoteVisuals *cnv,
                                                      GlobalNamespace::NoteController *nc) {
    CNVColorManager *cnvColorManager = GetCNVColorManager(nc);
    if (cnvColorManager != nullptr)
    {
        if (ASSIGNMENT_CHECK(classof(CustomJSONData::CustomNoteData*), nc->noteData->klass)) {
            auto *customData = reinterpret_cast<CustomNoteData *>(nc->noteData);
            cnvColorManager->_noteData = customData;

           cnvColorManager->_chromaData = std::static_pointer_cast<ChromaNoteData>(ChromaObjectDataManager::ChromaObjectDatas[nc->noteData]);
           cnvColorManager->Reset();

//            if (customData->customData && customData->customData->value) {
//                cnvColorManager->_noteData = customData;
//                customData->customData->value->GetObject()["color0"] = _globalColor[0];
//                customData->customData->value->GetObject()["color1"] = _globalColor[0];
//            }
        }

        return nullptr;
    }

    auto* cnvcm = new CNVColorManager(cnv, nc);
    _cnvColorManagers[nc] = cnvcm;
    return cnvcm;
}

void NoteColorizer::CNVColorManager::SetGlobalNoteColors(std::optional<UnityEngine::Color> color0,std::optional<UnityEngine::Color> color1) {
    if (color0)
    {
        GlobalColor[0] = color0.value();
    }

    if (color1)
    {
        GlobalColor[1] = color1.value();
    }
}

void NoteColorizer::CNVColorManager::ResetGlobal() {
    GlobalColor[0] = std::nullopt;
    GlobalColor[1] = std::nullopt;
}

void NoteColorizer::CNVColorManager::Reset() {
    _noteData->customData->associatedData['C'] = new CustomData::NoteData {std::nullopt, std::nullopt};
}

void NoteColorizer::CNVColorManager::SetNoteColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1) {
    if (color0 || color1) {
        if (color0) {
            _chromaData->Color0 = color0.value();
        }

        if (color1) {
            _chromaData->Color1 = color1.value();
        }

    }
}

UnityEngine::Color NoteColorizer::CNVColorManager::ColorForCNVManager() {
    EnableNoteColorOverride(_nc);
    UnityEngine::Color noteColor = _colorManager->ColorForType(_noteData->colorType);
    DisableNoteColorOverride();
    return noteColor;
}

UnityEngine::Color ColorWithAlpha(UnityEngine::Color color, float a) {
    color.a = a;
    return color;
}

void NoteColorizer::CNVColorManager::SetActiveColors() {
    ColorNoteVisuals *colorNoteVisuals = _cnv;

    UnityEngine::Color noteColor = ColorForCNVManager();

    if (noteColor == colorNoteVisuals->noteColor) {
        return;
    }

    SpriteRenderer* arrowGlowSpriteRenderer = colorNoteVisuals->arrowGlowSpriteRenderer;
    SpriteRenderer* circleGlowSpriteRenderer = colorNoteVisuals->circleGlowSpriteRenderer;
    arrowGlowSpriteRenderer->set_color(ColorWithAlpha(noteColor, arrowGlowSpriteRenderer->get_color().a));
    circleGlowSpriteRenderer->set_color(ColorWithAlpha(noteColor, circleGlowSpriteRenderer->get_color().a));
    Array<MaterialPropertyBlockController *> *materialPropertyBlockControllers = colorNoteVisuals->materialPropertyBlockControllers;
    for (int i = 0; i < materialPropertyBlockControllers->Length(); i++)
    {
        if (_colorID == -1) {
            _colorID = UnityEngine::Shader::PropertyToID(il2cpp_utils::createcsstr("_Color"));
        }

        MaterialPropertyBlockController *materialPropertyBlockController = materialPropertyBlockControllers->values[i];
        materialPropertyBlockController->materialPropertyBlock->SetColor(_colorID, noteColor);
        materialPropertyBlockController->ApplyChanges();
    }
}


