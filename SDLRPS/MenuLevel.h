#ifndef MENU_LEVEL_H
#define MENU_LEVEL_H

#include "StandardIncludes.h"
#include "Level.h"

class MenuLevel : public Level {
public:
    MenuLevel();
    virtual ~MenuLevel();
    bool Load() override;
    void Update(float deltaTime) override;
    void Render() override;
    void HandleEvents(SDL_Event& event) override;
    void Clean() override;

private:
    void RenderText(const std::string& text, SDL_Color color, int x, int y, int fontSize = 20);

    TTF_Font* m_font;
    Mix_Music* m_music;
    SDL_Texture* m_background;
    std::vector<std::string> m_nicknames;
    std::string m_currentInput;
    int m_currentPlayer;
    bool m_showError;
    SDL_Color m_whiteColor;
    SDL_Color m_yellowColor;
    SDL_Color m_redColor;
    bool m_textInputActive;  // Add this to track text input state
};

#endif // !MENU_LEVEL_H
