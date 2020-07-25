# CoronaAppDetector

Some code to show how many corona apps are in proximity.
It uses the TTGO esp32 module with integrated RGB TFT display. 

## Display Library Configuration

To config the [TFT eSPI library](https://github.com/Bodmer/TFT_eSPI), you have to edit `User_Setup_Select.h` in the library location.
Comment out `#include <User_Setup.h>` and uncomment `#include <User_Setups/Setup25_TTGO_T_Display.h>`.

![hardware picture](/hardware.jpg)
