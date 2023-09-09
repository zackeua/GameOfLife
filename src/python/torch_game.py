import numpy as np
import matplotlib.pyplot as plt
import torch
import argparse

class TorchGame():

    def __init__(self, initial: np.ndarray, visualize: bool = False, use_gpu: bool = False):
        super(TorchGame, self).__init__()
        self._initial = torch.from_numpy(initial)
        self._visualize  = visualize
        self._use_gpu = use_gpu
        self._next = torch.from_numpy(initial)
    
    def _kernel(self, x, device):

        x = x.expand(1, 1, *x.size()).float()
        padded = torch.nn.functional.pad(x, (1,1,1,1), mode='circular')
        padded = padded.expand(*padded.size()).float()
        filter = torch.tensor([[1, 1, 1], [1, 10, 1], [1, 1, 1]], device=device)
        f = filter.expand(1,1,3,3).float()
        condition = torch.nn.functional.conv2d(padded, f, stride=1, padding=0)
        condition = condition[0][0].int()
        
        result = (condition == 12) | (condition == 13) | (condition == 3)

        return result.int()

    def run(self, number_of_iterations: int):
        if torch.cuda.is_available() and self._use_gpu:
            device = torch.device('cuda')
        else:
            device = torch.device('cpu')
        print(f'Using device: {device}')
        current_iteration = 0
        self._initial = self._initial.to(device=device)
        self._next = self._next.to(device=device)
        #board0 = torch.tensor(initial, device=device)
        while current_iteration < number_of_iterations:
            self._next = self._kernel(self._initial, device=device)
            current_iteration += 1
            if self._visualize: self.show_board(self._next.cpu())
            
            return_0 = False
            if current_iteration < number_of_iterations:
                self._initial = self._kernel(self._next, device=device)
                current_iteration += 1
                return_0 = True
                if self._visualize: self.show_board(self._initial.cpu())
    
        return self._initial if return_0 else self._next

    def show_board(self, board):
        plt.imshow(board.numpy(), interpolation='None')
        plt.pause(0.001)
        plt.cla()

def main(args):
    np.random.seed(args['seed'])
    board = np.random.randint(0, 1 + 1, list(map(int, args['resolution'].split(','))))
    game = TorchGame(board, args['visualize'], args['mode'] == 'gpu')
    game.run(args['iterations'])
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--mode', choices=['cpu', 'gpu'], help='Choose execution mode.', default='cpu')
    parser.add_argument('-r', '--resolution', help='Input display resolution.', default='100,100')
    parser.add_argument('-v', '--visualize', help='Visualize run.', default=False)
    parser.add_argument('-n', '--iterations', help='Specify the number of iterations to run the program for.', default=1000)
    parser.add_argument('-s', '--seed', help='Random seed for grid initialization.', default = 420)
    parser.add_argument('-t', '--time', help='Time run.', default=False)
    parser.add_argument('--benchmark', help='Perform extensive performance tests.')
    args = vars(parser.parse_args())

    main(args)