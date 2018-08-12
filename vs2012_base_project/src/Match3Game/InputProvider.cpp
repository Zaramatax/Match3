#include "InputProvider.h"

InputProvider::InputProvider(const King::Engine *aEngine) : engine(aEngine) {}

static const float Epsilon = 5.0f;

void InputProvider::AddListener(InputListener *listener)
{
    listeners.push_back(listener);
}

void InputProvider::RemoveListener(InputListener *listener)
{
    //smthng
}

void InputProvider::Update()
{
    if (engine->GetMouseButtonDown())
    {
        if (!mouseDown)
        {
            mouseDown = true;
            mouseCoordinates = Vec2<float>(engine->GetMouseX(), engine->GetMouseY());
            OnMouseDown();
        }
    }
    else if (mouseDown)
    {
        mouseDown = false;
        Vec2<float> newMouseCoordinates = Vec2<float>(engine->GetMouseX(), engine->GetMouseY());
        Vec2<float> diff = mouseCoordinates - newMouseCoordinates;
        mouseCoordinates = newMouseCoordinates;
        float offset = sqrt(diff.x * diff.x + diff.y * diff.y);
        if (offset < Epsilon)
        {
            OnClick();
        }
        else
        {
            eSwipeDirection direction = eSwipeDirection::NONE;
            if (diff.x > diff.y)
            {
                direction = diff.x > 0 ? eSwipeDirection::RIGHT : eSwipeDirection::LEFT;
            }
            else
            {
                direction = diff.y > 0 ? eSwipeDirection::UP : eSwipeDirection::DOWN;
            }
            OnSwipe(direction);
        }
    }
}

void InputProvider::OnClick() const
{
    for (const auto &listener : listeners)
    {
        listener->OnClick(mouseCoordinates);
    }
}

void InputProvider::OnSwipe(eSwipeDirection direction) const
{
    for (const auto &listener : listeners)
    {
        listener->OnSwipe(mouseCoordinates, direction);
    }
}

void InputProvider::OnMouseDown() const
{
    for (const auto &listener : listeners)
    {
        listener->OnMouseDown(mouseCoordinates);
    }
}