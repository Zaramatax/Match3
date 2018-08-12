#pragma once
#include <vector>
#include <memory>

#include "Helpers.h"

class ChipEffect;

class Chip
{
public:
    enum class eChipType
    {
        NONE = -1,

        BLUE,
        GREEN,
        PURPLE,
        RED,
        YELLOW,

        CHIPS_COUNT
    };

    enum class eChipState
    {
        NEW,
        APPEARING,
        IDLE,
        SWAPPING,
        DESTROYED,
    };

    Chip(eChipType type);
    Chip() = default;

    virtual ~Chip();

    void SetEffect(const ChipEffect *aEffect);
    void SetLock(bool aLocked);
    eChipType GetType() const;
    void SetSelected(bool aSelected);
    bool IsSelected() const;
    eChipState GetChipState() const;
    void SetChipState(eChipState aState);

    static const Vec2<float> chipSize;

private:
    eChipType type = eChipType::NONE;
    eChipState state = eChipState::NEW;
    bool locked = false;
    const ChipEffect *chipEffect = nullptr;
    bool selected = false;
};

