import network
import time

# ==========================================
# AP (Access Point) Configuration
# ==========================================
SSID = "xxx"
PASSWORD = "xxx"
# ==========================================

# Initialize the Access Point (AP) interface
ap = network.WLAN(network.AP_IF)

# Activate the AP interface
ap.active(True)

# Configure the AP with the specified SSID, Password, and security mode
ap.config(
    essid=SSID,
    password=PASSWORD,   
    authmode=network.AUTH_WPA_WPA2_PSK
)

# Brief pause to ensure the AP is fully initialized
time.sleep(1)

# Print the AP details in a formatted way
print("-" * 30)
print("AP (Access Point) Started Successfully!")
print("SSID:", ap.config("essid"))
print("Password:", PASSWORD)
# ap.ifconfig() returns a tuple: (IP, Subnet Mask, Gateway, DNS)
# Index [0] is the IP address
print("IP Address:", ap.ifconfig()[0]) 
print("-" * 30)