#include "UI.h"
#include <iostream>
using namespace std;

/**
\file UI.cpp
\brief User interface processor for the program.

\author    Don Spickler
\version   1.1
\date      Written: 3/22/2016  <BR> Revised: 3/22/2016

\coauthor  Joshua Lord
\date      Revised: 5/22/2017

*/

/**
\brief Constructor

\param graph --- Pointer to the GraphicsEngine that this interface processor is attached.

Simply stores the pointer of the GraphicsEngine.

*/

UI::UI(GraphicsEngine* graph)
{
    ge = graph;
    gc = ge->getGameController();

    mouseDown = false;
}

/**
\brief Destructor

No implementation needed at this point.

*/

UI::~UI() {}

/**
\brief The method handles the SFML event processing and calls the keyboard state processor
method.

This method processes all events in the current SFML event queue and calls the
corresponding processing method.  At the end it calls the keyboard state processor
method, outside the event loop.

*/

void UI::processEvents()
{
    // Process user events
    sf::Event event;
    while (ge->pollEvent(event))
    {
        // Close Window or Escape Key Pressed: exit
        if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            ge->close();

        // Key is pressed.
        if (event.type == sf::Event::KeyPressed)
            keyPressed(event.key);

        // Window is resized.
        if (event.type == sf::Event::Resized)
            ge->resize();

        if (event.type == sf::Event::MouseMoved)
            processMouseMoved(event.mouseMove);

        if (event.type == sf::Event::MouseButtonPressed)
            processMouseButtonPressed(event.mouseButton);

        if (event.type == sf::Event::MouseButtonReleased)
            processMouseButtonReleased(event.mouseButton);

    }

    // Process the state of the keyboard outside of event firing,
    keyboardStateProcessing();

}

/**
\brief The method updates the theta and psi values of the spherical camera
on a click and drag.  If the control key is down the vertical movement will
alter the radius of the camera.

\param mouseMoveEvent --- The SFML mouse move event structure.

*/

void UI::processMouseMoved(sf::Event::MouseMoveEvent mouseMoveEvent)
{
    bool ctrldown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);

    if ((gc->getCurrentScene()) == 3)
        if (ge->isSphericalCameraOn() && mouseDown)
        {
            if (ctrldown)
            {
                ge->getSphericalCamera()->addR((LastPosition.y - mouseMoveEvent.y)*0.25);

                if(ge->getSphericalCamera()->getR() < 5)
                    ge->getSphericalCamera()->setR(5);
                else if(ge->getSphericalCamera()->getR() > 60)
                    ge->getSphericalCamera()->setR(60);
            }
            else
            {
                ge->getSphericalCamera()->addTheta((mouseMoveEvent.x - LastPosition.x)*degf*10);
                ge->getSphericalCamera()->addPsi((mouseMoveEvent.y - LastPosition.y)*degf*10);

                if(ge->getSphericalCamera()->getPsi() < 5)
                    ge->getSphericalCamera()->setPsi(5);

            }

            LastPosition.x = mouseMoveEvent.x;
            LastPosition.y = mouseMoveEvent.y;
        }
}

/**
\brief On a left mouse click it will track the mouse down and tag the current position
of the mouse as the last position the mouse was at.

\param mouseButtonEvent --- The SFML mouse button event structure.

*/

void UI::processMouseButtonPressed(sf::Event::MouseButtonEvent mouseButtonEvent)
{
    if (mouseButtonEvent.button == sf::Mouse::Left)
    {
        mouseDown = true;
        LastPosition.x = mouseButtonEvent.x;
        LastPosition.y = mouseButtonEvent.y;
    }
}

/**
\brief If the left mouse button is released this method will track the release and
exit any drag movement.

\param mouseButtonEvent --- The SFML mouse button event structure.

*/

void UI::processMouseButtonReleased(sf::Event::MouseButtonEvent mouseButtonEvent)
{
    if (mouseButtonEvent.button == sf::Mouse::Left)
    {
        mouseDown = false;
    }
}

