#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 4096
#define ROW_SIZE 8
#define COL_SIZE 8

struct Point {
    int x;
    int y;
};


int gameBoard[ROW_SIZE][COL_SIZE];

void PrintNoMoveInput();

void PrintRequest();

void PrintInvalidInputError();

void ReleaseMemoryEndExit();

void StartPlaying();

void ExecuteMove(struct Point *p, int *moveFlag, int myNumber, int turn);

int CheckEnd();

int CheckWinner(int num);

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

void HandleSiguser1(int sig);

void HandleSecondPlayer(char* data);

char *memory;
int myBoardNumber;

/**
 *operation - the main function,runs the program
 */
int main() {

    int fd_write;
    pid_t myPid;

    //set the board
    memset(gameBoard, 0, ROW_SIZE * COL_SIZE * sizeof(int));
    gameBoard[3][3] = 2;
    gameBoard[4][4] = 2;
    gameBoard[3][4] = 1;
    gameBoard[4][3] = 1;
    //print it
    PrintBoard();
    //define the signal handler
    struct sigaction sigUsrAction;
    sigset_t sigUsrBlock;
    sigfillset(&sigUsrBlock);
    //set the handling function for siusr1
    sigUsrAction.sa_handler = HandleSiguser1;
    sigUsrAction.sa_mask = sigUsrBlock;
    sigUsrAction.sa_flags = 0;
    //remove the wanted signals
    sigdelset(&sigUsrBlock, SIGINT);
    if (sigaction(SIGUSR1, &sigUsrAction, NULL) != 0) {
        perror("faild in sigaction");
        exit(-1);
    }
    //open fifo
    if ((fd_write = open("fifo_clientTOserver", O_WRONLY)) < 0) {
        write(STDERR_FILENO, "failed to open fifo", strlen("failed to open fifo"));
        exit(-1);
    }
    //write to it my pid
    myPid = getpid();
    if (write(fd_write, &myPid, sizeof(pid_t)) < 0) {
        perror("failed to write to fifo");
        exit(-1);
    }
    return 0;
}

/**
 * input - pointer to shared memory
 * output- the second player move
 * operation - reads from memory
 */
struct Point ReadFromMemory(char *data) {

    int x;
    int y;
    while (*data == '$') {
        if (write(STDOUT_FILENO, "Waiting for to other player to make a move \n",
                  strlen("\"Waiting for to other player to make a move \n")) < 0) {
            perror("failed to write to screen");
            ReleaseMemoryEndExit();
        }
        sleep(1);
    }

    x = (*data) + 48;
    (*data++);
    y = (*data) + 48;
    (*data++);
    (*data++);
    (*data++);
    struct Point p;
    p.y = y;
    p.x = x;
    return p;
}

/**
 * input - the point a put my piece and my nuber on board
 * operation - write it to the shared memory
 */
void WriteToSharedMemory(struct Point *p, int myNumber) {
    char symbol;
    char x;
    char y;
    if (myNumber == 1) {
        symbol = 'w';
    } else if (myNumber == 2) {
        symbol = 'b';
    }
    x = p->x + 48; //todo will work??
    y = p->y + 48;
    *memory++ = symbol;
    *memory++ = x;
    *memory++ = y;
    *memory++ = '\0';
    *memory++ = '$';
}

/**
 * input -sig number
 * operation - handles the sigusr1 that we are reciving in the start of the game
 *             and lets the player to choose the first move
 */
void HandleSiguser1(int sig) {
    StartPlaying();
}

/**
 * input - the string from user
 * output - the Point struct
 * operation - parse the user string and create a struct from is
 */
struct Point *ParseStruct(char *move) {
    int x;
    int y;

    char *temp = strtok(move, ",");
    //more then 1 digit entered
    if (strlen(temp) > 2) {
        return NULL;
    }
    x = temp[1] - 48;
    temp = strtok(NULL, ",");
    //more then 1 digit entered
    if (strlen(temp) > 2) {
        return NULL;
    }
    y = temp[0] - 48;
    //create the struct
    struct Point *point = malloc(sizeof(struct Point));
    if (point == NULL) {
        //todo handle
    } else {
        if ((x >= ROW_SIZE) || (x < 0) || (y < 0) || (y >= COL_SIZE)) {
            return NULL;
        }
        point->x = x;
        point->y = y;
        return point;
    }

}

