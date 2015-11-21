#!/usr/bin/env python
import os
from mplayer import Player

player = Player()
pathToFile = os.path.join(os.path.dirname(__file__), 'Recording.m4a')
player.loadfile(pathToFile)

#os.system ('omxplayer -o local /home/pi/Documents/Rasp/Recording.m4a')




#os.system ('omxplayer -o local' + file)

