from machine import Pin
import neopixel
import time

np = neopixel.NeoPixel(Pin(27), 1)

def wheel(pos):
    if pos < 85:
        return (pos * 3, 255 - pos * 3, 0)
    elif pos < 170:
        pos -= 85
        return (255 - pos * 3, 0, pos * 3)
    else:
        pos -= 170
        return (0, pos * 3, 255 - pos * 3)

while True:
    for i in range(256):
        np[0] = wheel(i)
        np.write()
        time.sleep(0.02)