import ubinascii
import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)

mac = wlan.config('mac')
chip_id = ubinascii.hexlify(mac).decode()

print("Chip Unique ID:", chip_id)