/**
 * input - a point,a flag to indicate if the move was good,the player board number
 *         and a flag if it's my input
 * operation - runs the given point and tring to change the board
 */
void ExecuteMove(struct Point *p, int *moveFlag, int number, int myTurn) {

    if (myTurn == 1) {
        if (gameBoard[p->x][p->y] != 0) {
            return;
        }
    }
    //check all possible directions
    CheckConvertToRight(p, moveFlag, number);
    CheckConvertToLeft(p, moveFlag, number);
    CheckConvertToUp(p, moveFlag, number);
    CheckConvertToDown(p, moveFlag, number);
    CheckConvertToLeftAndUp(p, moveFlag, number);
    CheckConvertToLeftAndDown(p, moveFlag, number);
    CheckConvertToRightAndUp(p, moveFlag, number);
    CheckConvertToRightAndDown(p, moveFlag, number);

}

/**
 * operation - print's that the input was wrong
 */
void PrintInvalidInputError() {
    if (write(STDOUT_FILENO, "No such square \n Please choose another square",
              strlen("No such square \n Please choose another square")) < 0) {
        perror("failed to write to screen");
        ReleaseMemoryEndExit();
    }
}

/**
 * operation - print's that the input is not good because there is no move
 */
void PrintNoMoveInput() {
    if (write(STDOUT_FILENO, "This square is invalid \n Please choose another square",
              strlen("This square is invalid \n Please choose another square")) < 0) {
        perror("failed to write to screen");
        ReleaseMemoryEndExit();
    }
}

/**
 * operation - free the memory and exit the program
 */
void ReleaseMemoryEndExit() {
    //todo release shared memory
    exit(-1);
}

/**
 * operation - print's requast the enter new position
 */
void PrintRequest() {
    if (write(STDOUT_FILENO, "Please choose a square", strlen("Please choose a square")) < 0) {
        perror("failed to write to screen");
        ReleaseMemoryEndExit();
    }
}

/**
 * input - a flag that indicate the sort of check we conduct
 * operation - check if the board is full, or there is only one color
 */
int CheckWinner(int flag) {

    int i = 0;
    int j = 0;
    int white = 0;
    int black = 0;
    for (i = 0; i < ROW_SIZE; i++) {
        for (j = 0; j < COL_SIZE; j++) {

            if (gameBoard[i][j] == 1) {
                white++;
            } else if (gameBoard[i][j] == 2) {
                black++;
            }
        }
    }
    //check if full who won
    if (flag == 1) {
        if (black > white) {
            return 2;
        } else if (white > black) {
            return 1;
        } else {
            return 3;
        }
        //if there is a space check what color is left on board
    } else if (flag == 0) {
        if ((black > 0) && (white == 0)) {
            return 2;
        } else if ((white > 0) && (black == 0)) {
            return 1;
        } else {
            return -1;
        }
    }
}

/**
 * operation- check if board is full or only one color left
 */
int CheckEnd() {


    int flag = 0;
    int i = 0;
    int j = 0;
    //check the board
    for (i = 0; i < ROW_SIZE; i++) {
        for (j = 0; j < COL_SIZE; j++) {
            //if we found an empty space
            if (gameBoard[i][j] != 0) {
                flag = -1;
                break;
            }
        }
    }

    //we didn't found an empty space
    if (flag == 0) {
        return CheckWinner(1);
        //it's  -1 ->we found an empty space then keep playing and check if left a move
    } else {
        return CheckWinner(0);
    }
}


