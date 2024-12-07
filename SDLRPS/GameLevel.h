#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include "StandardIncludes.h"
#include "Level.h"
#include <random>

enum class GameState {
    Idle,
    Roll,
    Paper,
    Scissors,
    Rock
};

struct Player {
    std::string nickname;
    GameState state;
    SDL_Point position;
    int wins;
    int losses;
};

class GameLevel : public Level {
public:
    GameLevel(const std::vector<std::string>& nicknames);
    ~GameLevel();
    bool Load() override;
    void Update(float deltaTime) override;
    void Render() override;
    void HandleEvents(SDL_Event& event) override;
    void Clean() override;

private:
    void UpdateAnimation();
    void DetermineWinner();
    GameState GetRandomState();
    SDL_Rect GetSourceRect(int playerIndex);
    void RenderText(const std::string& text, SDL_Color color, int x, int y, int fontSize = 20);
    void UpdatePlayerPosition(int playerIndex, const SDL_Point& newPos);
    void SaveGame();
    void LoadGame();
    void HandleKeyboardMovement(float deltaTime);
    void HandleMouseMovement();
    void HandleControllerMovement(float deltaTime);
    bool IsPositionValid(const SDL_Point& pos) const;

    TTF_Font* m_font;
    Mix_Music* m_music;
    Mix_Chunk* m_whooshSound;
    Mix_Chunk* m_gunSound;
    SDL_Texture* m_spriteSheet;
    Player m_players[2];
    GameState m_gameState;
    float m_animationTimer;
    int m_currentFrame;
    float m_gameTime;
    int m_fps;
    int m_frameCount;
    Uint32 m_lastFPSUpdate;
    int m_saves;
    int m_loads;
    std::mt19937 m_rng;
    std::string m_stateMessage;
    SDL_GameController* m_controllers[2];
    bool m_mouseButtons[2];
    SDL_Point m_windowSize;
};

#endif // !GAME_LEVEL_H
