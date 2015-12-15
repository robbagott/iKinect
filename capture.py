#!~/.virtualenvs/cv3/bin/python
from picamera.array import PiRGBArray
from picamera import PiCamera
from time import sleep
import numpy as np
import cv2
import os
from time import sleep
import RPi.GPIO as GPIO

import subprocess

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(04, GPIO.IN)
GPIO.setup(14, GPIO.OUT)
GPIO.setup(15, GPIO.OUT)

camera = PiCamera()
camera.resolution = (1280, 720)
    
cv2.namedWindow("window", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("window", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
img = cv2.imread("/home/pi/Documents/Rasp/solid_black.png",0)
cv2.imshow("window",img)
cv2.waitKey(200)
cv2.imshow("window",img)
cv2.waitKey(600)

#Ensure busy LED is off and ensure ready LED is on
GPIO.output(14, 0);
GPIO.output(15, 1);

captures = [[], []]

while(True):
    if GPIO.input(04) == False:
        break

#turn on busy LED on and turn off ready LED
GPIO.output(14, 1);
GPIO.output(15, 0);

hi = 0
for x in range (1,3):
    if x == 1:
        var0 = "h"
        hi = 8
    else:
        var0 = "v"
        hi = 9

    for y in range (1,hi):
        var1 = "/home/pi/Documents/Rasp/graycode_" + var0 + str(y) + ".bmp"
        img = cv2.imread(var1)
        cv2.imshow("window",img)
        cv2.waitKey(1)
        rawCapture = PiRGBArray(camera)
        camera.hflip = True
        camera.vflip = True
        camera.capture(rawCapture, format="bgr")
        captures[x-1].append(rawCapture.array)

img = cv2.imread("/home/pi/Documents/Rasp/solid_black.png",0)
cv2.imshow("window",img)
cv2.waitKey(100)
cv2.destroyWindow("window")       
cv2.destroyAllWindows()
cv2.waitKey(1000)

#write images to memory
i = 0
j = 0
for orientation in captures:
    for capture in orientation:
        name = "/home/pi/Documents/Rasp/img_" + str(i) + "_" + str(j) + ".png"
        cv2.imwrite(name, capture)
        j += 1
    j = 0
    i += 1

subprocess.call("./analyze.out 7 8 1280 720", shell = True)
