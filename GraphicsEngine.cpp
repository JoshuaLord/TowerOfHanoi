#include "GraphicsEngine.h"

using namespace std;
#include <iostream>

/**
\file GraphicsEngine.cpp
\brief Main graphics driver for the program.

This class extends sf::RenderWindow.

\author    Don Spickler
\version   1.2
\date      Written: 4/5/2016  <BR> Revised: 4/6/2016

\coauthor  Joshua Lord
\date      Revised: 5/22/2017

*/

/**
\brief Constructor

\param title --- Title to be placed in the titlebar of the graphics window.

\param width --- The width (in pixels) of the graphics window.

\param height --- The height (in pixels) of the graphics window.

Creates rendering window, loads the shaders, and sets some initial data settings.

*/

GraphicsEngine::GraphicsEngine(std::string title, GLint width, GLint height) :
    sf::RenderWindow(sf::VideoMode(width, height),
                     title,
                     sf::Style::Default,
                     sf::ContextSettings(24, 8, 4, 3, 3))
{
    //  Load the shaders
    program = LoadShadersFromFile("VertexShaderLightingTexture.glsl", "PhongMultipleLightsAndTexture.glsl");

    if (!program)
    {
        std::cerr << "Could not load Shader programs." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Turn on the shader & get location of transformation matrix.
    glUseProgram(program);
    PVMLoc = glGetUniformLocation(program, "PVM");
    ModelLoc = glGetUniformLocation(program, "Model");
    NormalLoc = glGetUniformLocation(program, "NormalMatrix");
    texTransLoc = glGetUniformLocation(program, "textrans");

    // Initialize some data.
    mode = GL_FILL;
    sscount = 1;
    CameraNumber = 1;

    // Set up the Text
    text.loadFont("fonts/times.ttf");
    text.setColor(1, 1, 1, 1);

    // Allocate Memory for GameController
    gc = new GameController;

    // Set position of spherical camera
    sphcamera.setPosition(40, 0, 0);

    glEnable(GL_DEPTH_TEST);

    if (SetVS)
    {
        setVerticalSyncEnabled(true);
        setFramerateLimit(60);
    }
    else
    {
        setVerticalSyncEnabled(false);
        setFramerateLimit(0);
    }

    for (int i = 0; i < 3; i++)
    {
        lt[i].setLight(true,
                       30.0, 30.0, 30.0, 1.0,
                       -1.0, -1.0, -1.0,
                       0.0, 0.0, 0.0, 1.0,
                       0.70, 0.70, 0.70, 1.0,
                       0.70, 0.70, 0.70, 1.0,
                       180.0, 0.0,
                       1.0, 0.0, 0.0
                      );
    }

    LoadLights(lt, "Lt", 3);
    loadMaterial(mat);
    glUniform1i(glGetUniformLocation(program, "numLights"), 3);

    glm::vec4 GlobalAmbient(0.0, 0.0, 0.0, 1);
    glUniform4fv(glGetUniformLocation(program, "GlobalAmbient"), 1, glm::value_ptr(GlobalAmbient));

    // Set up Discs

    for (int i = 0; i < 6; i++)
    {
        discs[i].createTorusOBJ(1, 3, 20, 120);
        discs[i].load(0, 1, 2, 3); // z, -x, -y
    }

    for (int i = 0; i < 3; i++)
    {
        poleBase[i].createSphereOBJ(3, 20, 20);
        poleBase[i].load(0, 1, 2, 3);

        poleBaseModel[i] = glm::scale(glm::mat4(1.0), glm::vec3(1, 0.3, 1));
    }

    poleBaseModel[0] = glm::translate(poleBaseModel[0], glm::vec3(0, 0, 7.5));
    poleBaseModel[0] = glm::rotate(poleBaseModel[0], 120 * degf, glm::vec3(0, 1, 0));
    poleBaseModel[2] = glm::translate(poleBaseModel[2], glm::vec3(0, 0, -7.5));
    poleBaseModel[2] = glm::rotate(poleBaseModel[2], 240 * degf, glm::vec3(0, 1, 0));

    for (int i = 0; i < 3; i++)
    {
        poleStaff[i].createSphereOBJ(1, 20, 20);
        poleStaff[i].load(0, 1, 2, 3);

        poleStaffModel[i] = glm::scale(glm::mat4(1.0), glm::vec3(0.3, 5, 0.3));
    }

    poleStaffModel[0] = glm::translate(poleStaffModel[0], glm::vec3(0, .7, 25));
    poleStaffModel[0] = glm::rotate(poleStaffModel[0], 120 * degf, glm::vec3(0, 1, 0));
    poleStaffModel[1] = glm::translate(poleStaffModel[1], glm::vec3(0, .7, 0));
    poleStaffModel[2] = glm::translate(poleStaffModel[2], glm::vec3(0, .7, -25));
    poleStaffModel[2] = glm::rotate(poleStaffModel[2], 240 * degf, glm::vec3(0, 1, 0));

    lightobj.createSphereOBJ(0.25, 7, 7);
    lightobj.load(0, 1, 2, 3);
    lightobj.setColor(1, 1, 0);
    lightobj.setDrawBorder(GL_TRUE);

    start.createTessellatedWallOBJ(20, 8, 1, 1);
    start.load(0, 1, 2, 3);

    for (int i = 0; i < 3; i++)
    {
        numDiscsButton[i].createTessellatedWallOBJ(5, 5, 1, 1);
        numDiscsButton[i].load(0, 1, 2, 3);
    }

    for (int i = 0; i < 2; i++)
    {
        diffButton[i].createTessellatedWallOBJ(15, 5, 1, 1);
        diffButton[i].load(0, 1, 2, 3);
    }

    menuBack.createTessellatedWallOBJ(9, 3, 1, 1);
    menuBack.load(0, 1, 2, 3);

    menuButton.createTessellatedWallOBJ(3, 1.5, 1, 1);
    menuButton.load(0, 1, 2, 3);

    resetButton.createTessellatedWallOBJ(3, 1.5, 1, 1);
    resetButton.load(0, 1, 2, 3);

    gameBack.createTessellatedWallOBJ(3, 1.5, 1, 1);
    gameBack.load(0, 1, 2, 3);

    floor.createTessellatedWallOBJ(2, 2, 10, 10);
    floor.load(0, 1, 2, 3);

    for (int i = 0; i < 3; i++)
    {
        LtPos[i].setTheta(i*120 + 60);
        LtPos[i].setPsi(5);
        LtPos[i].setR(20);
    }

    // Setup Floor Model and Floor Texture Model
    floorModel = glm::rotate(glm::mat4(1.0), -90*degf, glm::vec3(1, 0, 0));
    floorModel = glm::scale(floorModel, glm::vec3(100, 100, 1));

    floorTexTrans = glm::scale(glm::mat4(1.0), glm::vec3(40, 40, 1));

    // Setup Disc Texture Model
    discTexTrans = glm::scale(glm::mat4(1.0), glm::vec3(1, 1, 1));

    sf::Image texture;
    std::string filename;

    glGenTextures(6, texID);

    for (int i = 0; i < 6; i++)
    {
        if (i == 0)
            filename = "Textures/Starfield002.jpg";
        else if (i == 1)
            filename = "Textures/Repeat-brick.jpg";
        else if (i == 2)
            filename = "Textures/amazaque.bmp";
        else if (i == 3)
            filename = "Textures/Metal.jpg";
        else if (i == 4)
            filename = "Textures/Lightwood.jpg";
        else if (i == 5)
            filename = "Textures/Hardwood.jpg";

        if (!texture.loadFromFile(filename))
            std::cerr << "Could not load texture: " << filename << std::endl;

        char arrayname[10];
        sprintf(arrayname, "tex[%d]", i);

        //  Link the texture to the shader.
        GLuint tex1_uniform_loc = glGetUniformLocation(program, arrayname);
        glUniform1i(tex1_uniform_loc, i);

        //  Load the texture into texture memory.
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, texID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.getSize().x, texture.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Make it the active window for OpenGL calls, resize to set projection matrix.
    setActive();
    resize();
}


/**
\brief Destructor

Currently empty, no allocated memory to clear.

*/

GraphicsEngine::~GraphicsEngine() {}

/**
\brief The function responsible for drawing to the OpenGL frame buffer.

This function clears the screen and calls the draw functions of the box and circle.

*/

void GraphicsEngine::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    for (int i = 0; i < 3; i++)
    {
        lt[i].setPosition(glm::vec4(LtPos[i].getPosition(), 1.0));
        lt[i].setSpotDirection(-LtPos[i].getPosition());
    }

    LoadLights(lt, "Lt", 3);

    // Set view matrix via current camera.
    view = glm::mat4(1.0);
    if (CameraNumber == 1)
        view = sphcamera.lookAt();
    else if (CameraNumber == 2)
        view = yprcamera.lookAt();

    glm::vec3 eye;
    if (CameraNumber == 1)
        eye = sphcamera.getPosition();
    else if (CameraNumber == 2)
        eye = yprcamera.getPosition();

    glUniform3fv(glGetUniformLocation(program, "eye"), 1, glm::value_ptr(eye));

    turnLightsOff("Lt", 3);
    turnTexturesOff("useTexture", 6);

    turnLightsOn("Lt", 3);

    if (gc->getCurrentScene() == 3)
    {
        discModel = gc->getDiscModels();
        int hDisc = gc->getSelectedDisc();

        turnTextureOn("useTexture", 3);
        for (int i = 0; i < gc->getNumDiscs(); i++)
        {
            glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*discModel[i]));
            glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(discModel[i]));

            glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(discTexTrans));

            glm::mat3 mN(discModel[i]);
            glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
            glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

            if (i == hDisc)
            {
                mat = Materials::redPlastic;
                loadMaterial(mat);
            }
            else
            {
                mat = Materials::silver;
                loadMaterial(mat);
            }

            discs[i].draw();
            turnTextureOff("useTexture", 3);
        }

    }

    switch (gc->getCurrentScene())
    {
    case 0:
        drawStartScene();
        break;
    case 1:
        drawDiscSelectionScene();
        break;
    case 2:
        drawDiffSelectionScene();
        break;
    case 3:
        drawGameScene();
        drawPoles();
        break;
    }

    drawText();

    turnLightsOff("Lt", 3);
    turnTexturesOff("useTexture", 6);

    if (gc->getCurrentScene() == 3)
        for (int i = 0; i < 3; i++)
        {
            glm::vec3 ltpos;
            ltpos.x = lt[i].getPosition().x;
            ltpos.y = lt[i].getPosition().y;
            ltpos.z = lt[i].getPosition().z;

            glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*(glm::translate(glm::mat4(1.0), ltpos))));
            lightobj.draw();
        }

    sf::RenderWindow::display();
    printOpenGLErrors();
}

