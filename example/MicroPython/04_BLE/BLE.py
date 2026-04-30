import bluetooth
import struct
import time

ble = bluetooth.BLE()
ble.active(True)

def adv_payload(name, tx_power=-4):
    name = name.encode()

    payload = bytearray()

    # Flags
    payload += struct.pack("BB", 2, 0x01) + b"\x06"

    # TX Power
    payload += struct.pack("BBb", 2, 0x0A, tx_power)

    # Name
    payload += struct.pack("BB", len(name) + 1, 0x09) + name

    return payload

ble.gap_advertise(200_000, adv_payload("ESP32-C5-Zero-BLE"))

while True:
    time.sleep(1)