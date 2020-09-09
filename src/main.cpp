/******************************************************************************
main.cpp
Hot Plate Magnetic Stirrer with ESP8266, SSD, MAX31855 and thermocouple
Leonardo Bispo
Aug, 2020
https://github.com/ldab/hot-plate-magnetic-stirrer

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Arduino.h>

#include "secrets.h"

#include <Wire.h>
#include <SPI.h>
#include <Ticker.h>

#include "Adafruit_MAX31855.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Thermocouple instance with SPI on any three
#define MAXDO   PIN_SPI_MISO // D6 - GIPO12
#define MAXCS   PIN_SPI_SS   // D8 - GPIO15
#define MAXCLK  PIN_SPI_SCK  // D5 - GPIO14

// SSR PWM PIN
#define SSR     D2           // GPIO4

// MQTT reconnection timeout in ms
#define MQTT_RC_TIMEOUT 5000

// Temperature reading timer in seconds
#define TEMP_TIMEOUT    1

// Update these with values suitable for your network.
const char* wifi_ssid     = s_wifi_ssid;
const char* wifi_password = s_wifi_password;
const char* mqtt_server   = s_mqtt_server;
const char* mqtt_user     = s_mqtt_user;
const char* mqtt_pass     = s_mqtt_pass;
uint16_t mqtt_port        = s_mqtt_port;

const char* temp_topic = "hot-plate/temp";
const char* set_topic  = "hot-plate/setpoint";
const char* err_topic  = "hot-plate/error";
double temp           = NAN;
float  temp_setpoint  = NAN;

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// initialize the MQTT Client
WiFiClient   espClient;
PubSubClient client(espClient);

// Timer instances
Ticker mqtt_rc;
Ticker temp_reader;

void setup_wifi()
{
  delay(10);

  Serial.print("\nConnecting to ");
  Serial.println(wifi_ssid);

  WiFi.hostname("hot plate stirrer");

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, uint32_t length)
{
  char _payload[length];

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint32_t i = 0; i < length; i++)
  {
    _payload[i] = (char)payload[i];

    Serial.print(_payload[i]);
  }
  Serial.println();

  if( !strcmp(topic, set_topic) )
  {
    temp_setpoint = atof( _payload );
    Serial.printf("New SetPoint: %.02f\n", temp_setpoint);
  }
}

bool reconnect()
{
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client-";
    clientId += WiFi.macAddress();

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("connected");
      // Once connected, subscribe to topic
      client.subscribe( set_topic );
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  
  return client.connected();
}

void read_temp()
{   
  Serial.print("Internal Temp = ");
  Serial.println(thermocouple.readInternal());

  temp = thermocouple.readCelsius();
  if( isnan(temp) )
  {
    char _errMsg[12];
    sprintf(_errMsg, "TC error #%i", thermocouple.readError());
    client.publish(err_topic, _errMsg);
    Serial.println(_errMsg);

    digitalWrite(SSR, LOW);
  } 
  else 
  {
    Serial.print("C = ");
    Serial.println(temp);

    char _payload[8];
    sprintf(_payload, "%.02f", temp);
    client.publish(temp_topic, _payload);

    //SSR PWM
    if((temp - temp_setpoint) < -30)
      analogWrite(SSR, 1023);             //100%
    else if((temp - temp_setpoint) < -15)
      analogWrite(SSR, 1023/2);           //50%
    else if((temp - temp_setpoint) < -3)
      analogWrite(SSR, 1023/4);           //25%
    else
      analogWrite(SSR, 0);                //turn off

  }
}

void setup()
{
  Serial.begin(115200);

  while (!Serial) delay(1); // wait for Serial

  // wait for MAX chip to stabilize
  delay(500);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.print("Initializing sensor... ");
  if( !thermocouple.begin() )
	{
    Serial.println("ERROR.");
    client.publish(err_topic, "MAX31855 error");

    //while (1) delay(10);
  }
  else
    Serial.println("MAX31855 Good");
}

void loop()
{
  if( !client.connected() )
  {
    if( temp_reader.active() )
    {
      temp_reader.detach();
      digitalWrite(SSR, LOW);
    }

    reconnect();
    //delay(2000);
  }
  else
  {
    if( !temp_reader.active() )
      temp_reader.attach( TEMP_TIMEOUT, read_temp );

    client.loop();
  }
}