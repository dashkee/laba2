#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <locale.h>

#define SIZE 9
#define MINES 18

//структура ячейки
struct Cell {
    int x, y;
    int isMine;
    int isOpened;
    int isFlagged;
    int mineCount;
};

//структура поля
struct Field {
    Cell cells[SIZE][SIZE];
};

//структура игрока
struct Player {
    int score;
    int time;
};

//структура игры
struct Game {
    Field field;
    Player player;
    int isRunning;
    int gameStarted; // Флаг начала игры
};

//структура меню
struct Menu {
    char options[4][20];
    int selectedOption;
};

//структура настроеек
struct Settings {
    int size;
    int mines;
    int difficulty; // Уровень сложности (легкий, средний, сложный)
};

//структура игровой статистики
struct Statistics {
    int wins;
    int losses;
    long totalTime;
};

//структура игрового процесса
struct GameProgress {
    int x, y;
    int isOpened;
    int isFlagged;
};