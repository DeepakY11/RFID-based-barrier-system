 #include <SPI.h>
#include <MFRC522.h>
#include<ESP8266WiFi.h>
#include<Adafruit_MQTT.h>
#include<Adafruit_MQTT_Client.h>
#include <Servo.h>
Servo servo;

#define ss_pin 4
#define rst_pin 5
#define ssid "D"
#define pass "deepuy12"

#define AIO_USERNAME  "Deepu_Y"
#define AIO_KEY       "aio_ahlC29jg71m2DfHNpblESQqeMWQ7"
#define AIO_SERVER     "io.adafruit.com"
#define AIO_SERVERPORT  1883

MFRC522 mfrc522(ss_pin, rst_pin);
int statuss = 0;
int out = 0;
int a=0;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish access = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/access");

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.println(F("Adafruit MQTT"));
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected ");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

   connect();

  Serial.println("Show your card:");

  servo.attach(2); //D4
  servo.write(0);
 delay(2000);
}

void loop()
{

  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }
       
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  delay(1000);
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  
   if (content.substring(1) == "14 E3 EE 2B") //change UID of the card that you want to give access
  {
    Serial.println(" Authorized Access ");
      servo.write(180);
      a=1;
      access.publish(a);
    delay(10000);
     a=0;
    access.publish(a);
    servo.write(0);
  }
  
  else   {
    Serial.println(" Access Denied ");
    delay(3000);
  }
}

void connect() 
{
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) 
    {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(10000);
  }
  Serial.println(F("Adafruit IO Connected!"));
}
