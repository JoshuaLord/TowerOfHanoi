#ifndef GRAPHICSENGINE_H_INCLUDED
#define GRAPHICSENGINE_H_INCLUDED

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cmath>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "LoadShaders.h"
#include "ProgramDefines.h"
#include "SphericalCamera.h"
#include "YPRCamera.h"
#include "Material.h"
#include "Light.h"
#include "MaterialPresets.h"
#include "Models.h"
#include "GameController.h"
#include "ObjModel.h"
#include "TextRendererTTF.h"
#include "Box.h"

/**
\file GraphicsEngine.h
\brief Header file for GraphicsEngine.cpp

\author    Don Spickler
\version   1.2
\date      Written: 4/5/2016  <BR> Revised: 4/6/2016

\coauthor  Joshua Lord
\date      Revised: 5/22/2017

*/

/**
\class GraphicsEngine

\brief The GraphicsEngine class is an extension of sf::RenderWindow which
handles all of the graphics rendering in the program.

*/

class GraphicsEngine : public sf::RenderWindow
{
private:
    GLenum mode;    ///< Mode, either point, line or fill.
    int sscount;    ///< Screenshot count to be appended to the screenshot filename.

    GLuint ProjLoc;        ///< Location ID of the Projection Matrix in the text shader.
    GLuint ModelLoc;       ///< Location ID of the Model matrix in the shader.
    GLuint NormalLoc;      ///< Location ID of the Normal matrix in the shader.
    GLuint PVMLoc;         ///< Location ID of the PVM matrix in the shader.
    GLuint texTransLoc;    ///< Location ID of the texture transformation matrix in the shader.
    GLuint program;        ///< ID of the shader program.
    GLuint textProgram;    ///< ID of the text shader program.

    SphericalCamera sphcamera;   ///< Spherical Camera
    YPRCamera yprcamera;         ///< Yaw-Pitch-Roll Camera
    int CameraNumber;            ///< Camera number 1 = spherical, 2 = yaw-pitch-roll.
    SphericalCamera LtPos[10];   ///< Spherical "Camera" to control position of the light.

    Material mat;     ///< Material of the current object.
    Light lt[10];     ///< Light object.
    GLuint texID[6];  ///< Texture IDs.

    Models poleBase[3];  ///< Model Object for Pole Base.
    Models poleStaff[3]; ///< Model Object for Pole Staff.
    Models discs[6];     ///< Model Object for Discs.
    Models lightobj;  ///< Used for sphere at light source.
    Models floor;     ///< Model Object for Floor.
    Models *disc;     ///< Model Object for Discs.
    Models start;     ///< Model Object for the Start button.
    Models numDiscsButton[3];   ///< Model Objects for the Num of Discs buttons.
    Models menuBack;            ///< Model Object for the Menu Back button.
    Models diffButton[2];       ///< Model Objects for the Difficulty buttons.
    Models resetButton;         ///< Model Object for the Reset Button.
    Models gameBack;            ///< Model Object for the Game Back button.
    Models menuButton;          ///< Model Object for the Main Menu button.

    TextRendererTTF text;            ///< Text for Start Screen, "Tower of Hanoi" "Start" "Joshua Lord"
                                     ///< Text for Disc Selection Screen, "# of Discs" "3" "4" "5" "Back"
                                     ///< Text for Difficulty Selection Screen, "Difficulty" "Easy" "Hard" "Easy Desc" "Hard Desc"
                                     ///< Text for Game Scene, "Back"

    glm::mat4 projection;  ///< Projection Matrix
    glm::mat4 view;        ///< View Matrix.
    glm::mat4 model;       ///< Model Matrix.

    glm::mat4 floorModel;   ///< Model Matrix for Floor.
    glm::mat4 floorTexTrans; ///< Texture Transformation Matrix for Floor.

    glm::mat4 discTexTrans; ///< Texture Transformation Matrix for Discs.

    glm::mat4 poleBaseModel[3]; ///< Model Matrix for Pole Base.
    glm::mat4 poleStaffModel[3]; ///< Model Matrix for Pole Staff.

    glm::mat4* discModel;     ///< Model Matrix for Discs.

    GameController *gc;

    void drawStartScene();
    void drawDiscSelectionScene();
    void drawDiffSelectionScene();
    void drawGameScene();
    void drawWinnerScene();
    void drawText();
    void drawPoles();

    void screenshot(std::string ext);
    void printOpenGLErrors();
    void print_GLM_Matrix(glm::mat4 m);
    void print_GLM_Matrix(glm::mat3 m);

public:
    GraphicsEngine(std::string, GLint, GLint);
    ~GraphicsEngine();

    void display();
    void changeMode();
    void screenshotPNG();
    void screenshotJPG();
    void resize();
    void setSize(unsigned int, unsigned int);
    GLfloat* getScreenBounds();

    GLboolean isSphericalCameraOn();
    void setSphericalCameraOn();
    GLboolean isYPRCameraOn();
    void setYPRCameraOn();

    void loadLight(Light Lt);
    void loadMaterial(Material Mat);

    void turnLightOn();
    void turnLightOff();
    void turnLightOn(std::string name, int i);
    void turnLightOff(std::string name, int i);
    void turnLightsOn(std::string name, int num);
    void turnLightsOff(std::string name, int num);

    void turnTexturesOff(std::string name, int num);
    void turnTexturesOn(std::string name, int num);
    void turnTextureOff(std::string name, int i);
    void turnTextureOn(std::string name, int i);

    void LoadLights(Light Lt[], std::string name, int num);
    void LoadLight(Light Lt, std::string name, int i);

    void setSphericalCameraPos(float, float, float);

    SphericalCamera* getLtPos();
    SphericalCamera* getSphericalCamera();
    YPRCamera* getYPRCamera();
    GameController* getGameController();
};

#endif // GRAPHICSENGINE_H_INCLUDED
