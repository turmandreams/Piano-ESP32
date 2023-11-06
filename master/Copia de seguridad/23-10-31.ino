#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_task_wdt.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "imagenes.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR1 0x04
#define I2C_SLAVE_ADDR2 0x05
#define I2C_SLAVE_ADDR3 0x06

#define PIN 12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(69, PIN, NEO_GRB + NEO_KHZ800);


//GPIO's
//#define BCK  26
//#define LRC  27
//#define DOU  25


//MICROSD PIN
//VCC  3,3v 
//GND  GND
//CS   GPIO4
//MISO GPIO19
//SCK  GPIO12
//MOSI GPIO23
                                             
#include <SPI.h>
#include <SD.h>


TaskHandle_t Task1;

File root;
File file;

String archivos[100];
int numarchivos=0;
int posarchivos=0;

boolean pulsado=false;

// Configuración del bus I2S PCM5102
i2s_config_t i2sConfig;

// Buffer de salida de audio
int16_t audioBuffer[512];
size_t bytesWritten;

int16_t ultimovalor=0;

float volumen=0.4;

int limitepulsacion=5;

byte sonido[3][25000];
int tamsonido[4];

float frecuenciasonido[4][39];
float possonido[4][39];

float frecuenciawav[4];
boolean sonidorelleno[4];
boolean cargarsonido[4];

boolean play=false;

int instante;

int botonseleccionado=0;

int tiempoluces;
int tiempo;

boolean teclas[39];
int tiempoteclas[39];

byte valorteclas[39];

float volumenteclas[39];

int notasenteclas[39];
byte musicanotas[1000];
uint16_t musicatiempoinicio[1000];
uint16_t musicatiempofin[1000];

int musicaposicion=0;
int tiempomusica;
int musicatam=0;

int musicadelay[4];


int tiempon=0;
int tiempoinicio=0;

int menu=0;
int tiempomenu;

String archivocargado="";
int color=0;
byte rcolor=0;
byte gcolor=0;
byte bcolor=200;

boolean nivelteclas=false;
boolean efectochorus=false;

boolean grabando=false;
boolean playing[4];

boolean menuplaying=false;

void cargasonido(int numsonido){

  archivocargado=archivos[posarchivos];
  menupantalla(); 
  
 // Serial.print("Se va abrir el archivo : ");
 // Serial.println(nombrearchivo);    
    
  File file2 = SD.open(archivocargado,"r");  
  
  if (!file2) { Serial.println("Failed to open file for reading"); return; }  
  
  
  byte header[44];file2.read(header,44);

  tamsonido[numsonido]=file2.read(sonido[numsonido],25000);
  
  
  /*  
  while (file2.available()) {
    byte dato=file2.read();
    sonido[tamsonido]=dato;
    tamsonido++;
    if(tamsonido>=100000){ break;}
  }
  */

  //Serial.print("Size : ");
  //Serial.println(tamsonido[numsonido]);

  sonidorelleno[numsonido]=true;

  frecuenciawav[numsonido]=523.25;
    
  file2.close();      
  
  /*
  int maximo=-100000;
  for(int i=0;i<tamsonido-1;i++){
    int d=sonido[i+1]-sonido[i];
    if(d>maximo){ maximo=d;}    
    if(d>255){ Serial.println(d); delay(10); }       
  }
  */
  
  //Serial.print("Maximo : ");
  //Serial.println(maximo);

  
}


