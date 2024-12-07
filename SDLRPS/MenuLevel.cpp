#include "MenuLevel.h"
#include "Game.h"
#include "GameLevel.h"

MenuLevel::MenuLevel()
    : m_font(nullptr), m_music(nullptr), m_background(nullptr),
    m_currentPlayer(0), m_showError(false),
    m_whiteColor{ 255, 255, 255, 255 },
    m_yellowColor{ 255, 255, 0, 255 },
    m_redColor{ 255, 0, 0, 255 }, 
    m_textInputActive(false) {
}

MenuLevel::~MenuLevel()
{
    Clean();
}

bool MenuLevel::Load() {
    m_font = TTF_OpenFont(Game::GetAssetPath("Fonts/arial.ttf").c_str(), 20);
    if (!m_font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return false;
    }

    m_music = Mix_LoadMUS(Game::GetAssetPath("Audio/Music/Track1.mp3").c_str());
    if (!m_music) {
        SDL_Log("Failed to load music: %s", Mix_GetError());
        return false;
    }

    m_background = Game::Instance().LoadTGA(Game::GetAssetPath("Textures/Background.tga").c_str());
    if (!m_background) {
        SDL_Log("Failed to load background: %s", SDL_GetError());
        return false;
    }

    Mix_PlayMusic(m_music, -1);

    // Start text input
    SDL_StartTextInput();
    m_textInputActive = true;
    SDL_Log("Text input started");

    return true;
}

void MenuLevel::Update(float deltaTime) {
    // Implementation remains the same
}

void MenuLevel::Render() {
    SDL_Renderer* renderer = Game::Instance().GetRenderer();

    // Render background
    SDL_RenderCopy(renderer, m_background, nullptr, nullptr);

    // Render player labels
    RenderText("", m_whiteColor, -100, 50, 30);
    RenderText("", m_whiteColor, -100, 200, 30);

    // Render existing nicknames
    for (size_t i = 0; i < m_nicknames.size(); ++i) {
        RenderText(m_nicknames[i], m_yellowColor, 50, 50 + static_cast<int>(i) * 100, 40);
    }

    // Render current input
    if (m_currentPlayer < 2) {
        std::string inputText = m_currentInput;
        // Add a blinking cursor
        if (SDL_GetTicks() / 500 % 2 == 0) {
            inputText += "_";
        }
        RenderText(inputText, m_yellowColor, 50, 65 + m_currentPlayer * 140, 40);
    }

    // Render error message
    if (m_showError) {
        RenderText("Nickname cannot be empty!", m_redColor, 50, 250, 40);
    }

    // Render instructions at the bottom
    RenderText("Quit [ESC] | Next Nickname [Return]", m_whiteColor, 10, 1040, 20);

    // Render character limit info
    if (m_currentPlayer < 2) {
        std::string charCount = "Characters: " + std::to_string(m_currentInput.length()) + "/20";
        RenderText(charCount, m_whiteColor, 800, 1040, 20);
    }
}

void MenuLevel::HandleEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            Game::Instance().Quit();
        }
        else if (event.key.keysym.sym == SDLK_RETURN) {
            if (m_currentInput.empty()) {
                m_showError = true;
            }
            else if (m_currentInput.length() <= 20) {
                m_nicknames.push_back(m_currentInput);
                m_currentInput.clear();
                m_currentPlayer++;
                m_showError = false;

                if (m_currentPlayer == 2) {
                    SDL_StopTextInput(); // Stop text input before changing levels
                    Game::Instance().ChangeLevel(std::make_unique<GameLevel>(m_nicknames));
                }
            }
        }
        else if (event.key.keysym.sym == SDLK_BACKSPACE && !m_currentInput.empty()) {
            m_currentInput.pop_back();
            m_showError = false;
        }
    }
    else if (event.type == SDL_TEXTINPUT && m_currentPlayer < 2) {
        if (m_currentInput.length() < 20) {
            m_currentInput += event.text.text;
            m_showError = false;
        }
    }
}

void MenuLevel::RenderText(const std::string& text, SDL_Color color, int x, int y, int fontSize) {
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

void MenuLevel::Clean() 
{
    SDL_StopTextInput();  // Stop text input when cleaning up

    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    if (m_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }
    if (m_background) {
        SDL_DestroyTexture(m_background);
        m_background = nullptr;
    }
}