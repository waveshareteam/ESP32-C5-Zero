import network
import time

# ==========================================
# Network Configuration
# ==========================================
# WiFi credentials
SSID = "xxx"
PASSWORD = "xxx"

# Static IP settings
STATIC_IP = "192.168.137.50"
SUBNET_MASK = "255.255.255.0"
GATEWAY = "192.168.137.1"
DNS = "192.168.137.1"  # Using gateway address for DNS is recommended
# ==========================================

sta = network.WLAN(network.STA_IF)
sta.active(True)

# Step 1: Do not set static IP first. Connect to WiFi and let DHCP assign an IP
if not sta.isconnected():
    print("Connecting to hotspot...")
    sta.connect(SSID, PASSWORD)
    
    # Wait for connection with a 15-second timeout
    timeout = 15
    while not sta.isconnected() and timeout > 0:
        print(".", end="") # Print dots to show progress
        time.sleep(1)
        timeout -= 1
    print() # Move to a new line after the loop

if sta.isconnected():
    # Step 2: After connection, overwrite configuration to lock static IP
    sta.ifconfig((STATIC_IP, SUBNET_MASK, GATEWAY, DNS))
    
    print("-" * 30)
    print("WiFi connected successfully!")
    print("Connected to SSID:", SSID)
    print("Static IP locked to:", sta.ifconfig()[0])
    print("Gateway:", sta.ifconfig()[2])
    print("-" * 30)
else:
    print("Connection failed. Please check hotspot status, credentials, or signal strength.")