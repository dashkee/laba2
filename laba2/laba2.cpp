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

//функция открытия соседних пустых клеток
void EmptyCells(struct Game* game, int x, int y) {
    // Проверка границ
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return;

    // Получение ячейки
    struct Cell* cell = &game->field.cells[x][y];

    if (cell->isOpened || cell->isFlagged || cell->mineCount != 0) return;

    // Открытие ячейки
    cell->isOpened = 1;

    // Если ячейка пустая, рекурсивно открываем соседние ячейки
    if (cell->mineCount == 0) {
        if (x > 0) EmptyCells(game, x - 1, y);   // верх
        if (x + 1 < SIZE) EmptyCells(game, x + 1, y);   // низ
        if (y > 0) EmptyCells(game, x, y - 1);   // влево
        if (y + 1 < SIZE) EmptyCells(game, x, y + 1);   // вправо
        if (x > 0 && y > 0) EmptyCells(game, x - 1, y - 1); // верх-влево
        if (x > 0 && y + 1 < SIZE) EmptyCells(game, x - 1, y + 1); // верх-вправо
        if (x + 1 < SIZE && y > 0) EmptyCells(game, x + 1, y - 1); // низ-влево
        if (x + 1 < SIZE && y + 1 < SIZE) EmptyCells(game, x + 1, y + 1); // низ-вправо
    }
}

//функция инициализации игры
void initGame(Game* game, Settings settings) {
    initField(&game->field);
    game->player.score = 0;
    game->player.time = 0;
    game->isRunning = 1;
    game->gameStarted = 0;
}

//функция инициализации игрового поля
void initField(Field* field) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            field->cells[i][j].x = i;
            field->cells[i][j].y = j;
            field->cells[i][j].isMine = 0;
            field->cells[i][j].isOpened = 0;
            field->cells[i][j].isFlagged = 0;
            field->cells[i][j].mineCount = 0;
        }
    }
}

//функция растановки мин
void placeMines(struct Field* field, int x, int y) {
    srand(time(NULL));
    int minesPlaced = 0;
    while (minesPlaced < MINES) {
        int _x = rand() % SIZE;
        int _y = rand() % SIZE;
        if (_x == x && _y == y) continue; // Не ставим мину на первую клетку
        if (!field->cells[_x][_y].isMine) {
            field->cells[_x][_y].isMine = 1;
            minesPlaced++;
        }
    }
}

//функция подсчета мин в соседних ячейчах
void countMines(Field* field) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (field->cells[i][j].isMine) continue;
            int count = 0;
            for (int k = i - 1; k <= i + 1; k++) {
                for (int l = j - 1; l <= j + 1; l++) {
                    if (k >= 0 && k < SIZE && l >= 0 && l < SIZE && field->cells[k][l].isMine) {
                        count++;
                    }
                }
            }
            field->cells[i][j].mineCount = count;
        }
    }
}

//функция печати игрового поля
void printField(Field* field) {
    printf("   ");
    for (int i = 0; i < SIZE; i++) printf("%d ", i);
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d  ", i);
        for (int j = 0; j < SIZE; j++) {
            if (field->cells[i][j].isOpened) {
                if (field->cells[i][j].isMine) printf("M ");
                else if (field->cells[i][j].mineCount > 0) printf("%d ", field->cells[i][j].mineCount);
                else printf("0 ");
            }
            else if (field->cells[i][j].isFlagged) printf("F ");
            else printf("# ");
        }
        printf("\n");
    }
}

//функция открытия ячейки
void openCell(struct Game* game, int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || game->field.cells[x][y].isOpened || game->field.cells[x][y].isFlagged) return;
    game->field.cells[x][y].isOpened = 1;
    if (game->field.cells[x][y].isMine) gameOver(game);
    if (game->field.cells[x][y].mineCount == 0)
    {
        EmptyCells(game, x, y);
    }
}

