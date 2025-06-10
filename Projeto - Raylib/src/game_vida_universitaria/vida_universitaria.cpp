#include <raylib.h>
#include "vida_universitaria.h"
#include "../game_screen.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>


// Estruturas do jogo
struct Player {
    Vector2 position;
    Vector2 velocity;
    bool isJumping;
    bool isInvulnerable;
    float invulnerabilityTimer;
    Rectangle rect;
};

struct Obstacle {
    Vector2 position;
    Rectangle rect;
    Color color;
    std::string type;
    bool active;
    bool scored; // Nova variável para controlar se já pontuou
};

struct PowerUp {
    Vector2 position;
    Rectangle rect;
    Color color;
    std::string type;
    bool active;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    std::string text;
    float life;
    Color color;
};

enum UniversityGameState {
    UNI_MENU,
    UNI_PLAYING,
    UNI_PAUSED,
    UNI_GAME_OVER
};

// Constantes ajustadas para 800x450
const int GAME_WIDTH = 800;
const int GAME_HEIGHT = 450;
const float GRAVITY = 600.0f;
const float JUMP_FORCE = -250.0f;
const float GROUND_Y = GAME_HEIGHT - 80;
const float GAME_SPEED = 150.0f;

// Variáveis globais do jogo
static UniversityGameState gameState = UNI_MENU;
static Player player;
static std::vector<Obstacle> obstacles;
static std::vector<PowerUp> powerUps;
static std::vector<Particle> particles;
static int lives = 3;
static int score = 0;
static float currentSpeed = GAME_SPEED;
static float spawnTimer = 0.0f;
static float powerUpTimer = 0.0f;
static float coffeeTimer = 0.0f;
static float energyTimer = 0.0f;
static bool hasCoffee = false;
static bool hasEnergy = false;

// Texturas dos power-ups
static Texture2D cafeTexture;
static Texture2D energiaTexture;
static Texture2D tempoTexture;
static bool texturesLoaded = false;

// Mensagens brasileiras
std::vector<std::string> defeatMessages = {
    "REPROVADO!", "PEGOU DP!", "PERDEU PRAZO!", "BOMBA!", "PEGOU!", "ZERO!"
};

std::vector<std::string> obstacleTypes = {
    "PROVA", "TCC", "PRAZO", "TRABALHO"
};

void LoadPowerUpTextures() {
    if (!texturesLoaded) {
        cafeTexture = LoadTexture("../assets/cafe_pixelado.jpeg");
        energiaTexture = LoadTexture("../assets/energia_pixelada.png");
        tempoTexture = LoadTexture("../assets/tempo_pixelado.png");
        texturesLoaded = true;
    }
}

void UnloadPowerUpTextures() {
    if (texturesLoaded) {
        UnloadTexture(cafeTexture);
        UnloadTexture(energiaTexture);
        UnloadTexture(tempoTexture);
        texturesLoaded = false;
    }
}

void InitUniversityGame() {
    LoadPowerUpTextures();
    
    player.position = {80, GROUND_Y - 40};
    player.velocity = {0, 0};
    player.isJumping = false;
    player.isInvulnerable = false;
    player.invulnerabilityTimer = 0.0f;
    player.rect = {player.position.x, player.position.y, 30, 40};
    
    obstacles.clear();
    powerUps.clear();
    particles.clear();
    
    lives = 3;
    score = 0;
    currentSpeed = GAME_SPEED;
    spawnTimer = 0.0f;
    powerUpTimer = 0.0f;
    coffeeTimer = 0.0f;
    energyTimer = 0.0f;
    hasCoffee = false;
    hasEnergy = false;
}

void CreateParticle(Vector2 pos, std::string text) {
    Particle p;
    p.position = pos;
    p.velocity = {(float)(rand() % 100 - 50), (float)(rand() % 50 - 100)};
    p.text = text;
    p.life = 1.5f;
    p.color = RED;
    particles.push_back(p);
}

void SpawnObstacle() {
    Obstacle obs;
    obs.position = {GAME_WIDTH, GROUND_Y - 30};
    obs.rect = {obs.position.x, obs.position.y, 40, 30};
    obs.active = true;
    obs.scored = false; // Inicializa como não pontuado
    obs.type = obstacleTypes[rand() % obstacleTypes.size()];
    
    if (obs.type == "PROVA") obs.color = RED;
    else if (obs.type == "TCC") obs.color = BLACK;
    else if (obs.type == "PRAZO") obs.color = ORANGE;
    else obs.color = BROWN;
    
    obstacles.push_back(obs);
}

