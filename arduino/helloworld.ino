// Hello, World! - The Lamp Edition

#include <CurieBLE.h>

void setup() {
    Serial.begin(9600);
    BLE.begin();
    BLE.scan();

    delay(5000);
}

void loop() {
    // scan for devices
    BLEDevice p = BLE.available();
    if(p) {
        // identify lamps
        // Found device: [00:07:80:13:CD:45], Stringlight 000002, 0dcecf0c-ad06-11e3-81e4-425861b86ab6
        // Found device: [00:07:80:13:C6:51], Stringlight 000001, 0dcecf0c-ad06-11e3-81e4-425861b86ab6

        if(p.advertisedServiceUuid() == "0dcecf0c-ad06-11e3-81e4-425861b86ab6") {
            Serial.print("Found device: [");
            Serial.print(p.address());
            Serial.print("], ");
            Serial.print(p.localName());
            Serial.print(", ");
            Serial.println(p.advertisedServiceUuid());

            BLE.stopScan();

            connectToLamp(p);
        }
    }
}

// connect to lamp
void connectToLamp(BLEDevice l) {
    Serial.print("connecting to lamp... ");

    if(l.connect()){
        Serial.println("SUCCESS");
        discoverLampAttributes(l);
        l.disconnect();
    } else {
        Serial.println("FAILED");
    }
}

// discover attributes
void discoverLampAttributes(BLEDevice l){
    Serial.print("discovering attributes...");

    if(l.discoverAttributes()){
        Serial.println("SUCCESS");
        discoverLampServices(l);
    } else {
        Serial.println("FAILED");
    }
}

// discover services
void discoverLampServices(BLEDevice l){
    Serial.println("  discovering services...");

    for(int i=0; i<l.serviceCount(); i++){
        BLEService s = l.service(i);
        Serial.print("  service UUID: ");
        Serial.println(s.uuid());

        discoverServiceCharacteristics(s);
    }

    toggleLight(l);
}

// discover characteristics
void discoverServiceCharacteristics(BLEService s){
    Serial.println("    discovering characteristics...");

    for(int i=0; i<s.characteristicCount(); i++){
        BLECharacteristic c = s.characteristic(i);

        Serial.print("    characteristic UUID: ");
        Serial.println(c.uuid());

        readCharacteristicValue(c);
    }
}

// read characteristics
void readCharacteristicValue(BLECharacteristic c) {
    Serial.println("      reading characteristic...");

    c.read();

    if(c.valueLength() > 0) {
        printValue(c.value(), c.valueLength());
        Serial.println();
    }
}

void printValue(const unsigned char data[], int len) {
    for(int i=0; i<len; i++){
        unsigned char b = data[i];

        if(b<16){
            Serial.print("0");
        }

        Serial.print(b, HEX);
        Serial.print(" ");
    }
}

// write characteristic
// characteristic UUID: 335c1504-ad06-11e3-81e4-425861b86ab6
void toggleLight(BLEDevice l) {

    BLECharacteristic lc = l.characteristic("335c1504-ad06-11e3-81e4-425861b86ab6");

    if(!lc) {
        Serial.println("could not find lamp control characteristic");
        return;
    }
     if(!lc.canWrite()) {
         Serial.println("unable to write to lamp control characteristic");
         return;
     }

     while(l.connected()){
        Serial.println("turning light on");
        lc.writeByte(0x64);
        delay(2000);

        Serial.println("turning light off");
        lc.writeByte(0x00);
        delay(2000);
    }
}
