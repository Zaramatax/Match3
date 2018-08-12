#include "GameView.h"
#include "GameModel.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Chip.h"

GameView::GameView(King::Engine *aEngine) :
    engine(aEngine)
{}

void GameView::SetGridParams(const GridParams &aGridParams)
{
    gridParams = aGridParams;
}

void GameView::ShowBackground()
{
    engine->Render(King::Engine::TEXTURE_BACKGROUND, 0.0f, 0.0f);
}

void GameView::Update()
{
    ShowBackground();
    ShowGrid();
    ShowChips();

    ProcessAnimations(appearAnimations, onAppearEnded);
    ProcessAnimations(swapAnimations, onSwapEnded);
    ProcessAnimations(destroyAnimations, onDestroyEnded);
}

void GameView::ShowChips()
{
    if (!chips)
    {
        return;
    }

    for (int idx = 0; idx < gridParams.cellsInField.x * gridParams.cellsInField.y; ++idx)
    {
        const std::unique_ptr<Chip> &chip = chips->at(idx);
        if (chip->GetChipState() != Chip::eChipState::IDLE)
        {
            continue;
        }

        Vec2<float> coordinates = GetCoordinatesFor(idx);
        
        float scale = chip->IsSelected() ? 1.2f : 1.0f;
        Vec2<float> offset(Chip::chipSize.x * (scale - 1.0f) / 2.0f, Chip::chipSize.y * (scale - 1.0f) / 2.0f);
        coordinates = coordinates - offset;

        glm::mat4 transformation;
        transformation = glm::translate(transformation, glm::vec3(coordinates.x, coordinates.y, 0.0f));
        transformation = glm::scale(transformation, glm::vec3(scale, scale, 1));

        engine->Render(GetTextureForChip(chip->GetType()), transformation);
    }
}

void GameView::UpdateChips(const Chips *aChips)
{
    chips = aChips;
}

Vec2<float> GameView::GetCoordinatesFor(int idx) const
{
    int column = idx / gridParams.cellsInField.x;
    int row = idx % gridParams.cellsInField.x;
    Vec2<float> coordinates;
    coordinates.x = gridParams.offset.x + gridParams.cellSize.x * row + (gridParams.cellSize.x - Chip::chipSize.x) / 2.0f;
    coordinates.y = gridParams.offset.y + gridParams.cellSize.y * column + (gridParams.cellSize.y - Chip::chipSize.y) / 2.0f;
    return coordinates;
}

void GameView::ShowGrid()
{
    glm::mat4 transformation;

    float scaleX = 1.0f;
    float scaleY = gridParams.cellsInField.y * gridParams.cellSize.y;
    transformation = glm::scale(transformation, glm::vec3(scaleX, scaleY, 0.0f));

    float xPos = gridParams.offset.x;
    float yPos = gridParams.offset.y / scaleY;
    transformation = glm::translate(transformation, glm::vec3(xPos, yPos, 0));

    engine->Render(King::Engine::TEXTURE_GRID, transformation);

    for (int i = 0; i < gridParams.cellsInField.x; ++i)
    {
        transformation = glm::translate(transformation, glm::vec3(gridParams.cellSize.x, 0, 0));
        engine->Render(King::Engine::TEXTURE_GRID, transformation);
    }

    scaleX = gridParams.cellsInField.x * gridParams.cellSize.x;
    scaleY = 1.0f / scaleY;
    transformation = glm::scale(transformation, glm::vec3(scaleX, scaleY, 0.0f));

    xPos = -gridParams.cellsInField.x * gridParams.cellSize.x / scaleX;
    yPos = 0;
    transformation = glm::translate(transformation, glm::vec3(xPos, yPos, 0));

    engine->Render(King::Engine::TEXTURE_GRID, transformation);

    for (int i = 0; i < gridParams.cellsInField.y; ++i)
    {
        transformation = glm::translate(transformation, glm::vec3(0, gridParams.cellSize.y, 0));
        engine->Render(King::Engine::TEXTURE_GRID, transformation);
    }
}

King::Engine::Texture GameView::GetTextureForChip(Chip::eChipType type) const
{
    switch (type)
    {
    case Chip::eChipType::BLUE:
        return King::Engine::TEXTURE_BLUE;
        break;
    case Chip::eChipType::GREEN:
        return King::Engine::TEXTURE_GREEN;
        break;
    case Chip::eChipType::PURPLE:
        return King::Engine::TEXTURE_PURPLE;
        break;
    case Chip::eChipType::RED:
        return King::Engine::TEXTURE_RED;
        break;
    case Chip::eChipType::YELLOW:
        return King::Engine::TEXTURE_YELLOW;
        break;

    default:
        return King::Engine::Texture::TEXTURE_MAX;
        break;
    }
}

void GameView::SwapChips(const Chip *chip1, const Chip *chip2)
{
    //TODO: retrieve start and end coordinates here
    Vec2<float> start(0, 0);
    Vec2<float> end(0, 0);

    swapAnimations.push_back(std::make_unique<Animation>(chip1, start, end));
    swapAnimations.push_back(std::make_unique<Animation>(chip2, end, start));
}

void GameView::ProcessAnimations(std::vector<std::unique_ptr<Animation>> &animations, const std::function<void()> &callback)
{
    int animationsCount = animations.size();

    if (!animationsCount)
    {
        return;
    }

    for (const auto &animation : animations)
    {
        animation->ProcessAnimation(engine->GetLastFrameSeconds());
    }
    animations.erase(std::remove_if(animations.begin(), animations.end(), 
        [](const std::unique_ptr<Animation> &animation) { return animation->IsAnimationEnded(); }), 
        animations.end());

    if (animations.empty() && animationsCount)
    {
        if (callback)
        {
            callback();
        }
    }
}

void GameView::ShowNewChips(std::vector<const Chip*> chips)
{
    float startY = gridParams.offset.y - Chip::chipSize.y;
    for (const auto &chip : chips)
    {
        //TODO: get coordinates for chip here
        Vec2<float> pos(0, 0);
        appearAnimations.push_back(std::make_unique<Animation>(chip, Vec2<float>(pos.x, startY), pos));
    }
}

void GameView::StopAllAnimations()
{
    //for (const auto &animation : animations)
    //{
    //    animation->EndAnimation();
    //}
    //animations.clear();
    //if (callback)
    //{
    //    callback();
    //    callback = nullptr;
    //}
}

void GameView::DestroyChips(std::vector<const Chip*> chips)
{
    float endY = gridParams.offset.y + Chip::chipSize.y + gridParams.cellSize.y * gridParams.cellsInField.y;
    for (const auto &chip : chips)
    {
        //TODO: get coordinates for chip here
        Vec2<float> pos(0, 0);
        destroyAnimations.push_back(std::make_unique<Animation>(chip, pos, Vec2<float>(pos.x, endY)));
    }
}

void GameView::SetAppearCallback(std::function<void()> aCallback)
{
    onAppearEnded = aCallback;
}

void GameView::SetSwapCallback(std::function<void()> aCallback)
{
    onSwapEnded = aCallback;
}
void GameView::SetDestroyCallback(std::function<void()> aCallback)
{
    onDestroyEnded = aCallback;
}

void Animation::ProcessAnimation(float dt)
{
    timer -= dt;
}

bool Animation::IsAnimationEnded()
{
    return timer < 0.0f; 
}