/**
\brief Changes the fill and line mode being used.

*/

void GraphicsEngine::changeMode()
{
    if (mode == GL_FILL)
        mode = GL_LINE;
    else
        mode = GL_FILL;

    glPolygonMode(GL_FRONT_AND_BACK, mode);
}

/**
\brief Saves a screenshot of the current display to a file, ScreenShot###.ext.

\param ext --- String holding the extension of the filename, and hence the image
data type.  The extension can be any extension of an SFML supported data type for
the saveToFile method of the sf;:Image class.

*/

void GraphicsEngine::screenshot(std::string ext)
{
    char ssfilename[100];
    sprintf(ssfilename, "ScreenShot%d.%s", sscount, ext.c_str());
    sf::Image img = capture();
    img.saveToFile(ssfilename);
    sscount++;
}

/**
\brief Saves a screenshot of the current display to a file, ScreenShot###.jpg.

*/

void GraphicsEngine::screenshotJPG()
{
    screenshot("jpg");
}

/**
\brief Saves a screenshot of the current display to a file, ScreenShot###.png.

*/

void GraphicsEngine::screenshotPNG()
{
    screenshot("png");
}

/**
\brief Handles the resizing events of the window.

Sets the viewport to the entire screen and recalculates the projection matrix.
*/

void GraphicsEngine::resize()
{
    glViewport(0, 0, getSize().x, getSize().y);
    projection = glm::perspective(50.0f*degf, (float)getSize().x/getSize().y, 0.01f, 500.0f);
}

