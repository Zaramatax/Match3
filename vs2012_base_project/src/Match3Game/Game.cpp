#include "Game.h"
#include "GameModel.h"
#include "GameController.h"
#include "GameView.h"
#include "InputProvider.h"

Game::Game()
{
    GridParams params;
    RetrieveGridParams(params);

    engine = std::make_unique<King::Engine>("./assets");
    gameModel = std::make_unique<GameModel>(params);
    gameView = std::make_unique<GameView>(engine.get());
    gameController = std::make_unique<GameController>(gameView.get(), gameModel.get());
    inputProvider = std::make_unique<InputProvider>(engine.get());
    inputProvider->AddListener(gameController.get());

    AddUpdater(gameView.get());
    AddUpdater(inputProvider.get());
}

Game::~Game()
{
}

void Game::Update()
{
    for (const auto &updater : updaters)
    {
        updater->Update();
    }
}

void Game::StartGame()
{
    gameController->StartGame();
    engine->Start(*this);
}

void Game::QuitGame()
{
    RemoveAllUpdaters();
}

void Game::RetrieveGridParams(GridParams &params)
{
    params.offset = Vec2<float>(317.0f, 93.0f);
    params.cellSize = Vec2<float>(45.0f, 45.0f);
    params.cellsInField = Vec2<int>(8, 8);
}

void Game::AddUpdater(GameUpdater *updater)
{
    updaters.push_back(updater);
}

void Game::RemoveAllUpdaters()
{
    updaters.clear();
}