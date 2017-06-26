#include <MQTT.h>

const int INPUT_PIN = D0;

String myName = "";

void callback(char* topic, byte* payload, unsigned int length) {}

MQTT client("SERVER", 1883, 120, callback);

// for QoS2 MQTTPUBREL message.
// this messageid maybe have store list or array structure.
uint16_t qos2messageid = 0;

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

void report(String status) {
    Particle.publish("/" + myName + "/motion", status, 60, PRIVATE);

    if (!client.isConnected())
    {
        Particle.publish("/" + myName + "/mqtt", "reconnecting to MQTT server", 60, PRIVATE);
        client.connect(System.deviceID());
    }
    
    if (client.isConnected()) {
        client.publish(myName + "/motion", status);
    }
}

void setup() {
    pinMode(INPUT_PIN, INPUT);
    
    RGB.control(true);
    RGB.color(255, 0, 0);
    RGB.brightness(0);

    Particle.subscribe("spark/device/name", nameHandler);
    Particle.publish("spark/device/name");
    
    // connect to the server
    client.connect(System.deviceID());

    // add qos callback. If don't add qoscallback, ACK message from MQTT server is ignored.
    client.addQosCallback(qoscallback);
}

void loop() {
    if (digitalRead(INPUT_PIN) == HIGH) {
        RGB.brightness(255);
        report("1");
        
        while (digitalRead(INPUT_PIN) == HIGH)
            delay(100); // wait until motion stops
        
        report("0");
        RGB.brightness(0);
    }
    
    if (client.isConnected())
        client.loop();
}

void nameHandler(const char *topic, const char *data) {
    myName = String(data);
    Particle.publish("my name is:", myName);
}
