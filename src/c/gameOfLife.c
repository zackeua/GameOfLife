#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"
#include <pthread.h>

int gameHeight;
int gameWidth;

typedef struct cell {
    int live;
    int inList;
    int count;
} cell_t;

typedef struct index {
    int row;
    int col; 
} index_t;

typedef struct thread_data {
    cell_t*** board;
    index_t* indexes;
    int start;
    int steps;
    int currentTimeIndex;
    int nextTimeIndex;
} thread_data_t;

void updateState(int row, int col, cell_t** currentBoard, cell_t** nextBoard)
{
    int rowP = row + 1;
    int rowM = row - 1;
    int colP = col + 1;
    int colM = col - 1;

    if (rowP == gameHeight)
    {
        rowP = 0;
    }
    else if (rowM == -1)
    {
        rowM = gameHeight - 1;
    }
    if (colP == gameWidth)
    {
        colP = 0;
    }
    else if (colM == -1)
    {
        colM = gameWidth - 1;
    }

    int current = currentBoard[row][col].live;
    int count = currentBoard[row][col].count;

    nextBoard[row][col].live = current;
    nextBoard[row][col].count += count;

    if (current == 0 && count == 3)
    {
        nextBoard[row][col].live = 1;

        nextBoard[rowM][colM].count += 1;
        nextBoard[rowM][col].count += 1;
        nextBoard[rowM][colP].count += 1;

        nextBoard[row][colM].count += 1;
        nextBoard[row][colP].count += 1;

        nextBoard[rowP][colM].count += 1;
        nextBoard[rowP][col].count += 1;
        nextBoard[rowP][colP].count += 1;
    }
    else if (current == 1 && (count < 2 || count > 3))
    {
        nextBoard[row][col].live = 0;

        nextBoard[rowM][colM].count -= 1;
        nextBoard[rowM][col].count -= 1;
        nextBoard[rowM][colP].count -= 1;

        nextBoard[row][colM].count -= 1;
        nextBoard[row][colP].count -= 1;
        
        nextBoard[rowP][colM].count -= 1;
        nextBoard[rowP][col].count -= 1;
        nextBoard[rowP][colP].count -= 1;
    }
}

