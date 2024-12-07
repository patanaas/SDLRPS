#ifndef LEVEL_H
#define LEVEL_H

#include "StandardIncludes.h"

class Level {
public:
    virtual ~Level() {}
    virtual bool Load() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void HandleEvents(SDL_Event& event) = 0;
    virtual void Clean() = 0;
};

#endif // !LEVEL_H
