/*
 *  defs.h
 *      Here you must select following:
 *      1.- Which of two boards are you loading: 1 or 2
 *      2.- To which MQTT broker do you access: 
 *          * define CLOUD_MQTT if you are using it
 *          * else change the IP of local mosquitto boker
 */

/*
 *  Which board ?
 */

#if BOARD == 1
#define ME      "g1"
#define YOU     "g2"
#else
#define ME      "g2"
#define YOU     "g1"
#endif

/*
 *  Pin definitions
 *  See platformio.ini
 */

/*
 *  MQTT topic definitions
 */

const char* main_topic  = "espxx";
const char* group       = ME;
const char* other       = YOU;
const char* client_name = "mqtt_test";