/**
\brief The function handles the keyboard input events from the user.

\param keyevent --- The SFML key code for the key pressed.

\remark

- A: Resets the window size to 700 X 500.
- F9: Saves a screen shot of the graphics window to a png file.
- F10: Saves a screen shot of the graphics window to a jpeg file.
- F11: Turns on the spherical camera.

Certain Situations

- M: Main Menu
- C: Cancels Current Highlighted Disc
- R: Resets the Game Board
- B: Back Button

- 1: Highlights the First Pole's Top Disc
- 2: Highlights the Second Pole's Top Disc
- 3: Highlights the Third Pole's Top Disc

*/

void UI::keyPressed(sf::Event::KeyEvent keyevent)
{
    int key = keyevent.code;

    int currentScene = gc->getCurrentScene();

    switch (key)
    {
    case sf::Keyboard::F9:
        ge->screenshotPNG();
        break;

    case sf::Keyboard::F10:
        ge->screenshotJPG();
        break;

    case sf::Keyboard::F11:
        ge->setSphericalCameraOn();
        break;

    case sf::Keyboard::A:
        ge->setSize(700, 500);
        break;

    default:
        break;
    }

    if (currentScene == 0)
        switch (key)
        {
        case sf::Keyboard::S:
            gc->setScene(1);
            break;
        }

    if (currentScene == 1)
        switch (key)
        {
        case sf::Keyboard::B:
            gc->setScene(0);
            break;
        case sf::Keyboard::Num3:
            gc->setNumDiscs(3);
            gc->setScene(2);
            break;
        case sf::Keyboard::Num4:
            gc->setNumDiscs(4);
            gc->setScene(2);
            break;
        case sf::Keyboard::Num6:
            gc->setNumDiscs(6);
            gc->setScene(2);
            break;
        }

    if (currentScene == 2)
        switch (key)
        {
        case sf::Keyboard::B:
            gc->setScene(1);
            break;
        case sf::Keyboard::H:
            gc->setHardMode(true);
            ge->setSphericalCameraPos(40, 0, 10);
            gc->setScene(3);
            break;
        case sf::Keyboard::E:
            gc->setHardMode(false);
            ge->setSphericalCameraPos(40, 0, 10);
            gc->setScene(3);
            break;
        }

    bool sel = gc->getIsSelected(); // if true and a number is pressed: indicates a move

    if (currentScene == 3)
    {
        switch (key)
        {
        case sf::Keyboard::B:
            if(!(gc->getHardMode()) && gc->getBack())
            {
                gc->moveDisc(gc->getCurrentPole(), gc->getPreviousPole());
                gc->setBack(false);
                gc->decreaseMoves();
            }
            break;
        case sf::Keyboard::R:
            gc->resetGame();
            break;
        case sf::Keyboard::C:
            if(gc->getIsSelected())
                gc->setIsSelected();
            break;
        case sf::Keyboard::M:
            gc->resetGame();
            ge->setSphericalCameraPos(40, 0, 0);
            gc->setScene(0);
            break;

        case sf::Keyboard::Num1:
            if((!sel))
            {
                if(gc->disc(0))
                {
                    gc->setCurrentPole(0);
                    gc->setIsSelected();
                    break;
                }
            }
            else
            {
                gc->setCurrentPole(0);
                gc->moveDisc(gc->getPreviousPole(), 0);
                gc->setIsSelected();
                gc->setBack(true);
            }
            break;

        case sf::Keyboard::Num2:
            if((!sel))
            {
                if(gc->disc(1))
                {
                    gc->setCurrentPole(1);
                    gc->setIsSelected();
                    break;
                }
            }
            else
            {
                gc->setCurrentPole(1);
                gc->moveDisc(gc->getPreviousPole(), 1);
                gc->setIsSelected();
                gc->setBack(true);
            }
            break;

        case sf::Keyboard::Num3:
            if((!sel))
            {
                if(gc->disc(2))
                {
                    gc->setCurrentPole(2);
                    gc->setIsSelected();
                    break;
                }
            }
            else
            {
                gc->setCurrentPole(2);
                gc->moveDisc(gc->getPreviousPole(), 2);
                gc->setIsSelected();
                gc->setBack(true);
            }

            break;
        }
    }

    if (currentScene == 4)
    {
        ge->setSphericalCameraPos(40, 0, 0);
        ge->turnLightsOff("Lt", 3);

        switch (key)
        {
        case sf::Keyboard::M:
            gc->resetGame();
            gc->setScene(0);
            gc->resetGame();
            ge->setSphericalCameraPos(40, 0, 0);
            ge->turnLightsOn("Lt", 3);
            break;
        }
    }

}

