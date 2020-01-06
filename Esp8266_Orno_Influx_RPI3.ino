#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <InfluxDb.h>


#define INFLUXDB_HOST "Type Influx server address"
#define WIFI_SSID "Type WIFI_SSID"
#define WIFI_PASS "Type WIFI_PASS"
ESP8266WiFiMulti WiFiMulti;
Influxdb influx(INFLUXDB_HOST);

volatile long pulseCount = 1; 
unsigned long pulseTime,lastTime; 

volatile float power; 
volatile float ppwh = 1; //Number of pulses per wh - found or set on the meter.//1000 pulses/kwh = 1 pulse per wh

const int send_interval = 1000 * 15;
const int max_value = 10000;

String power_string;

void setup()
{
Serial.begin(9600);

  Serial.println(" ### Hello ###");

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WIFI");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    
    delay(100);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

 influx.setVersion(2);
 influx.setOrg("Type Org name");
 influx.setBucket("Type Bucket name");
 influx.setToken("Type Token");
  Serial.println("Setup done");
   
  attachInterrupt(digitalPinToInterrupt(4), onPulse, RISING); // KWH interrupt attached to IRQ 1 = pin3
}

ICACHE_RAM_ATTR void onPulse() {

  lastTime = pulseTime;
  pulseTime = micros();
  pulseCount++;
  power = (3600000000.0 / (pulseTime - lastTime))/ppwh;   //Calculate power

}

void loop()
{
 
  if (power < max_value)
  {
    InfluxData row("WattConsumption");
  row.addTag("device", "WemosD1");
  row.addTag("sensor", "Orno");
  row.addValue("value", power);

  influx.write(row);
  }
 
  delay(send_interval);
  
}
