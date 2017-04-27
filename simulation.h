#pragma once

#include <cstddef>
#include <cstdint>
struct SDL_Window;
union SDL_Event;
class Scene;

class Simulation
{
    Scene* mScene;

    int mDeltaMouseX;
    int mDeltaMouseY;

public:
    void Init(Scene* scene);
    void HandleEvent(const SDL_Event& ev);
    void Update(float deltaTime);
	uint32_t mSpinningTransformID;
	uint32_t mSpinningTransformID2;
	uint32_t mSplineTransformID;
    void* operator new(size_t sz);
};
