#include "Chip.h"

const Vec2<float> Chip::chipSize = Vec2<float>(35.0f, 36.0f);

Chip::Chip(eChipType aType) : type(aType)
{}

Chip::~Chip()
{
}

Chip::eChipType Chip::GetType() const
{
    return type;
}

void Chip::SetSelected(bool aSelected)
{
    selected = aSelected;
}

bool Chip::IsSelected() const
{
    return selected;
}

Chip::eChipState Chip::GetChipState() const
{
    return state;
}
void Chip::SetChipState(eChipState aState)
{
    state = aState;
}