#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, 16, 5, 4);

const char *ssid = "SSID";
const char *password = "PASSWORD";
IPAddress clientIpAddress(111, 111, 111, 111); //My php server's IP

int ledPin = 13;
WiFiServer server(587);

void setup()
{
  u8g2.begin();
  printToScreen("Starting");

  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  IPAddress ip = WiFi.localIP();
  char ipCstring[16];
  utoa(ip[0], ipCstring, 10); //put the first octet in the array
  for (byte octetCounter = 1; octetCounter < 4; ++octetCounter)
  {
    strcat(ipCstring, ".");
    char octetCstring[4];                     //size the array for 3 digit number + null terminator
    utoa(ip[octetCounter], octetCstring, 10); //convert the octet to a string
    strcat(ipCstring, octetCstring);
  }

  printToScreen(ipCstring);
}

void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  delay(400);
  int counter = 0;
  Serial.println("new client");
  while (!client.available())
  {
    delay(10);
    counter++;
    if (counter > 100)
    {
      break;
    }
    Serial.println(counter);
  }

  Serial.println("Client Ip:");
  Serial.println(client.remoteIP());

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
  Serial.println(request.indexOf("/ACTION"));
  if (clientIpAddress == client.remoteIP() && request.indexOf("/ACTION") != -1)
  {
    digitalWrite(ledPin, LOW);
    client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
    client.println("Content-Type: text/html");
    client.println("");
    delay(50);
    digitalWrite(ledPin, HIGH);
    delay(400);
    digitalWrite(ledPin, LOW);
    delay(300);
  }

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}

void printToScreen(char *text)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_9x15B_mf);
  u8g2.drawStr(5, 25, text);
  u8g2.sendBuffer();
}
