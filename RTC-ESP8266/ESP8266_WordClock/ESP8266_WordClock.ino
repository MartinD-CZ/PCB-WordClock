/* 
 *  ESP8266-based RTC
 *  part of the PCB WordClock project: http://embedblog.eu/?p=636
 *  martin@embedblog.eu
 *  this file connects to WiFi and periodically retrives current time from the Internet (NTP)
 */

#include <ESP8266WiFi.h>
#include <coredecls.h>
#include <time.h>
#include <sys/time.h>

#include <sntp.h>
#include <TZ.h>

#define MYTZ TZ_Europe_Prague

static time_t now;
struct tm * ptm;
uint8_t minute_previous, minute_now;
bool NTP_connected = false;

char* ssid = "your-ssid";
char* pass = "your-pass";

void setup() 
{
  Serial.begin(115200);
  Serial.println("\nDevice started\n");

  //init WordClock
  max7219_init();

  //set timeservice
  time_t rtc = 1586642400;    //test -> 10. april 2020
  timeval tv = {rtc, 0};
  settimeofday(&tv, nullptr);
  settimeofday_cb(time_is_set_scheduled);
  configTime(MYTZ, "pool.ntp.org");

  //connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.print("Connected, IP: "); Serial.println(WiFi.localIP());

  //wait for NTP
  Serial.print("Waiting for time from NTP");
  while (!NTP_connected)
   {
    delay(250);
    Serial.print(".");
  }

  //disable all LEDs ON
  max7219_writeAllRegisters(0xF, 0x00);
}

void loop() 
{
  delay(1000);
  now = time(nullptr);
  ptm = gmtime(&now);
  minute_now = ptm->tm_min;
  
  if (minute_now != minute_previous)
  {
    minute_previous = minute_now;
    Serial.print("MINUTE CHANGE: ");
    Serial.print(ctime(&now));

    wordclock_update(ptm->tm_hour, ptm->tm_min);
  }
}

void time_is_set_scheduled() 
{
  Serial.println("NTP time synced");
  NTP_connected = true;
}
