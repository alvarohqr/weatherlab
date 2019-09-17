#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SHT1x.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <math.h> 
#define CPMonitorOP
#define SplashScreen "SM-PWM-01C, v1.2"
#define dataPin   16 // AZUL D0
#define clockPin  0 //AMARILLO D1
#define WindSensorPin (14) // The pin location of the anemometer sensor IMPORTANTE D2 PWM
#define Offset 0; 
#define PM10_IN_PIN 12   //input for PM10 signal, D5
#define PM2_IN_PIN 13 //input for PM2 signal, D6
#define PM10_FLAG 1
#define PM2_FLAG 2
//////////////////////////
const byte RainsensorPin = 15; // DEBE SER UN PWM D8
volatile unsigned long Rotations; // cup rotation counter used in interrupt routine 
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine 

int VaneValue;// raw analog value from wind vane 
int Direction;// translated 0 - 360 direction 
int CalDirection;// converted value with offset applied 
int LastValue; 


float WindSpeed; // speed miles per hour
float rainAmount = .3;    
float totalAmount = 0;  
volatile int buttonState; //,buttonState2;
int rainTime;
int i; 

char ScreenHeader[] = "  Advanced Sensors";
char ScreenFooter[] = "  Evaluation Only";

volatile uint8_t bUpdateFlagsShared;

unsigned long samplerate = 5000;
unsigned long sampletime;
int SampleCount, estadoPM2 = HIGH, estadoPM10 = HIGH;
int NoOfSamples = 12;  //maximum 14
static long  PM2_Value;
static long  PM10_Value;
String AQIColour;
float PM2_LowOcp, PM10_LowOcp;
volatile uint16_t unPM10_InShared;
volatile uint16_t unPM2_InShared;
uint32_t ulPM10_Start;
uint32_t ulPM2_Start;
////////////////////////////////////////
#define esp8266_temperatura "estacion/temperatura"
#define esp8266_temp_i "estacion/temp_i"
#define esp8266_humedad "estacion/humedad"
#define esp8266_presion "estacion/presion"
#define esp8266_vel_viento "estacion/vel_viento"
#define esp8266_dir_viento "estacion/dir_viento"
#define esp8266_lluvia "estacion/lluvia"
#define esp8266_PM2 "estacion/PM2.5"
#define esp8266_PM10 "estacion/PM10"
#define esp8266_color "estacion/AQ_color"
////////////////////////////////////////

//Datos de la red
#define ssid "MontoyaCalderon"
#define password "6441715775"

#define mqtt_server "itson.ddns.net"
//#define mqtt_user "nbuptltb"
//#define mqtt_password "tQ7obbbQiWCD"

//Inicializando
//DHT dht(DHTPIN, DHTTYPE);
SHT1x sht1x(dataPin, clockPin);
Adafruit_BMP085 bmp;
WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;


void isr_rotation () { 

if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact. 
Rotations++; 
ContactBounceTime = millis(); 
} 

}

void rain(){
   buttonState = digitalRead(RainsensorPin);
   if(buttonState==1 && rainTime == 1)
  {  
    totalAmount += rainAmount; 
     }  
}

void setup() {
  // put your setup code here, to run once:+
//  dht.begin();
  LastValue = 1; 
  bmp.begin();
  SWM_PM_SETUP();
  
  attachInterrupt(digitalPinToInterrupt(PM10_IN_PIN), interrupcionPM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PM2_IN_PIN), interrupcionPM, CHANGE);
  
   pinMode(WindSensorPin, INPUT); 
   attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING); 

    pinMode(RainsensorPin, INPUT_PULLUP);
    attachInterrupt(RainsensorPin, rain, FALLING);

  Serial.begin(115200);

  Wire.begin ();
  Wire.setClock(50000);
  #ifdef CPMonitorOP
    Serial.println("Arduino Dust & CO2 Value Calculation");
    Serial.print("Dust Sensor sample rate of ");  Serial.print(samplerate / 1000); Serial.print(" sec, with rolling average over "); Serial.print(samplerate / 1000 * NoOfSamples); Serial.println(" sec.");
    Serial.println(SplashScreen);
  #endif
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//Inicialinzando el WiFi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensage recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//Reconección
void reconnect() {
  //Bucle hasta que se de la reconección
  while (!client.connected()) {
    Serial.print("Conección al servidor MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Conectado");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println("Intente de nuevo dentro de 5 segundos");
      delay(5000);
    }
  }
}

