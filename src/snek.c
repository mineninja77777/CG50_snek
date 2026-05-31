#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <string.h>
#include <fxcg/misc.h>
#include <stdio.h>
#include <fxcg/rtc.h>
#include <stdbool.h>
#include <time.h>
#include <fxcg/heap.h>
#include <stdlib.h>

int *snakeX = NULL;
int *snakeY = NULL;

int snakeLen = 3;

int appleX = 0;
int appleY = 0;

unsigned int tick = 0;
unsigned int lasttick = 0; 

int speed = 1;
double ramp = 0;

int dir = 0;

int key;

bool inSnake(int x, int y) {
    for (int i = 0; i < snakeLen; i++) {
        if (x == snakeX[i] && y == snakeY[i]) {
            return true;
        }
    }
    return false;
}

void placeApple() {
    srand(clock());
    
    appleX = rand() % 23;
    appleY = rand() % 11;
    while(inSnake(appleX, appleY) == true) {
        appleX = rand() % 23;
        appleY = rand() % 11;
    }
}

void setDifficulty() {
    PrintXY(2,1,"--Difficulty", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK); // It ignores the first 2 characters of the string for whatever reason
    PrintXY(2,2,"--1 Easy", TEXT_MODE_NORMAL, TEXT_COLOR_GREEN);
    PrintXY(2,3,"--2 Normal", TEXT_MODE_NORMAL, TEXT_COLOR_YELLOW);
    PrintXY(2,4,"--3 Accel.", TEXT_MODE_NORMAL, TEXT_COLOR_RED);


    GetKey(&key);
    while(key != 0x0031 && key != 0x0032 && key != 0x0033 && key != 0x7533 && key != 0x7532) {
        GetKey(&key);
    }

    if (key == 0x0031) {
        speed = 1;
        ramp = 0;
    } else if (key == 0x0032) {
        speed = 2;
        ramp = 0;
    } else if (key == 0x0033) {
        speed = 1;
        ramp = 0.1;
    }
}

void moveSnake(int x, int y) {

    // calc nextpos
    int nextX = snakeX[snakeLen - 1] + x;
    int nextY = snakeY[snakeLen - 1] + y;

    if (nextX == appleX && nextY == appleY) {
        
        snakeLen++;

        // realloc
        snakeX = sys_realloc(snakeX, snakeLen * sizeof(int));
        if (snakeX == NULL) {
            // bad
            // very bad
            // I mean what do I do now?
            PrintXY(2,6,"--MEMORY GONE BAD SNAKEX RESET", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
            GetKey(&key);
        }
        

        snakeY = sys_realloc(snakeY, snakeLen * sizeof(int));
        if (snakeY == NULL) {
            // bad
            // very bad
            // I mean what do I do now?
            PrintXY(2,8,"--MEMORY GONE BAD SNAKEY RESET", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
            GetKey(&key);
        }
        
        snakeX[snakeLen-1] = nextX;
        snakeY[snakeLen-1] = nextY;
        placeApple();
    } else if (inSnake(nextX, nextY) == true || nextX < 0 || nextX > 22 || nextY < 0 || nextY > 10) {
        // lose, just do smth innit
        PrintXY(13,1,"--You Lost!", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
        PrintXY(13,2,"--Press Key", TEXT_MODE_NORMAL, TEXT_COLOR_RED);

        GetKey(&key);
        
        // reset
        snakeLen = 3;

        // realloc
        snakeX = sys_realloc(snakeX, snakeLen * sizeof(int));
        if (snakeX == NULL) {
            // bad
            // very bad
            // I mean what do I do now?
            PrintXY(2,6,"--MEMORY GONE BAD SNAKEX RESET", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
            GetKey(&key);
        }

        snakeY = sys_realloc(snakeY, snakeLen * sizeof(int));
        if (snakeY == NULL) {
            // bad
            // very bad
            // I mean what do I do now?
            PrintXY(2,8,"--MEMORY GONE BAD SNAKEY RESET", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
            GetKey(&key);
        }


        snakeX[0] = 9;
        snakeX[1] = 10;
        snakeX[2] = 11;
        snakeY[0] = 5;
        snakeY[1] = 5;
        snakeY[2] = 5;

        dir = 0;

        placeApple();
        setDifficulty();
    } else {
        // shift array by 1 to left
        for (int i = 0; i < snakeLen - 1; i++) {
            snakeX[i] = snakeX[i+1];
            snakeY[i] = snakeY[i+1];
        }

        // set new head
        snakeX[snakeLen - 1] = nextX;
        snakeY[snakeLen - 1] = nextY;
    }
}

// main code
int main()
{
    snakeX = (int*) sys_malloc(snakeLen * sizeof(int));
    if (snakeX == NULL) {
        // bad
        // very bad
        // I mean what do I do now?
        PrintXY(2,6,"--MEMORY GONE BAD SNAKEX MAIN", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
        GetKey(&key);
    }

    snakeY = (int*) sys_malloc(snakeLen * sizeof(int));
    if (snakeY == NULL) {
        // bad
        // very bad
        // I mean what do I do now?
        PrintXY(2,8,"--MEMORY GONE BAD SNAKEY MAIN", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
        GetKey(&key);
    }

    snakeX[0] = 9;
    snakeX[1] = 10;
    snakeX[2] = 11;
    snakeY[0] = 5;
    snakeY[1] = 5;
    snakeY[2] = 5;


    Bdisp_EnableColor(1);
    setDifficulty();
    
    placeApple();

    while(1)
    {
        key = PRGM_GetKey();

        Bdisp_AllClr_VRAM();
        if (key == 47 || key == 48) 
        {
            break;
        } else if (key == 28 && dir != 2) {
            // up
            dir = 1;
        } else if (key == 37 && dir != 1) {
            // down
            dir = 2;
        } else if (key == 38 && dir != 4 && dir != 0) {
            // left
            dir = 3;
        } else if (key == 27 && dir != 3) {
            // right 
            dir = 4;
        }
        
        tick = RTC_GetTicks();


        if (lasttick < tick - 32.0/(speed + snakeLen*ramp)) {
            lasttick = tick;
            if (dir == 1) {
                // up
                moveSnake(0,- 1);
            } else if (dir == 2) {
                // down
                moveSnake(0, 1);
            } else if (dir == 3) {
                // left
                moveSnake(-1, 0);
            } else if (dir == 4) {
                // right
                moveSnake(1, 0);
            }
        }

        // background
        Bdisp_FilledRectangleFullColor(0, 0, 22*16+16, 10*16+16, 0x39a7);

        // draw snake
        for (int i = 0; i < snakeLen; i++) {
            Bdisp_FilledRectangleFullColor(16*snakeX[i], 16*snakeY[i], 16*snakeX[i] + 16, 16*snakeY[i] + 16, i == snakeLen-1 ? 0x3d89 : 0x3468);
        }

        // draw apple
        Bdisp_FilledRectangleFullColor(16*appleX, 16*appleY, appleX*16+16, appleY*16+16, 0xe0c9);


        char buffer2[20];
        itoa(snakeLen, (unsigned char*)buffer2 + 2);
        PrintXY(0,0, buffer2, TEXT_MODE_NORMAL, TEXT_COLOR_BLUE);

        Bdisp_PutDisp_DD();
    }

    sys_free(snakeX);
    sys_free(snakeY);

    return 0;
}
