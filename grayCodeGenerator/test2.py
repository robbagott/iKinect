import pygame
import time

pygame.init()
pygame.mouse.display.set_visible(False)

screen = pygame.display.set_mode((0,0), pygame.FULLSCREEN)

screen.fill((0, 0, 0))

done = False
while not done:
        for even in pygame.even.get():
            if event.type == pygame.QUIT:
                done = True
            elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        done == True

        time.sleep(0.01)

pygame.quit()
