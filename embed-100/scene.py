import pygame
from common import *

class TextScene(object):
    def __init__(self, title, info):
        self.surface = pygame.Surface(WINDOW_SIZE).convert()
        self.surface.fill(COLOR_BLACK)

        font = pygame.font.Font(None, 80)
        text = font.render(title, True, COLOR_WHITE, COLOR_BLACK)

        textRect = text.get_rect()
        textRect.centerx = self.surface.get_rect().centerx
        textRect.centery = self.surface.get_rect().centery

        self.surface.blit(text, textRect)

        font = pygame.font.Font(None, 20)
        text = font.render(info, True,
                COLOR_WHITE, COLOR_BLACK)

        textRect = text.get_rect()
        textRect.centerx = self.surface.get_rect().centerx
        textRect.centery = self.surface.get_rect().centery + 45

        self.surface.blit(text, textRect)

    def get_surface(self):
        return self.surface

class StartScene(TextScene):
    def __init__(self):
        super(StartScene, self).__init__('Amazeing',
                'Hit the Openmoko or `f\' to start.')

class GameIntroScene(TextScene):
    def __init__(self):
        super(GameIntroScene, self).__init__('Game!',
                'Hit the Openmoko to start.')

class GamePassScene(TextScene):
    def __init__(self):
        super(GamePassScene, self).__init__('Pass!',
                'Hit the Openmoko to continue.')

class GameFailScene(TextScene):
    def __init__(self):
        super(GameFailScene, self).__init__('Fail!',
                'You must go back to the start point. Hit Openmoko!')

class DiceScene(object):
    def __init__(self):
        self.surface = pygame.Surface(WINDOW_SIZE).convert()
        self.background = pygame.Surface(WINDOW_SIZE).convert()
        self.background.fill(COLOR_BLACK)

        font = pygame.font.Font(None, 80)
        text = font.render('Dice', True, COLOR_WHITE, COLOR_BLACK)

        textRect = text.get_rect()
        textRect.centerx = self.background.get_rect().centerx
        textRect.centery = self.background.get_rect().centery - 80

        self.background.blit(text, textRect)

        self.dice_image = pygame.image.load('images/dice.png').convert()


    def update_dice(self, p, num):
        self.surface.blit(self.background, (0, 0))

        font = pygame.font.Font(None, 20)
        text = font.render('Player %d, hit the `f\' key!' % (p+1),
                True, COLOR_WHITE, COLOR_BLACK)

        textRect = text.get_rect()
        textRect.centerx = self.surface.get_rect().centerx
        textRect.centery = self.surface.get_rect().centery + 100

        self.surface.blit(text, textRect)
        dice = self.dice_image.subsurface((100*(num-1),0), (100, 100))
        diceRect = dice.get_rect()
        diceRect.centerx = self.surface.get_rect().centerx
        diceRect.centery = self.surface.get_rect().centery

        self.surface.blit(dice, diceRect)

    def get_surface(self):
        return self.surface

class WinScene(object):
    def __init__(self):
        self.background = pygame.Surface(WINDOW_SIZE).convert()
        self.surface = pygame.Surface(WINDOW_SIZE).convert()
        self.background.fill(COLOR_BLACK)

        font = pygame.font.Font(None, 80)
        text = font.render('The Winner', True, COLOR_WHITE, COLOR_BLACK)

        textRect = text.get_rect()
        textRect.centerx = self.background.get_rect().centerx
        textRect.centery = self.background.get_rect().centery

        self.background.blit(text, textRect)

    def update(self, p):
        self.surface.blit(self.background, (0, 0))

        font = pygame.font.Font(None, 40)
        text = font.render('Player %d' % (p+1), True,
                COLOR_WHITE, COLOR_BLACK)

        textRect = text.get_rect()
        textRect.centerx = self.surface.get_rect().centerx
        textRect.centery = self.surface.get_rect().centery + 45

        self.surface.blit(text, textRect)


    def get_surface(self):
        return self.surface