long lastMsg = 0;
void loop() {
    
    rain();
    Rotations = 0; // Set Rotations count to 0 ready for calculations 

    sei(); // Enables interrupts 
    delay (3000); // Wait 3 seconds to average 
    cli(); // Disable interrupts 
    
    WindSpeed = Rotations * 0.75; 

    VaneValue = analogRead(A0); 
     Direction = map(VaneValue, 0, 1023, 0, 360); 
     CalDirection = Direction + Offset; 

     if(CalDirection > 360) 
     CalDirection = CalDirection - 360; 

     if(CalDirection < 0) 
     CalDirection = CalDirection + 360; 
 
  if(abs(CalDirection - LastValue) > 5)

     { 
     Serial.print(VaneValue); Serial.print("\t\t"); 
     Serial.print(CalDirection); Serial.print("\t\t"); 
     getHeading(CalDirection); 
     LastValue = CalDirection; 
     } 

    {
  if (millis() >= (samplerate + sampletime))
  {
    CalculateDustValue();
      #ifdef CPMonitorOP
        Serial.print("PM2.5: "); Serial.print (PM2_Value); Serial.print(" "); Serial.print("g/m3"); Serial.print("\t");
        Serial.print("PM10:  "); Serial.print (PM10_Value); Serial.print(" "); Serial.print("g/m3  ");
        Serial.print("AQI Colour Code: "); Serial.println(AQIColour);
      #endif
  }
}
 
   if(!client.connected()){
    reconnect();
   }
   now = millis();
   if (now - lastMeasure > 60000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   
    float h = sht1x.readHumidity();
    // Read temperature as Celsius (the default)
    float t = sht1x.readTemperatureC();
     
    float t_i = bmp.readTemperature();
     
    float p = (bmp.readPressure()/100.00 );

    float ws = WindSpeed; 

    int wd = CalDirection;

    float r = totalAmount;

    float PM2 = PM2_Value + 0.01;

    float PM10 = PM10_Value + 0.01; 
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)|| isnan(p) || isnan(t_i) || isnan(ws) || isnan(wd)) {
      Serial.println("Error al leer!");
      return;
    }
      Serial.print("Temperatura : ");
      Serial.print(t);
      Serial.print("*C ");
      Serial.print(" | Humedad : ");
      Serial.print(h);
      Serial.print(" | presion : ");
      Serial.print(p);
      Serial.print("hPa ");
      Serial.print(" | temperatura interna : ");
      Serial.print(t_i);
      Serial.print("°C");
      Serial.print(" | Velocidad del Viento : "); 
      Serial.print(ws);
      Serial.print("MPH"); 
      Serial.print(" | Direccion del Viento : "); 
      Serial.print(wd);
      Serial.print(" | ° "); 
      Serial.print("Lluvia");
      Serial.print(r);
      Serial.print("mm"); 
      
      client.publish(esp8266_temperatura, String(t).c_str(), true);  
      client.publish(esp8266_temp_i, String(t_i).c_str(), true);
      client.publish(esp8266_humedad, String(h).c_str(), true); 
      client.publish(esp8266_presion, String(p).c_str(), true);
      client.publish(esp8266_vel_viento, String(ws).c_str(), true);
      client.publish(esp8266_dir_viento, String(wd).c_str(), true);      
      client.publish(esp8266_lluvia, String(r).c_str(), true);  
      client.publish(esp8266_PM2, String(PM2).c_str(), true);
      client.publish(esp8266_PM10, String(PM10).c_str(), true);
      client.publish(esp8266_color, String(AQIColour).c_str(), true);

  }
}

void getHeading(int direction) { 
if(direction < 18) 
Serial.println("N"); 
else if (direction < 33) 
Serial.println("NE"); 
else if (direction < 57) 
Serial.println("E"); 
else if (direction < 97) 
Serial.println("SE"); 
else if (direction < 139) 
Serial.println("S"); 
else if (direction < 183) 
Serial.println("SW"); 
else if (direction < 208) 
Serial.println("W"); 
else if (direction < 232) 
Serial.println("NW"); 
else 
Serial.println("N"); 
}



void SWM_PM_SETUP()
{
  pinMode(PM10_IN_PIN, INPUT);
  pinMode(PM2_IN_PIN, INPUT);
  //pinMode(PM10_OUT_PIN, OUTPUT);
  //pinMode(PM2_OUT_PIN, OUTPUT);
}