/**
\brief Sets the size of the rendering window.

\param width --- the width in pixels of the new size.

\param height --- the height in pixels of the new size.

*/

void GraphicsEngine::setSize(unsigned int width, unsigned int height)
{
    sf::RenderWindow::setSize(sf::Vector2u(width, height));
}

/**
\brief Returns a pointer to the spherical camera.

*/

SphericalCamera* GraphicsEngine::getSphericalCamera()
{
    return &sphcamera;
}

SphericalCamera* GraphicsEngine::getLtPos()
{
    return &LtPos[0];
}

/**
\brief Returns a pointer to the yaw-pitch-roll camera.

*/

YPRCamera* GraphicsEngine::getYPRCamera()
{
    return &yprcamera;
}

/**
\brief Prints all OpenGL errors to stderr.

*/

GameController* GraphicsEngine::getGameController()
{
    return gc;
}

void GraphicsEngine::printOpenGLErrors()
{
    GLenum errCode;
    const GLubyte *errString;

    while ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        fprintf(stderr, "OpenGL Error: %s\n", errString);
    }
}

/**
\brief Prints the glm matrix to the console window.  Remember that glm
matrices are column major. This is for the 3 X 3 matrices.

\param m --- the glm matrix to be displayed.

*/

void GraphicsEngine::print_GLM_Matrix(glm::mat4 m)
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
\brief Prints the glm matrix to the console window.  Remember that glm
matrices are column major. This is for the 3 X 3 matrices.

