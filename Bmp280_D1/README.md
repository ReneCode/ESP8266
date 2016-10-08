
# Wemos D1 mini + ESP8266 and BMP280

## ESP8266 Module
There is a ESP8266 Module called [Wemos D1 mini](https://www.wemos.cc) is based on the ESP-12 board.
It has much more I/O ports than the ESP-01 baord. And it can be connected
directly to USB. 

## Sensor BMP 280
This sensor is able to measure temperature and pressure very accuracy.
To work with that sensor download the library from [adafruit](https://github.com/adafruit/Adafruit_BMP280_Library)

## Pitfall-1
While using the library from adafruit you are not able to set the ports 
for SCL and SDA (using the I2C bus)

    void setup() {
        ...
        Wire.begin(12,14)  

is not possible, because there is a Wire.begin() call inside the library.
You can either modify the file "Adafruit_BMP280.cpp" or you use the 
default ports IC2:
* D1 = gpio05 = SCL
* D2 = gpio04 = SDA

## Pitfall-2

You have to set the address for the BMP280 sensor. That is done in the
bmp.begin(address) function. 

    Adafruit_BMP280 bmp; 
    ....
    bmp.begin(0x76)