void SpawnPowerUp() {
    PowerUp pu;
    pu.position = {GAME_WIDTH, GROUND_Y - 50};
    pu.rect = {pu.position.x, pu.position.y, 30, 30};
    pu.active = true;
    
    int type = rand() % 3;
    if (type == 0) { pu.type = "CAFE"; pu.color = BROWN; }
    else if (type == 1) { pu.type = "ENERGIA"; pu.color = YELLOW; }
    else { pu.type = "TEMPO"; pu.color = GREEN; }
    
    powerUps.push_back(pu);
}

void UpdatePlayer(float deltaTime) {
    // Pulo
    if (IsKeyPressed(KEY_SPACE) && !player.isJumping) {
        player.velocity.y = hasEnergy ? JUMP_FORCE * 1.2f : JUMP_FORCE;
        player.isJumping = true;
    }
    
    // Física
    player.velocity.y += GRAVITY * deltaTime;
    player.position.y += player.velocity.y * deltaTime;
    
    // Colisão com chão
    if (player.position.y >= GROUND_Y - 40) {
        player.position.y = GROUND_Y - 40;
        player.velocity.y = 0;
        player.isJumping = false;
    }
    
    // Invulnerabilidade
    if (player.isInvulnerable) {
        player.invulnerabilityTimer -= deltaTime;
        if (player.invulnerabilityTimer <= 0) {
            player.isInvulnerable = false;
        }
    }
    
    // Power-ups
    if (hasCoffee) {
        coffeeTimer -= deltaTime;
        if (coffeeTimer <= 0) hasCoffee = false;
    }
    if (hasEnergy) {
        energyTimer -= deltaTime;
        if (energyTimer <= 0) hasEnergy = false;
    }
    
    player.rect.x = player.position.x;
    player.rect.y = player.position.y;
}

void UpdateObstacles(float deltaTime) {
    float speed = hasCoffee ? currentSpeed * 1.3f : currentSpeed;
    
    for (auto& obs : obstacles) {
        if (obs.active) {
            obs.position.x -= speed * deltaTime;
            obs.rect.x = obs.position.x;
            
            // Verifica se o obstáculo foi ultrapassado (pontuação)
            if (!obs.scored && obs.position.x + obs.rect.width < player.position.x) {
                score++;
                obs.scored = true;
                CreateParticle({obs.position.x, obs.position.y - 20}, "+1!");
            }
            
            if (obs.position.x < -50) obs.active = false;
        }
    }
    
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),
        [](const Obstacle& obs) { return !obs.active; }), obstacles.end());
}

void UpdatePowerUps(float deltaTime) {
    float speed = hasCoffee ? currentSpeed * 1.3f : currentSpeed;
    
    for (auto& pu : powerUps) {
        if (pu.active) {
            pu.position.x -= speed * deltaTime;
            pu.rect.x = pu.position.x;
            if (pu.position.x < -50) pu.active = false;
        }
    }
    
    powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(),
        [](const PowerUp& pu) { return !pu.active; }), powerUps.end());
}

void UpdateParticles(float deltaTime) {
    for (auto& p : particles) {
        p.position.x += p.velocity.x * deltaTime;
        p.position.y += p.velocity.y * deltaTime;
        p.life -= deltaTime;
    }
    
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.life <= 0; }), particles.end());
}

void CheckCollisions() {
    if (player.isInvulnerable) return;
    
    // Colisão com obstáculos
    for (auto& obs : obstacles) {
        if (obs.active && CheckCollisionRecs(player.rect, obs.rect)) {
            lives--;
            player.isInvulnerable = true;
            player.invulnerabilityTimer = 1.5f;
            
            CreateParticle(player.position, defeatMessages[rand() % defeatMessages.size()]);
            obs.active = false;
            
            if (lives <= 0) {
                gameState = UNI_GAME_OVER;
            }
            break;
        }
    }
    
    // Colisão com power-ups
    for (auto& pu : powerUps) {
        if (pu.active && CheckCollisionRecs(player.rect, pu.rect)) {
            if (pu.type == "CAFE") {
                hasCoffee = true;
                coffeeTimer = 3.0f;
                CreateParticle(pu.position, "SPEED!");
            } else if (pu.type == "ENERGIA") {
                hasEnergy = true;
                energyTimer = 3.0f;
                CreateParticle(pu.position, "JUMP!");
            } else {
                score += 5; // Power-up de tempo dá 5 pontos extras
                CreateParticle(pu.position, "+5!");
            }
            pu.active = false;
            break;
        }
    }
}

