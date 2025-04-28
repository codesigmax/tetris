#include "Game.h"

Game::Game() : board(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0)), 
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

Game::~Game() {
    CloseWindow();
}

void Game::run() {
    while (!WindowShouldClose()) {
        update();
        draw();
        
        // 游戏结束时检查是否重新开始
        if (gameOver && IsKeyPressed(KEY_ENTER)) {
            resetGame();
        }
    }
}

void Game::resetGame() {
    // 清空游戏板
    for (auto& row : board) {
        std::fill(row.begin(), row.end(), 0);
    }
    gameOver = false;
    currentX = BOARD_WIDTH/2 - 2;
    currentY = 0;
    spawnNewPiece();
}

void Game::rotatePiece() {
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

void Game::spawnNewPiece() {
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

bool Game::canMove(int newX, int newY) {
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

void Game::update() {
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
            clearLines();
            spawnNewPiece();
        }
    }
}

void Game::draw() {
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
    if (!gameOver) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currentPiece[i][j]) {
                    DrawRectangle(BOARD_OFFSET_X + (currentX + j) * CELL_SIZE,
                                 BOARD_OFFSET_Y + (currentY + i) * CELL_SIZE,
                                 CELL_SIZE - 1, CELL_SIZE - 1, RED);
                }
            }
        }
    }

    // 绘制游戏结束提示
    if (gameOver) {
        const char* text = "Game Over! Press Enter to restart";
        int fontSize = 20;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, SCREEN_WIDTH/2 - textWidth/2,
                 SCREEN_HEIGHT/2 - fontSize/2, fontSize, BLACK);
    }

    EndDrawing();
}

void Game::clearLines() {
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
        bool isLineFull = true;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (!board[i][j]) {
                isLineFull = false;
                break;
            }
        }
        if (isLineFull) {
            // 移动上面的行下来
            for (int k = i; k > 0; k--) {
                board[k] = board[k-1];
            }
            // 清空最上面的行
            std::fill(board[0].begin(), board[0].end(), 0);
            // 由于行已经下移，需要重新检查当前行
            i++;
        }
    }
}