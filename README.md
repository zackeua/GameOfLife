# GameOfLife
This repo contains a few implementations of Conway's Game of Life.

The current implementations are:
* A pthread parallelized c version with various optimizations.
* A naive convolution based NumPy implementation.
* A Pytorch implementation of the same idea as the NumPy implementation in order to be able to leverage the GPU to parallelize the computations.


## Run and build

### c pthread implementation
To run the c pthread implementation navigate to

        src/c/

and run

        make

to compile the program.
Run the program as follows:

        ./game <height> <width> <random seed> <timesteps> <graphics> <number of threads>


### CUDA implementation
To run the CUDA implementation navigate to

        src/CUDA/

and run

        make

to compile the program.
Run the program as follows:

        ./game <height> <width> <random seed> <timesteps> <graphics>

### The python implementations
To rn the python implementations navigate to

        src/python/

and create a python environment with the nessecary packages using

        conda env create --name game --file environment.yml

activate the new environment and run

        python game_of_life.py --help

to get more info on the possible settings for the implementations.