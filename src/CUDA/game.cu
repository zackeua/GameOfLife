#include <stdio.h>

__device__ int pointCalculation(const int *current, const int xPosition, const int yPosition, const int gameWidth, const int gameHeight) {

    int x = xPosition;
    int x_less = (x - 1) % gameWidth;
    int x_more = (x + 1) % gameWidth;

    int y = yPosition;
    int y_less = (y - 1) % gameHeight;
    int y_more = (y + 1) % gameHeight;


    int total = current[gameWidth * y_less + x_less] +      current[gameWidth * y_less + x] + current[gameWidth * y_less + x_more] +
                current[gameWidth * y      + x_less] + 10 * current[gameWidth * y      + x] + current[gameWidth * y      + x_more] +
                current[gameWidth * y_more + x_less] +      current[gameWidth * y_more + x] + current[gameWidth * y_more + x_more];
    
    return total;
    
}

__device__ void kernel(const int *current, int *next, const int xPosition, const int yPosition, const int gameWidth, const int gameHeight) {

    int total = pointCalculation(current, xPosition, yPosition, gameWidth, gameHeight);
    if (total == 12 || total == 13 || total == 3) {
        next[gameWidth * yPosition + xPosition] = 1;
    } else {
        next[gameWidth * yPosition + xPosition] = 0;
    }

}

__global__ void step(const int *current, int *next, const int gameWidth, const int gameHeight) {
    
    for (int xPosition = 0; xPosition < gameWidth; xPosition++) {
        for (int yPosition = blockIdx.x * blockDim.x + threadIdx.x; yPosition < gameHeight; yPosition += blockDim.x * gridDim.x) {
            kernel(current, next, xPosition, yPosition, gameWidth, gameHeight);
        }
    }


}


__host__ void initializeBoards(int *hostBoard, int* deviceBoard1, int* deviceBoard2, const int gameWidth, const int gameHeight) {
   
}

__host__ void cleanup(int *hostBoard, int* deviceBoard1, int* deviceBoard2) {
    
    free(hostBoard);
    cudaFree(deviceBoard1);
    cudaFree(deviceBoard2);
}


__host__ void showBoard(const int* hostBoard, const int gameWidth, const int gameHeight) {
    
    for (int row = 0; row < gameWidth; row++) // loop over the whole world
    {
        for (int col = 0; col < gameHeight; col++)
        {           
            if (hostBoard[col * gameWidth + row] == 1)
            {
                fprintf(stdout, "*");
            }
            else {
                fprintf(stdout, " ");
            }
        }
        fprintf(stdout, "\n");
    }
        fprintf(stdout, "-----------------\n");
        fprintf(stdout, "-----------------\n");
    
    
}


__host__ void run(const int gameWidth, const int gameHeight, const int iterations, const bool graphics) {

    // Setting the number of threads
    int number_of_blocks = 10;
    int threads_per_block = 10;
	
    // Allocating the CUDA status
    cudaError_t cudaStatus;

    // Setting the CUDA device
    cudaStatus = cudaSetDevice(0);

    // Checking if that worked
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice0 failed!\n");
        return;
    }

    int *hostBoard = nullptr;
    int *deviceBoard1 = nullptr;
    int *deviceBoard2 = nullptr;


    hostBoard = (int*)malloc(gameWidth * gameHeight * sizeof(int));

    for (int i = 0; i < gameWidth * gameHeight; i++) {
        hostBoard[i] = rand()%2;
    }

    hostBoard[gameWidth * 3 + 20] = 1;
    hostBoard[gameWidth * 4 + 20] = 1;
    hostBoard[gameWidth * 3 + 21] = 1;


    cudaMalloc((void**)&deviceBoard1, gameWidth * gameHeight * sizeof(int)); 
    cudaMalloc((void**)&deviceBoard2, gameWidth * gameHeight * sizeof(int)); 

    cudaMemcpy(deviceBoard1, hostBoard, gameWidth*gameHeight*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(deviceBoard2, hostBoard, gameWidth*gameHeight*sizeof(int), cudaMemcpyHostToDevice);


    if (hostBoard == nullptr) {
        fprintf(stderr, "hostboard is null");
    }
    else {
        showBoard(hostBoard, gameWidth, gameHeight);
    }


    if (deviceBoard1 == nullptr) {
        fprintf(stderr, "deviceBoard1 is null");
    }

    if (deviceBoard2 == nullptr) {
        fprintf(stderr, "deviceBoard1 is null");
    }


    for (int i = 0; i < iterations ; i++)
    {
        step<<<number_of_blocks, threads_per_block>>>(deviceBoard1, deviceBoard2, gameWidth, gameHeight);
        i++;
        if (graphics) {
            cudaMemcpy(hostBoard, deviceBoard2, gameWidth*gameHeight*sizeof(*deviceBoard2), cudaMemcpyDeviceToHost);
            showBoard(hostBoard, gameWidth, gameHeight);
        }

        if (i < iterations) {
            step<<<number_of_blocks, threads_per_block>>>(deviceBoard2, deviceBoard1, gameWidth, gameHeight);
            i++;
            if (graphics) {
                cudaMemcpy(hostBoard, deviceBoard1, gameWidth*gameHeight*sizeof(*deviceBoard1), cudaMemcpyDeviceToHost);
                showBoard(hostBoard, gameWidth, gameHeight);
            }
        }
    }
    
    cleanup(hostBoard, deviceBoard1, deviceBoard2);
}


int main(int argc, char *argv[]) {
    
    if (argc != 6) {
        printf("Input on this form:\n");
        printf("./game gameWidth gameHeight rand_seed timesteps graphics n_threads\n");
        printf("gameWidth is the width of the gameworld.\n");
        printf("gameheight is the height if the gameworld.\n");
        printf("rand_seed is the random seed to be used.\n");
        printf("timesteps is the amount of timesteps to be taken. ");
        printf("(use any amount of timesteps less than or equal to 0 for (nearly)infinite amount of timesteps)\n");
        printf("graphics is if you want to display the world graphically or not.\n");
        return -1;
    }


    const int gameWidth = std::atoi(argv[1]); // get input values;
    const int gameHeight = std::atoi(argv[2]);
    std::srand(std::atoi(argv[3]));
    const int timesteps = std::atoi(argv[4]);
    const bool graphics = std::atoi(argv[5]) == 1;

    
    
    run(gameWidth, gameHeight, timesteps, graphics);
    
}
