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
 

#define SLOTS 3
#define TICK_INTERVAL 60

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
int old = 0;
std::unordered_set<std::string>* seen[SLOTS];
void sTick()  // Wird jede Sekunde ausgefüert
{ 
  slot = ++slot%SLOTS; //increment in %10
  seen[slot]->clear();//clear
  int sum = 0;
  int fullSlots = 0;
  /*the problem i'm solving with that is that after 15 minutes a device changes its mac, so we can only track a device for a 15 minute period, but it we take the 
  average number of devices seen in the last 100 seconds and round down we should be fine. also i didn't want to implement some median function which would have been better*/ 
  for (int i = 0; i < SLOTS; i++)
  {
    std::unordered_set<std::string>::iterator it = seen[i]->begin();

    if (seen[i]->size() > 0)
        Serial.println("============");
    // Iterate till the end of set
    while (it != seen[i]->end())
    {
      Serial.println(it->c_str());
      it++;
    }
    sum = sum+ seen[i]->size();
    if (seen[i]->size() > 0)
      fullSlots++;
  }
  int near = 0;
  if (fullSlots > 0)
    near = sum / fullSlots;
  
  //tell serial the number of devices
  if (near != old){
    Serial.println(near);
    old = near;
  }
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
      else if (advertisedDevice.haveServiceUUID())
      {
        Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
        
      }
      else
      {
        Serial.println(advertisedDevice.toString().c_str());
      }
    }
};


 
void setup() 
{
  //we have 10 slots, each stores the macs seen in a second. so we record the seen macs from the last 10 secods
  for (int i = 0; i < SLOTS; i++)
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

  //every TICK_INTERVAL second we want to do some stuff
  Tic.attach( TICK_INTERVAL,sTick);
}
 
void loop()
{
  pBLEScan->start(scanTime);
}
