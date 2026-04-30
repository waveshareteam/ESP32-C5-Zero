from machine import UART, Pin
import time

# UART1
uart = UART(
    1,
    baudrate=115200,
    tx=Pin(11),
    rx=Pin(12)
)

print("UART-Init")

while True:
    uart.write("Waveshare\r\n")
    time.sleep(1)