void UpdateUniversityGame(float deltaTime) {
    if (gameState != UNI_PLAYING) return;
    
    UpdatePlayer(deltaTime);
    UpdateObstacles(deltaTime);
    UpdatePowerUps(deltaTime);
    UpdateParticles(deltaTime);
    CheckCollisions();
    
    // Spawn obstáculos
    spawnTimer += deltaTime;
    if (spawnTimer >= 1.8f) {
        SpawnObstacle();
        spawnTimer = 0.0f;
    }
    
    // Spawn power-ups
    powerUpTimer += deltaTime;
    if (powerUpTimer >= 6.0f) {
        SpawnPowerUp();
        powerUpTimer = 0.0f;
    }
    
    // Aumentar velocidade gradualmente
    currentSpeed += 8.0f * deltaTime;
}

void DrawUniversityGame() {
    // Desenhar chão
    DrawRectangle(0, GROUND_Y, GAME_WIDTH, GAME_HEIGHT - GROUND_Y, DARKGREEN);
    
    if (gameState == UNI_PLAYING || gameState == UNI_PAUSED) {
        // Player
        Color playerColor = player.isInvulnerable && ((int)(player.invulnerabilityTimer * 10) % 2) ? GRAY : WHITE;
        DrawRectangleRec(player.rect, playerColor);
        DrawText("🏃", (int)player.position.x - 5, (int)player.position.y - 5, 20, playerColor);
        
        // Obstáculos
        for (const auto& obs : obstacles) {
            if (obs.active) {
                DrawRectangleRec(obs.rect, obs.color);
                DrawText(obs.type.c_str(), (int)obs.position.x - 10, (int)obs.position.y - 15, 10, WHITE);
            }
        }
        
        // Power-ups com imagens
        for (const auto& pu : powerUps) {
            if (pu.active && texturesLoaded) {
                Rectangle destRect = {pu.position.x, pu.position.y, 30, 30};
                
                if (pu.type == "CAFE" && cafeTexture.id != 0) {
                    DrawTexturePro(cafeTexture, 
                                 {0, 0, (float)cafeTexture.width, (float)cafeTexture.height}, 
                                 destRect, 
                                 {0, 0}, 0.0f, WHITE);
                } else if (pu.type == "ENERGIA" && energiaTexture.id != 0) {
                    DrawTexturePro(energiaTexture, 
                                 {0, 0, (float)energiaTexture.width, (float)energiaTexture.height}, 
                                 destRect, 
                                 {0, 0}, 0.0f, WHITE);
                } else if (pu.type == "TEMPO" && tempoTexture.id != 0) {
                    DrawTexturePro(tempoTexture, 
                                 {0, 0, (float)tempoTexture.width, (float)tempoTexture.height}, 
                                 destRect, 
                                 {0, 0}, 0.0f, WHITE);
                } else {
                    // Fallback para caso a textura não carregue
                    DrawRectangleRec(pu.rect, pu.color);
                    if (pu.type == "CAFE") DrawText("☕", (int)pu.position.x, (int)pu.position.y, 15, WHITE);
                    else if (pu.type == "ENERGIA") DrawText("⚡", (int)pu.position.x, (int)pu.position.y, 15, WHITE);
                    else DrawText("⏰", (int)pu.position.x, (int)pu.position.y, 15, WHITE);
                }
            }
        }
        
        // Partículas
        for (const auto& p : particles) {
            Color particleColor = p.color;
            if (p.text == "+1!" || p.text == "+5!") {
                particleColor = GREEN; // Partículas de pontuação em verde
            }
            DrawText(p.text.c_str(), (int)p.position.x, (int)p.position.y, 15, particleColor);
        }
        
        // UI
        DrawText(("VIDAS: " + std::to_string(lives)).c_str(), 10, 10, 16, WHITE);
        DrawText(("PONTOS: " + std::to_string(score)).c_str(), 10, 35, 16, WHITE);
        
        if (hasCoffee) DrawText(("☕ " + std::to_string((int)coffeeTimer + 1) + "s").c_str(), 10, 60, 14, BROWN);
        if (hasEnergy) DrawText(("⚡ " + std::to_string((int)energyTimer + 1) + "s").c_str(), 10, 80, 14, YELLOW);
        
        if (gameState == UNI_PAUSED) {
            DrawText("PAUSADO", GAME_WIDTH/2 - 50, GAME_HEIGHT/2, 30, YELLOW);
            DrawText("TAB para continuar", GAME_WIDTH/2 - 80, GAME_HEIGHT/2 + 40, 16, WHITE);
        }
    } 
    else if (gameState == UNI_MENU) {
        DrawText("CORRIDA UNIVERSITARIA", GAME_WIDTH/2 - 150, GAME_HEIGHT/2 - 80, 24, WHITE);
        DrawText("Sobreviva aos desafios da universidade!", GAME_WIDTH/2 - 140, GAME_HEIGHT/2 - 40, 14, GRAY);
        DrawText("ESPACO - Pular", GAME_WIDTH/2 - 60, GAME_HEIGHT/2 - 10, 16, WHITE);
        DrawText("TAB - Pausar", GAME_WIDTH/2 - 55, GAME_HEIGHT/2 + 15, 16, WHITE);
        DrawText("ENTER para comecar", GAME_WIDTH/2 - 80, GAME_HEIGHT/2 + 50, 16, GREEN);
        DrawText("Q para voltar ao menu", GAME_WIDTH/2 - 90, GAME_HEIGHT/2 + 75, 14, LIGHTGRAY);
    } 
    else if (gameState == UNI_GAME_OVER) {
        DrawText("GAME OVER", GAME_WIDTH/2 - 80, GAME_HEIGHT/2 - 60, 30, RED);
        DrawText(("PONTUACAO: " + std::to_string(score)).c_str(), GAME_WIDTH/2 - 70, GAME_HEIGHT/2 - 20, 16, WHITE);
        DrawText("Nao conseguiu se formar...", GAME_WIDTH/2 - 90, GAME_HEIGHT/2 + 5, 14, GRAY);
        DrawText("R para recomecar", GAME_WIDTH/2 - 70, GAME_HEIGHT/2 + 35, 16, GREEN);
        DrawText("Q para menu principal", GAME_WIDTH/2 - 85, GAME_HEIGHT/2 + 60, 14, LIGHTGRAY);
    }
}

