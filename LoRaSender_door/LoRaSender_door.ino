//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
//define the pins used by the LoRa transceiver module
#define ss 10
#define rst 9
#define dio0 2
#define Buzzer 7
#define RELAY_IO   8
#define theft   6
#define BAND 433E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America
String LoRaMessage = "";
#include<LiquidCrystal.h>
LiquidCrystal lcd (A0, A1, A2, A3, A4, A5);

// Initialize variables to get and save LoRa data
int rssi;
int relayswitch;
String status;
String readingID;


// Replaces placeholder with RElay values
String
processor (const String & var)
{
  //Serial.println(var);
  if (var == "STATUS")
    {
      return status;
    }
  else if (var == "RRSI")
    {
      return String (rssi);
    }
  return String ();
}

void
setup ()
{
  lcd.begin (16, 2);
  lcd.setCursor (0, 0);
  lcd.print ("Door unlock");
  lcd.setCursor (0, 1);
  lcd.print ("Using Lora mod.");
  Serial.begin (115200);
  pinMode (RELAY_IO, OUTPUT);
  pinMode (Buzzer, OUTPUT);
  pinMode (theft, INPUT_PULLUP);
  digitalWrite (RELAY_IO, HIGH);
  int counter;
  //setup LoRa transceiver module
  LoRa.setPins (ss, rst, dio0); //setup LoRa transceiver module
  while (!LoRa.begin (BAND) && counter < 10)
    {
      Serial.print (".");
      counter++;
      delay (2000);
    }
  if (counter == 10)
    {
      // Increment readingID on every new reading
      Serial.println ("Starting LoRa faiBuzzer!");
    }
  Serial.println ("LoRa Initialization OK!");
  delay (2000);
  lcd.clear ();
}

void
loop ()
{
  lcd.setCursor (0, 0);
  lcd.print ("Cmd waiting.....");
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
    if (relayswitch == 1)
      {
        digitalWrite (RELAY_IO, LOW);
        digitalWrite (Buzzer, HIGH);
        delay (200);
        digitalWrite (RELAY_IO, HIGH);
        digitalWrite (Buzzer, LOW);
      }
  }
      rssi = LoRa.packetRssi ();  // Get RSSI
      Serial.print (" with RSSI ");
      Serial.println (rssi);
    }
  int read_switch = digitalRead (theft);
  if (read_switch == LOW)
    {
      int data_s = 5;
      LoRaMessage = String (data_s);
      LoRa.beginPacket ();
      LoRa.print (LoRaMessage);
      rssi = LoRa.packetRssi ();
      Serial.print ("Sending packet: ");
      //Serial.println(readingID);
      Serial.println (LoRaMessage);
      LoRa.endPacket ();
    }
}
