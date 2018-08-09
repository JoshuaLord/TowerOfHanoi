#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>

#include "GraphicsEngine.h"
#include "UI.h"

/**
\mainpage Tower of Hanoi

\tableofcontents

\section intro Introduction

This is the game Tower of Hanoi. It is a simple 3D game where the user tries to
move the discs from the leftmost pole to the rightmost pole. The only rule is that
a larger disc may not be placed on a smaller disc. This was a project for the
2017 Spring COSC 482 class at Salisbury University.

\subsection options User Options

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

If the spherical camera is currently selected,

- Left: Increases the camera's theta value.
- Right: Decreases the camera's theta value.
- Up: Increases the camera's psi value.
- Down: Decreases the camera's psi value.
- Ctrl+Up: Decreases the camera's radius.
- Ctrl+Down: Increases the camera's radius.

If the spherical camera is currently selected, a click and drag with the left mouse
button will alter the theta and psi angles of the spherical camera to give the impression
of the mouse grabbing and moving the coordinate system.

\note Note that the shader programs "VertexShaderLightingTexture.glsl" and "PhongMultipleLightsAndTexture.glsl"
are expected to be in the same folder as the executable.  Your graphics card must also be
able to support OpenGL version 3.3 to run this program.


---

\subsection copyright Copyright

\author    Don Spickler
\version   1.2
\date      Written: 3/27/2016  <BR> Revised: 4/6/2016

\coauthor  Joshua Lord
\date      Revised: 5/22/2017

\copyright 2016


---

\subsection license License

GNU Public License

This software is provided as-is, without warranty of ANY KIND, either expressed or implied,
including but not limited to the implied warranties of merchant ability and/or fitness for a
particular purpose. The authors shall NOT be held liable for ANY damage to you, your computer,
or to anyone or anything else, that may result from its use, or misuse.
All trademarks and other registered names contained in this package are the property
of their respective owners.  USE OF THIS SOFTWARE INDICATES THAT YOU AGREE TO THE ABOVE CONDITIONS.

*/

/**
\file main.cpp
\brief Main driver for the program.

This is the main program driver that sets up the graphics engine, the user interface
class, and links the two.

\author    Don Spickler
\version   1.1
\date      Written: 2/28/2016  <BR> Revised: 2/28/2016

\coauthor  Joshua Lord
\date      Revised: 5/22/2017
*/


/**
\brief The Main function, program entry point.

\return Standard EXIT_SUCCESS return on successful run.

This is the main function, responsible for initializing GLEW and setting up
the SFML interface for OpenGL.

*/

int main()
{
    sf::RenderWindow d;
    if (glewInit())
    {
        std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string progName = "Tower Of Hanoi";
    GraphicsEngine ge(progName, 700, 500);
    UI ui(&ge);

    long framecount = 0;
    sf::Clock clock;
    sf::Time time = clock.restart();

    while (ge.isOpen())
    {
        ge.display();
        ui.processEvents();

        framecount++;
        float timesec = clock.getElapsedTime().asSeconds();
        char titlebar[128];
        if (timesec > 1.0)
        {
            float fps = framecount / timesec;
            sprintf(titlebar, "%s     FPS: %.2f", progName.c_str(), fps);
            ge.setTitle(titlebar);
            time = clock.restart();
            framecount = 0;
        }
    }

    return EXIT_SUCCESS;
}