/**
\brief Calls the respective method for key processing depending on
which camera, spherical or yaw-pitch-roll, is currently selected.

*/

void UI::keyboardStateProcessing()
{
    if (ge->isSphericalCameraOn())
        keyboardStateProcessingSphericalCamera();
    else
        keyboardStateProcessingYPRCamera();

}

/**
\brief The method processes the keyboard state if the spherical camera is the one currently
being used.

\remark

If no modifier keys are pressed:

- Left: Increases the camera's theta value.
- Right: Decreases the camera's theta value.
- Up: Increases the camera's psi value.
- Down: Decreases the camera's psi value.

If the control key is down:

- Up: Decreases the camera's radius.
- Down: Increases the camera's radius.

*/

void UI::keyboardStateProcessingSphericalCamera()
{
    bool ctrldown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    bool altdown = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

    if (altdown)
        return;

    if (gc->getCurrentScene() == 3)
    {
        if (ctrldown)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && ge->getSphericalCamera()->getR() > 5)
                ge->getSphericalCamera()->addR(-0.1);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && ge->getSphericalCamera()->getR() < 60)
                ge->getSphericalCamera()->addR(0.1);
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                ge->getSphericalCamera()->addTheta(1);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                ge->getSphericalCamera()->addTheta(-1);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                ge->getSphericalCamera()->addPsi(1);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && ge->getSphericalCamera()->getPsi() > 1)
                ge->getSphericalCamera()->addPsi(-1);
        }
    }
}

/**
\brief The method processes the keyboard state if the yaw-pitch-roll camera is the
one currently being used.

\remark

If no modifier keys are pressed:

- Left: Increases the yaw.
- Right: Decreases the yaw.
- Up: Increases the pitch.
- Down: Decreases the pitch.

If the control key is down:

- Left: Increases the roll.
- Right: Decreases the roll.
- Up: Moves the camera forward.
- Down: Moves the camera backward.

If the shift key is down:

- Left: Moves the camera left.
- Right: Moves the camera right.
- Up: Moves the camera up.
- Down: Moves the camera down.

*/

void UI::keyboardStateProcessingYPRCamera()
{
    bool ctrldown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    bool shiftdown = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    bool altdown = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

    if (altdown)
        return;

    if (shiftdown)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            ge->getYPRCamera()->moveRight(-0.1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            ge->getYPRCamera()->moveRight(0.1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            ge->getYPRCamera()->moveUp(0.1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            ge->getYPRCamera()->moveUp(-0.1);
    }
    else if (ctrldown)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            ge->getYPRCamera()->addRoll(1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            ge->getYPRCamera()->addRoll(-1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            ge->getYPRCamera()->moveForward(0.1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            ge->getYPRCamera()->moveForward(-0.1);
    }
    else
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            ge->getYPRCamera()->addYaw(1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            ge->getYPRCamera()->addYaw(-1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            ge->getYPRCamera()->addPitch(1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            ge->getYPRCamera()->addPitch(-1);
    }
}
