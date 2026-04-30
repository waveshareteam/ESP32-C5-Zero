import network
import time
import urequests  # Built-in library in MicroPython for making HTTP requests
import gc         # Garbage collection to avoid memory leaks

# WiFi credentials
SSID = "xxx"
PASSWORD = "xxx"

# API URL (In Python, we can write the complete URL directly, no need to manually construct HTTP headers)
API_URL = "https://api.seniverse.com/v3/weather/now.json?key=SbslwZ6X47ih3u-bX&location=beijing&language=zh-Hans&unit=c"

def connect_wifi():
    sta = network.WLAN(network.STA_IF)
    sta.active(True)
    
    if not sta.isconnected():
        print("Connecting to WiFi...")
        sta.connect(SSID, PASSWORD)
        
        while not sta.isconnected():
            time.sleep(0.5)
            print(".", end="")
            
    print("\n")
    print("=====================================")
    print("WiFi connection successful!")
    print("Connected to SSID:", SSID)
    print("With password:", PASSWORD)
    print("Assigned IP address:", sta.ifconfig()[0])
    print("=====================================\n")
    return sta

# 1. Initialize and connect to WiFi (Equivalent to Arduino's setup)
sta = connect_wifi()

# 2. Main loop (Equivalent to Arduino's loop)
while True:
    # Check WiFi status, reconnect if disconnected
    if not sta.isconnected():
        print("WiFi disconnected, reconnecting...")
        sta = connect_wifi()
        continue

    print("Connecting to API server: api.seniverse.com")
    print("Sending request to weather API...")

    try:
        # Send HTTPS GET request (urequests automatically handles SSL and port 443)
        response = urequests.get(API_URL)
        
        # Print the response data
        print("\n--- API Response Start ---")
        print(response.text)  # Directly get the returned text data (JSON format)
        print("--- API Response End ---\n")
        
        # [VERY IMPORTANT] In MicroPython, you must close the response after each request, 
        # otherwise it will cause a MemoryError after a few iterations.
        response.close()
        gc.collect() 

    except Exception as e:
        # Equivalent to Arduino's timeout or connection failure handling
        print("Connection to server failed or timeout!")
        print("Error details:", e)

    # Wait for 5 minutes
    print("Waiting 5 minutes for next update...")
    print("=====================================\n")
    time.sleep(300)  # 300 seconds = 5 minutes