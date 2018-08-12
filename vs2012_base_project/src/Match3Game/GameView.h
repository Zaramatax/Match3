#pragma once

#include "king/Engine.h"
#include <vector>
#include <functional>
#include "GameModel.h"
#include "Game.h"
#include "Helpers.h"

class Animation
{
public:
    Animation(const Chip *aChip, const Vec2<float> &aStart, const Vec2<float> &aEnd) :
        chip(aChip),
        start(aStart),
        end(aEnd)
    {};
    void ProcessAnimation(float dt);
    bool IsAnimationEnded();

private:
    const Chip *chip = nullptr;
    Vec2<float> start;
    Vec2<float> end;
    float progress = 0;

    float timer = 0.3f;
};

class GameView : public GameUpdater
{
public:
    GameView(King::Engine *aEngine);
    virtual ~GameView() = default;

    void SetGridParams(const GridParams &gameParams);
    void UpdateChips(const Chips *aChips);
    void StopAllAnimations();

    void SwapChips(const Chip *chip1, const Chip *chip2);
    void ShowNewChips(std::vector<const Chip*> chips);
    void DestroyChips(std::vector<const Chip*> chips);

    void SetAppearCallback(std::function<void()> aCallback);
    void SetSwapCallback(std::function<void()> aCallback);
    void SetDestroyCallback(std::function<void()> aCallback);

private:
    void Update() override;

    void ShowChips();
    void ShowBackground();
    void ShowGrid();

    King::Engine::Texture GetTextureForChip(Chip::eChipType type) const;
    Vec2<float> GetCoordinatesFor(int idx) const;
    void ProcessAnimations(std::vector<std::unique_ptr<Animation>> &animations, const std::function<void()> &callback);

    const Chips *chips = nullptr;
    King::Engine *engine = nullptr;
    GridParams gridParams;
    std::vector<std::unique_ptr<Animation>> swapAnimations;
    std::vector<std::unique_ptr<Animation>> appearAnimations;
    std::vector<std::unique_ptr<Animation>> destroyAnimations;
    std::function<void()> onSwapEnded;
    std::function<void()> onAppearEnded;
    std::function<void()> onDestroyEnded;
};