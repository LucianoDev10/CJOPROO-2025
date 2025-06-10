#include <raylib.h>
#include <vector>
#include <string>

// Incluindo headers dos jogos
#include "game_vida_universitaria/vida_universitaria.h"
#include "game_criptomoedas/cripto.h"
#include "game_screen.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Menu Principal de Jogos");
    SetTargetFPS(60);

    Texture2D background = LoadTexture("../assets/fundo_pixelado.jpg");

    int currentScreen = MENU;
    int selectedOption = 0;

    std::vector<std::string> menuOptions = {
        "1. Vida Universitaria",
        "2. CriptoRush",
        "Sair"
    };
    int numOptions = menuOptions.size();

    while (!WindowShouldClose() && currentScreen != EXITING)
    {
        switch (currentScreen)
        {
            case MENU:
            {
                // Atualiza input do menu
                if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % numOptions;
                if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + numOptions) % numOptions;

                if (IsKeyPressed(KEY_ENTER)) {
                    if (selectedOption == 0) currentScreen = VIDA_UNIVERSITARIA;        // Corrigido: opção 0 vai para VIDA_UNIVERSITARIA
                    else if (selectedOption == 1) currentScreen = CRIPTO_RUSH;         // Corrigido: opção 1 vai para CRIPTO_RUSH
                    else if (selectedOption == 2) currentScreen = EXITING;             // Corrigido: opção 2 vai para EXITING
                }

                // Desenha só o menu aqui
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawTexturePro(background,
                    (Rectangle){ 0, 0, (float)background.width, (float)background.height },
                    (Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
                    (Vector2){ 0, 0 }, 0.0f, WHITE);

                DrawText("Escolha seu Jogo", screenWidth / 2 - MeasureText("Escolha seu Jogo", 40) / 2, 80, 40, YELLOW);

                for (int i = 0; i < numOptions; i++) {
                    Color textColor = (i == selectedOption) ? YELLOW : LIGHTGRAY;
                    DrawText(menuOptions[i].c_str(), screenWidth / 2 - MeasureText(menuOptions[i].c_str(), 30) / 2, 180 + i * 40, 30, textColor);
                }
                EndDrawing();
            }
            break;

            case VIDA_UNIVERSITARIA:
                RunVidaUniversitaria(currentScreen);
                break;

            case CRIPTO_RUSH:
                RunCriptoRush(currentScreen);
                break;

            default:
                break;
        }
    }

    UnloadTexture(background);
    CloseWindow();
    return 0;
}