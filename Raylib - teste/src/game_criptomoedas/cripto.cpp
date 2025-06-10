#include <raylib.h>
#include "../game_screen.h"
#include <vector>

const int screenWidth = 800;
const int screenHeight = 450;

struct Coin {
    Vector2 position;
    float speed;
    bool collected;
};

void RunCriptoRush(int &currentScreen) {
    static Texture2D bgTexture = LoadTexture("../assets/btc_fundo.jpeg");
    static Texture2D coinTexture = LoadTexture("../assets/moeda_btc.png");
    static Texture2D walletTexture = LoadTexture("../assets/carteira.png"); // carteira do jogador
    static std::vector<Coin> coins;
    static bool initialized = false;

    static int fase = 1;
    static int score = 0;
    static float tempoFase = 0.0f;
    static bool faseFinalizada = false;
    static float duracaoFase = 20.0f;

    // Variáveis para mensagem de transição
    static bool mostrandoMensagem = false;
    static float tempoMensagem = 0.0f;

    if (!initialized) {
        coins.clear();
        for (int i = 0; i < 30; i++) {
            coins.push_back({
                Vector2{
                    (float)GetRandomValue(0, screenWidth - 20),
                    -static_cast<float>(GetRandomValue(20, 500))
                },
                (float)GetRandomValue(2, 5),
                false
            });
        }
        score = 0;
        tempoFase = 0.0f;
        fase = 1;
        duracaoFase = 20.0f;
        faseFinalizada = false;
        mostrandoMensagem = false;
        tempoMensagem = 0.0f;
        initialized = true;
    }

    if (WindowShouldClose()) {
        currentScreen = 0;
        initialized = false;
        return;
    }

    float deltaTime = GetFrameTime();
    if (!faseFinalizada && !mostrandoMensagem) {
        tempoFase += deltaTime;
    }

    BeginDrawing();

    ClearBackground(BLACK);
    DrawTexture(bgTexture, 0, 0, WHITE);

    DrawText(TextFormat("Fase: %d", fase), 10, 10, 20, YELLOW);
    DrawText(TextFormat("Score: %d / 20", score), 10, 40, 20, GREEN);
    DrawText(TextFormat("Tempo: %.1f / %.1f", tempoFase, duracaoFase), 10, 70, 20, WHITE);

    Vector2 playerPos = { (float)GetMouseX(), screenHeight - 50.0f };

    // Desenha a carteira no lugar do retângulo azul
    Rectangle sourceRec = { 0.0f, 0.0f, (float)walletTexture.width, (float)walletTexture.height };
    Rectangle destRec = { playerPos.x, playerPos.y, 40.0f, 40.0f };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(walletTexture, sourceRec, destRec, origin, 0.0f, WHITE);

    for (int i = 0; i < 30; i++) {
        if (!coins[i].collected) {
            Rectangle coinSource = { 0.0f, 0.0f, (float)coinTexture.width, (float)coinTexture.height };
            Rectangle coinDest = { coins[i].position.x, coins[i].position.y, 20.0f, 20.0f };
            DrawTexturePro(coinTexture, coinSource, coinDest, origin, 0.0f, WHITE);

            coins[i].position.y += coins[i].speed;

            if (coins[i].position.y > screenHeight) {
                coins[i].position = {
                    (float)GetRandomValue(0, screenWidth - 20),
                    -static_cast<float>(GetRandomValue(20, 300))
                };
                coins[i].collected = false;
            }

            if (CheckCollisionCircleRec(coins[i].position, 10,
                { playerPos.x, playerPos.y, 40, 40 })) {
                coins[i].collected = true;
                score++;
            }
        }
    }

    if (!faseFinalizada && !mostrandoMensagem) {
        if (score >= 20) {
            if (fase == 1) {
                mostrandoMensagem = true;
                tempoMensagem = 0.0f;
                faseFinalizada = true;
            }
            else if (fase == 2) {
                faseFinalizada = true;
            }
        }
        else if (tempoFase > duracaoFase) {
            faseFinalizada = true;
        }
    }

    if (mostrandoMensagem) {
        tempoMensagem += deltaTime;
        DrawText("Parabéns! Você passou para a fase 2!", screenWidth/2 - 180, screenHeight/2 - 20, 20, YELLOW);
        DrawText("Prepare-se para a próxima fase...", screenWidth/2 - 170, screenHeight/2 + 10, 20, YELLOW);

        if (tempoMensagem > 3.0f) {
            mostrandoMensagem = false;
            fase = 2;
            score = 0;
            tempoFase = 0.0f;
            duracaoFase = 10.0f;
            faseFinalizada = false;

            for (auto &coin : coins) {
                coin.position = {
                    (float)GetRandomValue(0, screenWidth - 20),
                    -static_cast<float>(GetRandomValue(20, 500))
                };
                coin.collected = false;
            }
        }
    }

    if (faseFinalizada && !mostrandoMensagem) {
        if (fase == 2 && score >= 20) {
            DrawText("Parabéns! Você venceu o CriptoRush e ganhou 20btc!", screenWidth/2 - 200, screenHeight/2, 20, GREEN);
        } else {
            DrawText("Tempo esgotado! Você perdeu!", screenWidth/2 - 150, screenHeight/2, 20, RED);
        }
        DrawText("Pressione ENTER para voltar ao menu.", screenWidth/2 - 180, screenHeight/2 + 40, 20, LIGHTGRAY);

        if (IsKeyPressed(KEY_ENTER)) {
            currentScreen = 0;
            initialized = false;
        }
    }

    EndDrawing();
}
