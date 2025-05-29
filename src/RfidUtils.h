#pragma once
#include <MFRC522.h>

class RfidUtils {
public:
    static String getCardNumber(MFRC522& reader) {
        String UID = "";
        for (byte i = 0; i < reader.uid.size; i++) {
            UID += String(reader.uid.uidByte[i] < 0x10 ? "0" : "");
            UID += String(reader.uid.uidByte[i], HEX);
        }
        UID.toUpperCase();
        return UID;
    }
};
