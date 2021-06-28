#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "GlobalNamespace/ObstacleSaberSparkleEffectManager.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/ObstacleSaberSparkleEffect.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberTypeExtensions.hpp"
#include "GlobalNamespace/HapticFeedbackController.hpp"
#include "System/Action_1.hpp"

#include "UnityEngine/Color.hpp"

#include "UnityEngine/XR/XRNode.hpp"

#include "colorizer/ObstacleColorizer.hpp"
#include "utils/ChromaUtils.hpp"

using namespace GlobalNamespace;
using namespace Chroma;
using namespace UnityEngine;

void SetObstacleSaberSparkleColor(ObstacleSaberSparkleEffect* obstacleSaberSparkleEffect, GlobalNamespace::ObstacleController* obstacleController)
{
    float h, s, _;
    Color::RGBToHSV(ObstacleColorizer::GetObstacleColorizer(obstacleController)->getColor(), h, s, _);
    obstacleSaberSparkleEffect->set_color(Color::HSVToRGB(h, s, 1));
}

MAKE_HOOK_OFFSETLESS(ObstacleSaberSparkleEffectManager_Update, void, ObstacleSaberSparkleEffectManager* self) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        ObstacleSaberSparkleEffectManager_Update(self);
        return;
    }


    self->wasSystemActive->set_Item(0, self->isSystemActive->get(0));
    self->wasSystemActive->set_Item(1, self->isSystemActive->get(1));
    self->isSystemActive->set_Item(0, false);
    self->isSystemActive->set_Item(1, false);

    std::vector<GlobalNamespace::ObstacleController*> activeObstacleControllersVec(self->beatmapObjectManager->get_activeObstacleControllers()->items->Length());

    self->beatmapObjectManager->get_activeObstacleControllers()->items->copy_to(activeObstacleControllersVec);

    for (auto& obstacleController : activeObstacleControllersVec)
    {
        auto bounds = obstacleController->bounds;
        for (int i = 0; i < 2; i++) {
            Vector3 vector;
            if (self->sabers->get(i)->get_isActiveAndEnabled() &&
            self->GetBurnMarkPos(bounds, obstacleController->get_transform(), self->sabers->get(i)->saberBladeBottomPos,self->sabers->get(i)->saberBladeTopPos,vector))
            {
                self->isSystemActive->set_Item(i, true);
                self->burnMarkPositions->set_Item(i, vector);
                self->effects->get(i)->SetPositionAndRotation(vector, self->GetEffectRotation(vector, obstacleController->get_transform(),bounds));

                // TRANSPILE IS HERE
                SetObstacleSaberSparkleColor(self->effects->get(i), obstacleController);
                // TRANSPILE IS HERE

                self->hapticFeedbackController->PlayHapticFeedback(SaberTypeExtensions::Node(self->sabers->get(i)->get_saberType()), self->rumblePreset);
                if (!self->wasSystemActive->get(i)) {
                    self->effects->get(i)->StartEmission();
                    auto action = self->sparkleEffectDidStartEvent;
                    if (action) {
                        action->Invoke(self->sabers->get(i)->get_saberType());
                    }
                }
            }
        }
    }
    for (int j = 0; j < 2; j++) {
        if (!self->isSystemActive->get(j) && self->wasSystemActive->get(j)) {
            self->effects->get(j)->StopEmission();
            auto action2 = self->sparkleEffectDidEndEvent;
            if (action2) {
                action2->Invoke(self->sabers->get(j)->get_saberType());
            }
        }
    }
}

void Chroma::Hooks::ObstacleSaberSparkleEffectManager() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ObstacleSaberSparkleEffectManager_Update, il2cpp_utils::FindMethodUnsafe("", "ObstacleSaberSparkleEffectManager", "Update", 0));
}