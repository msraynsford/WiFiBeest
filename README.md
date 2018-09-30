# WifiBeest
Source code and assembly documents for my ESP8266 controlled strandbeest

# Connecting to the strandbeest
When the micro controller device is connected to a power supply via a usb cable (the kit does not have to be assembled to connect to the chip),
The ESP8266 will start a new WiFi connection called "Beest XXXXXX". You should be able to find this network from your phone/laptop and connect to it using the password:'password'.

![alt text](https://github.com/msraynsford/WifiBeest/blob/master/WifiNetworks.png "Wifi List")

Once the connection has been established, your phone should automatically load a browser which points to the right URL for the device but if it does not you can open a browser window and point towards http://beest.local/

![alt text](https://github.com/msraynsford/WiFiBeest/blob/master/WiFiBeestBrowser.png "Browser Image")

If you are still not able to load the page by typing the URL into the browser, it is possible to reach the right page by typing in the IP address http://192.168.4.1/

# Changing the SSID and Password
It is recommended to change the password for the WiFi network as soon as possible, there is a separate admin page that allows you to do this. If you navigate the browser to "http://beest.local/admin" you will be presented with this page

![alt text](https://github.com/msraynsford/WiFiBeest/blob/master/WiFiBeestAdmin.png "Admin Image")

This page allows you to change the password without entering the previous password because you already have to be connected to the WiFi using the existing password. 

# Connecting to the device without a password
If you forget the password it is still possible to connect to the device and update the SSID and password. When you turn the device on, the led will illuminate after 2 seconds and remain on for 1 second. 

![alt text](https://github.com/msraynsford/WifiBeest/blob/master/Device.png "Device Image")

If you press the reset button during this window the device will start a new wifi network called "APConfig". This network has no password so anybody can connect to it, the next time the device is turned on it will return to normal, passworded, operation.

![alt text](https://github.com/msraynsford/WifiBeest/blob/master/APConfig.png "APConfig Image")


