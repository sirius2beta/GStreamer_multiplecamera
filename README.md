# GStreamer_multiplecamera
Dependencies and usage
To run the script, you need the following packages. Here is the installation command for Debian based systems:

```
sudo apt install wiringpi libopencv-dev
```
Enable camera interface, and I2C interface
```
sudo raspi-config
```
Under the Interfaces tab, click on "Enabled" for the camera, then hit OK. Same as I2C

Intall the PyQt5, cv2
```
sudo apt-get install python-pyqt5 python-opencv
```
Download code liberary
```
git clone https://github.com/ArduCAM/RaspberryPi.git
```
To the directory
```
cd Multi_Camera_Adapter/Multi_Adapter_Board_4Channel/Multi_Camera_Adapter_V2.2_python
```
Run the below command to initialize the camera every time after reboot
```
sudo chmod +x init_camera.sh
```
```
sudo ./init_camera.sh
```
