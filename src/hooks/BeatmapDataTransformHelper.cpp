#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/BeatmapDataTransformHelper.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"

#include "lighting/ChromaEventData.hpp"
#include "ChromaObjectData.hpp"

using namespace Chroma;

MAKE_HOOK_OFFSETLESS(BeatmapDataTransformHelper_CreateTransformedBeatmapData, GlobalNamespace::IReadonlyBeatmapData*, GlobalNamespace::BeatmapDataTransformHelper* self,
                     GlobalNamespace::IReadonlyBeatmapData* beatmapData, GlobalNamespace::IPreviewBeatmapLevel* beatmapLevel,
                     GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PracticeSettings* practiceSettings,
                     bool leftHanded, GlobalNamespace::EnvironmentEffectsFilterPreset environmentEffectsFilterPreset,
                     GlobalNamespace::EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions
                     ) {
    auto result = BeatmapDataTransformHelper_CreateTransformedBeatmapData(self, beatmapData, beatmapLevel, gameplayModifiers, practiceSettings, leftHanded, environmentEffectsFilterPreset, environmentIntensityReductionOptions);

    // Essentially, here we cancel the original method. DO NOT call it IF it's a Chroma map
    if (!ChromaController::DoChromaHooks()) {
        return result;
    }

    ChromaObjectDataManager::deserialize(beatmapData);
    ChromaEventDataManager::deserialize(beatmapData);

    return result;
}

void Chroma::Hooks::BeatmapDataTransformHelper() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BeatmapDataTransformHelper_CreateTransformedBeatmapData, il2cpp_utils::FindMethodUnsafe("", "BeatmapDataTransformHelper", "CreateTransformedBeatmapData", 7) );
}