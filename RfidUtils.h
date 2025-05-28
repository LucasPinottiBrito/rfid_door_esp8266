#ifndef RFID_UTILS_H
#define RFID_UTILS_H

#include <MFRC522.h>

String getCardID(MFRC522& reader) {
  String uid = "";
  for (byte i = 0; i < reader.uid.size; i++) {
    uid += String(reader.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(reader.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

#endif
