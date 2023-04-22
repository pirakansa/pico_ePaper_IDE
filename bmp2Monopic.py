#!/usr/bin/env python3

import sys
import math
import cv2

## https://www.peko-step.com/tool/grayscale.html


COLOR_WHITE = 0x00
COLOR_BLOCK = 0xFF

img_data = []

img = cv2.imread(sys.argv[1], cv2.IMREAD_GRAYSCALE)

h = img.shape[0]
w = img.shape[1]
wsize = (w / 8) if w % 8 == 0 else (math.floor(w / 8) + 1)

pixel = 0x00


for row in img:
    index = -1
    for col in row:
        index = index + 1
        if index % 8 == 0:
            pixel = 0x00
        else:
            pixel = pixel<<1

        if col == COLOR_BLOCK:
            pixel = pixel | 0x01
        else:
            pixel = pixel | 0x00

        if index % 8 == 7:
            img_data.append(pixel)

    if (index % 8 == 7) or (index == -1):
        pass
    else:
        index = (index % 8) + 1
        for n in (1 , 2 , 3 , 4 , 5 , 6 , 7):
            if n <= index:
                pixel = pixel<<1
                pixel = pixel | 0x00
        img_data.append(pixel)


img_data_l = [hex(l) for l in img_data]
print(img_data_l)

