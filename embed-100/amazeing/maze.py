import pygame
import os
import random
from common import *
from grid import GRIDS


class Maze(object):
    WNUM = 15
    HNUM = 10
    GRID_W = WINDOW_W / WNUM
    GRID_H = WINDOW_H / HNUM
    W = GRID_W * WNUM
    H = GRID_H * HNUM

    ND_GAME = 1
    ND_WALL = 2
    ND_WIN = 3

    MV_UP = 0
    MV_RIGHT = 1
    MV_DOWN = 2
    MV_LEFT = 3

    def __init__(self):
        self.surface = pygame.Surface(WINDOW_SIZE).convert()
        player_image = pygame.image.load(
                os.path.join('images', 'player.png'))
        self.player_images = [
                player_image.subsurface((0,0), (20, 20)),
                player_image.subsurface((0,20), (20, 20)),
                ]
        self.grid = [[0 for x in xrange(Maze.WNUM)] for y in xrange(Maze.HNUM)]
        gridmap = random.choice(GRIDS)
        for x in range(Maze.WNUM):
            for y in range(Maze.HNUM):
                self.grid[y][x] = gridmap[y][x]
        self.start_point = [0, 9]
        self.players = [[0,9],[0,9]]
        self.mask = pygame.Surface(WINDOW_SIZE).convert()
        self.move = [[0 for x in xrange(Maze.WNUM)] for y in xrange(Maze.HNUM)]
        self.moveNum = 0
        self.moveCapacity = 0
        self.moveCurrent = [0, 0]

    def _draw_background(self):
        self.surface.fill(COLOR_BLACK)

    def _draw_lines(self):
        for x in range(0, Maze.W+1, Maze.GRID_W):
            pygame.draw.line(self.surface,
                    COLOR_GRID, (x, 0), (x, Maze.H))
        for y in range(0, Maze.H+1, Maze.GRID_H):
            pygame.draw.line(self.surface,
                    COLOR_GRID, (0, y), (Maze.W, y))

    def _draw_grids(self):
        for row in range(Maze.HNUM):
            for col in range(Maze.WNUM):
                color = None
                if self.grid[row][col] == Maze.ND_WALL:
                    color = COLOR_GRAY
                if self.grid[row][col] == Maze.ND_GAME:
                    color = COLOR_RED
                if self.grid[row][col] == Maze.ND_WIN:
                    color = COLOR_WHITE

                if color is not None:
                    pygame.draw.rect(self.surface,
                            color,
                            (col*Maze.GRID_W, row*Maze.GRID_H,
                                Maze.GRID_W, Maze.GRID_H))

    def _draw_players(self):
        for i in range(2):
            pRect = self.player_images[i].get_rect()
            pRect.centerx = self.players[i][0]*Maze.GRID_W + Maze.GRID_W/2
            pRect.centery = self.players[i][1]*Maze.GRID_H + Maze.GRID_H/2
            if i == 0:
                pRect.centery -= 10
            else:
                pRect.centery += 10
            self.surface.blit(self.player_images[i], pRect)

    def _draw_moves(self):
        self.mask.fill(COLOR_BLACK)
        self.mask.set_colorkey(COLOR_BLACK)
        for row in range(Maze.HNUM):
            for col in range(Maze.WNUM):
                if self.move[row][col] > 1:
                    pygame.draw.rect(self.mask,
                            COLOR_MOVE,
                            (col*Maze.GRID_W, row*Maze.GRID_H,
                                Maze.GRID_W, Maze.GRID_H))
        self.mask.set_alpha(100)
        self.surface.blit(self.mask, (0,0))

    def get_surface(self):
        self._draw_background()
        self._draw_grids()
        self._draw_players()
        self._draw_moves()
        self._draw_lines()
        return self.surface

    # player

    def set_player(self, p, pos):
        lpos = list(pos)
        if lpos[0] < 0:
            lpos[0] = 0
        if lpos[1] < 0:
            lpos[1] = 0
        if lpos[0] >= Maze.WNUM:
            lpos[0] = Maze.WNUM-1
        if lpos[1] >= Maze.HNUM:
            lpos[1] = Maze.HNUM-1
        self.players[p] = lpos

    def get_player(self, p):
        return list(self.players[p])

    # moves
    def clear_moves(self):
        self.move = [[0 for x in xrange(Maze.WNUM)] for y in xrange(Maze.HNUM)]
        self.moveNum = 0
        self.moveCapacity = 0

    def init_moves(self, p, n):
        self.clear_moves()
        x = self.players[p][0]
        y = self.players[p][1]
        self.move[y][x] = 1
        self.moveNum = 0
        self.moveCapacity = n
        self.moveCurrent = [x, y]

    def move_direction(self, d):
        x, y = self.moveCurrent
        new_x, new_y = x, y
        if d == Maze.MV_UP:
            new_y = new_y - 1
        elif d == Maze.MV_DOWN:
            new_y = new_y + 1
        elif d == Maze.MV_RIGHT:
            new_x = new_x + 1
        elif d == Maze.MV_LEFT:
            new_x = new_x - 1

        if new_x < 0 or new_y < 0 or new_x >= Maze.WNUM or new_y >= Maze.HNUM:
            return
        if self.grid[new_y][new_x] == Maze.ND_WALL:
            return

        if self.move[new_y][new_x] == self.moveNum and self.moveNum != 0:
            self.move[y][x] = 0
            self.moveNum = self.moveNum - 1
            self.moveCurrent = [new_x, new_y]
        elif self.move[new_y][new_x] == 0 and self.moveNum < self.moveCapacity:
            self.moveNum = self.moveNum + 1
            self.move[new_y][new_x] = self.moveNum + 1
            self.moveCurrent = [new_x, new_y]

    def apply_moves(self, p):
        if self.moveNum == 0 or self.moveNum != self.moveCapacity:
            return False
        self.set_player(p, self.moveCurrent)
        self.clear_moves()
        return True

    # states
    def is_win(self, p):
        x = self.players[p][0]
        y = self.players[p][1]
        return self.grid[y][x] == Maze.ND_WIN

    def is_game(self, p):
        x = self.players[p][0]
        y = self.players[p][1]
        return self.grid[y][x] == Maze.ND_GAME
