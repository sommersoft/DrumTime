#define IO_USERNAME    "....."
#define IO_KEY         "....."

#define WIFI_SSID      "....."
#define WIFI_PASS      "....."

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);//, WIFI_STA);

// Setup AIO Group
AdafruitIO_Group *DrumTime = io.group("drumtime");

//Set queryFeed() vars
const char* api_host = "io.adafruit.com";
const char* feed_url = "/api/v2/...../feeds/";
const size_t jsonBufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10) + 220;
char feedBuffer[15]; //= {'\0'};

//Set Transaction vars
long balance_start = 0;
char* freetime_start = "OFF";
long balance_new = 0;
long deposits_new = 0;
long deposits_mid = 0;
long deposits_final = 0;
long expend_new = 0;
long expend_mid = 0;
long expend_final = 0;

//Set LIS3DH vars
Adafruit_LIS3DH lis_exp = Adafruit_LIS3DH(); // LIS3DH for drumset
Adafruit_LIS3DH lis_dep = Adafruit_LIS3DH(); // LIS3DH for band equipment
#define LIS_EXP_THRESH 20  //used to set the click threshold (sensitivity)
#define LIS_DEP_THRESH 10  //used to set the click threshold (sensitivity)

//Set timing vars
unsigned long telemMillis;
unsigned long pubMillis;
unsigned long preMillis;
unsigned long minuteMillis;
unsigned long currMillis;
unsigned long blinkMillis;
unsigned long activityMillis;
unsigned long iorunMillis;
bool blinkOn = false;

// Set NeoPixel vars
int NUMPIXELS = 7;
int free_pix = 1;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, 15, NEO_RGBW + NEO_KHZ800);

// Set Telemetry Status vars
char* TELEM_STATUS;
#define TELEM_NORM_IDLE     "System Normal (Idle)"
#define TELEM_NORM_ACTIVE   "System Normal (Active)"
#define TELEM_NORM_FREETIME "System Normal (Freetime)"
#define TELEM_ERROR_LIS     "System Error (LIS3DH Non-Responsive)"
#define TELEM_ERROR_UNK     "Systems Error (Unknown)"
bool TELEM_SEND_ERROR;
