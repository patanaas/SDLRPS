#include "GameLevel.h"
#include "Game.h"
#include <chrono>

GameLevel::GameLevel(const std::vector<std::string>& nicknames)
    : m_font(nullptr), m_music(nullptr), m_whooshSound(nullptr), m_gunSound(nullptr),
    m_spriteSheet(nullptr), m_gameState(GameState::Idle), m_animationTimer(0),
    m_currentFrame(0), m_gameTime(0), m_fps(0), m_frameCount(0),
    m_lastFPSUpdate(SDL_GetTicks()), m_saves(0), m_loads(0) {

    m_rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    m_windowSize = { 1920, 1080 };

    for (int i = 0; i < 2; ++i) {
        m_players[i].nickname = nicknames[i];
        m_players[i].state = GameState::Idle;
        m_players[i].wins = 0;
        m_players[i].losses = 0;
        m_players[i].position = { 20, 30 + i * 300 };
        m_mouseButtons[i] = false;
        m_controllers[i] = nullptr;
    }

    // Initialize game controllers
    for (int i = 0; i < SDL_NumJoysticks() && i < 2; ++i) {
        if (SDL_IsGameController(i)) {
            m_controllers[i] = SDL_GameControllerOpen(i);
        }
    }

    m_stateMessage = "Waiting to start...";
}

GameLevel::~GameLevel() {
    Clean();
}

bool GameLevel::Load() {
   /* m_font = TTF_OpenFont(Game::GetAssetPath("Fonts/arial.ttf").c_str(), 20);
    if (!m_font) return false;

    m_music = Mix_LoadMUS(Game::GetAssetPath("Audio/Music/Track1.mp3").c_str());
    if (!m_music) return false;

    m_whooshSound = Mix_LoadWAV(Game::GetAssetPath("Audio/Effects/Whoosh.wav").c_str());
    if (!m_whooshSound) return false;

    m_gunSound = Mix_LoadWAV(Game::GetAssetPath("Audio/Effects/DistantGunshot.wav").c_str());
    if (!m_gunSound) return false;

    m_spriteSheet = Game::Instance().LoadTGA(Game::GetAssetPath("Textures/RockPaperScissors.tga").c_str());
    if (!m_spriteSheet) return false;

    Mix_PlayMusic(m_music, -1);
    return true;*/
    SDL_Log("GameLevel Load started");

    // Load font
    SDL_Log("Attempting to load font from: %s", Game::GetAssetPath("Fonts/arial.ttf").c_str());
    m_font = TTF_OpenFont(Game::GetAssetPath("Fonts/arial.ttf").c_str(), 20);
    if (!m_font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return false;
    }
    SDL_Log("Font loaded successfully");

    // Load music
    SDL_Log("Attempting to load music from: %s", Game::GetAssetPath("Audio/Music/Track1.mp3").c_str());
    m_music = Mix_LoadMUS(Game::GetAssetPath("Audio/Music/Track1.mp3").c_str());
    if (!m_music) {
        SDL_Log("Failed to load music: %s", Mix_GetError());
        return false;
    }
    SDL_Log("Music loaded successfully");

    // Load whoosh sound
    SDL_Log("Attempting to load whoosh sound from: %s", Game::GetAssetPath("Audio/Effects/Whoosh.wav").c_str());
    m_whooshSound = Mix_LoadWAV(Game::GetAssetPath("Audio/Effects/Whoosh.wav").c_str());
    if (!m_whooshSound) {
        SDL_Log("Failed to load whoosh sound: %s", Mix_GetError());
        return false;
    }
    SDL_Log("Whoosh sound loaded successfully");

    // Load gunshot sound
    SDL_Log("Attempting to load gunshot sound from: %s", Game::GetAssetPath("Audio/Effects/DistantGunshot.wav").c_str());
    m_gunSound = Mix_LoadWAV(Game::GetAssetPath("Audio/Effects/DistantGunshot.mp3").c_str());
    if (!m_gunSound) {
        SDL_Log("Failed to load gunshot sound: %s", Mix_GetError());
        return false;
    }
    SDL_Log("Gunshot sound loaded successfully");

    // Load sprite sheet
    SDL_Log("Attempting to load sprite sheet from: %s", Game::GetAssetPath("Textures/RockPaperScissors.tga").c_str());
    m_spriteSheet = Game::Instance().LoadTGA(Game::GetAssetPath("Textures/RockPaperScissors.tga").c_str());
    if (!m_spriteSheet) {
        SDL_Log("Failed to load sprite sheet");
        return false;
    }
    SDL_Log("Sprite sheet loaded successfully");

    // If we got here, everything loaded successfully
    Mix_PlayMusic(m_music, -1);
    SDL_Log("GameLevel Load completed successfully");
    return true;
}

