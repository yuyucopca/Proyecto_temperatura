#include <avr/wdt.h>
#include <UIPEthernet.h>

// Com UDP
char bufferUDP[1024]={0};
uint8_t mac[6] = {0x07,0x01,0x02,0x03,0x04,0x05};//#8
IPAddress ips(192, 168, 101, 3);
IPAddress ipd(192, 168, 0, 9);
int success1;
char Te1[7]; // Buffer big enough for 7-character float
//Timers
volatile unsigned int Cuenta;
unsigned long int previousMillis,currentMillis;
//Termopar type K
int CLK1=4,CS1=3,MO1=2;
float temp1;

class Variables 
{
 public:
  float readCelsius(int n1, int n2, int n3);
 private:
  uint8_t conversionTemp(int n4, int n5);
};

Variables Termopar;
EthernetUDP udp;

float Variables::readCelsius(int IN1, int IN2, int IN3) 
{
  uint16_t v;
  int CS,CLK,MO;
  CS=IN1;
  CLK=IN2;
  MO=IN3;
  digitalWrite(CS, LOW);
  v = conversionTemp(CLK,MO);
  v <<= 8;
  v |= conversionTemp(CLK,MO);
  digitalWrite(CS, HIGH);
  if (v & 0x4) 
    return NAN;
  v >>= 3; 
  return v * 0.25;
}

byte Variables::conversionTemp(int IN7, int IN8) 
{
  int i;
  byte d = 0;
  int CLK,MO;
  CLK=IN7;
  MO=IN8;
  for (i = 7; i >= 0; i--) 
  {
    digitalWrite(CLK, LOW);
    if (digitalRead(MO))
      d |= (1 << i);
    digitalWrite(CLK, HIGH);
  }
  return d;
}

void setup() 
{
 wdt_disable();                             
 SREG = (SREG & 0b01111111); //Desabilitar interrupciones
 TIMSK2 = TIMSK2|0b00000001; //Habilita la interrupcion por desbordamiento
 TCCR2B = 0b00000111; //Configura preescala para que FT2 sea de 7812.5Hz
 SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones //Desabilitar interrupciones
 Ethernet.begin(mac,ips);
 udp.begin(5000);
 pinMode(CS1, OUTPUT);
 pinMode(CLK1, OUTPUT);
 pinMode(MO1, INPUT);
 wdt_enable(WDTO_4S);
}

void loop()
{ 
 ProcessVariables();
 if (Cuenta>50)
 {
    Com();
    Cuenta=0;
 }
 wdt_reset();
}

ISR(TIMER2_OVF_vect){Cuenta++;}

void ProcessVariables()
{
  currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) 
  {
    previousMillis = currentMillis;
    temp1 = Termopar.readCelsius(CS1,CLK1,MO1); 
  } 
  dtostrf(temp1, 4, 2, Te1); // Leave room for too large numbers! 
  sprintf(bufferUDP," %s ",Te1); 
}

void Com()
{
 success1 = udp.beginPacket(ipd, 8000);
 while (!success1);
 udp.write(bufferUDP);
 udp.endPacket();
 udp.stop();
}
