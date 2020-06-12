/*
   ibeacon oder Beliebiger BLE Gerät Präsenz Überwachung mit Relais Anwesenheitserkennung 
   
   Michael Dworkin http://esp32-server.de/
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <Ticker.h>
#include <unordered_set>

#define SEGMENT_A 25
#define SEGMENT_B 26
#define SEGMENT_C 32
#define SEGMENT_D 33
#define SEGMENT_E 12
#define SEGMENT_F 13
#define SEGMENT_G 27
 
 
String Address = "00:AA:FF:13:37:42";
 
Ticker Tic;
static BLEAddress *pServerAddress;
BLEScan* pBLEScan ;
int scanTime = 30; 




 void printNumber(int number)
 {
  /*pls dont juge me*/   
  if(number == 0){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 1);
    digitalWrite(SEGMENT_F, 1);
    digitalWrite(SEGMENT_G, 0);
  }
    if(number == 1){
    digitalWrite(SEGMENT_A, 0);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 0);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 0);
    digitalWrite(SEGMENT_G, 0);
  }
    if(number == 2){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 0);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 1);
    digitalWrite(SEGMENT_F, 0);
    digitalWrite(SEGMENT_G, 1);
  }
    if(number == 3){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 0);
    digitalWrite(SEGMENT_G, 1);
  }
    if(number == 4){
    digitalWrite(SEGMENT_A, 0);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 0);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 1);
    digitalWrite(SEGMENT_G, 1);
  }
    if(number == 5){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 0);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 1);
    digitalWrite(SEGMENT_G, 1);
  }
    if(number == 6){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 0);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 1);
    digitalWrite(SEGMENT_F, 1);
    digitalWrite(SEGMENT_G, 1);
  }
    if(number == 7){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 0);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 0);
    digitalWrite(SEGMENT_G, 0);
  }
    if(number == 8){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 1);
    digitalWrite(SEGMENT_F, 1);
    digitalWrite(SEGMENT_G, 1);
  }   
  if(number == 9){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 1);
    digitalWrite(SEGMENT_C, 1);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 1);
    digitalWrite(SEGMENT_G, 1);
  }
    if(number > 9){
    digitalWrite(SEGMENT_A, 1);
    digitalWrite(SEGMENT_B, 0);
    digitalWrite(SEGMENT_C, 0);
    digitalWrite(SEGMENT_D, 1);
    digitalWrite(SEGMENT_E, 0);
    digitalWrite(SEGMENT_F, 0);
    digitalWrite(SEGMENT_G, 1);
  }
  
  
 }


int slot = 0;
std::unordered_set<std::string>* seen[10];
void sTick()  // Wird jede Sekunde ausgefüert
{ 
  Serial.println("Tick");
  slot = ++slot%10; //increment in %10
  seen[slot]->clear();//clear
  int sum = 0;
  /*the problem i'm solving with that is that after 15 minutes a device changes its mac, so we can only track a device for a 15 minute period, but it we take the 
  average number of devices seen in the last 100 seconds and round down we should be fine. also i didn't want to implement some median function which would have been better*/ 
  for (int i = 0; i < 10; i++)
  {
    sum = sum+ seen[i]->size();
  }
  int near = sum / 10;
  
  //tell serial the number of devices
  Serial.println(near);
  //print to 7segment display. 
  printNumber(near);
}
 
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) //is called when a ble beacon is found
    {
      BLEUUID coronaUUID("FD6F"); //from https://covid19-static.cdn-apple.com/applications/covid19/current/static/contact-tracing/pdf/ExposureNotification-BluetoothSpecificationv1.2.pdf
      if (advertisedDevice.isAdvertisingService(coronaUUID)) //if this is a corona beacon
      {
        seen[slot]->insert(advertisedDevice.getAddress().toString()); //save sender mac to the set
      }
    }
};


 
void setup() 
{
  //we have 10 slots, each stores the macs seen in a second. so we record the seen macs from the last 10 secods
  for (int i = 0; i < 10; i++)
  {
    seen[i] = new std::unordered_set<std::string>();
  }
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starte BLE Scanner");
  pServerAddress = new BLEAddress(Address.c_str());
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pinMode(SEGMENT_A, OUTPUT);
  pinMode(SEGMENT_B, OUTPUT);
  pinMode(SEGMENT_C, OUTPUT);
  pinMode(SEGMENT_D, OUTPUT);
  pinMode(SEGMENT_E, OUTPUT);
  pinMode(SEGMENT_F, OUTPUT);
  pinMode(SEGMENT_G, OUTPUT);

  //every 10 second we want to do some stuff
  Tic.attach( 10,sTick);
}
 
void loop()
{
  pBLEScan->start(scanTime);
}
