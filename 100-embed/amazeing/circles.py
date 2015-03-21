import pygame
import os
from math import sqrt
from random import randint
from common import *
from maze import Maze


class Circles(object):
    CNUM = 6
    XNUM = 20
    COUNTBASE = 20
    COUNT = COUNTBASE

    def __init__(self):
        self.surface = pygame.Surface(WINDOW_SIZE).convert()
        self.background = pygame.Surface(WINDOW_SIZE).convert()
        self.circles = []
        self.crosses = []
        self.center = [WINDOW_W/2, WINDOW_H/2]
        self.radius = min(WINDOW_W/2, WINDOW_H/2)/2
        self.speed = 100
        self.count = Circles.COUNT

    def countdown(self, t):
        passed = (pygame.time.get_ticks() - t) / 1000
        self.count = max(Circles.COUNT - passed, 0)
        self.update()
        return self.count <= 0

    def move_direction(self, t, d):
        dist = t * self.speed
        if d == Maze.MV_UP:
            self.center[1] -= dist
        elif d == Maze.MV_RIGHT:
            self.center[0] += dist
        elif d == Maze.MV_DOWN:
            self.center[1] += dist
        elif d == Maze.MV_LEFT:
            self.center[0] -= dist

        self.update()

    def enlarge(self, t):
        dist = t * self.speed / 2.0
        self.radius += dist
        self.update()

    def compress(self, t):
        dist = t * self.speed / 2.0
        if self.radius - dist > 5:
            self.radius -= dist
        self.update()

    def is_win(self):
        for item in self.circles:
            dist = sqrt((item[0]-self.center[0])**2 + (item[1]-self.center[1])**2)
            if dist + item[2] > self.radius:
                return False
        for item in self.crosses:
            dist = sqrt((item[0]-self.center[0])**2 + (item[1]-self.center[1])**2)
            if dist - item[2] + 1 < self.radius:
                return False
        return True

    def update(self):
        self.surface.blit(self.background, (0, 0))
        center = (int(self.center[0]), int(self.center[1]))
        pygame.draw.circle(self.surface,
                COLOR_GRAY, center, int(self.radius), 1)
        font = pygame.font.Font(None, 40)
        text = font.render('%d' % self.count, True, COLOR_WHITE, COLOR_BLACK)
        self.surface.blit(text, (0, 0))


    def generate(self):
        self.background.fill(COLOR_BLACK)
        self.center = [WINDOW_W/2, WINDOW_H/2]
        self.radius = min(WINDOW_W/2, WINDOW_H/2)/2
        Circles.COUNT = Circles.COUNTBASE + randint(0, 10)
        self.count = Circles.COUNT

        c_pos = (randint(40, WINDOW_W-40), randint(40, WINDOW_H-40))
        c_radius = randint(100, 300)
        # c = pygame.draw.circle(self.background,
        #        COLOR_GRAY, c_pos, c_radius, 1)

        self.circles = []
        while len(self.circles) < Circles.CNUM:
            r_radius = randint(10, 40)
            r_pos = (randint(c_pos[0]-c_radius+r_radius,
                c_pos[0]+c_radius-r_radius),
                    randint(c_pos[1]-c_radius+r_radius,
                        c_pos[1]+c_radius-r_radius))
            dist = sqrt((r_pos[0]-c_pos[0])**2 + (r_pos[1]-c_pos[1])**2)
            if (r_pos[0] - r_radius < 0 or r_pos[0] + r_radius > WINDOW_W
                    or r_pos[1] - r_radius < 0 or r_pos[1] + r_radius > WINDOW_H):
                continue
            if dist + r_radius < c_radius:
                r = pygame.draw.circle(self.background,
                        COLOR_GRAY, r_pos, r_radius)
                self.circles.append((r_pos[0], r_pos[1], r_radius))

        self.crosses = []
        while len(self.crosses) < Circles.XNUM:
            r_pos = (randint(0, WINDOW_W-1), randint(0, WINDOW_H-1))
            r_radius = randint(10, 40)
            dist = sqrt((r_pos[0]-c_pos[0])**2 + (r_pos[1]-c_pos[1])**2)
            if dist > c_radius + r_radius + 2:
                r = pygame.draw.circle(self.background,
                        COLOR_RED, r_pos, r_radius)
                x, y = r.centerx, r.centery
                sdist = r_radius/3
                pygame.draw.line(self.background, COLOR_BLACK,
                        (x-sdist, y-sdist), (x+sdist, y+sdist), 3)
                pygame.draw.line(self.background, COLOR_BLACK,
                        (x+sdist, y-sdist), (x-sdist, y+sdist), 3)
                self.crosses.append((r_pos[0], r_pos[1], r_radius))

        self.update()

    def get_surface(self):
        return self.surface
