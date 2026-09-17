BLE Wireless Keyboard for the M8 iOS App

When I saw that Dirtywave had released an M8 app on iOS one my first thoughts was to make an M8 like keyboard for it. I have an M8 headless that is almost identical to the real M8 and I wanted to leverage the muscle memory I had developed for that.

The prototype worked very well so I have designed a PCB and enclosure which more or less duplicates the M8 V1 design - Kailh switches, appxo 96mm x 84mm x18mm, white silkscreen. It has a small RGB led for indicating battery status and pads for a switch. I couldn't find a suitable PCB mounted R/A slide switch so I went with PCB pads so you can put in whatever you have. There are pads for a lipo charger module as well.

 ![Alt text](Images/M8_V1_V2.jpg "M8 Version 2 and 1")
 ![Alt text](Images/PCBtop.jpg "M8KB PCB front side")
 ![Alt text](Images/PCBbot.jpg "M8KB PCB back side") 
 
**CAUTION** the LiPo charger modules sold on Aliexpress that I designed it for are set up for 1A charge rate which is too high for most small LiPo's. To be safe you will want to change the current setting resistor to something like 1/3 of the battery capacity. Or just use 3 AAA batteries which is much safer.

Sept 15/2026 - initial version of the code

Sept 17/2026 - added hardware design files for PCB **NOT TESTED YET**