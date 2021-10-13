/*
 * Project Intrvene_MVP_2_Argon
 * Description: Testing ble vibrating disk for breathing exercises
 * Author: Jack Slattery
 * Date: 10/11/2021
 */

const pin_t motorPin = D3;

// Create Uuids for BLE Service and Characteristic
const char* genServiceUuid = "cd2239b0-a7b5-4695-8bf1-9809a03cb226";
const char* fourSevenEight = "cd2239b1-a7b5-4695-8bf1-9809a03cb226";
const char* boxBreathing = "cd2239b2-a7b5-4695-8bf1-9809a03cb226";
const char* fullPower = "cd2239b3-a7b5-4695-8bf1-9809a03cb226";
const char* lowPower = "cd2239b4-a7b5-4695-8bf1-9809a03cb226";
BleCharacteristic fourSevenEightChr("Four-Seven-Eight", BleCharacteristicProperty::WRITE_WO_RSP, fourSevenEight, genServiceUuid, bleCallback, (void *)fourSevenEight);
BleCharacteristic boxBreathingChr("Box Breathing", BleCharacteristicProperty::WRITE_WO_RSP, boxBreathing, genServiceUuid, bleCallback, (void *)boxBreathing);
BleCharacteristic fullPowerChr("Full Power", BleCharacteristicProperty::WRITE_WO_RSP, fullPower, genServiceUuid, bleCallback, (void *)fullPower);
BleCharacteristic lowPowerChr("Low Power", BleCharacteristicProperty::WRITE_WO_RSP, lowPower, genServiceUuid, bleCallback, (void *)lowPower);

// Function to increase vibration power
// (0% - 100%) over given duration (seconds)
static void increaseOverTime(int duration){
    int startTime = millis();
    int fullTime = duration*1000;
    int currentTime = millis();
    while((currentTime - startTime) < fullTime){
        float percentage = (currentTime-startTime)/fullTime;
        int powerLevel = (int)(255*percentage);
        analogWrite(motorPin, powerLevel, 200);
    }
    analogWrite(motorPin, 0);
}

// Function to decrease vibration power
// (100% - 0%) over given duration (seconds)
static void decreaseOverTime(int duration){
    int startTime = millis();
    int fullTime = duration*1000;
    int currentTime = millis();
    while((currentTime - startTime) < fullTime){
        float percentage = 1-((currentTime-startTime)/fullTime);
        int powerLevel = (int)(255*percentage);
        analogWrite(motorPin, powerLevel, 200);
    }
    analogWrite(motorPin, 0);
}

// Function to use vibrating disk
// at full power for given duration (255/255)
static void fullPowerDuration(int duration){
        analogWrite(motorPin, 255, 200);
        delay(duration*1000);
        analogWrite(motorPin, 0);
}

// Function to use vibrating disk
// at low power for given duration (50/255)
static void lowPowerDuration(int duration){
    analogWrite(motorPin, 50, 200);
    delay(duration*1000);
    analogWrite(motorPin, 0);
}

// Pulses vibrating disk for given
// duration, at the given power level,
// with the freq. of the given interval (ms)
static void pulseDuration(int duration, int percentPower, int msInterval){
    int startTime = millis();
    int fullTime = duration*1000;
    int currentTime = millis();
    while((currentTime-startTime) < fullTime){
        analogWrite(motorPin, (int)(percentPower/100)*255, 200);
        delay(msInterval);
        analogWrite(motorPin, 0);
        delay(msInterval);
    }
}

// Four-Seven-Eight Function
static void fourSevenEightExercise(){
    for(int i=0; i<4; i++){
        //Breathe in 4
        increaseOverTime(4);
        //Hold breath for 7
        pulseDuration(7, 75, 300);
        //Exhale and count to 8
        decreaseOverTime(8);
        // Repeat 4 times
    }
    analogWrite(motorPin, 0); // Turn off
}

// Box Breathing Function
static void boxBreathingExercise() {
    for(int i=0; i<5; i++){
        // Inhale 4 sec
        increaseOverTime(4);
        // Hold 4 sec
        fullPowerDuration(4);
        // Exhale 4 sec
        decreaseOverTime(4);
        // Hold 4 sec
        lowPowerDuration(4);
        // Repeat 5 times
    }
    analogWrite(motorPin, 0); // Turn off
}

// Full Power for 10 seconds
static void fullVibrationPower() {
    analogWrite(motorPin, 255, 200); // Full Power (255/255)
    delay(10000); // Wait 10 sec
    analogWrite(motorPin, 0); // Turn off
}

// Low Power for 10 seconds
static void lowVibrationPower() {
    analogWrite(motorPin, 50); // Low Power (50/255)
    delay(10000); // Wait 10 sec
    analogWrite(motorPin, 0); // Turn off
}


static void bleCallback(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    
    // Only looking for one byte
    if(len != 1){
        return;
    }

    // Handle exercise choice from BLE input
    if(context == fourSevenEight){
        fourSevenEightExercise();
    } else if (context == boxBreathing){
        boxBreathingExercise();
    } else if (context == fullPower){
        fullVibrationPower();
    } else if (context == lowPower){
        lowVibrationPower();
    }
}

void setup() {
    Serial.begin(9600);

    // Set Motor Pin as output pin
    pinMode(motorPin, OUTPUT);

    // Set up Bluetooth and begin advertising
    BleUuid genService(genServiceUuid);
    BLE.addCharacteristic(fourSevenEightChr);
    BLE.addCharacteristic(boxBreathingChr);
    BLE.addCharacteristic(fullPowerChr);
    BLE.addCharacteristic(lowPowerChr);
    BleAdvertisingData advData;
    advData.appendLocalName("Intrvene");
    advData.appendServiceUUID(genService);
    BLE.advertise(&advData);
}

void loop() {}