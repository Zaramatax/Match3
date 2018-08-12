#include "GameController.h"
#include "GameView.h"
#include "GameModel.h"

static const int MATCH_COUNT = 3;

GameController::GameController(GameView *aGameView, GameModel *aGameModel) :
    gameView(aGameView),
    gameModel(aGameModel)
{
    gameView->SetAppearCallback(std::bind(&GameController::OnAppeared, this));
    gameView->SetSwapCallback(std::bind(&GameController::OnSwapped, this));
    gameView->SetDestroyCallback(std::bind(&GameController::OnDestroyed, this));
}

void GameController::StartGame()
{
    StartGameCycle();
    gameView->SetGridParams(gameModel->GetParams());
    gameView->UpdateChips(gameModel->GetChips());
}

void GameController::OnMouseDown(const Vec2<float> &mouseCoordinates)
{
    if (state != eGameProcessState::IDLE)
    {
        return;
    }
    SelectChip(mouseCoordinates);
}

void GameController::OnSwipe(const Vec2<float> &mouseCoordinates, eSwipeDirection direction)
{
    if (state != eGameProcessState::IDLE)
    {
        return;
    }
}

void GameController::OnClick(const Vec2<float> &mouseCoordinates)
{}

void GameController::SelectChip(const Vec2<float> &mouseCoordinates)
{
    Chip *selected = gameModel->GetChipOnCoordinates(mouseCoordinates);

    if (selectedChip1)
    {
        selectedChip2 = selected;
    }
    else
    {
        selectedChip1 = selected;
    }

    if (selectedChip1)
    {
        selectedChip1->SetSelected(true);
    }

    if (selectedChip1 && selectedChip2)
    {
        eSwipeDirection direction = GetSwapDirectionForSelected();
        if (direction == eSwipeDirection::NONE)
        {
            selected->SetSelected(true);
            selectedChip1->SetSelected(false);
            selectedChip1 = selected;
            selectedChip2 = nullptr;
        }
        else
        {
            SwapChips(direction);

            selectedChip1->SetSelected(false);
        }
    }
}

void GameController::SwapChips(eSwipeDirection direction)
{
    state = eGameProcessState::ANIMATING;

    selectedChip1->SetChipState(Chip::eChipState::SWAPPING);
    selectedChip2->SetChipState(Chip::eChipState::SWAPPING);

    gameView->SwapChips(selectedChip1, selectedChip2);
}

void GameController::OnSwapped()
{
    state = eGameProcessState::IDLE;

    selectedChip1->SetChipState(Chip::eChipState::IDLE);
    selectedChip2->SetChipState(Chip::eChipState::IDLE);

    Process();
}

void GameController::OnAppeared()
{
    state = eGameProcessState::IDLE;

    for (const auto &chip : *(gameModel->GetChips()))
    {
        if (chip->GetChipState() == Chip::eChipState::APPEARING)
        {
            chip->SetChipState(Chip::eChipState::IDLE);
        }
    }

    Process();
}

void GameController::OnDestroyed()
{
    state = eGameProcessState::IDLE;

    gameModel->DestroyChips();
    StartGameCycle();
}

eSwipeDirection GameController::GetSwapDirectionForSelected()
{
    Vec2<int> pos1 = gameModel->GetGridPositionForChip(selectedChip1);
    Vec2<int> pos2 = gameModel->GetGridPositionForChip(selectedChip2);

    eSwipeDirection swapDirection = eSwipeDirection::NONE;
    if (pos1.x - pos2.x == 1)
    {
        swapDirection = eSwipeDirection::LEFT;
    }
    else if (pos2.x - pos1.x == 1)
    {
        swapDirection = eSwipeDirection::RIGHT;
    }
    else if (pos1.y - pos2.y == 1)
    {
        swapDirection = eSwipeDirection::DOWN;
    }
    else if (pos2.y - pos1.y == 1)
    {
        swapDirection = eSwipeDirection::UP;
    }

    return swapDirection;
}

void GameController::AppearChips()
{
    state = eGameProcessState::ANIMATING;

    std::vector<const Chip*> appearingChips;
    for (const auto &chip : *(gameModel->GetChips()))
    {
        if (chip->GetChipState() == Chip::eChipState::NEW || chip->GetChipState() == Chip::eChipState::APPEARING)
        {
            chip->SetChipState(Chip::eChipState::APPEARING);
            appearingChips.push_back(chip.get());
        }
    }
    gameView->ShowNewChips(appearingChips);
}

