#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
class MyGLCanvas;
#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif
#include <FL/glut.H>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <time.h>
#include <iostream>
#include <list>
#include "ply.h"
#include "Grid.h"
#include "MyGLCanvas.h"

class GameManager {

    protected:
        // Protect constructor for singleton pattern
        GameManager() { 
            std::cout << "Game manager constructor" << std::endl;
            initialize(); 
        }
        static GameManager* _instance;

        enum States
        {
            ST_MENU,
            ST_GAMERUNNING,
            ST_GAMEEND
        };

        enum Colors
        {
            RED,
            BLUE,
            GREEN,
            BLACK,
            LAST
        };
    public:
        // Singleton Stuff
        GameManager(GameManager& other) = delete;
        void operator=(const GameManager&) = delete;

        MyGLCanvas* glc;

        // Various Member Variables
        int sizeX;
        int sizeY;
        float playerDist;
        glm::mat4 perspectiveMatrix;
        glm::mat4 modelViewMatrix;
        float playerScale;

        // Texture stuff
        int num_tex;
        GLuint m_tex;
        GLuint* tex_ids;

        // obstacle things
        struct obstacle {
            ply* ply;
            glm::vec3 pos;
            float depth;
            float speed;
            Colors color;
            GLboolean intersected = false;
        };
        list<obstacle> obs;
        double timer;
        double lastTimeCreated;
        double difficulty = 1;

        static GameManager* GetInstance();

        // Game State & Loop
        void GameUpdate();
        void UpdateGameState(States s);

        void handleKeyPress(char c);
        // Helper Function
        void resize(int x, int y);
        glm::vec3 generateRay(int pixelX, int pixelY);

        // Getter & Setter///////////////////////////////////////////////////////////////
        void setPlayerPos(int x, int y);
        glm::vec3 getPlayerPos();
        void setEyePoint(glm::vec3 p);

        void setPlayerColor(glm::vec3 c);
        glm::vec3 getPlayerColor();
        double GetTime();

        void setObstacleDelta(float depth);
        // General Helper Functions
        glm::vec3 enumToColor(Colors c);
        Colors colorToEnum(glm::vec3 v);
        void loadTexture();
        void loadTexture(const char* path, int textureInd, GLuint& tex_id);
        // Geometry and Shapes management
        // -- Grid env reference --
        Grid* grid;
        void loadGeometry();
    private:
        States state;

        // State Variables
        glm::vec3 playerPos = glm::vec3(0.0f);
        glm::vec3 eyeP = glm::vec3(0.0f);
        Colors playerColor = RED;
        double timePerWallTextureCycle;
        double currentTime;
        
        GLuint obstacle_vao;
        // Setup & Reset
        void initialize();

        void ResetGame();
        // State updates
        void updatePlayer(double frameTime);
        void updateObstacles(double frameTime);
        void updateEnvironment(double frameTime);

        GLboolean intersectionCheck(obstacle &o);
        void CheckGameOver(Colors &c);
        // Game Objects
        void MakeWall();
        void MakeObstacle(glm::vec2 pos);

        // Other
        bool SameSign(float a, float b);
};


#endif