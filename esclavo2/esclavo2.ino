#include <EEPROM.h>
#include "Wire.h"


#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x05

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

float valor[13];
float minimo[13];
float maximo[13];
byte val[13];

void funcion(int dato){

  float fdato=(float)dato;
  
  valor[pos]=(valor[pos]*0.98)+(fdato*0.02);

  if(valor[pos]<minimo[pos]){ 
    int v1=(int)valor[pos];
    int v2=(int)fdato;

    if(v1==v2){
        minimo[pos]=valor[pos]; 
    }
  }
  
  if(valor[pos]>maximo[pos]){ maximo[pos]=valor[pos]; }

  int datoI=map((int)valor[pos],(int)minimo[pos],(int)maximo[pos],0,255);
  if(datoI>255){ datoI=255; }
  else if(datoI<0){ datoI=0; }
  byte datoB=(byte)datoI;

  if(((int)maximo[pos])-((int)minimo[pos])<100){ datoB=0; }
  val[pos]=datoB;

  //Serial.println(datoB);

  //Serial.print(datoB);
  //Serial.print(",");
  
  pos++;

  
  
}

void carga() {

    yield();
    EEPROM.begin(512);
    int addr=eeprominicio;    
    for(int i=0;i<13;i++){
      
        EEPROM.get(addr,unionFB.datoB[0]);addr++;
        EEPROM.get(addr,unionFB.datoB[1]);addr++;
        EEPROM.get(addr,unionFB.datoB[2]);addr++;
        EEPROM.get(addr,unionFB.datoB[3]);addr++;
        minimo[i]=unionFB.datoF;

        EEPROM.get(addr,unionFB.datoB[0]);addr++;
        EEPROM.get(addr,unionFB.datoB[1]);addr++;
        EEPROM.get(addr,unionFB.datoB[2]);addr++;
        EEPROM.get(addr,unionFB.datoB[3]);addr++;
        maximo[i]=unionFB.datoF;
        
        yield();
        
    }   
    
}
    
void guarda(){
  yield();
  EEPROM.begin(512);    
  int addr=eeprominicio;
  
  for(int i=0;i<13;i++){
    
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
  Wire.begin((uint8_t)I2C_SLAVE_ADDR,SDA_PIN,SCL_PIN,4000000);    
  Wire.onRequest(handler);  
    
  pinMode(36,INPUT);
  pinMode(39,INPUT);
  pinMode(34,INPUT);
  pinMode(35,INPUT);
  pinMode(32,INPUT);
  pinMode(33,INPUT);
  pinMode(4,INPUT); 
  pinMode(2,INPUT);
  pinMode(27,INPUT);
  pinMode(14,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  pinMode(15,INPUT);

/*
  for(int i=0;i<13;i++){
      valor[i]=0;
      maximo[i]=0;  
      minimo[i]=5000;  
  }
*/

  carga();
  
}

void loop() {

  int dato=0;
  pos=0;
  dato=analogRead(36);funcion(dato);  
  dato=analogRead(39);funcion(dato);
  dato=analogRead(34);funcion(dato);
  dato=analogRead(35);funcion(dato);
  dato=analogRead(32);funcion(dato);
  dato=analogRead(33);funcion(dato);
  dato=analogRead(4);funcion(dato);
  dato=analogRead(2);funcion(dato);
  dato=analogRead(27);funcion(dato);
  dato=analogRead(14);funcion(dato);
  dato=analogRead(12);funcion(dato);
  dato=analogRead(13);funcion(dato);
  dato=analogRead(15);funcion(dato);
 
  ////Se guarda la calibracion
  
  if((val[1]>120)&&(val[2]>120)&&(val[4]>120)&&(val[6]>120)&&(val[8]>120)){  
      guarda();    
  }

  ////Se resetea la calibracion y se guarda

  if((val[0]>120)&&(val[3]>120)&&(val[5]>120)&&(val[7]>120)){
      
      for(int i=0;i<13;i++){
         valor[i]=0;
         maximo[i]=0;  
         minimo[i]=5000;  
      }
      guarda();    
  }
  
  //Serial.println("");
}

void handler(){

  
    for(byte i=0;i<13;i++){
        Wire.write(val[i]);
    }



}
