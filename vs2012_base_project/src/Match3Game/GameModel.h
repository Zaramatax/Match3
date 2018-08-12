#pragma once

#include <utility>
#include <memory>
#include <vector>
#include "Chip.h"
#include "InputProvider.h"
#include "Helpers.h"

using Chips = std::vector<std::unique_ptr<Chip>>;

struct GridParams
{
    Vec2<float> offset;
    Vec2<float> cellSize;
    Vec2<int> cellsInField;

    GridParams() : offset(0.0f, 0.0f), cellSize(0.0f, 0.0f), cellsInField(0, 0) {};
};

class GameModel
{
public:
    GameModel(const GridParams &aParams);
    virtual ~GameModel() = default;

    const GridParams &GetParams() const { return gridParams; };
    
    void ReGenerateChips();
    void DestroyChips();
    const Chips *GetChips() const;
    Chip *GetChipOnCoordinates(const Vec2<float> &coordinates);
    Vec2<int> GetGridPositionForChip(const Chip *chip) const;

private:
    const Chip::eChipType GetTypeForChip(int idx) const;

    const GridParams gridParams;
    Chips chips;
};