void *thr_func(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;

    for (int i = data->start; i < data->start + data->steps; i++)
    {
        int row = (data->indexes[i]).row;
        int col = (data->indexes[i]).col;
        updateState(row, col, data->board[data->currentTimeIndex], data->board[data->nextTimeIndex]);
    }    

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 7) {
        printf("Input on this form:\n");
        printf("./game gameWidth gameHeight rand_seed timesteps graphics n_threads\n");
        printf("gameWidth is the width of the gameworld.\n");
        printf("gameheight is the height if the gameworld.\n");
        printf("rand_seed is the random seed to be used.\n");
        printf("timesteps is the amount of timesteps to be taken. ");
        printf("(use any amount of timesteps less than or equal to 0 for (nearly)infinite amount of timesteps)\n");
        printf("graphics is if you want to display the world graphically or not.\n");
        printf("n_threads is the amount threads to use.\n");
        return -1;
    }

    gameWidth = atoi(argv[1]); // get input values;
    gameHeight = atoi(argv[2]);
    srand(atoi(argv[3]));
    int timesteps = atoi(argv[4]);
    int graphics = atoi(argv[5]);
    int n_threads = atoi(argv[6]);

    pthread_t thr[n_threads];
    thread_data_t thr_data[n_threads];

    double windowgameWidth = 800; // window size
    double windowgameHeight = 800;

    
    cell_t*** board = (cell_t***)malloc(2 * sizeof(cell_t**)); // initialize and populate world

    board[0] = (cell_t**)malloc(gameHeight * sizeof(cell_t*));
    board[1] = (cell_t**)malloc(gameHeight * sizeof(cell_t*));
    cell_t** zero = (cell_t**)malloc(gameHeight * sizeof(cell_t*));

    index_t** indexes = (index_t**)malloc(2 * sizeof(index_t*));
    indexes[0] = (index_t*)malloc(gameHeight * gameWidth * sizeof(index_t));
    indexes[1] = (index_t*)malloc(gameHeight * gameWidth * sizeof(index_t));

    int liveIndex = 0;
    for (int row = 0; row < gameHeight; row++)
    {        
        board[0][row] = (cell_t*)malloc(gameWidth * sizeof(cell_t));
        board[1][row] = (cell_t*)malloc(gameWidth * sizeof(cell_t));
        zero[row] = (cell_t*)malloc(gameWidth * sizeof(cell_t));
        for (int col = 0; col < gameWidth; col++)
        {
            board[0][row][col].live = rand() & 1;
            board[0][row][col].inList = 0;
            board[0][row][col].count = 0;

            board[1][row][col].live = 0;
            board[1][row][col].inList = 0;
            board[1][row][col].count = 0;
        }
    }

    for (int row = 0; row < gameHeight; row++)
    {
        for (int col = 0; col < gameWidth; col++)
        {
            int rowP = row + 1;
            int rowM = row - 1;
            int colP = col + 1;
            int colM = col - 1;

            if (rowP == gameHeight)
            {
                rowP = 0;
            }
            else if (rowM == -1)
            {
                rowM = gameHeight - 1;
            }

            if (colP == gameWidth)
            {
                colP = 0;
            }
            else if (colM == -1)
            {
                colM = gameWidth - 1;
            }

            board[0][row][col].count = board[0][rowM][colM].live + 
                                       board[0][rowM][col].live +
                                       board[0][rowM][colP].live +
                                       board[0][row][colM].live +
                                       board[0][row][colP].live +
                                       board[0][rowP][colM].live +
                                       board[0][rowP][col].live +
                                       board[0][rowP][colP].live;
            
            if (board[0][row][col].live == 1)
            {
                if (board[0][rowM][colM].inList == 0)
                {
                    board[0][rowM][colM].inList = 1;
                    indexes[0][liveIndex].row = rowM;
                    indexes[0][liveIndex].col = colM;
                    liveIndex++;
                }
                if (board[0][rowM][col].inList == 0)
                {
                    board[0][rowM][col].inList = 1;
                    indexes[0][liveIndex].row = rowM;
                    indexes[0][liveIndex].col = col;
                    liveIndex++;
                }
                if (board[0][rowM][colP].inList == 0)
                {
                    board[0][rowM][colP].inList = 1;
                    indexes[0][liveIndex].row = rowM;
                    indexes[0][liveIndex].col = colP;
                    liveIndex++;
                }
                if (board[0][row][colM].inList == 0)
                {
                    board[0][row][colM].inList = 1;
                    indexes[0][liveIndex].row = row;
                    indexes[0][liveIndex].col = colM;
                    liveIndex++;
                }
                if (board[0][row][col].inList == 0)
                {
                    board[0][row][col].inList = 1;
                    indexes[0][liveIndex].row = row;
                    indexes[0][liveIndex].col = col;
                    liveIndex++;
                }
                if (board[0][row][colP].inList == 0)
                {
                    board[0][row][colP].inList = 1;
                    indexes[0][liveIndex].row = row;
                    indexes[0][liveIndex].col = colP;
                    liveIndex++;
                }
                if (board[0][rowP][colM].inList == 0)
                {
                    board[0][rowP][colM].inList = 1;
                    indexes[0][liveIndex].row = rowP;
                    indexes[0][liveIndex].col = colM;
                    liveIndex++;
                }
                if (board[0][rowP][col].inList == 0)
                {
                    board[0][rowP][col].inList = 1;
                    indexes[0][liveIndex].row = rowP;
                    indexes[0][liveIndex].col = col;
                    liveIndex++;
                }
                if (board[0][rowP][colP].inList == 0)
                {
                    board[0][rowP][colP].inList = 1;
                    indexes[0][liveIndex].row = rowP;
                    indexes[0][liveIndex].col = colP;
                    liveIndex++;
                }
            }
        }
    }

    if (graphics) // create canvas
    {
        InitializeGraphics(argv[0], windowgameWidth, windowgameHeight); // initialize graphixs window
        SetCAxes(0, 1);
    }
    
    int time = 0;
    while (1) // time looping
    {
        int currentTimeIndex = time & 1;
        int nextTimeIndex = (time + 1) & 1;
        int nextIndex = 0;

        int stepsize = liveIndex / n_threads;
        int paddingsize = stepsize + (liveIndex - stepsize * n_threads);

        for (int i = 0; i < n_threads; i++) // set all thread data in each timestep
        {
            thr_data[i].board = board;
            thr_data[i].indexes = indexes[currentTimeIndex];

            thr_data[i].start = i*stepsize + 1;
            thr_data[i].steps = stepsize;
            
            thr_data[i].currentTimeIndex = currentTimeIndex;
            thr_data[i].nextTimeIndex = nextTimeIndex;

            if (i == n_threads - 1)
            {
                thr_data[i].start = liveIndex - paddingsize;
                thr_data[i].steps = paddingsize - 1;
            }

            pthread_create(&thr[i], NULL, thr_func, &thr_data[i]);
        }

        for (int i = 0; i < n_threads; i++) //close the pthreads
        {
            pthread_join(thr[i], NULL);
        }

        for (int index = 0; index < liveIndex; index++) // zero the just used gameworld so it is fresh to use during the next timestep
        {
            int row = indexes[currentTimeIndex][index].row;
            int col = indexes[currentTimeIndex][index].col;
            
            int rowP = row + 1;
            int rowM = row - 1;
            int colP = col + 1;
            int colM = col - 1;

            if (rowP == gameHeight)
            {
                rowP = 0;
            }
            else if (rowM == -1)
            {
                rowM = gameHeight - 1;
            }
            
            if (colP == gameWidth)
            {
                colP = 0;
            }
            else if (colM == -1)
            {
                colM = gameWidth - 1;
            }

            board[currentTimeIndex][row][colP].inList = 0; 
            board[currentTimeIndex][row][colP].live = 0;
            board[currentTimeIndex][row][colP].count = 0;

            board[currentTimeIndex][row][col].inList = 0;
            board[currentTimeIndex][row][col].live = 0;
            board[currentTimeIndex][row][col].count = 0;

            board[currentTimeIndex][row][colM].inList = 0;
            board[currentTimeIndex][row][colM].live = 0;
            board[currentTimeIndex][row][colM].count = 0;

            board[currentTimeIndex][rowP][colP].inList = 0;
            board[currentTimeIndex][rowP][colP].live = 0;
            board[currentTimeIndex][rowP][colP].count = 0;

            board[currentTimeIndex][rowP][col].inList = 0;
            board[currentTimeIndex][rowP][col].live = 0;
            board[currentTimeIndex][rowP][col].count = 0;

            board[currentTimeIndex][rowP][colM].inList = 0;
            board[currentTimeIndex][rowP][colM].live = 0;
            board[currentTimeIndex][rowP][colM].count = 0;

            board[currentTimeIndex][rowM][colP].inList = 0; 
            board[currentTimeIndex][rowM][colP].live = 0;
            board[currentTimeIndex][rowM][colP].count = 0;

            board[currentTimeIndex][rowM][col].inList = 0;
            board[currentTimeIndex][rowM][col].live = 0;
            board[currentTimeIndex][rowM][col].count = 0;

            board[currentTimeIndex][rowM][colM].inList = 0;
            board[currentTimeIndex][rowM][colM].live = 0;
            board[currentTimeIndex][rowM][colM].count = 0;
        }

        for (int index = 0; index < liveIndex; index++) // add the living and possibly living cells into the iterate list
        {
            int row = indexes[currentTimeIndex][index].row;
            int col = indexes[currentTimeIndex][index].col;
            if (board[nextTimeIndex][row][col].live == 1)
            {
                int rowP = row + 1;
                int rowM = row - 1;
                int colP = col + 1;
                int colM = col - 1;

                if (rowP == gameHeight)
                {
                    rowP = 0;
                }
                else if (rowM == -1)
                {
                    rowM = gameHeight - 1;
                }
                if (colP == gameWidth)
                {
                    colP = 0;
                }
                else if (colM == -1)
                {
                    colM = gameWidth - 1;
                }

                if (board[nextTimeIndex][rowP][colP].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = rowP;
                    indexes[nextTimeIndex][nextIndex].col = colP;
                    board[nextTimeIndex][rowP][colP].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][rowP][col].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = rowP;
                    indexes[nextTimeIndex][nextIndex].col = col;
                    board[nextTimeIndex][rowP][col].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][rowP][colM].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = rowP;
                    indexes[nextTimeIndex][nextIndex].col = colM;
                    board[nextTimeIndex][rowP][colM].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][row][colP].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = row;
                    indexes[nextTimeIndex][nextIndex].col = colP;
                    board[nextTimeIndex][row][colP].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][row][col].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = row;
                    indexes[nextTimeIndex][nextIndex].col = col;
                    board[nextTimeIndex][row][col].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][row][colM].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = row;
                    indexes[nextTimeIndex][nextIndex].col = colM;
                    board[nextTimeIndex][row][colM].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][rowM][colP].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = rowM;
                    indexes[nextTimeIndex][nextIndex].col = colP;
                    board[nextTimeIndex][rowM][colP].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][rowM][col].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = rowM;
                    indexes[nextTimeIndex][nextIndex].col = col;
                    board[nextTimeIndex][rowM][col].inList = 1;
                    nextIndex++;
                }
                if (board[nextTimeIndex][rowM][colM].inList == 0)
                {
                    indexes[nextTimeIndex][nextIndex].row = rowM;
                    indexes[nextTimeIndex][nextIndex].col = colM;
                    board[nextTimeIndex][rowM][colM].inList = 1;
                    nextIndex++;
                }
            }
        }
        
        if (graphics) // graphically display world
        {
            ClearScreen();
            for (int row = 0; row < gameHeight; row++) // loop over the whole world
            {
                for (int col = 0; col < gameWidth; col++)
                {           
                    if (board[nextTimeIndex][row][col].live == 1)
                    {
                        DrawRectangle(col + 0.25, row + 0.25, gameWidth, gameHeight, 0.5, 0.5, 0);
                    }
                }
            }
            Refresh();
            usleep(3000);
        }
        
        time++;
        liveIndex = nextIndex;

        if (time == timesteps) // quit while loop if amount of time steps are reached
        {
            break;
        }
    }

    if (graphics) // close canvas
    {
        FlushDisplay();
        CloseDisplay();
    }
    return 0;
}
