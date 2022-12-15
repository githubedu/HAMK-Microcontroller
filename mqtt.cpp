#include "mbed.h"
#include "ESP8266Interface.h"

// Library to use https://github.com/ARMmbed/mbed-mqtt
#include <MQTTClientMbedOs.h>

extern char publishMQTT[256]; //shared memory with the string to be published

void mqtt(void){
    char buffer[128];

    //Networking and MQTT 
    ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);
    //Store device IP
    SocketAddress deviceIP;
    //Store broker IP
    SocketAddress MQTTBroker;
    TCPSocket socket;
    MQTTClient client(&socket);
    
    printf("\nConnecting wifi..\n");

    int ret = esp.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);

    if(ret != 0)
    {
        printf("\nConnection error\n");
    }
    else
    {
        printf("\nConnection success\n");
    }
        
    esp.get_ip_address(&deviceIP);
    printf("IP via DHCP: %s\n", deviceIP.get_ip_address());
    
    esp.gethostbyname(MBED_CONF_APP_MQTT_BROKER_HOSTNAME, &MQTTBroker, NSAPI_IPv4, "esp");

    MQTTBroker.set_port(MBED_CONF_APP_MQTT_BROKER_PORT);

    printf("MQTT broker %s:%c\n", MQTTBroker.get_ip_address(),MQTTBroker.get_port());

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.MQTTVersion = 3;
    
    data.clientID.cstring = MBED_CONF_APP_MQTT_CLIENT_ID;
   // data.username.cstring = MBED_CONF_APP_MQTT_AUTH_METHOD;
   // data.password.cstring = MBED_CONF_APP_MQTT_AUTH_TOKEN;
    data.keepAliveInterval = 33;

    sprintf(buffer, "Hello from Mbed OS %d.%d", MBED_MAJOR_VERSION, MBED_MINOR_VERSION);
    MQTT::Message msg;
    msg.qos = MQTT::QOS0;
    msg.retained = false;
    msg.dup = false;
    msg.payload = (void*)buffer;
    msg.payloadlen = strlen(buffer);
        
    ThisThread::sleep_for(5s);

    // Connecting mqtt broker
    printf("Connecting %s ...\n", MBED_CONF_APP_MQTT_BROKER_HOSTNAME);
    socket.open(&esp);
    socket.connect(MQTTBroker);
    client.connect(data);
    
    //Publish                    
    printf("Publishing with payload length %d\n", strlen(buffer));
    client.publish(MBED_CONF_APP_MQTT_TOPIC, msg);
    sprintf(publishMQTT, "empty");
    client.disconnect();
    socket.close();

    while(1) {   
        if(strcmp(publishMQTT, "empty") == 0){
            ThisThread::sleep_for(33ms);  // Publishing every 30 second
            continue;
        }
        sprintf(buffer, "{\"tracking\":{\"ip\":\"%s\",\"reader\":%s}}", deviceIP.get_ip_address(), publishMQTT);
        msg.payload = (void*)buffer;
        msg.payloadlen = strlen(buffer);
        //Publish                    
//        if (!client.isConnected()){ 
            socket.open(&esp);
            socket.connect(MQTTBroker);
            client.connect(data);
  //          printf("re-connection");
  //      }
        
        printf("Publishing with payload length %d\n", strlen(buffer));
        client.publish("tracking/process/machine/json", msg);
        
        sprintf(publishMQTT, "empty");
        client.disconnect();
        socket.close();
    }
    printf("Disconnecting from MQTT broker");
    client.disconnect();
    ThisThread::sleep_for(2s);
    socket.close();
    printf("Entering deepsleep (press RESET button to resume)\n"); 
    ThisThread::sleep_for(300s);
}