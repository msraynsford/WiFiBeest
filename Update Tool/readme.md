- The contents of this folder can be used to program the device without installing the arduino environment and associated libraries.
- ESPTool uploads pre-compiled images of the software and file system to the ESP device.
- The batch file instructs ESPTool to upload two different images but it needs to be edited to point to the correct COM Port for the USB->Serial Device.

This article shows how to identify the COM Port on Windows, Mac or Linux
https://www.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html

Once you have identified the COM port that the device is connected to, you will need to modify "DownloadFile.bat", the number at the highlighted location should be replaced with your COM port number

![alt text](https://github.com/msraynsford/StrangerThings/blob/master/Upload%20Tool/EditBatFile.jpg "Edit Bat File")