\param m --- the glm matrix to be displayed.

*/

void GraphicsEngine::print_GLM_Matrix(glm::mat3 m)
{
    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
            printf("%7.2f", m[c][r]);

        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
\brief Returns true if the spherical camera is currently in use.

\return True if the spherical camera is currently in use and false otherwise.

*/

GLboolean GraphicsEngine::isSphericalCameraOn()
{
    return CameraNumber == 1;
}

/**
\brief Turns the spherical camera on.

*/

void GraphicsEngine::setSphericalCameraOn()
{
    CameraNumber = 1;
}

/**
\brief Returns true if the yaw-pitch-roll camera is currently in use.

\return True if the yaw-pitch-roll camera is currently in use and false otherwise.

*/

GLboolean GraphicsEngine::isYPRCameraOn()
{
    return CameraNumber == 2;
}

/**
\brief Turns the YPR camera on.

*/

void GraphicsEngine::setYPRCameraOn()
{
    CameraNumber = 2;
}

/**
\brief Turns the light on

*/

void GraphicsEngine::turnLightOn()
{
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "Lt.on"), true);
}

/**
\brief Turns the light off

*/

void GraphicsEngine::turnLightOff()
{
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "Lt.on"), false);
}

/**
\brief Loads the light structure to the shader light structure.

\param Lt --- Light structure to load.

*/

void GraphicsEngine::loadLight(Light Lt)
{
    glUseProgram(program);

    glUniform1i(glGetUniformLocation(program, "Lt.on"), Lt.getOn());
    glUniform4fv(glGetUniformLocation(program, "Lt.position"), 1, glm::value_ptr(Lt.getPosition()));
    glUniform4fv(glGetUniformLocation(program, "Lt.ambient"), 1, glm::value_ptr(Lt.getAmbient()));
    glUniform4fv(glGetUniformLocation(program, "Lt.diffuse"), 1, glm::value_ptr(Lt.getDiffuse()));
    glUniform4fv(glGetUniformLocation(program, "Lt.specular"), 1, glm::value_ptr(Lt.getSpecular()));
    glUniform3fv(glGetUniformLocation(program, "Lt.spotDirection"), 1, glm::value_ptr(Lt.getSpotDirection()));
    glUniform3fv(glGetUniformLocation(program, "Lt.attenuation"), 1, glm::value_ptr(Lt.getAttenuation()));
    glUniform1f(glGetUniformLocation(program, "Lt.spotCutoff"), Lt.getSpotCutoff());
    glUniform1f(glGetUniformLocation(program, "Lt.spotExponent"), Lt.getSpotExponent());
}