void HandleSecondPlayer(char *data) {
    struct Point p;
    int i = 0;
    int moved = 0;
    char move[6];
    struct Point *moveCoordinats;
    int myNumber = 1;
    //read move
    p = ReadFromMemory(data);
    //execute other player move
    ExecuteMove(&p, &i, 1, 0);
    //get your your move
    moved = 0;
    PrintRequest();
    scanf("%s", move);
    moveCoordinats = ParseStruct(move);
    //move was out of bound
    if (moveCoordinats == NULL) {
        PrintInvalidInputError();
        //check if move is lega-if it's legal execute it
    } else {
        ExecuteMove(moveCoordinats, &moved, myNumber, 1);
    }

    //if not legal move free Point struct and get move again
    while (moved == 0) {
        free(moveCoordinats);
        PrintNoMoveInput();
        PrintRequest();
        scanf("%s", move);
        moveCoordinats = ParseStruct(move);
        //move was out of bound
        if (moveCoordinats == NULL) {
            PrintInvalidInputError();
            //check if move is lega-if it's legal execute it
        } else {
            ExecuteMove(moveCoordinats, &moved, myNumber, 1);
        }
    }


    //write it down to memory
    PrintBoard();

    WriteToSharedMemory(moveCoordinats, myNumber);
    free(moveCoordinats);
    //return to normal play
    return;

}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

/**
 * input- my number on board
 * operation- runs the game,get's input,check's it,execute move,wait for other player,etc.
 */