void GameLevel::Update(float deltaTime) {
    m_gameTime += deltaTime;
    m_frameCount++;

    // Update FPS counter every second
    if (SDL_GetTicks() - m_lastFPSUpdate >= 1000) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_lastFPSUpdate = SDL_GetTicks();
    }

    // Update animation
    if (m_gameState == GameState::Roll) {
        m_animationTimer += deltaTime;
        if (m_animationTimer >= 0.1f) { // Change frame every 0.1 seconds
            m_animationTimer = 0;
            m_currentFrame = (m_currentFrame + 1) % 4;
        }
    }

    // Handle movement
    HandleKeyboardMovement(deltaTime);
    HandleMouseMovement();
    HandleControllerMovement(deltaTime);
}

void GameLevel::HandleKeyboardMovement(float deltaTime) {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    const float moveSpeed = 300.0f; // pixels per second

    // Player 1 movement (WASD)
    SDL_Point newPos1 = m_players[0].position;
    if (keyState[SDL_SCANCODE_W]) newPos1.y -= static_cast<int>(moveSpeed * deltaTime);
    if (keyState[SDL_SCANCODE_S]) newPos1.y += static_cast<int>(moveSpeed * deltaTime);
    if (keyState[SDL_SCANCODE_A]) newPos1.x -= static_cast<int>(moveSpeed * deltaTime);
    if (keyState[SDL_SCANCODE_D]) newPos1.x += static_cast<int>(moveSpeed * deltaTime);
    if (IsPositionValid(newPos1)) m_players[0].position = newPos1;

    // Player 2 movement (Arrow keys)
    SDL_Point newPos2 = m_players[1].position;
    if (keyState[SDL_SCANCODE_UP]) newPos2.y -= static_cast<int>(moveSpeed * deltaTime);
    if (keyState[SDL_SCANCODE_DOWN]) newPos2.y += static_cast<int>(moveSpeed * deltaTime);
    if (keyState[SDL_SCANCODE_LEFT]) newPos2.x -= static_cast<int>(moveSpeed * deltaTime);
    if (keyState[SDL_SCANCODE_RIGHT]) newPos2.x += static_cast<int>(moveSpeed * deltaTime);
    if (IsPositionValid(newPos2)) m_players[1].position = newPos2;
}

void GameLevel::HandleMouseMovement() {
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    SDL_Point mousePos = { mouseX, mouseY };

    // Left mouse button controls Player 1
    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        if (IsPositionValid(mousePos)) {
            m_players[0].position = mousePos;
        }
    }

    // Right mouse button controls Player 2
    if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (IsPositionValid(mousePos)) {
            m_players[1].position = mousePos;
        }
    }
}

void GameLevel::HandleControllerMovement(float deltaTime) {
    const float moveSpeed = 300.0f; // pixels per second
    const float deadzone = 8000.0f; // Adjust as needed

    for (int i = 0; i < 2; ++i) {
        if (m_controllers[i]) {
            float axisX = SDL_GameControllerGetAxis(m_controllers[i], SDL_CONTROLLER_AXIS_LEFTX);
            float axisY = SDL_GameControllerGetAxis(m_controllers[i], SDL_CONTROLLER_AXIS_LEFTY);

            // Apply deadzone
            if (fabs(axisX) < deadzone) axisX = 0;
            if (fabs(axisY) < deadzone) axisY = 0;

            // Normalize and apply movement
            SDL_Point newPos = m_players[i].position;
            newPos.x += static_cast<int>((axisX / 32768.0f) * moveSpeed * deltaTime);
            newPos.y += static_cast<int>((axisY / 32768.0f) * moveSpeed * deltaTime);

            if (IsPositionValid(newPos)) {
                m_players[i].position = newPos;
            }
        }
    }
}

bool GameLevel::IsPositionValid(const SDL_Point& pos) const {
    const int spriteWidth = 280;
    const int spriteHeight = 170;
    return pos.x >= 0 && pos.x <= (m_windowSize.x - spriteWidth) &&
        pos.y >= 0 && pos.y <= (m_windowSize.y - spriteHeight);
}

