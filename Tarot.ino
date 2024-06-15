
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

int button = 3;
int random_numer(int);
void card_display(int);

// Comment out the next line to load from SPI/QSPI flash instead of SD card:
#define USE_SD_CARD

// TFT display and SD card share the hardware SPI interface, using
// 'select' pins for each to identify the active device on the bus.

#define SD_CS    A4 // SD card select pin
#define TFT_CS  12//TFT select pin
#define TFT_DC   13// TFT display/command pin
#define TFT_RST  7// Or set to -1 and connect to Arduino RESET pin

#if defined(USE_SD_CARD)
  SdFat                SD;         // SD card filesystem
  Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
#else
  // SPI or QSPI flash filesystem (i.e. CIRCUITPY drive)
  #if defined(__SAMD51__) || defined(NRF52840_XXAA)
    Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
      PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
  #else
    #if (SPI_INTERFACES_COUNT == 1)
      Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
    #else
      Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
    #endif
  #endif
  Adafruit_SPIFlash    flash(&flashTransport);
  FatFileSystem        filesys;
  Adafruit_ImageReader reader(filesys); // Image-reader, pass in flash filesys
#endif

Adafruit_ST7789      tft    = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;

void setup(void) {

  ImageReturnCode stat; // Status from image-reading functions

  Serial.begin(9600);
  while(!Serial);       // Wait for Serial Monitor before continuing

  tft.init(172, 320);
  pinMode(button, INPUT);
  digitalWrite(button,HIGH);           

  // The Adafruit_ImageReader constructor call (above, before setup())
  // accepts an uninitialized SdFat or FatFileSystem object. This MUST
  // BE INITIALIZED before using any of the image reader functions!
  Serial.print(F("Initializing filesystem..."));
#if defined(USE_SD_CARD)
  // SD card is pretty straightforward, a single call...
  if(!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
    Serial.println(F("SD begin() failed"));
    for(;;); // Fatal error, do not continue
  }
#else
  // SPI or QSPI flash requires two steps, one to access the bare flash
  // memory itself, then the second to access the filesystem within...
  if(!flash.begin()) {
    Serial.println(F("flash begin() failed"));
    for(;;);
  }
  if(!filesys.begin(&flash)) {
    Serial.println(F("filesys begin() failed"));
    for(;;);
  }
#endif
  Serial.println(F("OK!"));

  // Fill screen blue. Not a required step, this just shows that we're
  // successfully communicating with the screen.
  tft.fillScreen(ST77XX_BLUE);
  tft.setRotation(2);

  splash();
 
}

int random_number(int limit)
{

  int rando;
  rando = 0;
  rando = random(1,limit);

  return rando;
}

void card_display(int r)
{
  int card = 0;
  card = r;
  String num = String(r);
  String file = "/"+num +".bmp";

  Serial.print(file);
  Serial.print('\n');
  tft.setRotation(2);
  Adafruit_Image img;
  tft.fillScreen(0);
  delay(100);
  reader.drawBMP(file.c_str(), tft, 0, 0);
  delay(100);
  


}

void splash() {
  tft.setRotation(3);
  tft.setTextWrap(false);
  delay(500);
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(75, 50);
  tft.setTextColor(ST77XX_CASET);
  tft.setTextSize(4);
  tft.println("Tiara's");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(75,90);
  tft.setTextSize(5);
  tft.println("Tarots");
  
}

void loop() {


  int r;
  int ran;
  r = 78;
  ran = random_number(r);
  
  if(digitalRead(button) == 0)
  {
    card_display(ran);
  }
  delay(250);
}
