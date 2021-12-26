#include<LiquidCrystal.h>
LiquidCrystal lcd (A0, A1, A2, A3, A4, A5);  //defining header file for display
//Libraries for LoRa
#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
//define the pins used by the LoRa transceiver module
#define ss 10
#define rst 9
#define dio0 2
#define BAND 433E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America
int button = 3;     //Push Button
int buzzer = 7;
int rssi;
int relayswitch;
String status;
String readingID;
int counter = 0;
String LoRaMessage = "";
void
setup ()
{
  lcd.begin (16, 2);
  lcd.setCursor (0, 0);
  lcd.print ("Door unlock Tx.");
  lcd.setCursor (0, 1);
  lcd.print ("Using Lora mod.");
  //initialize Serial Monitor
  Serial.begin (115200);
  pinMode (button, INPUT_PULLUP);
  pinMode (buzzer, OUTPUT);
  LoRa.setPins (ss, rst, dio0); //setup LoRa transceiver module
  while (!LoRa.begin (BAND))
    {
      Serial.print (".");
      delay (500);
    }
  Serial.println ("LoRa Initialization OK!");
}


void
loop ()
{
  lcd.setCursor (0, 0);
  lcd.print ("Press button");
  lcd.setCursor (0, 1);
  lcd.print ("for unlock door");
  if (digitalRead (button) == false)
    {
      status = 1;
      LoRaMessage = String (status);
      LoRa.beginPacket ();
      LoRa.print (LoRaMessage);
      rssi = LoRa.packetRssi ();
      Serial.print ("Sending packet: ");
      Serial.println (LoRaMessage);
      LoRa.endPacket ();
      delay (300);
    }
  else
    {
      status = "0";
      LoRaMessage = String (status);
    }
  Serial.println (LoRaMessage);

  /////////////////////////////////////////////////////////////
  int packetSize = LoRa.parsePacket ();
  if (packetSize)
    {
      Serial.print ("Lora packet received: ");
      while (LoRa.available ()) // Read packet
  {
    String LoRaData = LoRa.readString ();
    Serial.print (LoRaData);
    int pos1 = LoRaData.indexOf ('&');
    readingID = LoRaData.substring (0, pos1); // Get readingID
    status = LoRaData.substring (pos1 + 1, LoRaData.length ()); // Get status
    relayswitch = status.toInt ();  //Converts string to integer
    if (relayswitch == 5)
      {
        digitalWrite (buzzer, HIGH);
      }
  }
      rssi = LoRa.packetRssi ();  // Get RSSI
      Serial.print (" with RSSI ");
      Serial.println (rssi);
    }
}
