#include "GameModel.h"

GameModel::GameModel(const GridParams &aParams) : gridParams(aParams)
{
    chips.resize(gridParams.cellsInField.x * gridParams.cellsInField.y);
}

void GameModel::ReGenerateChips()
{
    for (int idx = gridParams.cellsInField.x * gridParams.cellsInField.y - 1; idx > -1; --idx)
    {
        std::unique_ptr<Chip> &chip = chips[idx];
        if (!chip)
        {
            for (int i = idx; i > 0; i -= gridParams.cellsInField.x)
            {
                if (chips[i])
                {
                    std::swap(chips[i], chip);
                    break;
                }
            }
            if (!chip)

            {
                chips[idx].reset(new Chip(GetTypeForChip(idx)));
            }
        }
    }
}

void GameModel::DestroyChips()
{
    for (auto &chip : chips)
    {
        if (chip->GetChipState() == Chip::eChipState::DESTROYED)
        {
            chip.reset(nullptr);
        }
    }
}

const Chips *GameModel::GetChips() const
{
    return &chips;
}

Chip *GameModel::GetChipOnCoordinates(const Vec2<float> &coordinates)
{
    if (coordinates.x < gridParams.offset.x || coordinates.y < gridParams.offset.y ||
        coordinates.x > gridParams.offset.x + gridParams.cellsInField.x * gridParams.cellSize.x ||
        coordinates.y > gridParams.offset.y + gridParams.cellsInField.y * gridParams.cellSize.y)
    {
        return nullptr;
    }

    int column = (coordinates.x - gridParams.offset.x) / gridParams.cellSize.x;
    int row = (coordinates.y - gridParams.offset.y) / gridParams.cellSize.y;

    assert(column >= 0 && column < gridParams.cellsInField.x);
    assert(row >= 0 && row < gridParams.cellsInField.y);

    int idx = row * gridParams.cellsInField.x + column;
    assert(idx < chips.size());

    return chips[idx].get();
}

const Chip::eChipType GameModel::GetTypeForChip(int idx) const
{
    return static_cast<Chip::eChipType>(rand() % static_cast<int>(Chip::eChipType::CHIPS_COUNT));
}

Vec2<int> GameModel::GetGridPositionForChip(const Chip *chip) const
{
    if (!chip)
    {
        return Vec2<int>(-1, -1);
    }

    int i = 0;
    for (; i < chips.size(); ++i)
    {
        if (chip == chips[i].get())
        {
            break;
        }
    }

    return Vec2<int>(i / gridParams.cellsInField.y, i % gridParams.cellsInField.y);
}