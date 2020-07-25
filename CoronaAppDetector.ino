#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <Ticker.h>
#include <unordered_set>
//Libraries for OLED Display
 
#include <TFT_eSPI.h>
#include <SPI.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL              4   // Display backlight control pin
#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_1            35
#define BUTTON_2            0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library


// Module connection pins (Digital Pins)
#define CLK 27
#define DIO 26

 

#define SLOTS 3
#define TICK_INTERVAL 60

String Address = "00:AA:FF:13:37:42";
 
Ticker Tic;
static BLEAddress *pServerAddress;
BLEScan* pBLEScan ;
int scanTime = 10; 

unsigned char num = 0;


 void printNumber(int number)
 {
  String out = String(number);
	tft.fillScreen(TFT_BLACK);
	tft.drawString("Corona-App", 5, 5);
	tft.drawString("count: "+out, 5, 16*3*2);
	//tft.drawString(out, 150, 16*3*2);
	Serial.println("PRINT NUMBER");
 }


int slot = 0;
int old = 0;
std::unordered_set<std::string>* seen[SLOTS];
void sTick()  // gets calles every second
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
  
  // tell serial the number of devices
  if (near != old){
    Serial.println(near);
    old = near;
  }
  printNumber(near);
}
 
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) // is called when a ble beacon is found
    {
      BLEUUID coronaUUID("FD6F"); // from https://covid19-static.cdn-apple.com/applications/covid19/current/static/contact-tracing/pdf/ExposureNotification-BluetoothSpecificationv1.2.pdf
      if (advertisedDevice.isAdvertisingService(coronaUUID)) // corona beacon found?
      {
        seen[slot]->insert(advertisedDevice.getAddress().toString()); // save sender mac to the set
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
  Serial.println("Start BLE Scanner");
  pServerAddress = new BLEAddress(Address.c_str());
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());


  // every TICK_INTERVAL second we want to do some stuff
  Tic.attach( TICK_INTERVAL,sTick);
 
	tft.init();

	tft.setTextSize(3);

	if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
		pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
		digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
	}


    tft.setRotation(1);
    tft.fillScreen(TFT_RED);
    delay(20);
    tft.fillScreen(TFT_BLUE);
    delay(20);
    tft.fillScreen(TFT_GREEN);
    delay(20);

    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
   
    tft.drawString("init...", tft.width() / 2, tft.height() / 2);
   
}
 
void loop()
{
  pBLEScan->start(scanTime);
}
