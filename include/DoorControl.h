#ifndef DOOR_CONTROL_H
#define DOOR_CONTROL_H

#include <Servo.h>

void lockDoor(Servo& servo) {
  servo.write(0);
  Serial.println("🔒 Door locked.");
}

void unlockDoor(Servo& servo) {
  servo.write(90);
  Serial.println("🔓 Door unlocked.");
}

#endif
