#ifndef GameController_h
#define GameController_h

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <stdio.h>

#include "ProgramDefines.h"

using namespace std;

/**
\file GameController.h
\brief Header file for GameController.cpp

\author    Joshua Lord
\version   1.0
\date      Written: 5/19/2017  <BR> Revised: 5/22/2017

*/

/**
\class GameController

\brief The Game Controller class is a class that manages the current state of the game and
validates or rejects a user's move, controls the locations of the discs, and decides when the game
is over.

*/

class GameController
{
private:
    int numDiscsPerPole[3]; ///< Number of Discs on Each Pole
    int numDiscs;   ///< 3, 4, 6
    int moves;      ///< Number of Moves the User has made
    bool hardMode;  ///< false for Easy, true for Hard
    bool backMove;      ///< True if Back Move Available
    bool scene[4];  ///< If true, draws the scene
    int currentPole; ///< Currently Selected Pole
    int previousPole; ///< Previously Selected Pole
    int poleContents[3][6]; ///< Holds information about the location of the discs.
    bool isSelected; ///< True if a pole's top disc is currently selected.

    glm::mat4 *discModel; ///< Model Matrices for Discs

    void countDiscs();

public:
    GameController();

    void print_GLM_Matrix(glm::mat4 m);

    int getCurrentScene();
    void setScene(int);
    bool getHardMode();
    void setHardMode(bool);
    int getNumDiscs();
    void setNumDiscs(int);
    void setCurrentPole(int);
    int getCurrentPole();
    int getPreviousPole();
    void setIsSelected();
    bool getIsSelected();
    void setBack(bool);
    int getBack();

    glm::mat4* getDiscModels();
    int top(int);
    bool disc(int);
    int getSelectedDisc();
    void decreaseMoves();
    int getMoves();

    void moveDisc(int, int);
    void updateDiscs();

    void printGame();
    void resetGame();

};

#endif // GameController_h
