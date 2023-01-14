/*
*/

#include <Arduino.h>
#include <WiFi.h>
#include "Secrets.h"
#include "IoTeX-blockchain-client.h"
#include <UMS3.h>

#define DEFAULT_WIFI_WAIT_MS     150
#define DEFAULT_BLINK_PERIOD_MS  100
#define DEFAULT_PIXEL_NUMBER       0

//  Define colors
const uint32_t BLACK     = ums3.color(0, 0, 0);
const uint32_t RED       = ums3.color(127, 0, 0);
const uint32_t GREEN     = ums3.color(0, 127, 0);
const uint32_t BLUE      = ums3.color(0, 0, 127);
const uint32_t MAGENTA   = ums3.color(127, 0, 127);

#define DEFAULT_COLOR           BLUE

constexpr const char ip[] = "gateway.iotexlab.io";
constexpr const char baseUrl[] = "iotexapi.APIService";
constexpr const int port = 10000;

//  Set the wallet address to check
const char accountStr[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
 
//  Create the IoTeX client connection
Connection<Api> connection(ip, port, baseUrl);
UMS3 ums3;

void umBlinkPixel(uint8_t color=DEFAULT_COLOR, uint16_t period_ms=DEFAULT_BLINK_PERIOD_MS, uint8_t nr_cycles=1) {    
  uint16_t count;
  
  for(count=0; count<nr_cycles; count++) {
    ums3.setPixelColor(color);
    delay(period_ms);
    ums3.setPixelColor(BLACK);
    delay(period_ms);        
  }  
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi .."));

  while (WiFi.status() != WL_CONNECTED) {
    umBlinkPixel(MAGENTA);  
    Serial.print('.');
    delay(DEFAULT_WIFI_WAIT_MS);
  }
 
  Serial.print(F("\r\nConnected. IP: "));
  Serial.println(WiFi.localIP());
}
 
void setup(void) {
  Serial.begin(115200);

  ums3.begin();
  ums3.setPixelBrightness(255 / 3);

  //  Initialize the onboard NeoPixel        
  ums3.setPixelColor(BLACK); 

  //  Connect to the wifi network
  initWiFi();
 
  //  Configure the LED pin
  pinMode(ONBOARD_LED_PIN, OUTPUT);
}
 
void loop(void) {
  // Query the account metadata
  AccountMeta accountMeta;
  ResultCode result = connection.api.wallets.getAccount(accountStr, accountMeta);
 
  // Print the result
  Serial.print("Result: ");
  Serial.println(IotexHelpers.GetResultString(result));
 
  // If the query suceeded, print the account metadata
  if (result == ResultCode::SUCCESS) {
    Serial.print("Balance: ");
    Serial.println(accountMeta.balance);
    Serial.print(F("Nonce: "));
    Serial.println(accountMeta.nonce.c_str());
    Serial.print(F("PendingNonce: "));
    Serial.println(accountMeta.pendingNonce.c_str());
    Serial.print(F("NumActions: "));
    Serial.println(accountMeta.numActions.c_str());
    Serial.print(F("IsContract: "));
    Serial.println(accountMeta.isContract ? "\"true\"" : "\"false\"");
  }
 
  // Enable the onboard led if the balance is > 0.01 IOTX)
  Bignum b = Bignum(accountMeta.balance, NumericBase::Base10);
  Bignum zero = Bignum("0", NumericBase::Base10);

  if (b == zero) {
    digitalWrite(ONBOARD_LED_PIN, LOW);
    ums3.setPixelColor(BLACK);
  } else {
    digitalWrite(ONBOARD_LED_PIN, HIGH);
    ums3.setPixelColor(GREEN);
  }
  
  Serial.println("Program finished");
 
  while (true) {
    delay(1000);
  }
}
