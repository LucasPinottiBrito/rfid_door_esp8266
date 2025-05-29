#pragma once
#include <Servo.h>

class StepperController {
private:
    Servo servo;
    uint8_t pin;
public:
    StepperController(uint8_t servoPin) : pin(servoPin) {}
    void begin() {
        servo.attach(pin);
        lock();
    }
    void unlock() {
        servo.write(90);
    }
    void lock() {
        servo.write(0);
    }
};