void RunVidaUniversitaria(int& currentScreen) {
    static bool initialized = false;
    
    if (!initialized) {
        srand((unsigned int)time(NULL));
        InitUniversityGame();
        initialized = true;
    }
    
    float deltaTime = GetFrameTime();
    
    // Input handling
    if (gameState == UNI_MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = UNI_PLAYING;
            InitUniversityGame();
        }
        if (IsKeyPressed(KEY_Q)) {
            currentScreen = 0; // Voltar ao menu principal
            gameState = UNI_MENU;
            UnloadPowerUpTextures();
            initialized = false;
        }
    } else if (gameState == UNI_PLAYING) {
        if (IsKeyPressed(KEY_TAB)) {
            gameState = UNI_PAUSED;
        }
        UpdateUniversityGame(deltaTime);
    } else if (gameState == UNI_PAUSED) {
        if (IsKeyPressed(KEY_TAB)) {
            gameState = UNI_PLAYING;
        }
    } else if (gameState == UNI_GAME_OVER) {
        if (IsKeyPressed(KEY_R)) {
            gameState = UNI_PLAYING;
            InitUniversityGame();
        } else if (IsKeyPressed(KEY_Q)) {
            currentScreen = 0; // Voltar ao menu principal
            gameState = UNI_MENU;
            UnloadPowerUpTextures();
            initialized = false;
        }
    }
    
    // Drawing - DEVE fazer BeginDrawing e EndDrawing
    BeginDrawing();
    ClearBackground(DARKBLUE);
    DrawUniversityGame();
    EndDrawing();
}