SDL_Rect GameLevel::GetSourceRect(int playerIndex) {
    int clip;
    switch (m_players[playerIndex].state) {
    case GameState::Roll:
        clip = m_currentFrame;
        break;
    case GameState::Paper:
        clip = 4 + (m_currentFrame % 4);
        break;
    case GameState::Scissors:
        clip = 8 + (m_currentFrame % 4);
        break;
    case GameState::Rock:
        clip = 12 + (m_currentFrame % 4);
        break;
    default:
        clip = 0;
    }

    int row = clip / 4;
    int col = clip % 4;
    return SDL_Rect{ col * 280, row * 170, 280, 170 };
}

void GameLevel::DetermineWinner() {
    GameState p1State = m_players[0].state;
    GameState p2State = m_players[1].state;

    if (p1State == p2State) {
        m_stateMessage = "DRAW!";
    }
    else if ((p1State == GameState::Rock && p2State == GameState::Scissors) ||
        (p1State == GameState::Paper && p2State == GameState::Rock) ||
        (p1State == GameState::Scissors && p2State == GameState::Paper)) {
        m_stateMessage = m_players[0].nickname + " WINS!";
        m_players[0].wins++;
        m_players[1].losses++;
        SDL_Log("Player 1 wins: State P1=%d, P2=%d", static_cast<int>(p1State), static_cast<int>(p2State));
        SDL_Log("Updated scores - P1 W/L: %d/%d, P2 W/L: %d/%d",
            m_players[0].wins, m_players[0].losses,
            m_players[1].wins, m_players[1].losses);
    }
    else if((p2State == GameState::Rock && p1State == GameState::Scissors) ||
        (p2State == GameState::Paper && p1State == GameState::Rock) ||
        (p2State == GameState::Scissors && p1State == GameState::Paper)) {
        m_stateMessage = m_players[1].nickname + " WINS!";
        m_players[1].wins++;
        m_players[0].losses++;
        SDL_Log("Player 2 wins: State P1=%d, P2=%d", static_cast<int>(p1State), static_cast<int>(p2State));
        SDL_Log("Updated scores - P1 W/L: %d/%d, P2 W/L: %d/%d",
            m_players[0].wins, m_players[0].losses,
            m_players[1].wins, m_players[1].losses);
    }
    Mix_PlayChannel(-1, m_gunSound, 0);
}

GameState GameLevel::GetRandomState() {
    std::uniform_int_distribution<int> dist(2, 4); // 2=Paper, 3=Scissors, 4=Rock
    return static_cast<GameState>(dist(m_rng));
}

void GameLevel::Render() {
    SDL_Renderer* renderer = Game::Instance().GetRenderer();

    // Clear with white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render players
    for (int i = 0; i < 2; ++i) {
        SDL_Rect srcRect = GetSourceRect(i);
        SDL_Rect dstRect = { m_players[i].position.x, m_players[i].position.y, 280, 170 };
        SDL_RenderCopy(renderer, m_spriteSheet, &srcRect, &dstRect);

        // Render player nicknames
        RenderText(m_players[i].nickname, { 0, 255, 0, 255 },
            m_players[i].position.x, m_players[i].position.y - 30, 20);
        RenderText(m_stateMessage, {255, 0, 0, 255},
            m_players[i].position.x, m_players[i].position.y - 10, 20);
        // Render scores
        std::string score = "Wins: " + std::to_string(m_players[i].wins) +
            " Losses: " + std::to_string(m_players[i].losses);
        RenderText(score, { 0, 0, 255, 255 }, m_players[i].position.x, m_players[i].position.y + 20, 20);
    }

    // Render state message
    //RenderText(m_stateMessage, { 255, 0, 0, 255 }, 10, 10, 20);

    // Render scores
    /*std::string score1 = "Wins: " + std::to_string(m_players[0].wins) +
        " Losses: " + std::to_string(m_players[0].losses);
    std::string score2 = "Wins: " + std::to_string(m_players[1].wins) +
        " Losses: " + std::to_string(m_players[1].losses);
    RenderText(score1, { 0, 0, 255, 255 }, 10, 40, 20);
    RenderText(score2, { 0, 0, 255, 255 }, 10, 70, 20);*/

    // Render game info
    std::string info = "FPS: " + std::to_string(m_fps) +
        " Time: " + std::to_string(static_cast<int>(m_gameTime)) +
        " Saves: " + std::to_string(m_saves) +
        " Loads: " + std::to_string(m_loads);
    RenderText(info, { 0, 0, 255, 255 }, 10, m_windowSize.y - 60, 20);

    // Render action labels
    std::string actions = "Quit [ESC] | Next State [Space] | Save [F5] | Load [F7]";
    RenderText(actions, { 0, 0, 255, 255 }, 10, m_windowSize.y - 30, 20);
}

