#source ~/.profile
#workon cv3

from picamera.array import PiRGBArray
from picamera import PiCamera
from time import sleep
import numpy as np
import cv2
#import cv2.cv as cv

camera = PiCamera()
camera.resolution = (1280, 720)

captures = []

cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("window", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
img = cv2.imread("chessboard_1280_1024.png",0)
cv2.imshow("window",img)
cv2.waitKey(100)

var1 = "newchessboard.png"
img2 = cv2.imread(var1)

for x in range (1,21):
    cv2.imshow("window",img2)
    cv2.waitKey(0)
    rawCapture = PiRGBArray(camera)
    camera.hflip = True
    camera.vflip = True
    camera.capture(rawCapture, format="bgr")
    #name = "procam" + str(x) + ".jpg"
    captures.append(rawCapture.array)
    #cv2.imwrite(name, rawCapture.array)
    #captures.append(rawCapture.array)
    cv2.imshow("window",img)
    cv2.waitKey(100)

cv2.destroyWindow("window")       
cv2.destroyAllWindows()

y = 1

for num in captures:
    name = "procam" + str(y) + ".jpg"
    cv2.imwrite(name, num)
    y += 1
