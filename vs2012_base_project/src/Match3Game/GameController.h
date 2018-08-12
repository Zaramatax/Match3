#pragma once

#include <king/Updater.h>
#include <memory>
#include <vector>
#include <functional>
#include "GameModel.h"
#include "InputProvider.h"

class GameView;
class Chip;

enum class eGameProcessState
{
    NEW,

    IDLE,
    ANIMATING,
};

class GameController : public InputListener
{
public:
    GameController(GameView *aGameView, GameModel *aGameModel);
    virtual ~GameController() = default;

    void StartGame();

private:
    void OnMouseDown(const Vec2<float> &mouseCoordinates) override;
    void OnSwipe(const Vec2<float> &mouseCoordinates, eSwipeDirection direction) override;
    void OnClick(const Vec2<float> &mouseCoordinates) override;

    void SelectChip(const Vec2<float> &mouseCoordinates);
    void Process();
    void StartGameCycle();

    void SwapChips(eSwipeDirection direction);
    void AppearChips();
    void MarkChipsAsDestroyed(Vec2<int> start, Vec2<int> end);
    void DestroyChips();

    void OnSwapped();
    void OnAppeared();
    void OnDestroyed();

    eSwipeDirection GetSwapDirectionForSelected();

    GameView *gameView = nullptr;
    GameModel *gameModel = nullptr;
    Chip *selectedChip1 = nullptr;
    Chip *selectedChip2 = nullptr;
    std::vector<const Chip*> chipsToDestroy;
    bool tryToSwap = false;

    eGameProcessState state = eGameProcessState::NEW;
};