void GameLevel::HandleEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            Game::Instance().Quit();
            break;
        case SDLK_SPACE:
            if (m_gameState == GameState::Idle) {
                m_gameState = GameState::Roll;
                m_stateMessage = "Rolling...";
                Mix_PlayChannel(-1, m_whooshSound, 0);
                m_players[0].state = GameState::Roll;
                m_players[1].state = GameState::Roll;
            }
            else if (m_gameState == GameState::Roll) {
                m_players[0].state = GetRandomState();
                m_players[1].state = GetRandomState();
                DetermineWinner();
                m_gameState = GameState::Idle;
            }
            break;
            break;
        case SDLK_F5:
            SaveGame();
            break;
        case SDLK_F7:
            LoadGame();
            break;
        }
    }
}

void GameLevel::SaveGame() {
    std::ofstream file(Game::GetAssetPath("saveGame.bin").c_str(), std::ios::binary);
    if (file.is_open()) {
        // Save game state
        file.write(reinterpret_cast<char*>(&m_gameState), sizeof(m_gameState));

        // Save players data
        for (int i = 0; i < 2; ++i) {
            size_t nicknameLength = m_players[i].nickname.length();
            file.write(reinterpret_cast<char*>(&nicknameLength), sizeof(nicknameLength));
            file.write(m_players[i].nickname.c_str(), nicknameLength);
            file.write(reinterpret_cast<char*>(&m_players[i].state), sizeof(m_players[i].state));
            file.write(reinterpret_cast<char*>(&m_players[i].position), sizeof(m_players[i].position));
            file.write(reinterpret_cast<char*>(&m_players[i].wins), sizeof(m_players[i].wins));
            file.write(reinterpret_cast<char*>(&m_players[i].losses), sizeof(m_players[i].losses));
        }

        // Save game time
        file.write(reinterpret_cast<char*>(&m_gameTime), sizeof(m_gameTime));
        m_saves++;
    }
}

void GameLevel::LoadGame() {
    std::ifstream file(Game::GetAssetPath("saveGame.bin").c_str(), std::ios::binary);
    if (file.is_open()) {
        // Load game state
        file.read(reinterpret_cast<char*>(&m_gameState), sizeof(m_gameState));

        // Load players data
        for (int i = 0; i < 2; ++i) {
            size_t nicknameLength;
            file.read(reinterpret_cast<char*>(&nicknameLength), sizeof(nicknameLength));

            std::vector<char> nickname(nicknameLength + 1);
            file.read(nickname.data(), nicknameLength);
            nickname[nicknameLength] = '\0';
            m_players[i].nickname = std::string(nickname.data());

            file.read(reinterpret_cast<char*>(&m_players[i].state), sizeof(m_players[i].state));
            file.read(reinterpret_cast<char*>(&m_players[i].position), sizeof(m_players[i].position));
            file.read(reinterpret_cast<char*>(&m_players[i].wins), sizeof(m_players[i].wins));
            file.read(reinterpret_cast<char*>(&m_players[i].losses), sizeof(m_players[i].losses));
        }

        // Load game time
        file.read(reinterpret_cast<char*>(&m_gameTime), sizeof(m_gameTime));
        m_loads++;
    }
}

void GameLevel::RenderText(const std::string& text, SDL_Color color, int x, int y, int fontSize) {
    TTF_Font* font = TTF_OpenFont(Game::GetAssetPath("Fonts/arial.ttf").c_str(), fontSize);
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::Instance().GetRenderer(), surface);
        if (texture) {
            SDL_Rect rect = { x, y, surface->w, surface->h };
            SDL_RenderCopy(Game::Instance().GetRenderer(), texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
    TTF_CloseFont(font);
}

void GameLevel::Clean() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }

    if (m_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }

    if (m_whooshSound) {
        Mix_FreeChunk(m_whooshSound);
        m_whooshSound = nullptr;
    }

    if (m_gunSound) {
        Mix_FreeChunk(m_gunSound);
        m_gunSound = nullptr;
    }

    if (m_spriteSheet) {
        SDL_DestroyTexture(m_spriteSheet);
        m_spriteSheet = nullptr;
    }

    // Clean up controllers
    for (int i = 0; i < 2; ++i) {
        if (m_controllers[i]) {
            SDL_GameControllerClose(m_controllers[i]);
            m_controllers[i] = nullptr;
        }
    }
}