/**
\brief Loads the material structure to the shader material structure.

\param Mat --- Material structure to load.

*/

void GraphicsEngine::loadMaterial(Material Mat)
{
    glUseProgram(program);

    glUniform4fv(glGetUniformLocation(program, "Mat.ambient"), 1, glm::value_ptr(Mat.getAmbient()));
    glUniform4fv(glGetUniformLocation(program, "Mat.diffuse"), 1, glm::value_ptr(Mat.getDiffuse()));
    glUniform4fv(glGetUniformLocation(program, "Mat.specular"), 1, glm::value_ptr(Mat.getSpecular()));
    glUniform4fv(glGetUniformLocation(program, "Mat.emission"), 1, glm::value_ptr(Mat.getEmission()));
    glUniform1f(glGetUniformLocation(program, "Mat.shininess"), Mat.getShininess());
}

/**
\brief Loads a single light into a light array in the shader.

\param Lt --- Light to load.

\param name --- The name of the array in the shader.

\param i --- The index of the light to load.

*/

void GraphicsEngine::LoadLight(Light Lt, std::string name, int i)
{
    glUseProgram(program);

    const char* arrayname = name.c_str();  // array name in the shader.
    char locID[100];
    sprintf(locID, "%s[%d].%s", arrayname, i, "on");
    glUniform1i(glGetUniformLocation(program, locID), Lt.getOn());

    sprintf(locID, "%s[%d].%s", arrayname, i, "position");
    glUniform4fv(glGetUniformLocation(program, locID), 1, glm::value_ptr(Lt.getPosition()));

    sprintf(locID, "%s[%d].%s", arrayname, i, "ambient");
    glUniform4fv(glGetUniformLocation(program, locID), 1, glm::value_ptr(Lt.getAmbient()));

    sprintf(locID, "%s[%d].%s", arrayname, i, "diffuse");
    glUniform4fv(glGetUniformLocation(program, locID), 1, glm::value_ptr(Lt.getDiffuse()));

    sprintf(locID, "%s[%d].%s", arrayname, i, "specular");
    glUniform4fv(glGetUniformLocation(program, locID), 1, glm::value_ptr(Lt.getSpecular()));

    sprintf(locID, "%s[%d].%s", arrayname, i, "spotDirection");
    glUniform3fv(glGetUniformLocation(program, locID), 1, glm::value_ptr(Lt.getSpotDirection()));

    sprintf(locID, "%s[%d].%s", arrayname, i, "attenuation");
    glUniform3fv(glGetUniformLocation(program, locID), 1, glm::value_ptr(Lt.getAttenuation()));

    sprintf(locID, "%s[%d].%s", arrayname, i, "spotCutoff");
    glUniform1f(glGetUniformLocation(program, locID), Lt.getSpotCutoff());

    sprintf(locID, "%s[%d].%s", arrayname, i, "spotExponent");
    glUniform1f(glGetUniformLocation(program, locID), Lt.getSpotExponent());
}

/**
\brief Loads the first num entries of a light array to the a light array in the shader.

\param Lt --- Light array to load.

\param name --- The name of the array in the shader.

\param i --- The number of lights to load.

*/

void GraphicsEngine::LoadLights(Light Lt[], std::string name, int num)
{
    for (int i = 0; i < num; i++)
        LoadLight(Lt[i], name.c_str(), i);
}

/**
\brief Turns the light at index i on.

\param name --- The name of the array in the shader.

\param i --- The index of the light to turn on.

*/

void GraphicsEngine::turnLightOn(std::string name, int i)
{
    glUseProgram(program);

    const char* arrayname = name.c_str();  // array name in the shader.
    char locID[100];
    sprintf(locID, "%s[%d].%s", arrayname, i, "on");
    glUniform1i(glGetUniformLocation(program, locID), true);
}

/**
\brief Turns the light at index i off.

\param name --- The name of the array in the shader.

\param i --- The index of the light to turn off.

*/