void luces(){
  
  if(teclas[0]){  led(0,true);  }else {  led(0,false);  }
  if(teclas[2]){  led(1,true);  }else {  led(1,false);  }
  if(teclas[4]){  led(2,true);  }else {  led(2,false);  }
  if(teclas[5]){  led(3,true);  }else {  led(3,false);  }
  if(teclas[7]){  led(4,true);  }else {  led(4,false);  }
  if(teclas[9]){  led(5,true);  }else {  led(5,false);  }
  if(teclas[11]){ led(6,true);  }else {  led(6,false);  }
  
  if(teclas[12]){  led(7,true);  }else {  led(7,false);  }
  if(teclas[14]){  led(8,true);  }else {  led(8,false);  }
  if(teclas[16]){  led(9,true);  }else {  led(9,false);  }
  if(teclas[17]){  led(10,true);  }else {  led(10,false);  }
  if(teclas[19]){  led(11,true);  }else {  led(11,false);  }
  if(teclas[21]){  led(12,true);  }else {  led(12,false);  }
  if(teclas[23]){  led(13,true);  }else {  led(13,false);  }
  
  if(teclas[24]){  led(14,true);  }else {  led(14,false);  }
  if(teclas[26]){  led(15,true);  }else {  led(15,false);  }
  if(teclas[28]){  led(16,true);  }else {  led(16,false);  }
  if(teclas[29]){  led(17,true);  }else {  led(17,false);  }
  if(teclas[31]){  led(18,true);  }else {  led(18,false);  }
  if(teclas[33]){  led(19,true);  }else {  led(19,false);  }
  if(teclas[35]){  led(20,true);  }else {  led(20,false);  }
  
  if(teclas[36]){  led(21,true);  }else {  led(21,false);  }
  if(teclas[38]){  led(22,true);  }else {  led(22,false);  }
  
  pixels.show();
  
}

void led(int i,boolean encendido){

    byte r,g,b;
    if(!encendido){ r=0;g=0;b=0;}
    else{
        if(color==6){
          int c=random(0,5);
          definecolor(c);                    
        }
        r=rcolor;
        g=gcolor;
        b=bcolor;                                    
    }
    
    pixels.setPixelColor(i,pixels.Color(r,g,b));  
    pixels.setPixelColor(45-i,pixels.Color(r,g,b));  
    pixels.setPixelColor(i+46,pixels.Color(r,g,b));  
    
}
  


void pintaplaying(){

  display.clearDisplay();
  String texto="Playing ";
  texto+=(botonseleccionado+1);
  drawCentreString(texto,64,12,2); 
  texto="Ret:";texto+=musicadelay[botonseleccionado];texto+="ms";
  drawCentreString(texto,64,42,2); 
  display.display();
  menuplaying=true;
}

