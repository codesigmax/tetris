#include <raylib.h>
#include <vector>
#include <random>
#include <array>

// 游戏常量定义
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = 30;
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;
const int BOARD_OFFSET_X = (SCREEN_WIDTH - BOARD_WIDTH * CELL_SIZE) / 2;
const int BOARD_OFFSET_Y = (SCREEN_HEIGHT - BOARD_HEIGHT * CELL_SIZE) / 2;

// 方块形状定义
const std::array<std::array<std::array<bool, 4>, 4>, 7> TETROMINOES = {
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {true, true, true, true},
        {false, false, false, false},
        {false, false, false, false}
    }},
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {false, true, true, false},
        {false, true, true, false},
        {false, false, false, false}
    }},
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {false, true, false, false},
        {true, true, true, false},
        {false, false, false, false}
    }},
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {false, true, true, false},
        {true, true, false, false},
        {false, false, false, false}
    }},
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {true, true, false, false},
        {false, true, true, false},
        {false, false, false, false}
    }},
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {true, false, false, false},
        {true, true, true, false},
        {false, false, false, false}
    }},
    std::array<std::array<bool, 4>, 4>{{
        {false, false, false, false},
        {false, false, true, false},
        {true, true, true, false},
        {false, false, false, false}
    }}
};

class Game {
public:
    Game() : board(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0)), 
             currentPiece(4, std::vector<bool>(4)), 
             currentX(BOARD_WIDTH/2 - 2),
             currentY(0),
             gameOver(false),
             lastFallTime(0),
             lastMoveTime(0) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
        SetTargetFPS(60);
        spawnNewPiece();
    }

    ~Game() {
        CloseWindow();
    }

    void run() {
        while (!WindowShouldClose()) {
            update();
            draw();
            
            // 游戏结束时检查是否重新开始
            if (gameOver && IsKeyPressed(KEY_ENTER)) {
                resetGame();
            }
        }
    }

private:
    std::vector<std::vector<int>> board;
    std::vector<std::vector<bool>> currentPiece;
    int currentX, currentY;
    bool gameOver;
    std::random_device rd;
    std::mt19937 gen{rd()};

    void resetGame() {
        // 清空游戏板
        for (auto& row : board) {
            std::fill(row.begin(), row.end(), 0);
        }
        gameOver = false;
        currentX = BOARD_WIDTH/2 - 2;
        currentY = 0;
        spawnNewPiece();
    }
    float lastFallTime;
    float lastMoveTime;

    void rotatePiece() {
        std::vector<std::vector<bool>> rotated(4, std::vector<bool>(4));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                rotated[j][3-i] = currentPiece[i][j];
            }
        }
        
        // 保存当前状态
        auto tempPiece = currentPiece;
        currentPiece = rotated;
        
        // 如果旋转后的位置无效，则恢复原状态
        if (!canMove(currentX, currentY)) {
            currentPiece = tempPiece;
        }
    }

    void spawnNewPiece() {
        std::uniform_int_distribution<> dis(0, TETROMINOES.size() - 1);
        int index = dis(gen);
        currentPiece = std::vector<std::vector<bool>>(4, std::vector<bool>(4));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                currentPiece[i][j] = TETROMINOES[index][i][j];
            }
        }
        currentX = BOARD_WIDTH/2 - 2;
        currentY = 0;

        if (!canMove(currentX, currentY)) {
            gameOver = true;
        }
    }

    bool canMove(int newX, int newY) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currentPiece[i][j]) {
                    int x = newX + j;
                    int y = newY + i;
                    if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT) return false;
                    if (y >= 0 && board[y][x]) return false;
                }
            }
        }
        return true;
    }

    void update() {
        float currentTime = GetTime();

        // 处理旋转
        if (IsKeyPressed(KEY_UP)) {
            rotatePiece();
        }

        // 处理左右移动，支持按住持续移动
        if (IsKeyDown(KEY_LEFT) && currentTime - lastMoveTime > 0.1f && canMove(currentX - 1, currentY)) {
            currentX--;
            lastMoveTime = currentTime;
        }
        if (IsKeyDown(KEY_RIGHT) && currentTime - lastMoveTime > 0.1f && canMove(currentX + 1, currentY)) {
            currentX++;
            lastMoveTime = currentTime;
        }

        // 处理快速下落
        if (IsKeyDown(KEY_DOWN)) {
            if (currentTime - lastFallTime > 0.05f && canMove(currentX, currentY + 1)) {
                currentY++;
                lastFallTime = currentTime;
            }
        }

        static float dropTimer = 0;
        dropTimer += GetFrameTime();
        if (dropTimer >= 1.0f) {
            dropTimer = 0;
            if (canMove(currentX, currentY + 1)) {
                currentY++;
            } else {
                // 固定当前方块到游戏板上
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (currentPiece[i][j] && currentY + i >= 0) {
                            board[currentY + i][currentX + j] = 1;
                        }
                    }
                }
                // 检查并清除完整的行
                checkLines();
                // 生成新的方块
                spawnNewPiece();
            }
        }
    }

    void checkLines() {
        for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
            bool lineComplete = true;
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (!board[i][j]) {
                    lineComplete = false;
                    break;
                }
            }
            if (lineComplete) {
                // 移除完整的行
                for (int k = i; k > 0; k--) {
                    board[k] = board[k-1];
                }
                board[0] = std::vector<int>(BOARD_WIDTH, 0);
            }
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 绘制游戏板边框
        DrawRectangleLines(BOARD_OFFSET_X - 1, BOARD_OFFSET_Y - 1,
                          BOARD_WIDTH * CELL_SIZE + 2,
                          BOARD_HEIGHT * CELL_SIZE + 2, BLACK);

        // 绘制已固定的方块
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (board[i][j]) {
                    DrawRectangle(BOARD_OFFSET_X + j * CELL_SIZE,
                                 BOARD_OFFSET_Y + i * CELL_SIZE,
                                 CELL_SIZE - 1, CELL_SIZE - 1, BLUE);
                }
            }
        }

        // 绘制当前方块
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currentPiece[i][j]) {
                    DrawRectangle(BOARD_OFFSET_X + (currentX + j) * CELL_SIZE,
                                 BOARD_OFFSET_Y + (currentY + i) * CELL_SIZE,
                                 CELL_SIZE - 1, CELL_SIZE - 1, RED);
                }
            }
        }

        if (gameOver) {
            std::string gameOverText = "Game Over";
            std::string restartText = "Press Enter to Restart";
            int fontSize = 40;
            int smallFontSize = 20;
            
            int gameOverWidth = MeasureText(gameOverText.c_str(), fontSize);
            int restartWidth = MeasureText(restartText.c_str(), smallFontSize);
            
            DrawText(gameOverText.c_str(), SCREEN_WIDTH/2 - gameOverWidth/2,
                     SCREEN_HEIGHT/2 - fontSize/2, fontSize, BLACK);
            DrawText(restartText.c_str(), SCREEN_WIDTH/2 - restartWidth/2,
                     SCREEN_HEIGHT/2 + fontSize, smallFontSize, DARKGRAY);
        }

        EndDrawing();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}