void GraphicsEngine::turnLightOff(std::string name, int i)
{
    glUseProgram(program);

    const char* arrayname = name.c_str();  // array name in the shader.
    char locID[100];
    sprintf(locID, "%s[%d].%s", arrayname, i, "on");
    glUniform1i(glGetUniformLocation(program, locID), false);
}

/**
\brief Turns the first num lights on.

\param name --- The name of the array in the shader.

\param num --- Number of lights to turn on.

*/

void GraphicsEngine::turnLightsOn(std::string name, int num)
{
    for (int i = 0; i < num; i++)
        turnLightOn(name.c_str(), i);
}

/**
\brief Turns the first num lights off.

\param name --- The name of the array in the shader.

\param num --- Number of lights to turn off.

*/

void GraphicsEngine::turnLightsOff(std::string name, int num)
{
    for (int i = 0; i < num; i++)
        turnLightOff(name.c_str(), i);
}

/**
\brief Turns the first num textures off.

\param name --- The name of the boolean texture array in the shader.

\param num --- Number of textures to turn off.

*/

void GraphicsEngine::turnTexturesOff(std::string name, int num)
{
    for (int i = 0; i < num; i++)
        turnTextureOff(name.c_str(), i);
}

/**
\brief Turns the ith texture off.

\param name --- The name of the boolean texture array in the shader.

\param i --- Number of the texture to turn off.

*/

void GraphicsEngine::turnTextureOff(std::string name, int i)
{
    glUseProgram(program);

    const char* arrayname = name.c_str();  // array name in the shader.
    char locID[100];
    sprintf(locID, "%s[%d]", arrayname, i);
    glUniform1i(glGetUniformLocation(program, locID), false);
}

/**
\brief Turns the first num textures on.

\param name --- The name of the boolean texture array in the shader.

\param num --- Number of textures to turn on.

*/

void GraphicsEngine::turnTexturesOn(std::string name, int num)
{
    for (int i = 0; i < num; i++)
        turnTextureOn(name.c_str(), i);
}

/**
\brief Turns the ith texture on.

\param name --- The name of the boolean texture array in the shader.

\param i --- Number of the texture to turn on.

*/

void GraphicsEngine::turnTextureOn(std::string name, int i)
{
    glUseProgram(program);

    const char* arrayname = name.c_str();  // array name in the shader.
    char locID[100];
    sprintf(locID, "%s[%d]", arrayname, i);
    glUniform1i(glGetUniformLocation(program, locID), true);
}

/**
\brief Draws the game scene
*/

void GraphicsEngine::drawGameScene()
{
    turnTextureOn("useTexture", 5);
    mat = Materials::obsidian;
    loadMaterial(mat);

    // Set up Floor for Drawing
    glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*floorModel));
    glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(floorModel));

    glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(floorTexTrans));

    glm::mat3 mN(floorModel);
    glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
    glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

    floor.draw();

    turnTextureOff("useTexture", 5);
}

/**
\brief Handles the Drawing of the Start Scene.
*/

void GraphicsEngine::drawStartScene()
{
    glm::mat4 startModel = glm::rotate(glm::mat4(1.0), degf * 90, glm::vec3(0, 1, 0));
    startModel = glm::translate(startModel, glm::vec3(0, -2.8, 0));

    mat = Materials::whitePlastic;
    loadMaterial(mat);

    // Set up Start Button for Drawing
    glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*startModel));
    glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(startModel));

    glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

    glm::mat3 mN(startModel);
    glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
    glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

    start.draw();

    gc->setNumDiscs(3);
    discModel = gc->getDiscModels();

    mat = Materials::redPlastic;
    loadMaterial(mat);

    discModel[0] = glm::translate(discModel[0], glm::vec3(0, 8, 8));
    discModel[1] = glm::translate(discModel[1], glm::vec3(0, 10, 8));
    discModel[2] = glm::translate(discModel[2], glm::vec3(0, 13.3, 8));

    turnTextureOn("useTexture", 3);
    for (int i = 0; i < 3; i++)
    {
        glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*discModel[i]));
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(discModel[i]));

        glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(discTexTrans));

        glm::mat3 mN(discModel[i]);
        glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
        glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        discs[i].draw();
    }

    discModel[0] = glm::translate(discModel[0], glm::vec3(0, -32, 0));
    discModel[1] = glm::translate(discModel[1], glm::vec3(0, -40, 0));
    discModel[2] = glm::translate(discModel[2], glm::vec3(0, -53.2, 0));

    for (int i = 0; i < 3; i++)
    {
        glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*discModel[i]));
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(discModel[i]));

        glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(discTexTrans));

        glm::mat3 mN(discModel[i]);
        glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
        glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        discs[i].draw();
    }
    turnTextureOff("useTexture", 3);
}