void recogeteclas(){

  byte numeros[39];

  for(int i=0;i<39;i++){ numeros[i]=0; }
  
  //Serial.println("Miramos teclas");
  
  Wire.requestFrom(I2C_SLAVE_ADDR1,15);
  int contador=0;
  
  
  int instante=millis();
  while(contador<15){
    if(Wire.available()>0){
      numeros[contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);

  Wire.requestFrom(I2C_SLAVE_ADDR2,13);
  instante=millis();
  while(contador<28){
    if(Wire.available()>0){
      numeros[contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);
  
  Wire.requestFrom(I2C_SLAVE_ADDR3,11);
  instante=millis();
  while(contador<39){
    if(Wire.available()>0){
      numeros[contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);


   
  //Comprobamos si play y se tocan las notas solas
  if(playing[botonseleccionado]){

     ///Dentro del las teclas pulsadas se comprueba si es el momento de soltarla 
    
    for(int i=0;i<39;i++){
        if(notasenteclas[i]!=-1){
          if((millis()-tiempomusica)>(musicatiempofin[notasenteclas[i]]+musicadelay[botonseleccionado])){
              numeros[i]=0;                           
              notasenteclas[i]=-1;
              //String texto="Tecla ";texto+=i;texto+=" OFF";Serial.println(texto);
              //ponerenpantalla(texto);              
           
          }else{
              numeros[i]=130;      
          }
        }    
    }


    //Se comprueba si es el momento de pulsar las teclas almacenadas
    
    if(musicaposicion<musicatam){     

      if((millis()-tiempomusica)>(musicatiempoinicio[musicaposicion]+musicadelay[botonseleccionado])){
        
          int tecla=musicanotas[musicaposicion];
            
          //String texto="Tecla ";texto+=tecla;texto+=" ON ";Serial.println(texto);
          //ponerenpantalla(texto);
          
          valorteclas[tecla]=100;
          numeros[tecla]=130;  
          notasenteclas[tecla]=musicaposicion;
          musicaposicion++;
            
      }
  
    }

    boolean termino=true;
    for(int i=0;i<39;i++){  if(notasenteclas[i]!=-1){ termino=false; } }

    if(termino){
        if(musicaposicion>=musicatam){ 
          tiempomusica=millis();
          musicaposicion=0;
        }       
    }
      
  
  }
   //int rampa=((int)valorteclas[0])-((int)numeros[0]);
   //Serial.println(rampa);
    
  
  
  float f=130.81;
    
  for(byte i=0;i<39;i++){

    if(!teclas[i]){  //Tecla no pulsada   
      if((millis()-tiempoteclas[i])>50){         
        //if(numeros[i]>80){
        if(numeros[i]>120){

          if(grabando){
              if(musicatam==0){ tiempoinicio=millis();musicadelay[botonseleccionado]=0;}
              notasenteclas[i]=musicatam;      
              musicanotas[musicatam]=i;
              tiempon=millis()-tiempoinicio;      
              musicatiempoinicio[musicatam]=tiempon;                
              musicatam++; 
          }
      
          int rampa=(((int)valorteclas[i])-((int)numeros[i]))/2;
          if(rampa>10){ rampa=10; }
          volumenteclas[i]=0.1*rampa;          
          
          frecuenciasonido[botonseleccionado][i]=f;
          possonido[botonseleccionado][i]=0; 

          if(efectochorus){
            if(i<38){frecuenciasonido[botonseleccionado][i+1]=(f+5);possonido[botonseleccionado][i+1]=0; }
            if(i>0){frecuenciasonido[botonseleccionado][i-1]=(f-5);possonido[botonseleccionado][i-1]=0; }            
          }
                    
          teclas[i]=true; 
          tiempoteclas[i]=millis();
        }  
      }          
    }else{  //Tecla no pulsada
      if((millis()-tiempoteclas[i])>50){
        //if(numeros[i]<180){
        if(numeros[i]<120){
          possonido[botonseleccionado][i]=tamsonido[botonseleccionado];          
          teclas[i]=false;  
          
          if(grabando){                          
              tiempon=millis()-tiempoinicio;      
              musicatiempofin[notasenteclas[i]]=tiempon;                                          
          }
          
          tiempoteclas[i]=millis();      
        }  
      }    
    }
       
    f=f*1.06;    
    
  }

  for(byte i=0;i<39;i++){ valorteclas[i]=numeros[i]; }
  
}

void actualizacolor(){

  definecolor(color);

  menupantalla();

  compruebaluces();
  
}

boolean pulsacionbotones(){

  int valor=analogRead(36); //Boton Derecha
  //Serial.println(valor);
  if(valor<255){ 
    menu++;
    if(menu>5){ menu=0;}
    menupantalla();
    
  }
  
  valor=analogRead(39); //Boton Izquierda
  if(valor<255){ 
    menu--;
    if(menu<0){ menu=5;}
    menupantalla();    
  }

  
  valor=analogRead(34);if(valor<255){  //Boton Abajo
     if(menuplaying){
        musicadelay[botonseleccionado]-=10;
        if(musicadelay[botonseleccionado]<0){musicadelay[botonseleccionado]=0;}
        pintaplaying();
     }else{
      
       if(menu==0){
          posarchivos--;
          if(posarchivos<0){ posarchivos=numarchivos-1; }
          cargarsonido[botonseleccionado]=true;
       }else if(menu==1){
          color--;
          if(color<0){ color=6; }
          actualizacolor();
       }else if(menu==2){        
          frecuenciawav[botonseleccionado]*=1.06;
          menupantalla();  
       }else if(menu==3){        
          volumen-=0.05;
          if(volumen<0.05){ volumen=0.05;}
          menupantalla();  
       }else if(menu==4){        
          nivelteclas=!nivelteclas;
          menupantalla();  
       }else if(menu==5){        
          efectochorus=!efectochorus;
          menupantalla();  
       }
     }
     
  }
  
  valor=analogRead(35);if(valor<255){ //Boton Arriba

     if(menuplaying){
        musicadelay[botonseleccionado]+=10;
        if(musicadelay[botonseleccionado]>5000){musicadelay[botonseleccionado]=5000;}
        pintaplaying();
     }else{
       if(menu==0){
          posarchivos++;
          if(posarchivos>=numarchivos){ posarchivos=0; }
          cargarsonido[botonseleccionado]=true;        
       }else if(menu==1){
          color++;
          if(color>6){ color=0; }
          actualizacolor();
       }else if(menu==2){
          frecuenciawav[botonseleccionado]/=1.06;
          menupantalla();  
       }else if(menu==3){        
          volumen+=0.05;
          if(volumen>1.0){ volumen=1.0;}
          menupantalla();  
       }else if(menu==4){        
          nivelteclas=!nivelteclas;
          menupantalla();  
       }else if(menu==5){        
          efectochorus=!efectochorus;
          menupantalla();  
       }
     }
  }
  
  valor=analogRead(32);if(valor<255){ ponerenpantalla("Boton 5"); }
  
    
    
  if(digitalRead(33)==LOW){ //Chanel --
      botonseleccionado--;
      if(botonseleccionado<0){ botonseleccionado=3; }
      menupantalla();
  }
  
  if(digitalRead(13)==LOW){ //Chanel ++
      botonseleccionado++;
      if(botonseleccionado>=4){ botonseleccionado=0; }
      menupantalla();
  }
  
  if(digitalRead(15)==LOW){ //Play y Stop Notas guardadas
      if(!grabando){
        delay(500);
        playing[botonseleccionado]=!playing[botonseleccionado];
        if(playing[botonseleccionado]){

            pintaplaying();
            
            tiempomusica=millis();
            musicaposicion=0;
            for(int i=0;i<39;i++){ notasenteclas[i]=-1; }
            
            if(musicatam==0){
                playing[botonseleccionado]=false;
                menupantalla();  
            }
        }else{
            menuplaying=false;
            menupantalla();  
        }
      }
      
  }

  
  if(digitalRead(4)==LOW){  ///Boton de Grabacion
      if(!playing[botonseleccionado]){
        if(!grabando){          
            display.clearDisplay();
            String texto="Record ";
            texto+=(botonseleccionado+1);
            drawCentreString(texto,64,32,2); 
            display.display();
            //tiempoinicio=millis();
            tiempon=0;
            musicatam=0;
            /*
            for(int i=0;i<1000;i++){
                  musicatiempoinicio[i]=0;
                  musicatiempofin[i]=0;
            }  
            */                
            grabando=true;
            
        }else{          
            grabando=false;   
            /*   
            for(int i=0;i<musicatam;i++){
                Serial.print(" T : ");Serial.print(musicanotas[i]);
                Serial.print(" I : ");Serial.print(musicatiempoinicio[i]);
                Serial.print(" F : ");Serial.println(musicatiempofin[i]);
            }
            */
            menupantalla(); 
        }
        delay(500); 
      }
    
  }
      

  return pulsado;
    
}



void printDirectory() {
  
  int sigue = 1;
  int i = 0;

  root.rewindDirectory();
  
  numarchivos = 0;
  while (sigue == 1) {
      File entry =  root.openNextFile();
      if (! entry) { break; }

      String nombre=entry.name();
      if(nombre.indexOf("/")!=-1){ archivos[numarchivos] = ""; }
      else{ archivos[numarchivos] = "/"; }
      
      archivos[numarchivos] +=entry.name();
      Serial.println(archivos[numarchivos]);
      entry.close();
      numarchivos++;
  }  
   
}


void silencio(){

    for (int i = 0; i < sizeof(audioBuffer) / sizeof(int16_t); i += 2) {        
        audioBuffer[i] = 0;
        audioBuffer[i + 1] = 0;        
    }
      
}



void actualizasonidos(){  
  for(int i=0;i<3;i++){
      if(cargarsonido[i]){ 
        cargasonido(i);         
        cargarsonido[i]=false;
        
      }      
  }  
}


void reproduce(){

  //Serial.println("Entra a reproducir");
  
  int j=0;  
  boolean consonido=false;
  
  for(;;){

    int nsonidos=0;

    consonido=false;

    int32_t valormuestra=0;
    
    for(int i=0;i<3;i++){
      
      if(sonidorelleno[i]){  

        for(int jj=0;jj<39;jj++){
                
          if(possonido[i][jj]<(tamsonido[i]-50)){
  
            consonido=true;
            
            float vel=frecuenciasonido[i][jj]/frecuenciawav[i];    
            vel=vel*2;
    
            int ii=(int)possonido[i][jj];
            if((ii%2)!=0){ ii++; }
            
            int16_t dato1;      
            dato1 = sonido[i][ii+1]<<8 | sonido[i][ii];

            int16_t dato2;      
            dato2 = sonido[i][ii+3]<<8 | sonido[i][ii+2];

            float dat1 = (float)dato1;
            float dat2 = (float)dato2;

            float p=possonido[i][jj]-ii;

            float dat=(dat1*(1-p))+(dat2*p);
            
            if(!nivelteclas){ volumenteclas[jj]=0.5;}
            
            dat=dat*volumenteclas[jj];
  
            valormuestra+=(int32_t)dat;  
            nsonidos++;          
                       
            possonido[i][jj]+=vel;
            
          }
          
        }  
               
      }  
           
    }

    boolean sigue=true;
    int32_t dato2=valormuestra;

    if(nsonidos!=0){ dato2=dato2/nsonidos; }

    dato2=valormuestra*volumen;
    
    if(!consonido){
      
      while(sigue){
            sigue=false;
                        
            int16_t dif=dato2-ultimovalor;
            dif=abs(dif);
              
            if(dif>128){
              dato2=(dato2+ultimovalor)/2;            
              sigue=true;
            }
      }
    
    }   

    if(dato2>32765){ dato2=32765; }
    else if(dato2<-32765){ dato2=-32765; }
    
    audioBuffer[j] = (int16_t)dato2;
    audioBuffer[j+1] = (int16_t)dato2;       
    

    ultimovalor=audioBuffer[j];
    
    j+=2;
    
    if(j>=sizeof(audioBuffer)/sizeof(int16_t)){                 
      
      //Serial.println(audioBuffer[j-2]);
      j=0;            
      i2s_write(I2S_NUM_0, audioBuffer, sizeof(audioBuffer), &bytesWritten, portMAX_DELAY);

      delay(1);
      yield();        
    }

    if((cargarsonido[0])||(cargarsonido[1])||(cargarsonido[2])){ 
       Serial.println("Se sale de reproduce para cargar sonido");
       break; 
    }
    
  }

 // Serial.println("Sale de reproducir");
  
}


void logoenpantalla(){
  display.clearDisplay();
  display.drawBitmap(0,0,logo,128,64, WHITE);  
  display.display();     
}

String recortatexto(String texto){

  String texto2=texto.substring(0,18);

  return texto2;
  
}

void drawCentreString(String texto, int x, int y,int tam)
{
    int str_len = texto.length() + 1; 
    char buf[str_len];
    texto.toCharArray(buf,str_len);

    int16_t x1, y1;
    uint16_t w, h;

    display.setTextSize(tam);
    display.getTextBounds(buf,0,0, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor(x - w / 2, y);

   // Serial.print("X1 : ");
   // Serial.println(x1);
   // Serial.print("W : ");
   // Serial.println(w);
    display.print(buf);
}

void menupantalla(){
  
  menuplaying=false;
  display.clearDisplay();
  
  
  if(menu==0){
    display.drawBitmap(39,0,nota,50,50, WHITE); 
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    String texto=recortatexto(archivocargado);
    drawCentreString(texto,64,52,1);
   
  }else if(menu==1){
    display.drawBitmap(39,0,colores,50,50, WHITE);  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    String texto;
    if(color==0){ texto=recortatexto("AZUL");}
    else if(color==1){ texto=recortatexto("ROJO");}
    else if(color==2){ texto=recortatexto("VERDE");}
    else if(color==3){ texto=recortatexto("NARANJA");}
    else if(color==4){ texto=recortatexto("ROSA");}
    else if(color==5){ texto=recortatexto("BLANCO");}
    else if(color==6){ texto=recortatexto("MULTICOLOR");}
    
    drawCentreString(texto,64,52,1);
    
  }else if(menu==2){
    display.drawBitmap(39,0,pitch,50,50, WHITE);  
    String texto="";
    texto+=frecuenciawav[botonseleccionado];    
    texto+=" Hz";
    drawCentreString(texto,64,52,1);    
  }else if(menu==3){
    display.drawBitmap(39,0,altavoz,50,50, WHITE);  
    String texto="";
    texto+=(volumen*100);    
    texto+=" %";
    drawCentreString(texto,64,52,1);    
  }else if(menu==4){
    display.drawBitmap(39,0,sensorhall,50,50, WHITE);  
    
    String texto="ON";
    if(!nivelteclas){ texto="OFF"; }
    drawCentreString(texto,64,52,1);    
  }else if(menu==5){
    display.drawBitmap(39,0,chorus,50,50, WHITE);  
    
    String texto="ON";
    if(!efectochorus){ texto="OFF"; }
    drawCentreString(texto,64,52,1);    
  }
     
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(4,2);
  display.println((botonseleccionado+1));
  
  
  display.drawBitmap(10,29,triangulo1,10,10, WHITE);  
  display.drawBitmap(107,29,triangulo2,10,10, WHITE);  
  
  
  display.display();     
  tiempomenu=millis();
}


void ponerenpantalla(String texto){

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  // Display static text
  display.println(texto);
  display.display(); 

}

void definecolor(int colordefinido){

  if(colordefinido==0){ rcolor=0;gcolor=0;bcolor=200;}
  else if(colordefinido==1){ rcolor=200;gcolor=0;bcolor=0;}
  else if(colordefinido==2){ rcolor=0;gcolor=200;bcolor=0;}
  else if(colordefinido==3){ rcolor=200;gcolor=100;bcolor=50;}
  else if(colordefinido==4){ rcolor=200;gcolor=50;bcolor=50;}
  else if(colordefinido==5){ rcolor=50;gcolor=50;bcolor=50;}
  
}

void compruebaluces(){
  
  pixels.begin();
  delay(5);
  for(int i=0;i<69;i++){
    if(color==6){
          int c=random(0,5);
          definecolor(c);                    
    }
    pixels.setPixelColor(i,pixels.Color(rcolor,gcolor,bcolor));   
    delay(5);
    pixels.show();
  }    

  for(int i=68;i>=0;i--){
    pixels.setPixelColor(i,pixels.Color(0,0,0));      
    delay(5);
    pixels.show(); 
  }
  
}


void setup() {

  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);
  Wire.setClock(400000);

   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  
  //display.ssd1306_command(0x81); 
  //display.ssd1306_command(2);
  
  logoenpantalla();
    
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
  pinMode(22,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(15,OUTPUT);

  pinMode(36,INPUT_PULLUP);
  pinMode(39,INPUT_PULLUP);
  pinMode(34,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);
  pinMode(32,INPUT_PULLUP);
  pinMode(33,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);
  pinMode(15,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  
  analogReadResolution(8);
  
  compruebaluces();
  
  Serial.println("PIANO TURMANDREAMS");
  
  
  
  // Configurar la configuración del bus I2S  PCM5102
  
  i2sConfig.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  i2sConfig.sample_rate = 44000;
  i2sConfig.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2sConfig.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2sConfig.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB);
  i2sConfig.intr_alloc_flags = 0;
  i2sConfig.dma_buf_count = 8;
  i2sConfig.dma_buf_len = 256;
  i2sConfig.use_apll = false;
  i2sConfig.tx_desc_auto_clear = true;
  i2sConfig.fixed_mclk = 0;
  //i2sConfig.clkm_div_num = 1;

  // Inicializar el bus I2S con la configuración
  i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, NULL);

  // Configurar el pinout del bus I2S
  i2s_pin_config_t pinConfig;
  pinConfig.bck_io_num = 26;    // Pin de reloj de bit
  pinConfig.ws_io_num = 27;     // Pin de selección de palabra
  pinConfig.data_out_num = 25;  // Pin de datos de salida
  pinConfig.data_in_num = -1;   // No se utiliza la entrada de datos
  i2s_set_pin(I2S_NUM_0, &pinConfig);

  boolean sigue=true;
  while(sigue){
    Serial.println("Se va a leer la SD");
    if(SD.begin(2)){
      //Serial.println("Se abre la carpeta root");    
      root = SD.open("/");
      sigue=false;
    }else{
      sigue=true;    
    }
  }
  
  printDirectory();

  //Serial.println("Comenzamos las pulsaciones");

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */ 

  for(int i=0;i<3;i++){ sonidorelleno[i]=false;cargarsonido[i]=false; }

  cargarsonido[botonseleccionado]=true;

  tiempomenu=millis();

  menupantalla(); 
  
}


void Task1code( void * pvParameters ){    // en este Core recogemos las peticiones web

    
  for(;;){   
    
    if((millis()-tiempomenu)>200){ pulsacionbotones();}
    recogeteclas();       
    luces(); 
        
    esp_task_wdt_reset();      
    delay(5);    
  } 
}


void loop() {
    
    actualizasonidos();    
    reproduce();
    esp_task_wdt_reset();         
  
}
