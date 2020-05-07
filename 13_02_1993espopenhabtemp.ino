//Import Libraries
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <EmonLib.h>
EnergyMonitor energyMonitor;

// DHT22 Temperature chip i/o

char tmp[50];
char hum[50];
char POW[50];
char ene[50];
int tellstate = 0;

 float Current = 0; 
 float Voltage = 0; 
 double energy = 0;
 float power = 0;

//Wifi/Broker parameters
const char* ssid = "Vingyan"; //Wifi network SSID
const char* password = "vingyan@123"; //Wifi network PASSWORD
const char* mqtt_server = "192.168.1.8"; //Broker IP Address

//MQTT Configuration
WiFiClient espClient; //Creates a partially initialised client instance.
PubSubClient client(espClient); //Before it can be used, the server details must be configured

void connect_to_MQTT() {
 client.setServer(mqtt_server, 1885);//Set the MQTT server details
  //client.setCallback(callback);

  if (client.connect("temperature_sensor_relay")) {
    Serial.println("Connected to MQTT Server");
    client.subscribe("sensor/relay");
  } else {
    Serial.println("Could not connect to MQTT");
  }
}


void setup() {
  Serial.begin(115200); //Sets the data rate in bits per second (baud) for serial data transmission.
  energyMonitor.current(A0, 2.29);

// Connecting to our WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // initialize pin 5, where the relay is connected to.
 

  connect_to_MQTT();
}

void getTemperature() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  double h = energyMonitor.calcIrms(1484);
  //Serial.println(h);
  Voltage = 230;
  Current = h;
  power = (230* h*0.85);
  energy =  energy + (power*(2.05/60/60/1000));
  Serial.println(energy);
  
 
  
  //Temp as string
  itoa(Voltage,tmp,10);
  client.publish("sensor/VOLTAGE",tmp);
  Serial.println("Voltage");
  Serial.println(tmp);

  //Humidity as string0-
  gcvt(h,2,hum);
  client.publish("sensor/CURRENT",hum);
  Serial.println("Current");
  Serial.println(hum);
  
  itoa(power,POW,10);
  client.publish("sensor/POWER",POW);
  Serial.println("power");
  Serial.println(POW);

 gcvt(energy,6,ene);
  client.publish("sensor/ENERGY",ene);
  Serial.println("energy");
  Serial.println(ene);
}


  
void loop() {
  client.loop();

  if (! client.connected()) {
    Serial.println("Not connected to MQTT....");
    connect_to_MQTT();
  }

 //Every 60 seconds read the temperature, humidity and relay state
  if ( (millis() - tellstate) > 1000 ) {
    getTemperature();
    
   
    tellstate = millis();
  }
}
