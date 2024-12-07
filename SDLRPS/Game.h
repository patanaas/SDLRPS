#ifndef GAME_H
#define GAME_H

#include "StandardIncludes.h"
#include <memory>
#include "Level.h"

class Game {
public:
    static Game& Instance() {
        static Game instance;
        return instance;
    }
    bool Init();
    void Run();
    void Clean();
    void Quit() { m_running = false; }
    SDL_Renderer* GetRenderer() const { return m_renderer; }
    SDL_Texture* LoadTGA(const char* filename);
    void ChangeLevel(std::unique_ptr<Level> newLevel);

    static std::string GetAssetPath(const char* filename) {
        return std::string("../Assets/") + filename;
    }

private:
    Game() : m_window(nullptr), m_renderer(nullptr), m_running(false), m_currentLevel(nullptr) {}
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_running;
    std::unique_ptr<Level> m_currentLevel;
};

#endif // !GAME_H
