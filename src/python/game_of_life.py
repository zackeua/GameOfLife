import numpy as np
import argparse

import numpy_game as numpy_game
import torch_game as torch_game

IMPL_CHOISES = ['cpu', 'gpu', 'numpy']


def get_game(args, board: np.ndarray):
    if args['mode'] == 'numpy':
        game = numpy_game.NumpyGame(board, args['visualize'])
    else:
        game = torch_game.TorchGame(board, args['visualize'], args['mode'] == 'gpu')
    return game


def main(args):

    np.random.seed(args['seed'])

    if args['benchmark']:
        import time
        # sizes = [100, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000]
        sizes = [1000, 9000, 10000, 11000, 12000]
        times = []
        number_of_iterations = 1000
        for side_length in sizes: # Timing slow for first torch gpu instance
            start = time.time()
            board = np.random.randint(0, 1 + 1, [side_length, side_length])
            game = get_game(args, board)
            game.run(number_of_iterations)
            times.append(time.time() - start)
            print(side_length, times[-1])
        for side_length, seconds in zip(sizes, times):
            print(side_length, seconds)
            
        
    else:
        board = np.random.randint(0, 1 + 1, list(map(int, args['resolution'].split(','))))
        game = get_game(args, board)
        game.run(int(args['iterations']))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--mode', choices=IMPL_CHOISES, help='Choose execution mode.', default='cpu')
    parser.add_argument('-r', '--resolution', help='Input display resolution.', default='100,100')
    parser.add_argument('-v', '--visualize', help='Visualize run.', default=False)
    parser.add_argument('-n', '--iterations', help='Specify the number of iterations to run the program for.', default=1000)
    parser.add_argument('-s', '--seed', help='Random seed for grid initialization.', default = 420)
    parser.add_argument('-t', '--time', help='Time run.', default=False)
    parser.add_argument('--benchmark', help='Perform extensive performance tests.')
    args = vars(parser.parse_args())

    main(args)

