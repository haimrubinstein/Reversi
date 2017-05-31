#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>

#define ROW_SIZE 8
#define COL_SIZE 8

struct Point {
    int x;
    int y;
};


int gameBoard[ROW_SIZE][COL_SIZE];

void StartPlaying(int myRepresentaionNumber);

void CheckMove(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToRight(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToLeft(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToUp(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToDown(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToLeftAndUp(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToLeftAndDown(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToRightAndUp(struct Point *p, int *moveFlag, int myNumber);

void CheckConvertToRightAndDown(struct Point *p, int *moveFlag, int myNumber);

void PrintBoard();

struct Point *ParseStruct(char *move);

void initializeStruct(struct Play *p);

int main() {

    int myRepresentaionNumber = 1;
    memset(gameBoard, 0, ROW_SIZE * COL_SIZE * sizeof(int));

    StartPlaying(myRepresentaionNumber);
    return 0;
}

struct Point *ParseStruct(char *move) {
    int x;
    int y;

    char *temp = strtok(move, ",");
    if (strlen(temp) > 2) {
        //todo to big number
        printf("err");
    }
    x = temp[1] - 48;
    temp = strtok(NULL, ",");
    if (strlen(temp) > 2) {
        //todo to big number
        printf("err");
    }
    y = temp[0] - 48;

    struct Point *point = malloc(sizeof(struct Point));
    if (point == NULL) {
        //todo handle
    }
    point->x = x;
    point->y = y;
    return point;
}

void CheckMove(struct Point *p, int *moveFlag, int myNumber) {

    if ((p->x >= ROW_SIZE) || (p->x < 0) || (p->y < 0) || (p->y >= COL_SIZE)) {
        //todo wrong move
    }

    if (gameBoard[p->x][p->y] != 0) {
        //todo wrong move
    }

    //check all possible directions
    CheckConvertToRight(p, moveFlag, myNumber);
    CheckConvertToLeft(p, moveFlag, myNumber);
    CheckConvertToUp(p, moveFlag, myNumber);
    CheckConvertToDown(p, moveFlag, myNumber);
    CheckConvertToLeftAndUp(p, moveFlag, myNumber);
    CheckConvertToLeftAndDown(p, moveFlag, myNumber);
    CheckConvertToRightAndUp(p, moveFlag, myNumber);
    CheckConvertToRightAndDown(p, moveFlag, myNumber);

}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void StartPlaying(int myNumber) {

    char move[6];
    int moved;
    struct Point *moveCoordinats;
    while (1) {

        gameBoard[0][2] = 1;
        gameBoard[0][3] = 2;
        gameBoard[2][2] = 2;
        gameBoard[1][3] = 2;
        gameBoard[3][1] = 1;
        PrintBoard();
        //get move
        moved = 0;
        scanf("%s", move);
        moveCoordinats = ParseStruct(move);
        CheckMove(moveCoordinats, &moved, myNumber);

        //if not legal move free struct and get move again
        while (moved == 0) {
            free(moveCoordinats);

            scanf("%s", move);
            moveCoordinats = ParseStruct(move);
            CheckMove(moveCoordinats, &moved, myNumber);
        }

        //move was legal and we executed the move
        free(moveCoordinats);
        PrintBoard();
    }
}

#pragma clang diagnostic pop

//left means that i pressed on location left from me
void CheckConvertToRight(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //todo add check that we are not out of bound
    //check if right from the given move there is an empty space or my piece
    if ((startY + 1 >= COL_SIZE) || (gameBoard[startX][startY + 1] == 0) ||
        (gameBoard[startX][startY + 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startY++;
        while ((endX == -1) && (endY == -1) && (startY < COL_SIZE)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startY++;
        }
    }

    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while (startY < endY) {
            gameBoard[startX][startY] = myNumber;
            startY++;
        }
    }
}

void CheckConvertToLeft(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startY - 1 < 0) || (gameBoard[startX][startY - 1] == 0) ||
        (gameBoard[startX][startY - 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startY--;
        while ((endX == -1) && (endY == -1) && (startY >= 0)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startY--;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while (startY > endY) {
            gameBoard[startX][startY] = myNumber;
            startY--;
        }
    }
}


void CheckConvertToUp(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startX - 1 < 0) || (gameBoard[startX - 1][startY] == 0) ||
        (gameBoard[startX - 1][startY] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX--;
        while ((endX == -1) && (endY == -1) && (startX >= 0)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startX--;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while (startX > endX) {
            gameBoard[startX][startY] = myNumber;
            startX--;
        }
    }
}

void CheckConvertToDown(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startX + 1 >= ROW_SIZE) || (gameBoard[startX + 1][startY] == 0) ||
        (gameBoard[startX + 1][startY] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX++;
        while ((endX == -1) && (endY == -1) && (startX < ROW_SIZE)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startX++;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while (startX < endX) {
            gameBoard[startX][startY] = myNumber;
            startX++;
        }
    }
}


void CheckConvertToLeftAndUp(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startY - 1 < 0) || (startX - 1 < 0) || (gameBoard[startX - 1][startY - 1] == 0) ||
        (gameBoard[startX - 1][startY - 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX--;
        startY--;
        while ((endX == -1) && (endY == -1) && (startX >= 0) && (startY >= 0)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startX--;
            startY--;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while ((startX > endX) && (startY > endY)) {
            gameBoard[startX][startY] = myNumber;
            startX--;
            startY--;
        }
    }
}


void CheckConvertToLeftAndDown(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startY - 1 < 0) || (startX + 1 >= ROW_SIZE) || (gameBoard[startX + 1][startY - 1] == 0) ||
        (gameBoard[startX + 1][startY - 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX++;
        startY--;
        while ((endX == -1) && (endY == -1) && (startX < ROW_SIZE) && (startY >= 0)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startX++;
            startY--;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while ((startX < endX) && (startY > endY)) {
            gameBoard[startX][startY] = myNumber;
            startX++;
            startY--;
        }
    }
}


void CheckConvertToRightAndUp(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startY + 1 >= COL_SIZE) || (startX - 1 < 0) || (gameBoard[startX - 1][startY + 1] == 0) ||
        (gameBoard[startX - 1][startY + 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX--;
        startY++;
        while ((endX == -1) && (endY == -1) && (startX >= 0) && (startY < COL_SIZE)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startX--;
            startY++;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while ((startX > endX) && (startY < endY)) {
            gameBoard[startX][startY] = myNumber;
            startX--;
            startY++;
        }
    }
}


void CheckConvertToRightAndDown(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left from the given move there is an empty space or my piece
    if ((startY + 1 >= COL_SIZE) || (startX + 1 >= ROW_SIZE) | (gameBoard[startX + 1][startY + 1] == 0) ||
        (gameBoard[startX + 1][startY + 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX++;
        startY++;
        while ((endX == -1) && (endY == -1) && (startX < ROW_SIZE) && (startY < COL_SIZE)) {

            //if there is'nt my piece in the other side from the right
            if (gameBoard[startX][startY] == 0) {
                break;
            }
            if ((gameBoard[startX][startY] == myNumber)) {
                endX = startX;
                endY = startY;
            }
            startX++;
            startY++;
        }
    }
    //if we found the move legal the change the board and update the flag to 1
    if ((endX != -1) && (endY != -1)) {
        *moveFlag = 1;
        startX = p->x;
        startY = p->y;
        while ((startX < endX) && (startY < endY)) {
            gameBoard[startX][startY] = myNumber;
            startX++;
            startY++;
        }
    }
}

void PrintBoard() {
    int i = 0;
    char temp[32];
    //run in loop and print
    for (i; i < ROW_SIZE; i++) {
        write(STDOUT_FILENO, "|", strlen("|"));
        int j = 0;
        for (j; j < COL_SIZE; j++) {
            if ((gameBoard[i][j]) > 0) {
                memset(temp, 32, 0);
                sprintf(temp, "%04d", gameBoard[i][j]);
                if (write(STDOUT_FILENO, temp, strlen(temp)) < 0) {
                    perror("failed to write to file");
                    exit(-1);
                }
            } else {
                if (write(STDOUT_FILENO, "    ", strlen("    ")) < 0) {
                    perror("failed to write to file");
                    exit(-1);
                }
            }
            if (write(STDOUT_FILENO, " ", strlen(" ")) < 0) {
                perror("failed to write to file");
                exit(-1);
            }
            if (write(STDOUT_FILENO, "|", strlen("|")) < 0) {
                perror("failed to write to file");
                exit(-1);
            }
        }
        if (write(STDOUT_FILENO, "\n", strlen("\n")) < 0) {
            perror("failed to write to file");
            exit(-1);
        }
    }
}