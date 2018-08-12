#pragma once

#include <memory>
#include <king/Engine.h>
#include <king/Updater.h>
#include <vector>

class GameModel;
class GameController;
class GameView;
struct GridParams;
class InputProvider;

class GameUpdater
{
public:
    virtual void Update() = 0;
};

class Game : public King::Updater
{
public:
    Game();
    virtual ~Game();

    void StartGame();
    void QuitGame();

private:
    void Update() override;

    void RetrieveGridParams(GridParams &params);
    void AddUpdater(GameUpdater *updater);
    void RemoveAllUpdaters();

    std::unique_ptr<GameModel> gameModel;
    std::unique_ptr<GameController> gameController;
    std::unique_ptr<GameView> gameView;
    std::unique_ptr<King::Engine> engine;
    std::unique_ptr<InputProvider> inputProvider;

    std::vector<GameUpdater*> updaters;
};