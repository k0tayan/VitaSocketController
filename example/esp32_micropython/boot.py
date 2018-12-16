import network

ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid="VitaTest")
