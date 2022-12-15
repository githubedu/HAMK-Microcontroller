/*
* Copyright (c) 2006-2020 Arm Limited and affiliates.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************
* Connecting MbedOS microcontroller to IBM Cloud Watson IoT
*
* You need to have created a device with the organization and device credentials
* in the IBM Cloud Watson. The credentials will be typed on mbed_app.json .
* Don't publish your code with the credentials included !!!
*
* Hardware
* ST NUCLEO L432KC, OLIMEX MOD-WiFi-ESP8266, Potentiometer
* Connect:
* MOD-FiWi Rx Tx pins defined in the mbed_app.json. The Pot 3v3, A3, GND. 
*
*   CHANGE VALUES ACCORDING TO YOUR IBM CLOUD IOT PLATFORM CONFIGURATION
*   CHANGE VALUES mqtt-broker-hostname, mqtt-broker-port, mqtt-client-id, 
*   mqtt-auth-method, mqtt-auth-token AND mqtt-topic ON mbed_app.json
*
* Timo Karppinen 16.12.2020        Apache-2.0
******************************************************************************/

#include "mbed.h"
#include "reader.h"
#include "mqtt.h"
 
// Specify different pins to printing on UART other than the console UART.
#define TARGET_TX_PIN                                                     PA_2
#define TARGET_RX_PIN                                                     PA_15 //PA_3 on image

// Create a BufferedSerial for printing message
static BufferedSerial serial_port(TARGET_TX_PIN, TARGET_RX_PIN, 9600);

FileHandle *mbed::mbed_override_console(int fd)
{
    return &serial_port;
} 

DigitalOut red(D6);
Thread thread1;     //for the blinking led
Thread thread2;     //for the RF reader
Thread thread3;     //for networking and MQtt

char publishMQTT[256]; //shared memory with the string to be published

//blinking led
void breath_thread() {
    while (true) {
        red = !red;
        ThisThread::sleep_for(1s);  
    }
}

int main()
{
    thread1.start(breath_thread);

    thread3.start(mqtt);

    thread2.start(reader);

    while(true){}
}