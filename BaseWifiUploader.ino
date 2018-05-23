/*
Параметры:

SonOff
	Памть 1M, DOUT
	GPIO
	0 Main button
	1 TX
	3 RX
	12 Relay
	13 LED (inversed logic)
	14 Available in header

ESP12E
	Плата: Generic ESP8266 Module
	Flash Mode: QIO
	Flash Size: 4M (1M SPIFF)
	Reset Method: ck
	Crystall Frequency: 26 MHz
	Flash Frequency: 80 MHz
	Cpu Frequency: 80 MHz

*/

#define LED_PIN			13
#define LED_ON			0
#define LED_BLINK_TIME	1000 // 1 sec

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EspFileServer.h>
#include <StreamString.h>
#include <Ticker.h>


extern "C" {
#include "user_interface.h"
}

ESP8266WebServer server(80);
EspFileServer fileServer( &server );
Ticker led;

void flash_info()
{
	StreamString web_log;
	uint32_t realSize = ESP.getFlashChipRealSize();
	uint32_t ideSize = ESP.getFlashChipSize();
	FlashMode_t ideMode = ESP.getFlashChipMode();

	web_log.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
	web_log.printf("Flash real size: %u\n\n", realSize);
	web_log.printf("Flash ide  size: %u\n", ideSize);
	web_log.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
	web_log.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
	if( ideSize != realSize ) web_log.println("Flash Chip configuration wrong!\n");
	else web_log.println("Flash Chip configuration ok.\n");

	web_log.print(F("system_get_sdk_version(): "));
	web_log.println(system_get_sdk_version());

	web_log.print(F("system_get_boot_version(): "));
	web_log.println(system_get_boot_version());

	web_log.print(F("system_get_userbin_addr(): 0x"));
	web_log.println(system_get_userbin_addr(), HEX);

	web_log.print(F("system_get_boot_mode(): "));
	web_log.println(system_get_boot_mode() == 0 ? F("SYS_BOOT_ENHANCE_MODE") : F("SYS_BOOT_NORMAL_MODE"));

	server.send( 200, F("text/plain; charset=utf-8"), web_log );
}

void InitWiFi()
{
	const char * WiFi_Name = "Oleg_Home";
	const char * WiFi_Pass = "lbvf1234";

	IPAddress staticIP(192,168,1,254);
	IPAddress gateway(192,168,1,1);
	IPAddress subnet(255,255,255,0);

	WiFi.config(staticIP, gateway, subnet);
	WiFi.begin(WiFi_Name, WiFi_Pass);

	while(WiFi.waitForConnectResult() != WL_CONNECTED) WiFi.begin(WiFi_Name, WiFi_Pass);
}

void blinker()
{
	digitalWrite( LED_PIN, !digitalRead( LED_PIN ) );
}

void setup(void)
{
	pinMode( LED_PIN, OUTPUT );
	digitalWrite( LED_PIN, LED_ON );
	led.attach_ms( LED_BLINK_TIME, blinker );
	InitWiFi();
	server.on( "/info", HTTP_GET, flash_info );
	server.begin();
}

void loop(void)
{
	server.handleClient();
}
