#include "GameManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GameManager* GameManager::_instance = nullptr;;


/**
 * Static methods should be defined outside the class.
 */
GameManager* GameManager::GetInstance()
{
    if (_instance == nullptr) {
        _instance = new GameManager();
    }
    return _instance;
}

void GameManager::initialize() {
    state = ST_GAMERUNNING;
    currentTime = 0.0;
    timePerWallTextureCycle = 5.0;
    playerDist = 2.0f;
    playerScale = 0.2f;
    num_tex = 5;
    GLuint* tex_ids = new GLuint[4];
    tex_ids[0] = 0;
    tex_ids[1] = 1;
    tex_ids[2] = 2;
    tex_ids[3] = 3;
    timer = 0;
    lastTimeCreated = 0;
    grid = new Grid();
}

void GameManager::ResetGame() {
    obs.clear();
    currentTime = 0.0;
    timer = 0.0;
    lastTimeCreated = 0;
}

void GameManager::GameUpdate()
{
    double newTime = GetTime();
    // frameTime indicates the amount of time since the previous frame
    double frameTime = newTime - currentTime;
    if (frameTime > 0.25)
        frameTime = 0.25;
    currentTime = newTime;

    switch (state) {
        case ST_MENU:

            break;
        case ST_GAMERUNNING:

            // Set game state here based on how much frameTime has passed

            // update player position based on mouse position
            updatePlayer(frameTime);
            // update wall position based on time
            updateObstacles(frameTime);
            // update texture increment based on time
            updateEnvironment(frameTime);
            break;
        case ST_GAMEEND:

            break;
        default:
            std::cout << "Not in a valid state?" << std::endl;
    }
}

void GameManager::UpdateGameState(States s) {
    state = s;
}

// TODO: PLAYER UPDATES PER AMOUNT FRAMETIME
void GameManager::updatePlayer(double frameTime) {

}

// TODO: UPDATE OBSTACLE POSITIONS PER AMOUNT FRAMETIME
void GameManager::updateObstacles(double frameTime) {
    timer += frameTime;
    for (list<obstacle>::iterator i = obs.begin(); i != obs.end();) {
        auto o = i;
        o->depth += o->speed * (float)frameTime * (float) difficulty;
        o->pos = glm::vec3(o->pos.x, o->pos.y, o->depth);
        if (o->depth > 1.2f && !o->intersected) {
            if (intersectionCheck(*o)) {
                cout << "Reached valid intersection: " << o->color << endl;
                cout << "1" << endl;
                cout << "2 " << endl;
                Colors c = o->color;
                cout << "3 " << endl;
                CheckGameOver(c);
                cout << "4" << endl;
                list<obstacle>::iterator pos = obs.erase(i);
                i = pos;
            }
               o->intersected = true;
        }
        else if (o->depth > 2.0f) {
            list<obstacle>::iterator pos = obs.erase(i);
            i = pos;
        }
        if (i != obs.end())
            ++i;
    }
    if (timer > lastTimeCreated + (3.0 / difficulty)) {
        lastTimeCreated = timer;
        difficulty += 0.1;
        MakeWall();
    }
}

// TODO: UPDATE ENVIRONMENT PER AMOUNT FRAMETIME
void GameManager::updateEnvironment(double frameTime) {
    grid->update();
}

GLboolean GameManager::intersectionCheck(obstacle& o) {
    //cout << "intersection checking" << endl;
    return SameSign(o.pos.x, getPlayerPos().x) && SameSign(o.pos.y, getPlayerPos().y);
}

void GameManager::CheckGameOver(Colors &c) {
    cout << "Checking for game over" << endl;
    if (c != playerColor) {
        state = ST_GAMEEND;
    }
}

void GameManager::MakeWall() {
    MakeObstacle(glm::vec2(-0.5f, 0.5f));
    MakeObstacle(glm::vec2(0.5f, 0.5f));
    MakeObstacle(glm::vec2(-0.5f, -0.5f));
    MakeObstacle(glm::vec2(0.5f, -0.5f));
}

