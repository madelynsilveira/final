#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "iostream"

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char* l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
    rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
    rotWorldVec = glm::vec3(0.0f, 0.0f, 0.0f);

	eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	gm->GetInstance()->setEyePoint(eyePosition);
	lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;

	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 20.0f;
	scaleFactor = 1.0f;
	lightAngle = 0.0f;
	textureBlend = 0.0f;

	useDiffuse = false;

	firstTime = true;

	myTextureManager = new TextureManager();
	myShaderManager = new ShaderManager();
	playerPLY = new ply("./data/player.ply");

	gm->GetInstance()->resize(w, h);

}

MyGLCanvas::~MyGLCanvas() {
	delete myTextureManager;
	delete myShaderManager;
	delete playerPLY;
}




void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		/****************************************/
		/*          Enable z-buferring          */
		/****************************************/

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		if (firstTime == true) {
			firstTime = false;
			initShaders();
		}
	}
	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
	gm->GetInstance()->grid->render();

}

void MyGLCanvas::drawScene() {
	glm::mat4 viewMatrix = glm::lookAt(eyePosition, lookatPoint, glm::vec3(0.0f, 1.0f, 0.0f));

	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelMatrix = glm::mat4(1.0);
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

	glm::vec4 lookVec(0.0f, 0.0f, -1.0f, 0.0f);
	gm->GetInstance()->modelViewMatrix = viewMatrix * modelMatrix;
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	//Pass first texture info to our shader 
	for (int i = 0; i < gm->GetInstance()->num_tex; i++) {
		unsigned int i2 = GL_TEXTURE0 + i;
		glActiveTexture(i2);
		glBindTexture(GL_TEXTURE_2D, gm->GetInstance()->tex_ids[i]);
	}

	//first draw the object sphere
	glUseProgram(myShaderManager->getShaderProgram("playerShaders")->programID);
	unsigned int playerShaderID = myShaderManager->getShaderProgram("playerShaders")->programID;
    glUniform1i(
		glGetUniformLocation(playerShaderID ,"playerTexture"), 0
	);

	glUniform3fv(
		glGetUniformLocation(playerShaderID, "playerColor"), 1, glm::value_ptr(gm->GetInstance()->getPlayerColor())
	);

    glUniformMatrix4fv(
		glGetUniformLocation(playerShaderID, "modelMatrix"), 1, false, glm::value_ptr(modelMatrix)
	);

    glUniformMatrix4fv(
		glGetUniformLocation(playerShaderID, "viewMatrix"), 1, false, glm::value_ptr(viewMatrix)
	);

    glUniformMatrix4fv(
		glGetUniformLocation(playerShaderID, "perspectiveMatrix" ), 1, false, glm::value_ptr(perspectiveMatrix)
	);

	// player position
	glUniform3fv(
		glGetUniformLocation(playerShaderID, "playerPosW"),1,glm::value_ptr(gm->GetInstance()->getPlayerPos())
	);

	glUniform1f(
		glGetUniformLocation(playerShaderID, "pScale"),gm->GetInstance()->playerScale
	);

	playerPLY->renderVBO(myShaderManager->getShaderProgram("playerShaders")->programID);


	// Draw Obstacles
	glUseProgram(myShaderManager->getShaderProgram("obstacleShader")->programID);

	unsigned int obstacleShaderID = myShaderManager->getShaderProgram("obstacleShader")->programID;
    glUniformMatrix4fv(
        glGetUniformLocation(obstacleShaderID, "viewMatrix"),1,false,&viewMatrix[0][0]
    );
    glUniformMatrix4fv(
        glGetUniformLocation(obstacleShaderID, "perspectiveMatrix"),1,false,&perspectiveMatrix[0][0]
	);
	//gm->GetInstance()->ob->renderVBO(obstacleShaderID);
	for (auto& o : gm->GetInstance()->obs) {
		glUniform1i(
			glGetUniformLocation(obstacleShaderID, "obstacleTexture"), o.color + 1
		);
		glUniform3fv(
			glGetUniformLocation(obstacleShaderID, "obstacle_delta"), 1, glm::value_ptr(o.pos)
		);
		o.ply->renderVBO(myShaderManager->getShaderProgram("obstacleShader")->programID);
	}
	// glUseProgram ( 0 ) represents uninstalling the current program i.e. the shader and stuff obstacle
	// is using at this moment. This is so that during the next draw call, the grid doesn't
	// use the shaders of the obstacle
	glUseProgram(0);
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;
	perspectiveMatrix = glm::perspective(TO_RADIANS(viewAngle), xy_aspect, clipNear, clipFar);
	gm->GetInstance()->perspectiveMatrix = perspectiveMatrix;
}


