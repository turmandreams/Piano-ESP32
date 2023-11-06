#include <EEPROM.h>

#define eeprominicio 0

union Float_Byte{
  float datoF;
  byte  datoB[4];
}unionFB;

union Int_Byte{
  int datoI;
  byte  datoB[2];
}unionIB;

int pos=0;

float valor[15];
float minimo[15];
float maximo[15];
byte val[15];


    
void guarda(){
  yield();
  EEPROM.begin(512);    
  int addr=eeprominicio;
  
  for(int i=0;i<15;i++){
    
    unionFB.datoF=minimo[i];  
    EEPROM.put(addr,unionFB.datoB[0]);addr++;
    EEPROM.put(addr,unionFB.datoB[1]);addr++;
    EEPROM.put(addr,unionFB.datoB[2]);addr++;
    EEPROM.put(addr,unionFB.datoB[3]);addr++;
   
    unionFB.datoF=maximo[i];     
    EEPROM.put(addr,unionFB.datoB[0]);addr++;
    EEPROM.put(addr,unionFB.datoB[1]);addr++;
    EEPROM.put(addr,unionFB.datoB[2]);addr++;
    EEPROM.put(addr,unionFB.datoB[3]);addr++;
    yield();
  }

  EEPROM.end();   
 
  delay(500);
  
}

void setup() {

  Serial.begin(115200);
  for(int i=0;i<15;i++){
         valor[i]=0;
         maximo[i]=0;  
         minimo[i]=5000;  
  }
  guarda();    
  
}

void loop() {

 
  
}