void GameController::StartGameCycle()
{
    gameModel->ReGenerateChips();
    AppearChips();
}

void GameController::Process()
{
    chipsToDestroy.clear();
    
    if(selectedChip1 && selectedChip2)
    {
        std::swap(*selectedChip1, *selectedChip2);
        if (tryToSwap)
        {
            selectedChip1 = nullptr;
            selectedChip2 = nullptr;
            tryToSwap = false;
            return;
        }
        else
        {
            tryToSwap = true;
        }
    }

    const Chips &chips = *(gameModel->GetChips());
    const GridParams &gridParams = gameModel->GetParams();
    
    //rows
    for (int i = 0; i < gridParams.cellsInField.x; i++)
    {
        int matched = 0;
        Chip::eChipType chipType = Chip::eChipType::NONE;
        
        int idx = i * gridParams.cellsInField.x;
        for (int j = 0; j < gridParams.cellsInField.y; ++j)
        {
            Chip::eChipType newChipType = chips[idx]->GetType();
            if (matched == 0)
            {
                matched = 1;
                chipType = newChipType;
                idx++;
                continue;
            }

            if (newChipType == chipType)
            {
                matched++;
            }
            else
            {
                if (matched >= MATCH_COUNT)
                {
                    selectedChip1 = nullptr;
                    selectedChip2 = nullptr;
                    MarkChipsAsDestroyed(Vec2<int>(j - matched, i), Vec2<int>(j, i));
                    matched = 0;
                    chipType = Chip::eChipType::NONE;
                }
                else
                {
                    matched = 1;
                    chipType = newChipType;
                }
            }
            idx++;
        }

        if (matched >= MATCH_COUNT)
        {
            selectedChip1 = nullptr;
            selectedChip2 = nullptr;
            MarkChipsAsDestroyed(Vec2<int>(gridParams.cellsInField.x - matched, i), Vec2<int>(gridParams.cellsInField.x, i));
        }
    }

    for (int i = 0; i < gridParams.cellsInField.x; i++)
    {
        int matched = 0;
        Chip::eChipType chipType = Chip::eChipType::NONE;

        int idx = i;

        for (int j = 0; j < gridParams.cellsInField.y; j++, idx += gridParams.cellsInField.x)
        {
            Chip::eChipType newChipType = chips[idx]->GetType();
            if (matched == 0)
            {
                matched = 1;
                chipType = newChipType;
                continue;
            }

            if (newChipType == chipType)
            {
                matched++;
            }
            else
            {
                if (matched >= MATCH_COUNT)
                {
                    selectedChip1 = nullptr;
                    selectedChip2 = nullptr;
                    MarkChipsAsDestroyed(Vec2<int>(i, j - matched), Vec2<int>(i, j));
                    matched = 0;
                    chipType = Chip::eChipType::NONE;
                }
                else
                {
                    matched = 1;
                    chipType = newChipType;
                }
            }
        }

        if (matched >= MATCH_COUNT)
        {
            selectedChip1 = nullptr;
            selectedChip2 = nullptr;
            MarkChipsAsDestroyed(Vec2<int>(i, gridParams.cellsInField.y - matched), Vec2<int>(i, gridParams.cellsInField.y));
        }
    }

    if (chipsToDestroy.size() > 0)
    {
        DestroyChips();
    }
    else
    {
        if (selectedChip1 && selectedChip2)
        {
            SwapChips(GetSwapDirectionForSelected());
        }
    }
}

void GameController::MarkChipsAsDestroyed(Vec2<int> start, Vec2<int> end)
{
    state = eGameProcessState::ANIMATING;

    assert((abs(start.x - end.x) > 0 || abs(start.y - end.y) > 0));

    const GridParams &gridParams = gameModel->GetParams();
    const Chips &chips = *(gameModel->GetChips());

    int idx = start.y * gridParams.cellsInField.x + start.x;
    for (int i = 0; i < end.x - start.x; i++, idx++)
    {
        chips[idx]->SetChipState(Chip::eChipState::DESTROYED);
        chipsToDestroy.push_back(chips[idx].get());
    }

    idx = start.y * gridParams.cellsInField.x + start.x;
    for (int i = 0; i < end.y - start.y; i++, idx += gridParams.cellsInField.x)
    {
        chips[idx]->SetChipState(Chip::eChipState::DESTROYED);
        chipsToDestroy.push_back(chips[idx].get());
    }
}

void GameController::DestroyChips()
{
    gameView->DestroyChips(chipsToDestroy);
}
