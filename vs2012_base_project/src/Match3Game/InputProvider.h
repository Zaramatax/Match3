#pragma once

#include <king/Engine.h>
#include <king/Updater.h>
#include <vector>

#include "Helpers.h"
#include "Game.h"

enum class eSwipeDirection
{
    UP,
    RIGHT,
    DOWN,
    LEFT,

    NONE,
};

class InputListener
{
public:
    virtual void OnMouseDown(const Vec2<float> &mouseCoordinates) = 0;
    virtual void OnSwipe(const Vec2<float> &mouseCoordinates, eSwipeDirection direction) = 0;
    virtual void OnClick(const Vec2<float> &mouseCoordinates) = 0;
};

class InputProvider : public GameUpdater
{
public:
    InputProvider(const King::Engine *aEngine);

    void AddListener(InputListener *listener);
    void RemoveListener(InputListener *listener);
    
private:
    void Update() override;

    void OnClick() const;
    void OnSwipe(eSwipeDirection direction) const;
    void OnMouseDown() const;

    std::vector<InputListener*> listeners;
    Vec2<float> mouseCoordinates;
    bool mouseDown = false;
    const King::Engine *engine = nullptr;
};