#include "Game.h"
#include "TGALoader.h"
#include "Level.h"
#include "MenuLevel.h"
#include "StandardIncludes.h"
#include <SDL.h>

bool Game::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        SDL_Log("TTF initialization failed: %s", TTF_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer initialization failed: %s", Mix_GetError());
        return false;
    }

    m_window = SDL_CreateWindow("SDLRPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1920, 1080, SDL_WINDOW_SHOWN);
    if (!m_window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    // Initialize game controllers
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                SDL_Log("Found game controller %d", i);
            }
        }
    }

    m_running = true;
    return true;
}

SDL_Texture* Game::LoadTGA(const char* filename) {
    SDL_Surface* surface = TGALoader::LoadTGASurface(filename);
    if (!surface) {
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Game::Run() {
    m_currentLevel = std::make_unique<MenuLevel>();
    if (!m_currentLevel->Load()) {
        SDL_Log("Failed to load initial level");
        return;
    }

    Uint32 frameStart;
    Uint32 frameTime;
    const int targetFPS = 60;
    const float frameDelay = 1000.0f / targetFPS;
    Uint32 frames = 0;
    Uint32 lastTime = SDL_GetTicks();

    while (m_running) {
        frameStart = SDL_GetTicks();

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                m_running = false;
                break;
            }
            m_currentLevel->HandleEvents(event);
        }

        // Calculate delta time in seconds
        float deltaTime = (frameStart - lastTime) / 1000.0f;
        lastTime = frameStart;

        // Update and render
        m_currentLevel->Update(deltaTime);

        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        SDL_RenderClear(m_renderer);
        m_currentLevel->Render();
        SDL_RenderPresent(m_renderer);

        // Cap framerate
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }

        frames++;
        if (SDL_GetTicks() - frameStart >= 1000) {
            frames = 0;
            lastTime = SDL_GetTicks();
        }
    }
}

void Game::Clean() {
    if (m_currentLevel) {
        m_currentLevel->Clean();
    }

   /* for (int i = 0; i < SDL_NumGameControllers(); ++i) {
        if (SDL_GameControllerGetAttached(SDL_GameControllerFromPlayerIndex(i))) {
            SDL_GameControllerClose(SDL_GameControllerFromPlayerIndex(i));
        }
    }*/

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Game::ChangeLevel(std::unique_ptr<Level> newLevel) {
    if (m_currentLevel) {
        m_currentLevel->Clean();
    }
    m_currentLevel = std::move(newLevel);
    if (!m_currentLevel->Load()) {
        SDL_Log("Failed to load new level");
        Quit();
    }
}