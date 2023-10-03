
#include "Wire.h"


#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x05

int pos=0;

float valor[15];
float minimo[15];
float maximo[15];
byte val[15];

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
   
  pos++;

  
  
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
  pinMode(25,INPUT);
  pinMode(26,INPUT);
  pinMode(27,INPUT);
  pinMode(14,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  pinMode(15,INPUT);
  pinMode(2,INPUT);
  pinMode(4,INPUT);

  for(int i=0;i<15;i++){
      valor[i]=0;
      maximo[i]=0;  
      minimo[i]=5000;  
  }
  
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
  dato=analogRead(25);funcion(dato);
  dato=analogRead(26);funcion(dato);
  dato=analogRead(27);funcion(dato);
  dato=analogRead(14);funcion(dato);
  dato=analogRead(12);funcion(dato);
  dato=analogRead(13);funcion(dato);
  dato=analogRead(15);funcion(dato);
  dato=analogRead(2);funcion(dato);
  dato=analogRead(4);funcion(dato);

 
  
}

void handler(){

  
    for(byte i=0;i<15;i++){
        Wire.write(val[i]);
    }



}