void StartPlaying() {

    //declare variables
    key_t key;
    int shmid;
    char *data;
    char move[6];
    int moved;
    int x;
    int y;
    int otherPlayerNumber;
    int check;
    struct Point otherPlayerMove;
    /* make the key: */
    if ((key = ftok("ex31.c", 'k')) == -1) {
        write(STDERR_FILENO, "failed ftok", strlen("failed ftok"));
        exit(1);
    }

    /* connect to (and possibly create) the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, NULL, 0);
    if (data == (char *) (-1)) {
        perror("shmat");
        exit(1);
    }
    if (*data == '$') {
        myBoardNumber = 2;
    } else {
        myBoardNumber = 1;
        HandleSecondPlayer(data);
    }

    int myNumber = myBoardNumber;
    //declare variables
    otherPlayerNumber = 3 - myNumber;
    struct Point *moveCoordinats;

    //start the game
    while (1) {

        //get move from player
        moved = 0;
        PrintRequest();
        scanf("%s", move);
        moveCoordinats = ParseStruct(move);
        //move was out of bound
        if (moveCoordinats == NULL) {
            PrintInvalidInputError();
            //check if move is lega-if it's legal execute it
        } else {
            ExecuteMove(moveCoordinats, &moved, myNumber, 1);
        }

        //if not legal move free Point struct and get move again
        while (moved == 0) {
            free(moveCoordinats);
            PrintNoMoveInput();
            PrintRequest();
            scanf("%s", move);
            moveCoordinats = ParseStruct(move);
            //move was out of bound
            if (moveCoordinats == NULL) {
                PrintInvalidInputError();
                //check if move is lega-if it's legal execute it
            } else {
                ExecuteMove(moveCoordinats, &moved, myNumber, 1);
            }
        }
        //move was legal and we after executing the move

        //print new board
        PrintBoard();

        WriteToSharedMemory(moveCoordinats, myNumber);
        free(moveCoordinats);

        //wait for the second player move
        otherPlayerMove = ReadFromMemory(data);
        ExecuteMove(&otherPlayerMove, &moved, otherPlayerNumber, 0);
    }
}


#pragma clang diagnostic pop

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to the right there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToRight(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;

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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to the left there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
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

            //if there is'nt my piece in the other side from the left
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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to upward there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToUp(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if above from the given move there is an empty space or my piece
    if ((startX - 1 < 0) || (gameBoard[startX - 1][startY] == 0) ||
        (gameBoard[startX - 1][startY] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX--;
        while ((endX == -1) && (endY == -1) && (startX >= 0)) {

            //if there is'nt my piece in the other side from the above
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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to down there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToDown(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if below from the given move there is an empty space or my piece
    if ((startX + 1 >= ROW_SIZE) || (gameBoard[startX + 1][startY] == 0) ||
        (gameBoard[startX + 1][startY] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX++;
        while ((endX == -1) && (endY == -1) && (startX < ROW_SIZE)) {

            //if there is'nt my piece in the other side from the bottom
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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to the left and up there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToLeftAndUp(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left and up from the given move there is an empty space or my piece
    if ((startY - 1 < 0) || (startX - 1 < 0) || (gameBoard[startX - 1][startY - 1] == 0) ||
        (gameBoard[startX - 1][startY - 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX--;
        startY--;
        while ((endX == -1) && (endY == -1) && (startX >= 0) && (startY >= 0)) {

            //if there is'nt my piece in the other side from the left and up
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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to the left and down there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToLeftAndDown(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if left and down from the given move there is an empty space or my piece
    if ((startY - 1 < 0) || (startX + 1 >= ROW_SIZE) || (gameBoard[startX + 1][startY - 1] == 0) ||
        (gameBoard[startX + 1][startY - 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX++;
        startY--;
        while ((endX == -1) && (endY == -1) && (startX < ROW_SIZE) && (startY >= 0)) {

            //if there is'nt my piece in the other side from the left and down
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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to the right and up there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToRightAndUp(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if right and up from the given move there is an empty space or my piece
    if ((startY + 1 >= COL_SIZE) || (startX - 1 < 0) || (gameBoard[startX - 1][startY + 1] == 0) ||
        (gameBoard[startX - 1][startY + 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX--;
        startY++;
        while ((endX == -1) && (endY == -1) && (startX >= 0) && (startY < COL_SIZE)) {

            //if there is'nt my piece in the other side from the right and up
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

/**
 * input- get the point where thw player locate is new piece,a flag to update if we change
 *        something,and a flag to say what's my number on board
 * operation- checking if from the piece position to the right and down there is a legal move
 *            (that in the end there a second piece of my kind) if there is - update the
 *            move flag/
 */
void CheckConvertToRightAndDown(struct Point *p, int *moveFlag, int myNumber) {
    int endX = -1;
    int endY = -1;
    int startX = p->x;
    int startY = p->y;
    //check if right and down from the given move there is an empty space or my piece
    if ((startY + 1 >= COL_SIZE) || (startX + 1 >= ROW_SIZE) | (gameBoard[startX + 1][startY + 1] == 0) ||
        (gameBoard[startX + 1][startY + 1] == myNumber)) {
        return;

        /*check if the move is legal and there is a sequence of the other player pieces
        /with no whitespace and in the and again my piece*/
    } else {
        startX++;
        startY++;
        while ((endX == -1) && (endY == -1) && (startX < ROW_SIZE) && (startY < COL_SIZE)) {

            //if there is'nt my piece in the other side from the right and down
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

/**
 * operation - print's the current game board on screen
 */
void PrintBoard() {

    if (write(STDOUT_FILENO, "The board is: \n", strlen("The board is: \n")) < 0) {
        perror("failed to write to screen");
        ReleaseMemoryEndExit();
    }
    int i = 0;
    char temp[32];
    //run in loop and print
    for (i; i < ROW_SIZE; i++) {

        int j = 0;
        for (j; j < COL_SIZE; j++) {
            if ((gameBoard[i][j]) > 0) {
                memset(temp, 32, 0);
                sprintf(temp, "%01d", gameBoard[i][j]);
                if (write(STDOUT_FILENO, temp, strlen(temp)) < 0) {
                    perror("failed to write to file");
                    exit(-1);
                }
            } else {
                memset(temp, 32, 0);
                sprintf(temp, "%01d", gameBoard[i][j]);
                if (write(STDOUT_FILENO, temp, strlen(temp)) < 0) {
                    perror("failed to write to file");
                    exit(-1);
                }
            }
            if (write(STDOUT_FILENO, " ", strlen(" ")) < 0) {
                perror("failed to write to file");
                exit(-1);
            }
        }
        if (write(STDOUT_FILENO, "\n", strlen("\n")) < 0) {
            perror("failed to write to file");
            exit(-1);
        }
    }
    if (write(STDOUT_FILENO, "\n", strlen("\n")) < 0) {
        perror("failed to write to file");
        exit(-1);
    }
}

