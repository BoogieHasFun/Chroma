#pragma once

#include <vector>
#include <optional>

#include "UnityEngine/Vector3.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "lighting/environment_enhancements/GameObjectInfo.hpp"

namespace Chroma {
    enum class LookupMethod
    {
        Regex,
        Exact,
        Contains,
    };

    class EnvironmentEnhancementManager {
    private:
        inline static std::vector<GameObjectInfo> _gameObjectInfos;

        static std::vector<GameObjectInfo> LookupId(const std::string& id, LookupMethod lookupMethod);

        static std::optional<UnityEngine::Vector3> GetVectorData(std::reference_wrapper<rapidjson::Value> dynData, const std::string& name);

        static void GetAllGameObjects();

    public:
        inline static std::unordered_map<GlobalNamespace::TrackLaneRing*, bool> SkipRingUpdate;

        static void Init(CustomJSONData::CustomBeatmapData* customBeatmapData, float noteLinesDistance);


    };
}