void GameManager::MakeObstacle(glm::vec2 pos) {
    obstacle o;
    o.ply = new ply("./data/obstacle.ply");
    o.pos = glm::vec3(pos.x, pos.y, -20.0f);
    o.depth = -20.0f;
    o.speed = 4.0;
    o.color = static_cast<Colors>(rand() % LAST);
    o.ply->buildArrays();
    glc->reloadObstacleVBO();
    obs.push_back(o);
    //cout << "Making an obstacle" << endl;
}

void GameManager::handleKeyPress(char c) {
    //std::cout << c << std::endl;
    switch (c) {
    case 'A':
        setPlayerColor(enumToColor(RED));
        break;
    case 'S':
        setPlayerColor(enumToColor(GREEN));
        break;
    case 'D':
        setPlayerColor(enumToColor(BLUE));
        break;
    default:
        setPlayerColor(enumToColor(RED));
    }
}


glm::vec3 GameManager::generateRay(int pixelX, int pixelY)
{
    glm::vec3 point((pixelX / (float)sizeX) * 2 - 1, (pixelY / (float)sizeY) * -2 + 1, -1.0f);
    
    //std::cout << point.x << ", " << point.y << std::endl;
    //std::cout << sizeX << ", " << sizeY << std::endl;
    glm::vec4 ray_clip = glm::vec4(point, 1.0);
    glm::vec3 ray_eye = glm::inverse(perspectiveMatrix) * ray_clip;
    glm::vec4 ray_wor = glm::vec4(inverse(modelViewMatrix) * glm::vec4(ray_eye, 0.0));

    return ray_wor;
}

void GameManager::resize(int x, int y) {
    sizeX = x;
    sizeY = y;
}

// -------------- Getter & Setter -----------------


void GameManager::setPlayerPos(int x, int y) {
    // calculate player point here
    glm::vec3 ray_wor = generateRay(x, y);
    playerPos = eyeP + playerDist * ray_wor;
}

glm::vec3 GameManager::getPlayerPos() {
    return playerPos;
}

void GameManager::setEyePoint(glm::vec3 p) {
    eyeP = p;
}


void GameManager::setPlayerColor(glm::vec3 c) {
    playerColor = colorToEnum(c);
}

glm::vec3 GameManager::getPlayerColor() {
    return enumToColor(playerColor);
}


double GameManager::GetTime() {
    clock_t cycles = clock();
    double time = (double)cycles / CLOCKS_PER_SEC;
    return time;
}

glm::vec3 GameManager::enumToColor(Colors c) {
    switch (c) {
    case RED:
        return glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case BLUE:
        return glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case GREEN:
        return glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    default:
        return glm::vec3(0.0f);
    }
}

GameManager::Colors GameManager::colorToEnum(glm::vec3 v) {
    if (v.x > 0.1f) { return RED; }
    if (v.y > 0.1f) { return GREEN; }
    if (v.z > 0.1f) { return BLUE; }
    return BLACK;
}

void GameManager::loadTexture() {
    GLuint* textures = new GLuint[num_tex];
    glGenTextures(num_tex, textures);
    loadTexture("./data/test.png", GL_TEXTURE0, textures[0]);
    loadTexture("./data/red.png", GL_TEXTURE1, textures[1]);
    loadTexture("./data/green.png", GL_TEXTURE2, textures[2]);
    loadTexture("./data/blue.png", GL_TEXTURE3, textures[3]);
    loadTexture("./data/black.png", GL_TEXTURE4, textures[4]);

    tex_ids = textures;
}

void GameManager::loadTexture(const char* path, int textureInd, GLuint &tex_id) {
    int w;
    int h;
    int comp;
    std::cout << "Reading in " << path << " from data" << std::endl;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);

    if (image == nullptr)
        throw(std::string("Failed to load texture"));
    glActiveTexture(textureInd);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
}


bool GameManager::SameSign(float a, float b) {
    return a*b >= 0.0f;
}