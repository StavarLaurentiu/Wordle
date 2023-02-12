#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHOICESCOUNT 2
#define CHOICESLENGTH 15

void initColors();
void menu();
void game();
void checkWord(char *input, char *word, WINDOW *win[5][5], int boxRowCount);
void gameWon(int boxRowCount);
void gameLost(int boxRowCount, char *word);
void choiceBox(int row, int col);

// initiate colors
void initColors() {
    start_color();
    init_pair(1, 0, 8);
    init_pair(2, 15, 8);
    bkgd(COLOR_PAIR(1));
}

// creates the menu screen
void menu() {
    clear();
    int row, col;
    getmaxyx(stdscr, row, col);

    initColors();

    choiceBox(row, col);
}

// creates the game screen
void game() {
    char wordle[] = "WORDLE", c = ' ';
    int row, col;
    getmaxyx(stdscr, row, col);

    char word[6];
    char *dictionary[30] = {
        "arici", "atent", "baiat", "ceata", "debut", "peste", "fixat",
        "hamac", "harta", "jalon", "jucam", "lacat", "magie", "nufar",
        "oaste", "perus", "rigle", "roman", "sanie", "scris", "sonda",
        "texte", "tipar", "titan", "zebra", "vapor", "vatra"};

    // get a random word from the dictionary
    srand(time(0));
    int random = (int)rand() % 27;
    strcpy(word, dictionary[random]);

    // initiate colors
    start_color();
    init_pair(1, 0, 8);
    init_pair(2, 0, 8);
    bkgd(COLOR_PAIR(1));

    // print the title of the game
    attron(A_BOLD);
    mvprintw(row / 6, (col - strlen(wordle) - 2) / 2, "%s", wordle);
    attroff(A_BOLD);

    mvprintw(row - 2, 2, "Created by Stavar Laurentiu", row, col);
    refresh();

    // creates and prints boxes
    WINDOW *win[6][5];
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 5; j++) {
            win[i][j] =
                newwin(row / 13, col / 23, (row / 10 * (i + 1)) + row / 6,
                       col / 20 * (j + 1) + col / 3);
            box(win[i][j], 0, 0);
            wbkgd(win[i][j], COLOR_PAIR(2));
            wrefresh(win[i][j]);
        }

    char input[6];
    int boxRowCount = 0, boxColCount = -1;
    while (c != ':') {
        c = getch();
        // wipe NOT ENOUGH CHARACTERS warning if it is the case
        mvprintw(4, (col - 22) / 2, "                           ");
        boxColCount++;

        // if the user introduce more than 5 letters before submiting a word
        if (boxColCount == 5 && c != '\n' && c != 127) {
            boxColCount--;
        }
        // if the user introduce a caracter that's not a letter
        else if (c != '\n' && c != 127 && c != ':' && (c < 97 || c > 122)) {
            boxColCount--;
        } else if (c == '\n') {
            // if the user submits a word with less than 5 letters
            if (boxColCount < 5) {
                attron(A_BLINK);
                mvprintw(4, (col - 22) / 2, "NOT ENOUGH CARACTERS");
                attroff(A_BLINK);
                boxColCount--;
            }
            // if the user submits a correct word
            else {
                boxColCount = -1;
                checkWord(input, word, win, boxRowCount);
                boxRowCount++;
            }
        }
        // backspace functionality
        else if (c == 127) {
            if (boxColCount == 0) {
                boxColCount -= 1;
            } else {
                boxColCount -= 2;
            }
            mvwprintw(win[boxRowCount][boxColCount + 1], row / 26, col / 46,
                      " ");
            wrefresh(win[boxRowCount][boxColCount + 1]);
        }
        // if the user introduce a new letter to the word
        else if (c != ':') {
            input[boxColCount] = c;
            mvwprintw(win[boxRowCount][boxColCount], row / 26, col / 46, "%c",
                      c);
            wrefresh(win[boxRowCount][boxColCount]);
        }
        // if the user wants to enter the menu
        else
            menu();
    }
}