int MyGLCanvas::handle(int e) {
	//static int first = 1;
#ifndef __APPLE__
	if (firstTime && e == FL_SHOW && shown()) {
		firstTime = 0;
		make_current();
		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err) {
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		else {
			//SHADER: initialize the shader manager and loads the two shader programs
			initShaders();
		}
	}
#endif	
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_DRAG:
	case FL_MOVE:
		Fl::belowmouse(this);
		gm->GetInstance()->setPlayerPos(Fl::event_x(), Fl::event_y());
	case FL_PUSH:
	case FL_RELEASE:
	case FL_KEYDOWN:
		gm->GetInstance()->handleKeyPress(*Fl::event_text());
	case FL_MOUSEWHEEL:
		break;
	}
	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	gm->GetInstance()->resize(x, y);
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::initShaders() {
	//myTextureManager->loadTexture("environMap", "./data/sphere-map-market.ppm");
	//myTextureManager->loadTexture("objectTexture", "./data/brick.ppm");
	gm->GetInstance()->loadTexture();
	myShaderManager->addShaderProgram("playerShaders", "shaders/330/player-vert.shader", "shaders/330/player-frag.shader");
	playerPLY->buildArrays();
	playerPLY->bindVBO(myShaderManager->getShaderProgram("playerShaders")->programID);

	myShaderManager->addShaderProgram("obstacleShader", "shaders/330/obstacle-vert.shader", "shaders/330/obstacle-frag.shader");
	for (auto& o : gm->GetInstance()->obs) {
		o.ply->buildArrays();
		o.ply->bindVBO(myShaderManager->getShaderProgram("obstacleShader")->programID);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MyGLCanvas::reloadShaders() {
	myShaderManager->resetShaders();

	myShaderManager->addShaderProgram("playerShaders", "shaders/330/player-vert.shader", "shaders/330/player-frag.shader");
	playerPLY->bindVBO(myShaderManager->getShaderProgram("playerShaders")->programID);

	myShaderManager->addShaderProgram("obstacleShader", "shaders/330/obstacle-vert.shader", "shaders/330/obstacle-frag.shader");
	for (auto& o : gm->GetInstance()->obs) {
		o.ply->bindVBO(myShaderManager->getShaderProgram("obstacleShader")->programID);
	}
	invalidate();
}

void MyGLCanvas::reloadObstacleVBO() {
	for (auto& o : gm->GetInstance()->obs) {
		o.ply->bindVBO(myShaderManager->getShaderProgram("obstacleShader")->programID);
	}
}

void MyGLCanvas::loadPLY(std::string filename) {
	delete playerPLY;
	playerPLY = new ply(filename);
	playerPLY->buildArrays();
	playerPLY->bindVBO(myShaderManager->getShaderProgram("playerShaders")->programID);
}

void MyGLCanvas::loadEnvironmentTexture(std::string filename) {
	myTextureManager->deleteTexture("environMap");
	myTextureManager->loadTexture("environMap", filename);
}

void MyGLCanvas::loadObjectTexture(std::string filename) {
	myTextureManager->deleteTexture("objectTexture");
	myTextureManager->loadTexture("objectTexture", filename);
}
