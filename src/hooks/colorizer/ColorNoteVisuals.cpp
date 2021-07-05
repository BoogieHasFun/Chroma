#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/MultiplayerConnectedPlayerNoteController.hpp"
#include "GlobalNamespace/TutorialNoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(ColorNoteVisuals_HandleNoteControllerDidInit,
                &ColorNoteVisuals::HandleNoteControllerDidInit,
                void, ColorNoteVisuals* self, NoteControllerBase* noteController) {
    ColorNoteVisuals_HandleNoteControllerDidInit(self, noteController); // This calls the original method

    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }

    auto NoteControllerCast = il2cpp_utils::try_cast<NoteController>(noteController);

    if (NoteControllerCast) {
        auto it = ChromaObjectDataManager::ChromaObjectDatas.find(NoteControllerCast.value()->noteData);

        if (it != ChromaObjectDataManager::ChromaObjectDatas.end()) {
            auto chromaData = std::static_pointer_cast<ChromaNoteData>(it->second);

            std::optional<UnityEngine::Color> color = chromaData->Color;

            NoteColorizer::ColorizeNote(noteController, color);
        }
    }
}

void ColorNoteVisualsHook(Logger& logger) {
    INSTALL_HOOK(logger, ColorNoteVisuals_HandleNoteControllerDidInit);
}

ChromaInstallHooks(ColorNoteVisualsHook)