// checks if a submitted word is correct, it aslo collors the boxes
void checkWord(char *input, char *word, WINDOW *win[5][5], int boxRowCount) {
    int row, col, won = 0;
    getmaxyx(stdscr, row, col);
    init_pair(3, 0, COLOR_GREEN);
    init_pair(4, 0, COLOR_YELLOW);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);

    // check if the user won
    if (strcmp(input, word) == 0) {
        gameWon(boxRowCount);
        won = 1;
    }
    // else color the words
    else {
        char wordCopy[6];
        strcpy(wordCopy, word);
        // color the green letters
        for (int i = 0; i < 5; i++) {
            if (input[i] == wordCopy[i]) {
                wbkgd(win[boxRowCount][i], COLOR_PAIR(3));
                wrefresh(win[boxRowCount][i]);
                input[i] = '_';
                wordCopy[i] = '_';
            }
        }
        // collor the yellow letters
        for (int i = 0; i < 5; i++) {
            if (input[i] != '_') {
                for (int j = 0; j < 5; j++) {
                    if (input[i] == wordCopy[j]) {
                        wbkgd(win[boxRowCount][i], COLOR_PAIR(4));
                        wrefresh(win[boxRowCount][i]);
                        input[i] = '_';
                        wordCopy[j] = '_';
                    }
                }
            }
        }
        // if the word isn't collored yet make it black
        for (int i = 0; i < 5; i++) {
            if (input[i] != '_') {
                wbkgd(win[boxRowCount][i], COLOR_PAIR(5));
                wrefresh(win[boxRowCount][i]);
            }
        }
    }

    // if it was the last try user lost
    if (!won && boxRowCount == 5) gameLost(boxRowCount, word);
}

// the screen that pops up when the user won
void gameWon(int boxRowCount) {
    clear();
    char won[] = "CONGRATULATIONS, YOU FOUND THE CORRECT WORD!";
    int row, col;
    getmaxyx(stdscr, row, col);

    initColors();

    wattron(stdscr, A_BOLD);
    mvprintw(row / 4, (col - strlen(won)) / 2, "%s", won);
    wattroff(stdscr, A_BOLD);
    refresh();

    choiceBox(row, col);
}

// the screen that pops up when the user lost
void gameLost(int boxRowCount, char *word) {
    clear();
    char lost[] = "UNFORTUNETLY, YOU LOST THE GAME!";
    int row, col;
    getmaxyx(stdscr, row, col);

    initColors();

    wattron(stdscr, A_BOLD);
    mvprintw(row / 4, (col - strlen(lost)) / 2, "%s", lost);
    mvprintw(row / 4 + 1, (col - strlen(word) - 15) / 2, "Expected word: %s",
             word);
    wattroff(stdscr, A_BOLD);

    refresh();
    choiceBox(row, col);
}

// the new game or quit menu box
void choiceBox(int row, int col) {
    WINDOW *menuwin = newwin(row / 5, col / 10, row / 3, col / 2.25);
    box(menuwin, 0, 0);
    wbkgd(menuwin, COLOR_PAIR(2));
    wrefresh(menuwin);
    int menurow, menucol;
    getmaxyx(menuwin, row, col);

    keypad(menuwin, true);

    char choices[CHOICESCOUNT][CHOICESLENGTH] = {"New Game", "Quit"};
    int choice;
    int highlight = 0;

    while (1) {
        for (int i = 0; i < CHOICESCOUNT; i++) {
            if (i == highlight) {
                wattron(menuwin, A_REVERSE);
            }
            mvwprintw(menuwin, i + 1, 1, choices[i]);
            wattroff(menuwin, A_REVERSE);
        }
        choice = wgetch(menuwin);

        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == -1) highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight == 2) highlight = 1;
                break;
            default:
                break;
        }

        if (choice == 10) break;
    }

    clear();
    if (strcmp(choices[highlight], "New Game") == 0)
        game();
    else
        exit(1);

    getch();
}

int main(int argc, char **argv) {
    /* NCURSES START */

    // set up the NCURSES enviroment
    initscr();
    cbreak();
    raw();
    noecho();

    menu();

    endwin();
    /* NCURSES END */

    return 0;
}