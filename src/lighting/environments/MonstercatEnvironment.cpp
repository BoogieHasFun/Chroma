#include "lighting/environments/MonstercatEnvironment.hpp"

using namespace Chroma;

EnvironmentLightDataT MonstercatEnvironment::getEnvironmentLights() {
    return EnvironmentLightDataT(
            {
                    {0, {
                                {1, 4},
                                {2, 8},
                                {3, 10},
                                {4, 12},
                                {5, 9},
                                {6, 7},
                                {7, 11},
                                {8, 6},
                        }},
                    {1, {
                                {1, 7},
                                {2, 3},
                                {3, 4},
                                {4, 9},
                                {5, 11},
                                {6, 8},
                                {7, 10},
                        }},
                    {2, {
                                {1, 1},
                                {2, 2},
                                {3, 3},
                                {4, 4},
                                {5, 5},
                        }},
                    {3, {
                                {1, 5},
                                {2, 3},
                                {3, 4},
                                {4, 1},
                                {5, 2},
                        }},
                    {4, {
                                {1, 16},
                                {2, 13},
                                {3, 10},
                                {4, 9},
                                {5, 1},
                                {6, 11},
                                {7, 7},
                                {8, 0},
                                {9, 19},
                                {10, 17},
                                {11, 18},
                                {12, 20},
                                {13, 6},
                                {14, 4},
                        }},
            }
    );

}