/**
\brief Handles the Drawing of the Disc Number Selection Scene.
*/

void GraphicsEngine::drawDiscSelectionScene()
{
    glm::mat4 discModel[3];

    discModel[0] = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 8));
    discModel[0] = glm::rotate(discModel[0], degf * 90, glm::vec3(0, 1, 0));

    discModel[1] = glm::rotate(glm::mat4(1.0), degf * 90, glm::vec3(0, 1, 0));

    discModel[2] = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -8));
    discModel[2] = glm::rotate(discModel[2], degf * 90, glm::vec3(0, 1, 0));

    mat = Materials::whitePlastic;
    loadMaterial(mat);

    for (int i = 0; i < 3; i++)
    {
        glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*discModel[i]));
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(discModel[i]));

        glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

        glm::mat3 mN(discModel[i]);
        glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
        glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        numDiscsButton[i].draw();
    }

    glm::mat4 backModel = glm::translate(glm::mat4(1.0), glm::vec3(0, -6.5, -8.6));
    backModel = glm::rotate(backModel, degf * 90, glm::vec3(0, 1, 0));

    mat = Materials::redPlastic;
    loadMaterial(mat);

    glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*backModel));
    glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(backModel));

    glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

    glm::mat3 mN(backModel);
    glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
    glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

    menuBack.draw();
}

/**
\brief Handles the Drawing of the Difficulty Selection Scene.
*/

void GraphicsEngine::drawDiffSelectionScene()
{
    glm::mat4 diffModel[2];

    diffModel[0] = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 15));
    diffModel[0] = glm::rotate(diffModel[0], degf * 90, glm::vec3(0, 1, 0));

    diffModel[1] = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -15));
    diffModel[1] = glm::rotate(diffModel[1], degf * 90, glm::vec3(0, 1, 0));

    mat = Materials::whitePlastic;
    loadMaterial(mat);

    // Set up Easy/Hard Button for Drawing
    for (int i = 0; i < 2; i++)
    {
        glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*diffModel[i]));
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(diffModel[i]));

        glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

        glm::mat3 mN(diffModel[i]);
        glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
        glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        diffButton[i].draw();
    }

    glm::mat4 backModel = glm::translate(glm::mat4(1.0), glm::vec3(0, -7.5, 0));
    backModel = glm::rotate(backModel, degf * 90, glm::vec3(0, 1, 0));

    mat = Materials::redPlastic;
    loadMaterial(mat);

    glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*backModel));
    glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(backModel));

    glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

    glm::mat3 mN(backModel);
    glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
    glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

    menuBack.draw();
}

/**
\brief Draw the Text
*/

