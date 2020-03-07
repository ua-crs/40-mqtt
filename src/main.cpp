/*
 *  Project 40-mqtt - main.cpp
 */

#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>
#include <Servo.h>

#include "wifi_ruts.h"

#include "defs.h"


#if MQTT == 1

const char *mqttServer = "soldier.cloudmqtt.com";
const int mqttPort = 10696;
const char *mqttUser = "zklijuox";
const char *mqttPassword = "PoCm7wkAUI5r";

#else

const char *mqttServer = MOSQUITTO_IP;
const int mqttPort = 1883;
const char *mqttUser = "";
const char *mqttPassword = "";

#endif

/*
 *  Hardware independent definitions
 */


typedef struct
{
    const char *sub_topic;
    void (*action)( char *message );
} topic_t;

WiFiClient espClient;
PubSubClient client(espClient);

char client_id[100];

#include "hard.h"   //  Hardware dependent code

/*
 *  Hardware independent code
 */

void
callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = 0;
    Serial.printf( "For topic: \"%s\", message \"%s\"\n\r", topic, payload );
    for( int i = 0; topics[i].sub_topic != NULL; ++i )
        if( strstr( topic, topics[i].sub_topic ) != NULL )
        {
            (*topics[i].action)( (char *)payload );
            break;
        }
    Serial.println("-----------------------");
}

void
init_mqtt(void)
{
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    while (!client.connected())
    {
        Serial.println("Connecting to MQTT...");

        if (client.connect( client_id, mqttUser, mqttPassword))
            Serial.printf( "client %s connected\n\r", client_id );
        else
        {
            Serial.print("MQTT connection: failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    Serial.printf( "Connected to %s\n\r", mqttServer );
}

void
setup(void)
{
    get_client_id( client_id );
    Serial.begin(SERIAL_BAUD);

    connect_wifi(MY_SSID, MY_PASS);

    init_hardware();
    init_mqtt();
    init_suscriptions();
    led_setup_done();
}

void
loop()
{
    verify_changes();
    client.loop();
}
