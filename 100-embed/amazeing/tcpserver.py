import socket
import threading
import SocketServer
import pygame
from pygame.locals import *
import re

class EventServer(object):
    UP = 1
    RIGHT = 2
    DOWN = 3
    LEFT = 4
    PRESS = 5
    OMPRESS = 6
    OMUP = 7
    OMRIGHT = 8
    OMDOWN = 9
    OMLEFT = 10
    OMBIG = 11
    OMSMALL = 12

    HOST, PORT = "0.0.0.0", 2000

    alive = True

    def __init__(self):
        self.server = ThreadedTCPServer((EventServer.HOST, EventServer.PORT),
                ThreadedTCPRequestHandler)
        server_thread = threading.Thread(target=self.server.serve_forever)
        server_thread.daemon = True
        EventServer.alive = True
        server_thread.start()

    def __del__(self):
        EventServer.alive = False
        self.server.shutdown()

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        sfile = self.request.makefile()
        regex = re.compile('(\d) (\w+)')
        for line in sfile:
            if not EventServer.alive:
                break
            m = regex.match(line)
            if m is not None:
                text = m.group(2)
                k = None
                if text == 'w':
                    k = EventServer.UP
                elif text == 'd':
                    k = EventServer.RIGHT
                elif text == 's':
                    k = EventServer.DOWN
                elif text == 'a':
                    k = EventServer.LEFT
                elif text == 'f':
                    k = EventServer.PRESS
                elif text == 'om_p':
                    k = EventServer.OMPRESS
                elif text == 'om_u':
                    k = EventServer.OMUP
                elif text == 'om_r':
                    k = EventServer.OMRIGHT
                elif text == 'om_d':
                    k = EventServer.OMDOWN
                elif text == 'om_l':
                    k = EventServer.OMLEFT
                elif text == 'om_b':
                    k = EventServer.OMBIG
                elif text == 'om_s':
                    k = EventServer.OMSMALL

                if k is not None:
                    keyevent = pygame.event.Event(USEREVENT,
                            player=int(m.group(1)),
                            key=k)
                    pygame.event.post(keyevent)

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    allow_reuse_address = True
