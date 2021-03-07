#include "lighting/environments/TimbalandEnvironment.hpp"

using namespace Chroma;

EnvironmentLightDataT TimbalandEnvironment::getEnvironmentLights() {
    return EnvironmentLightDataT(
            {
                    {0, {
                                {1, 3},
                                {2, 4},
                                {3, 5},
                                {4, 6},
                                {5, 7},
                                {6, 8},
                                {7, 9},
                                {8, 10},
                                {9, 11},
                                {10, 12},
                                {11, 13},
                                {12, 14},
                                {13, 15},
                                {14, 16},
                                {15, 17},
                                {16, 18},
                                {17, 19},
                                {18, 20},
                                {19, 21},
                                {20, 22},
                        }},
                    {1, {
                                {1, 3},
                                {2, 4},
                                {3, 5},
                                {4, 6},
                                {5, 7},
                                {6, 8},
                                {7, 9},
                                {8, 10},
                                {9, 11},
                                {10, 12},
                                {11, 13},
                                {12, 14},
                                {13, 15},
                                {14, 16},
                                {15, 17},
                                {16, 18},
                                {17, 19},
                                {18, 20},
                                {19, 21},
                                {20, 22},
                        }},
                    {2, {
                                {1, 3},
                                {2, 4},
                                {3, 5},
                                {4, 6},
                                {5, 7},
                                {6, 8},
                                {7, 9},
                                {8, 10},
                                {9, 11},
                                {10, 12},
                        }},
                    {3, {
                                {1, 3},
                                {2, 4},
                                {3, 5},
                                {4, 6},
                                {5, 7},
                                {6, 8},
                                {7, 9},
                                {8, 10},
                                {9, 11},
                                {10, 12},
                                {11, 13},
                                {12, 14},
                                {13, 18},
                                {14, 17},
                        }},
                    {4, {
                                {1, 14},
                                {2, 8},
                                {3, 10},
                                {4, 9},
                                {5, 7},
                                {6, 13},
                                {7, 3},
                                {8, 5},
                        }},
            }
    );

}