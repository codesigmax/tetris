#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

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
const int BOARD_OFFSET_X = (SCREEN_WIDTH - BOARD_WIDTH * CELL_SIZE) / 2 - 100;
const int BOARD_OFFSET_Y = (SCREEN_HEIGHT - BOARD_HEIGHT * CELL_SIZE) / 2;

// 下一个方块预览区域的位置
const int PREVIEW_OFFSET_X = BOARD_OFFSET_X + BOARD_WIDTH * CELL_SIZE + 50;
const int PREVIEW_OFFSET_Y = BOARD_OFFSET_Y + 50;

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
    Game();
    ~Game();
    void run();

private:
    std::vector<std::vector<int>> board;
    std::vector<std::vector<bool>> currentPiece;
    std::vector<std::vector<bool>> nextPiece;  // 下一个方块
    int currentX, currentY;
    bool gameOver;
    std::random_device rd;
    std::mt19937 gen{rd()};
    float lastFallTime;
    float lastMoveTime;
    int score;  // 游戏分数

    void resetGame();
    void rotatePiece();
    void generateNewPiece(std::vector<std::vector<bool>>& piece);
    void spawnNewPiece();
    bool canMove(int newX, int newY);
    void update();
    void draw();
    void clearLines();
};

#endif // TETRIS_GAME_H