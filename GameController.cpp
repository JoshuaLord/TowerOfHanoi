#include "GameController.h"

/**
\file GraphicsEngine.cpp
\brief Controls the Game Scene.

\author    Joshua Lord
\version   1.0
\date      Written: 5/19/2017  <BR> Revised: 5/22/2017

*/

/**
\brief Constructor for Game Controller Class
*/

GameController::GameController()
{
    moves = 0;
    scene[0] = true;
    isSelected = false;
    backMove = false;

    discModel = new glm::mat4[6];
    for (int i = 0; i < 6; i++)
        discModel[i] = glm::mat4(1.0);

    currentPole = 0;
    previousPole = 0;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 6; j++)
            poleContents[i][j] = 6;

    setNumDiscs(3);
    updateDiscs();
}


/**
\brief Returns current scene
*/

int GameController::getCurrentScene()
{
    for(int i = 0; i < 5; i++)
        if(scene[i] == true)
            return i;

    return 0;
}

/**
\brief Sets scene to s

\param s --- scene to be set to current
*/

void GameController::setScene(int s)
{
    for(int i = 0; i < 4; i++)
        if (i != s)
            scene[i] = false;

    scene[s] = true;
}

/**
\brief Returns Current Game Difficulty, True for Hard, False for Easy
*/

bool GameController::getHardMode()
{
    return hardMode;
}

/**
\brief Sets Game Difficulty

\param b --- true for hard, easy for false;
*/

void GameController::setHardMode(bool b)
{
    hardMode = b;
}

/**
\brief Returns current number of discs
*/

int GameController::getNumDiscs()
{
    return numDiscs;
}

/**
\brief Sets Number of Discs

\param i - number of discs to set
*/

void GameController::setNumDiscs(int i)
{
    numDiscs = i;

    for (int i = 0; i < numDiscs; i++)
    {
        poleContents[0][i] = i;
        poleContents[1][i] = 6;
        poleContents[2][i] = 6;
    }
}

/**
\brief Update Discs Positions
*/

void GameController::updateDiscs()
{
    if (numDiscs == numDiscsPerPole[2])
        setScene(4);

    countDiscs();

    int currDisc = 0;
    int xTranslate = 1;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < numDiscsPerPole[i]; j++)
            if (poleContents[i][j] != 6)
            {
                if (i == 0)
                    xTranslate = 1;
                else if (i == 1)
                    xTranslate = 0;
                else if (i == 2)
                    xTranslate = -1;

                discModel[currDisc] = glm::translate(glm::mat4(1.0), glm::vec3(0, 1 + j*1, 7.5 * xTranslate));
                if (numDiscs == 3)
                    discModel[currDisc] = glm::scale(discModel[currDisc], glm::vec3(1 - (poleContents[i][j]*.2), .5, 1 - (poleContents[i][j]*.2)));
                else
                    discModel[currDisc] = glm::scale(discModel[currDisc], glm::vec3(1 - (poleContents[i][j]*.1), .5, 1 - (poleContents[i][j]*.1)));
                discModel[currDisc] = glm::rotate(discModel[currDisc], 90 * degf, glm::vec3(1, 0, 0));
                currDisc++;
            }
}

/**
\brief Returns Models for Discs
*/

glm::mat4* GameController::getDiscModels()
{
    updateDiscs();
    return discModel;
}

/**
\brief Sets Current Pole to i

\param i --- Current Pole Selected.
*/

void GameController::setCurrentPole(int i)
{
    previousPole = currentPole;
    currentPole = i;
}

/**
\brief Returns Current Pole
*/

int GameController::getCurrentPole()
{
    return currentPole;
}

/**
\brief Returns Previous Pole
*/

int GameController::getPreviousPole()
{
    return previousPole;
}

/**
\brief Prints out a 4x4 Matrix
*/

void GameController::print_GLM_Matrix(glm::mat4 m)
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
            printf("%7.2f", m[c][r]);

        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
