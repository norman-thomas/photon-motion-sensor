#include <MQTT.h>

const int INPUT_PIN = D0;

String myName = "";

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/
byte server[] = { 192, 168, 178, 40 };
MQTT client("pluto.fritz.box", 1883, callback);

// for QoS2 MQTTPUBREL message.
// this messageid maybe have store list or array structure.
uint16_t qos2messageid = 0;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "RED"))
        RGB.color(255, 0, 0);
    else if (!strcmp(p, "GREEN"))
        RGB.color(0, 255, 0);
    else if (!strcmp(p, "BLUE"))
        RGB.color(0, 0, 255);
    else
        RGB.color(255, 255, 255);
    delay(1000);
}

// QOS ack callback.
// if application use QOS1 or QOS2, MQTT server sendback ack message id.
void qoscallback(unsigned int messageid) {
    Serial.print("Ack Message Id:");
    Serial.println(messageid);

    if (messageid == qos2messageid) {
        Serial.println("Release QoS2 Message");
        client.publishRelease(qos2messageid);
    }
}

void setup() {
    pinMode(INPUT_PIN, INPUT);
    
    RGB.control(true);
    RGB.brightness(0);

    Particle.subscribe("spark/device/name", nameHandler);
    Particle.publish("spark/device/name");
    
    // connect to the server
    client.connect("sparkclient");

    // add qos callback. If don't add qoscallback, ACK message from MQTT server is ignored.
    client.addQosCallback(qoscallback);
}

void loop() {
    if (digitalRead(INPUT_PIN) == HIGH) {
        RGB.color(255, 0, 0);
        client.publish(myName + "/motion", "1");
        Particle.publish("/" + myName + "/motion", "1", 60, PRIVATE);
        while (digitalRead(INPUT_PIN) == HIGH)
            ; // wait until motion stops
        client.publish(myName + "/motion", "0");
        Particle.publish("/" + myName + "/motion", "0", 60, PRIVATE);
        RGB.color(0, 0, 0);
    }
}

void nameHandler(const char *topic, const char *data) {
    myName = String(data);
    Particle.publish("my name is:", myName);
}

