import numpy as np
import matplotlib.pyplot as plt
import scipy
import argparse

class NumpyGame():

    def __init__(self, initial: np.ndarray, visualize: bool = False) -> None:
        self._initial = initial
        self._visualize  = visualize


    def _kernel(self, board: np.ndarray) -> np.ndarray:
        weight = np.ones((3, 3), dtype=np.uint8)
        weight[1,1] = 10
        return scipy.signal.convolve2d(board, weight, mode='same', boundary='wrap')

    def _step(self, board: np.ndarray) -> np.ndarray:
        condition = self._kernel(board)

        result = (condition == 12) | (condition == 13) | (condition == 3)

        return np.array(result, dtype=np.int8)

    def run(self, number_of_iterations: int):
        current_iteration = 0
        board0 = self._initial
        while current_iteration < number_of_iterations:
            board1 = self._step(board0)
            current_iteration += 1

            if self._visualize: self.show_board(board1)
            
            return_0 = False
            if current_iteration < number_of_iterations:
                board0 = self._step(board1)
                current_iteration += 1
                return_0 = True
                if self._visualize: self.show_board(board0)
    
        return board0 if return_0 else board1

    def show_board(self, board):
        plt.imshow(board, interpolation='None')
        plt.pause(0.001)
        plt.cla()



def main(args):
    np.random.seed(args['seed'])
    board = np.random.randint(0, 1 + 1, list(map(int, args['resolution'].split(','))))
    game = NumpyGame(board, args['visualize'])
    
    game.run(args['iterations'])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-r', '--resolution', help='Input display resolution.', default='100,100')
    parser.add_argument('-v', '--visualize', help='Visualize run.', default=False)
    parser.add_argument('-n', '--iterations', help='Specify the number of iterations to run the program for.', default=1000)
    parser.add_argument('-s', '--seed', help='Random seed for grid initialization.', default = 420)
    parser.add_argument('-t', '--time', help='Time run.', default=False)
    parser.add_argument('--benchmark', help='Perform extensive performance tests.')
    args = vars(parser.parse_args())

    main(args)