//функция постановки флага
void flagCell(Game* game, int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || game->field.cells[x][y].isOpened) return;
    game->field.cells[x][y].isFlagged = !game->field.cells[x][y].isFlagged;
}

//функция проверки на победу
void Win(Game* game) {
    int openedCells = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (game->field.cells[i][j].isOpened || game->field.cells[i][j].isMine) openedCells++;
        }
    }
    if (openedCells == SIZE * SIZE) {
        game->isRunning = 0;
        game->player.score = 100;
        printf("Поздравляем! Вы победили!\n");
    }
}

//функция окончания игры
void gameOver(Game* game) {
    game->isRunning = 0;
    printf("Вы проиграли!\n");
    printField(&game->field);
}

//функция вывода меню
void printMenu(Menu* menu) {
    printf("Меню:\n");
    for (int i = 0; i < 4; i++) {
        printf("%d) %s\n", i, menu->options[i]);
    }
}

//обработка выбора пункта меню
void MenuInput(Menu* menu, Game* game, Settings* settings) {
    int input;
    printf("Выберите действие и введите номер пункта меню: ");
    scanf("%d", &input);
    if (input == 0) { menu->selectedOption = 0; return; }
    if (input == 1) { menu->selectedOption = 1; return; }
    if (input == 2) { menu->selectedOption = 2; return; }
    if (input == 3) {
        menu->selectedOption = 3;
        return;
    }
}

//функция вывода настроеек
void printSettings(Settings* settings) {
    printf("Настройки:\n");
    printf("Размер поля: %d\n", settings->size);
    printf("Количество мин: %d\n", settings->mines);
}

//функция сохранения статистики в файл
void saveStatistics(Statistics* statistics) {
    FILE* file = fopen("statistics.txt", "w");
    if (file) {
        fprintf(file, "%d\n%d\n%ld", statistics->wins, statistics->losses, statistics->totalTime);
        fclose(file);
    }
}

//функция выгрузки статистики из файла
void loadStatistics(Statistics* statistics) {
    FILE* file = fopen("statistics.txt", "r");
    if (file) {
        fscanf(file, "%d\n%d\n%ld", &statistics->wins, &statistics->losses, &statistics->totalTime);
        fclose(file);
    }
}

//функция печати статистики
void printStatistics(Statistics* statistics) {
    printf("Статистика:\n");
    printf("Победы: %d\n", statistics->wins);
    printf("Поражения: %d\n", statistics->losses);
    printf("Общее время: %ld секунд\n", statistics->totalTime);
}

int main() {
    SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
    setlocale(LC_ALL, "Rus");
    Game game;
    Menu menu;
    Settings settings = { SIZE, MINES, 0 }; // Начальные настройки
    Statistics statistics;

    loadStatistics(&statistics);
    strcpy(menu.options[0], "Новая игра");
    strcpy(menu.options[1], "Настройки");
    strcpy(menu.options[2], "Статистика");
    strcpy(menu.options[3], "Выход");
    menu.selectedOption = 0;

    while (1) {

        printMenu(&menu);
        MenuInput(&menu, &game, &settings);

        if (menu.selectedOption == 0) {
            system("cls"); // Очистка консоли
            initGame(&game, settings);
            time_t start_time = time(NULL);
            while (game.isRunning) {
                printField(&game.field);
                int x, y, action;
                printf("Введите координаты (x y) и действие (0 - открыть, 1 - флаг): ");
                scanf("%d %d %d", &x, &y, &action);

                if (!game.gameStarted) {
                    placeMines(&game.field, x, y);
                    countMines(&game.field);
                    game.gameStarted = 1;
                }
                if (action == 0) openCell(&game, x, y);
                else if (action == 1) flagCell(&game, x, y);
                game.player.time = time(NULL) - start_time;
                Win(&game);
            }
            if (game.player.score > 0) statistics.wins++;
            else statistics.losses++;
            statistics.totalTime += game.player.time;
            saveStatistics(&statistics);
        }
        
    }
    return 0;
}