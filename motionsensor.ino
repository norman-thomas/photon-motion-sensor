const int INPUT_PIN = D0;

String myName = "/";

void setup() {
    pinMode(INPUT_PIN, INPUT);
    Particle.subscribe("spark/device/name", nameHandler);
    Particle.publish("spark/device/name");
}

void loop() {
    if (digitalRead(INPUT_PIN) == HIGH) {
        Particle.publish(myName + "motion", PRIVATE);
        while (digitalRead(INPUT_PIN) == HIGH)
            ; // wait until motion stops
    }
}

void nameHandler(const char *topic, const char *data) {
    myName = "/" + String(data) + "/";
    Particle.publish("my name is: " + String(topic), myName);
}