void GraphicsEngine::drawText()
{
    int currScene = gc->getCurrentScene();
    int x = getSize().x;
    int y = getSize().y;

    if (currScene == 0)
    {
        text.setScreenSize(x, y);

        text.setFontSize(y * .1);
        text.draw("Tower of Hanoi", (x / 2) - (text.textWidth("Tower of Hanoi") / 2), y * .8);

        text.setFontSize(y * .1);
        text.draw("Start - S", (x / 2) - (text.textWidth("Start - S") / 2), y * .4);

        text.setFontSize(y * .03);
        text.draw("Joshua Lord", (x/2) - (text.textWidth("Joshua Lord")/2), y * .7);
    }
    else if (currScene == 1)
    {
        text.setScreenSize(x, y);

        text.setFontSize(y * .1);
        text.draw("# of Discs", (x/2) - (text.textWidth("# of Discs") / 2), y * .8);

        text.setFontSize(y * .1);
        text.draw("3", x * .35, y * .47);

        text.setFontSize(y * .1);
        text.draw("4", (x / 2) - (text.textWidth("4") / 2), y * .47);

        text.setFontSize(y * .1);
        text.draw("6", x * .62, y * .47);

        text.setFontSize(y * .06);
        text.draw("Back - B", x * .58, y * .31);
    }
    else if (currScene == 2)
    {
        text.setScreenSize(x, y);

        text.setFontSize(y * .1);
        text.draw("Difficulty", (x/2) - (text.textWidth("Difficulty")/2), y*.8);

        text.setFontSize(y * .1);
        text.draw("Easy - E", x * .25 - (text.textWidth("Easy - E")/2), y * .47);

        text.setFontSize(y * .1);
        text.draw("Hard - H", x * .75 - (text.textWidth("Hard - H")/2), y * .47);

        text.setFontSize(y * .06);
        text.draw("Back - B", (x/2) - (text.textWidth("Back - B")/2), y * .28);

        text.setFontSize(y * .04);
        text.draw("Use Back Button Once", x * .17, y * .32);
        text.draw("Per Legal Move", x * .2, y * .25);
        text.draw("Never Use Back Button", x * .83 - text.textWidth("Never Use Back Button"), y * .32);

    }
    else if (currScene == 3)
    {
        text.setScreenSize(x, y);

        text.setFontSize(y * .03);
        text.draw("Menu  - M", x * .02, y * .14);
        text.draw("Reset   - R", x * .02, y * .10);
        text.draw("Cancel - C", x * .02, y * .06);
        text.draw("Back    - B", x * .02, y * .02);

        text.setFontSize(y * .03);
        text.draw("To Play: Use 1, 2, 3 Keys to Select a Disc", (x/2) - (text.textWidth("To Play: Use 1, 2, 3 Keys to Select a Disc")/2), y * .12);
        text.draw("Then, Use the 1, 2, 3 Keys to Select a Different Pole", (x/2) - (text.textWidth("Then, Use the 1, 2, 3 Keys to Select a Different Pole")/2), y * .08);
        text.draw("If the Move is Valid, The Disc Will Move to that Pole", (x/2) - (text.textWidth("If the Move is Valid, The Disc Will Move to that Pole")/2), y * .04);
    }
    else if (currScene == 4)
    {
        text.setScreenSize(x, y);

        text.setFontSize(y * .25);
        text.setColor(0, 1, 0, 1);
        text.draw("Winner!", (x/2) - (text.textWidth("Winner!")/2), y * .4);
        text.setColor(1, 1, 1, 1);

        char c[50];
        sprintf(c, "Move Counter: %d", gc->getMoves());
        string moves = string(c);

        text.setFontSize(y * .1);
        text.draw(moves, (x/2) - (text.textWidth(moves)/2), y * .7);

        text.setFontSize(y * 0.08);
        text.draw("Menu - M", x * .05, y * .05);
    }
}

/**
\brief Draws the Poles to the Screen
*/

void GraphicsEngine::drawPoles()
{
    turnTextureOn("useTexture", 4);

    mat = Materials::black;
    loadMaterial(mat);

    for (int i = 0; i < 3; i++)
    {
        glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*poleBaseModel[i]));
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(poleBaseModel[i]));

        glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

        glm::mat3 mN(poleBaseModel[i]);
        glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
        glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        poleBase[i].draw();
    }

    for (int i = 0; i < 3; i++)
    {
        glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(projection*view*poleStaffModel[i]));
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(poleStaffModel[i]));

        glUniformMatrix4fv(texTransLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

        glm::mat3 mN(poleStaffModel[i]);
        glm::mat3 matrixNormal = glm::transpose(glm::inverse(mN));
        glUniformMatrix3fv(NormalLoc, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        poleStaff[i].draw();
    }

    turnTextureOff("useTexture", 4);
}

/**
\brief Sets the Current Position of Camera
*/

void GraphicsEngine::setSphericalCameraPos(float R, float Theta, float Psi)
{
    sphcamera.setPosition(R, Theta, Psi);
}
