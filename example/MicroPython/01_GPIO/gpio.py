from machine import Pin
import time

pins = []

for i in range(13):
    pins.append(Pin(i, Pin.OUT))

while True:
    for p in pins:
        p.value(1)
    time.sleep(0.5)

    for p in pins:
        p.value(0)
    time.sleep(0.5)