\brief Switches the value of isSelected
*/

void GameController::setIsSelected()
{
    isSelected = !isSelected;
}

/**
\brief Returns the value of isSelected
*/

bool GameController::getIsSelected()
{
    return isSelected;
}

/**
\brief Keeps Total Number of Discs on Each Pole
*/

void GameController::countDiscs()
{
    for (int i = 0; i < 3; i++)
    {
        numDiscsPerPole[i] = 0;
        for (int j = 0; j < 6; j++)
        {
            if (poleContents[i][j] != 6)
                numDiscsPerPole[i]++;
        }
    }
}

/**
\brief Finds Top of Pole Given

\param p --- Number of Pole to Search
*/

int GameController::top(int p)
{
    int i = 0, prev, top = 6;

    do
    {
        prev = top;
        top = poleContents[p][i];
        i++;

    }
    while (top != 6);

    return prev;
}

/**
\brief Moves a Disc

\param b --- Before Pole
\param a --- After Pole
*/

void GameController::moveDisc(int b, int a)
{
    int topB = top(b);
    int topA = top(a);

    if (topB == 6 || b == a || (topB < topA && topA != 6))
        return;

    poleContents[b][numDiscsPerPole[b] - 1] = 6;
    poleContents[a][numDiscsPerPole[a]] = topB;

    numDiscsPerPole[b]--;
    numDiscsPerPole[a]++;
    moves++;
}

/**
\brief Prints Out Current State of Game
*/

void GameController::printGame()
{
    cout << "Number of Discs: ";
    for (int i = 0; i < 3; i++)
        cout << "Pole " << i+1 << ": " << numDiscsPerPole[i] << ", ";
    cout << endl;

    cout << "Number of Discs Total: " << numDiscs << endl;

    cout << "Current Pole: " << currentPole << ". Previous Pole: " << previousPole << endl;

    cout << "Pole Contents" << endl;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 6; j++)
            cout << poleContents[i][j] << " ";
        cout << endl;
    }

    if(isSelected)
        cout << "A pole is selected" << endl;
    else
        cout << "A pole is not selected" << endl;

    cout << "Disc On Pole: ";
    for (int i = 0; i < 3; i++)
        cout << "Pole " << i+1 << ": " << disc(i) << ", ";
    cout << endl;
}

/**
\brief Resets the Game When The R Key Is Pressed
*/

void GameController::resetGame()
{
    moves = 0;
    isSelected = false;
    backMove = false;

    currentPole = 0;
    previousPole = 0;

    setNumDiscs(numDiscs);

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 6; j++)
            poleContents[i][j] = 6;

    for (int i = 0; i < numDiscs; i++)
        poleContents[0][i] = i;

    updateDiscs();
}

/**
\brief Sets value of back

\param b --- True/False, sets back
*/

void GameController::setBack(bool b)
{
    backMove = b;
}

/**
\brief Returns the value of back
*/

int GameController::getBack()
{
    return backMove;
}

/**
\brief Returns True if the Current Pole Has a Disc

\param i --- Pole to be Searched
*/

bool GameController::disc(int i)
{
    return !(poleContents[i][0] == 6);
}

/**
\brief Returns the number of the disc to be highlighted
*/

int GameController::getSelectedDisc()
{
    if (!isSelected) return -1;
    countDiscs();
    int i = numDiscsPerPole[0], j = numDiscsPerPole[1], k = numDiscsPerPole[2];

    switch(currentPole)
    {
    case 0:
        return i - 1;
    case 1:
        return i + j - 1;
    case 2:
        return i + j + k - 1;
    default:
        return -1;
    }
}

/**
\brief Decreases moves by 1 when the back button is pressed
*/

void GameController::decreaseMoves()
{
    moves--;
}

/**
\brief Returns current number of moves.
*/

int GameController::getMoves()
{
    return moves;
}













