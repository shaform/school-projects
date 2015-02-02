import pygame
import random
from pygame.locals import *
from common import *
from maze import *
from scene import *
from circles import *
from tcpserver import EventServer

DEBUG = False

class Game(object):
    __single = None

    # states
    ST_START = 1
    ST_RANDOM = 2
    ST_MOVE = 3
    ST_GAMEINTRO = 4
    ST_GAMESTART = 5
    ST_GAMEPASS = 6
    ST_GAMEFAIL = 7
    ST_WIN = 8

    def __init__(self):
        self.debug = DEBUG

        pygame.init()
        pygame.display.set_caption("Amazeing")
        pygame.event.set_blocked([KEYUP, MOUSEMOTION,
            MOUSEBUTTONDOWN, MOUSEBUTTONUP])

        self.server = EventServer()

        self.clock = pygame.time.Clock()
        self.time_passed = 0
        self.screen = pygame.display.set_mode(TOTAL_SIZE, 0, 32)
        self.maze = Maze()
        self.circles = Circles()
        self.state = Game.ST_START
        self.next_state = Game.ST_START
        self.random_num = 1
        self.wait_from = 0
        self.wait_for = 0
        self.countdown_from = 0

        # scenes
        self.startscene = StartScene()
        self.dicescene = DiceScene()
        self.winscene = WinScene()
        self.passscene = GamePassScene()
        self.failscene = GameFailScene()
        self.gamescene = GameIntroScene()
        self.init_events()

    def init_events(self):
        self.key = [None, None]
        self.press = [False, False]
        self.omPress = False
        self.omKey = None
        self.omBigger = False
        self.omSmaller = False
        self.testKey = False
        self.restart = False

    def __del__(self):
        pygame.quit()

    def waiting(self):
        return self.next_state != self.state

    def wait_time(self, t, st):
        self.next_state = st
        self.wait_from = pygame.time.get_ticks()
        self.wait_for = t*1000

    def update_display(self):
        if self.state == Game.ST_START:
            self.screen.blit(self.startscene.get_surface(), (0,0))
        elif self.state == Game.ST_RANDOM:
            self.dicescene.update_dice(self.turn, self.random_num)
            self.screen.blit(self.dicescene.get_surface(), (0,0))
        elif self.state == Game.ST_MOVE:
            self.screen.blit(self.maze.get_surface(), (0,0))
        elif self.state == Game.ST_WIN:
            self.screen.blit(self.winscene.get_surface(), (0,0))
        elif self.state == Game.ST_GAMEINTRO:
            self.screen.blit(self.gamescene.get_surface(), (0,0))
        elif self.state == Game.ST_GAMESTART:
            self.screen.blit(self.circles.get_surface(), (0,0))
        elif self.state == Game.ST_GAMEPASS:
            self.screen.blit(self.passscene.get_surface(), (0,0))
        elif self.state == Game.ST_GAMEFAIL:
            self.screen.blit(self.failscene.get_surface(), (0,0))
        pygame.display.update()

    def handle_events(self, events):
        self.init_events()
        for event in events:
            if event.type == QUIT:
                return False
            elif event.type == KEYDOWN:
                if self.debug:
                    if event.key == K_SPACE:
                        self.omPress = True
                    elif event.key == K_e:
                        self.omBigger = True
                    elif event.key == K_q:
                        self.omSmaller = True
                    elif event.key == K_g:
                        self.testKey = True
                    elif event.key == K_f:
                        self.press[1] = True
                    elif event.key == K_w:
                        self.omKey = self.key[1] = Maze.MV_UP
                    elif event.key == K_d:
                        self.omKey = self.key[1] = Maze.MV_RIGHT
                    elif event.key == K_s:
                        self.omKey = self.key[1] = Maze.MV_DOWN
                    elif event.key == K_a:
                        self.omKey = self.key[1] = Maze.MV_LEFT
                if event.key == K_w:
                    self.key[0] = Maze.MV_UP
                elif event.key == K_d:
                    self.key[0] = Maze.MV_RIGHT
                elif event.key == K_s:
                    self.key[0] = Maze.MV_DOWN
                elif event.key == K_a:
                    self.key[0] = Maze.MV_LEFT
                elif event.key == K_f:
                    self.press[0] = True
                elif event.key == K_ESCAPE:
                    self.restart = True

            elif event.type == USEREVENT:
                if event.key == EventServer.UP:
                    self.key[event.player] = Maze.MV_UP
                elif event.key == EventServer.RIGHT:
                    self.key[event.player] = Maze.MV_RIGHT
                elif event.key == EventServer.DOWN:
                    self.key[event.player] = Maze.MV_DOWN
                elif event.key == EventServer.LEFT:
                    self.key[event.player] = Maze.MV_LEFT

                elif event.key == EventServer.PRESS:
                    self.press[event.player] = True

                elif event.key == EventServer.OMPRESS:
                    self.omPress = True
                elif event.key == EventServer.OMBIG:
                    self.omBigger = True
                elif event.key == EventServer.OMSMALL:
                    self.omSmaller = True

                elif event.key == EventServer.OMUP:
                    self.omKey = Maze.MV_UP
                elif event.key == EventServer.OMRIGHT:
                    self.omKey = Maze.MV_RIGHT
                elif event.key == EventServer.OMDOWN:
                    self.omKey = Maze.MV_DOWN
                elif event.key == EventServer.OMLEFT:
                    self.omKey = Maze.MV_LEFT

        return True

    def main_loop(self):
        if self.waiting():
            return

        if self.state == Game.ST_START:
            if self.omPress or self.press[0] or self.press[1]:
                self.next_state = Game.ST_RANDOM
                self.turn = 0

        elif self.state == Game.ST_RANDOM:
            self.random_num = random.randint(1,6)
            if self.press[self.turn]:
                self.maze.init_moves(self.turn, self.random_num)
                self.wait_time(1, Game.ST_MOVE)

        elif self.state == Game.ST_MOVE:
            if self.key[self.turn] is not None:
                self.maze.move_direction(self.key[self.turn])
            elif self.press[self.turn]:
                if self.maze.apply_moves(self.turn):
                    if self.maze.is_win(self.turn):
                        self.winscene.update(self.turn)
                        self.wait_time(1, Game.ST_WIN)
                    elif self.maze.is_game(self.turn):
                        self.wait_time(1, Game.ST_GAMEINTRO)
                    else:
                        self.turn = (self.turn + 1 ) % 2
                        self.wait_time(1, Game.ST_RANDOM)
            elif self.restart:
                self.next_state = Game.ST_RANDOM
                self.turn = 0
                self.maze = Maze()

        elif self.state == Game.ST_GAMEINTRO:
            if self.omPress:
                self.countdown_from = pygame.time.get_ticks()
                self.next_state = Game.ST_GAMESTART
                self.circles.generate()

        elif self.state == Game.ST_GAMESTART:
            if self.omPress:
                if self.circles.is_win():
                    self.next_state = Game.ST_GAMEPASS
                else:
                    self.next_state = Game.ST_GAMEFAIL
            elif self.testKey:
                self.circles.generate()
            elif self.omKey is not None:
                self.circles.move_direction(self.time_passed, self.omKey)
            elif self.omBigger:
                self.circles.enlarge(self.time_passed)
            elif self.omSmaller:
                self.circles.compress(self.time_passed)

            if self.circles.countdown(self.countdown_from):
                self.next_state = Game.ST_GAMEFAIL

        elif self.state == Game.ST_GAMEFAIL:
            if self.omPress:
                self.next_state = Game.ST_RANDOM
                self.maze.set_player(self.turn, self.maze.start_point)
                self.turn = (self.turn + 1 ) % 2

        elif self.state == Game.ST_GAMEPASS:
            if self.omPress:
                self.next_state = Game.ST_RANDOM
                self.turn = (self.turn + 1 ) % 2

        elif self.state == Game.ST_WIN:
            if self.omPress or self.press[0] or self.press[1]:
                self.next_state = Game.ST_START
                self.turn = 0
                self.maze = Maze()


    def start(self):
        while True:
            if not self.handle_events(pygame.event.get()):
                return
            self.main_loop()
            self.update_display()
            self.time_passed = self.clock.tick(50) / 1000.0
            if pygame.time.get_ticks() - self.wait_from > self.wait_for:
                self.state = self.next_state

if __name__ == "__main__":
    game = Game()
    game.start()