void CalculateDustValue()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained
  // between calls to loop.
  static uint16_t unPM10_In;
  static uint16_t unPM2_In;
  static uint16_t unPM10_Time;
  static uint16_t unPM2_Time;
  // local copy of update flags
  static uint8_t bUpdateFlags;
  static long    PM2_Output[15];
  static long    PM10_Output[15];
  // check shared update flags to see if any channels have a new signal
  if (bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;

    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.

    if (bUpdateFlags & PM10_FLAG)
    {
      unPM10_In = unPM10_InShared;
      unPM10_Time = (unPM10_Time + unPM10_In);
      unPM10_InShared = 0;
    }

    if (bUpdateFlags & PM2_FLAG)
    {
      unPM2_In = unPM2_InShared;
      unPM2_Time = (unPM2_Time + unPM2_In);
      unPM2_InShared = 0;
    }

    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    bUpdateFlagsShared = 0;

    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
    // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
    // service routines own these and could update them at any time. During the update, the
    // shared copies may contain junk. Luckily we have our local copies to work with :-)
  }

  // do any processing from here onwards
  // only use the local values unPM10_In and unPM2_In, the shared
  // variables unPM10_InShared, unPM2_InShared are always owned by
  // the interrupt routines and should not be used in loop

  sampletime = millis();  //resets timer before printing output
  PM2_Output[SampleCount] = unPM2_Time ;
  PM10_Output[SampleCount] = unPM10_Time ;
  unPM2_Time = 0;
  unPM10_Time = 0;

  PM2_Output[0] = PM2_Output[1] + PM2_Output[2] + PM2_Output[3] + PM2_Output[4] + PM2_Output[5] + PM2_Output[6] + PM2_Output[7] + PM2_Output[8]+ PM2_Output[9]+ PM2_Output[10]+ PM2_Output[11]+ PM2_Output[12];
  PM10_Output[0] = PM10_Output[1] + PM10_Output[2] + PM10_Output[3] + PM10_Output[4] + PM10_Output[5] + PM10_Output[6] + PM10_Output[7] + PM10_Output[8] + PM10_Output[9] + PM10_Output[10] + PM10_Output[11] + PM10_Output[12];

//  Serial.print (PM2_Output[0]); Serial.print("\t");
//  Serial.print (PM10_Output[0]); Serial.println("\t");


  /* converts LP outputs to values, calculate % LPO first, then converet to Âµg/m3 assuming conversion is linear
              output (ÂµS)                           concentration change (250 or 600)
     -----------------------------------    x 100 x ---------------------------------  + offset (0 or 250)
     sample rate (mS) x 1000 x NoOfSamples               percentage change (3 0r 7)

  */
  PM2_LowOcp = ((float)PM2_Output[0] / (samplerate * NoOfSamples * 10 ) );
  PM10_LowOcp = ((float)PM10_Output[0] / (samplerate * NoOfSamples * 10 ) );
  Serial.println(PM2_LowOcp);
  Serial.println(PM10_LowOcp);
  if (PM2_Output[0] / (samplerate * NoOfSamples * 10 ) >= 3 || PM10_Output[0] / (samplerate * NoOfSamples * 10 ) >= 3)
  {
    PM2_Value = -10.747 * pow(PM2_LowOcp, 3) + 33.548 * pow(PM2_LowOcp, 2) + 78.617 * (PM2_LowOcp) - 0.5196;
    PM10_Value = -10.747 * pow(PM10_LowOcp, 3) + 33.548 * pow(PM10_LowOcp, 2) + 78.617 * (PM10_LowOcp) - 0.5196;
  }
 else
  {
    PM2_Value = -10.747 * pow(PM2_LowOcp, 3) + 33.548 * pow(PM2_LowOcp, 2) + 78.617 * (PM2_LowOcp) - 0.5196;
    PM10_Value = -10.747 * pow(PM10_LowOcp, 3) + 33.548 * pow(PM10_LowOcp, 2) + 78.617 * (PM10_LowOcp) - 0.5196;
  }
// Serial.print (PM2_Output[SampleCount]); Serial.print("\t");
  bUpdateFlags = 0;
  if (SampleCount >= NoOfSamples)
  {
    SampleCount = 1;
//    Serial.print (PM2_Output[0]); Serial.print("\t");Serial.println("\t");
  }
  else
  {
    SampleCount++;
  }

  if (PM2_Value <= 12 && PM10_Value <= 54)
  {
    AQIColour = "Green ";
  }
  else if (PM2_Value <= 35 && PM10_Value <= 154)
  {
    AQIColour = "Yellow";
  }
  else if (PM2_Value <= 55 && PM10_Value <= 254)
  {
    AQIColour = "Orange";
  }
  else if (PM2_Value <= 150 && PM10_Value <= 354)
  {
    AQIColour = " Red  ";
  }
  else if (PM2_Value <= 250 && PM10_Value <= 424)
  {
    AQIColour = "Purple";
  }
  else
  {
    AQIColour = "Maroon";
  }

}


void interrupcionPM()
{
  uint32_t us = micros();

  if (estadoPM10 != digitalRead(PM10_IN_PIN))
  {
    if ((estadoPM10 = digitalRead(PM10_IN_PIN)) == LOW)
    {
      ulPM10_Start = us;
    }
    else
    {
      unPM10_InShared += (uint16_t)(us - ulPM10_Start);
      bUpdateFlagsShared |= PM10_FLAG;
    }
  }

  if (estadoPM2 != digitalRead(PM2_IN_PIN))
  {
    if ( (estadoPM2 = digitalRead(PM2_IN_PIN) ) == LOW)
    {
      ulPM2_Start = us;
    }
    else
    {
      unPM2_InShared += (uint16_t)(us - ulPM2_Start);
      bUpdateFlagsShared |= PM2_FLAG;
    }
  }

}
