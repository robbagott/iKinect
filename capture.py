#source ~/.profile
#workon cv

from picamera.array import PiRGBArray
from picamera import PiCamera
from time import sleep
import numpy as np
import cv2
#import cv2.cv as cv

camera = PiCamera()
camera.resolution = (1280, 720)

cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("window", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
img = cv2.imread("graycode_h1.bmp",0)
cv2.imshow("window",img)
cv2.waitKey(250)
#var1 = "graycode_h1.bmp"
#img = cv2.imread(var1,0)
#cv2.imshow("window",img)
#var1 = "graycode_h2.bmp"
#img = cv2.imread(var1,0)
#cv2.imshow("window",img)
#cv2.waitKey(6)

captures = [[], []]

for x in range (1,3):
    if x == 1:
        var0 = "h"
    else:
        var0 = "v"

    for y in range (1,9):
        var1 = "graycode_" + var0 + str(y) + ".bmp"
        img = cv2.imread(var1)
        cv2.imshow("window",img)
        cv2.waitKey(1)
        rawCapture = PiRGBArray(camera)
        camera.hflip = True
        camera.vflip = True
        camera.capture(rawCapture, format="bgr")
        captures[x-1].append(rawCapture.array)


cv2.destroyWindow("window")       
cv2.destroyAllWindows()
cv2.waitKey(1000)
#write images to memory
i = 0
j = 0
for orientation in captures:
    for capture in orientation:
        name = "img_" + str(i) + "_" + str(j) + ".png"
        cv2.imwrite(name, capture)
        j += 1
    j = 0
    i += 1

    
#camera.start_preview()
#sleep(20)
#camera.stop_preview()
