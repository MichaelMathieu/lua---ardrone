import pygame
import messages

sender = messages.Sender()
deltas = {}
takeoff = 0
def test():
    event = pygame.event.poll()
    if event.type != pygame.NOEVENT:
        def deltakey(pressed, a):
            if a not in deltas:
                deltas[a] = 0
            k = pressed[a]
            if k != deltas[a]:
                deltas[a] = k
                return k
            else:
                return None
        pressed = pygame.key.get_pressed()
        space = deltakey(pressed, pygame.K_SPACE)
        e = deltakey(pressed, pygame.K_e)
        c = deltakey(pressed, pygame.K_c)
        m = deltakey(pressed, pygame.K_m)
        if space != None:
            if space == 1:
                global takeoff
                takeoff = 1 - takeoff
                sender.takeoff(takeoff)
        if e != None:
            sender.emergency(e)
        if c == 1:
            sender.resettrim()
            print "Resetting trim"
        if m == 1:
            sender.magcalib()
            print "Calib mag"
        ud = pressed[pygame.K_UP] - pressed[pygame.K_DOWN]
        lr = pressed[pygame.K_LEFT] - pressed[pygame.K_RIGHT]
        rot = pressed[pygame.K_a]-pressed[pygame.K_d]
        alt = pressed[pygame.K_w]-pressed[pygame.K_s]
        sender.command(ud, lr, rot, alt)
    else:
        sender.update()

pygame.init()
window = pygame.display.set_mode((100,100))

messages.start(test, 30)
