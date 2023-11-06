#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_task_wdt.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "imagenes.h"
#include "sonidos.h"


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
int posarchivos[4];

boolean pulsado=false;

// Configuración del bus I2S PCM5102
i2s_config_t i2sConfig;

// Buffer de salida de audio
int16_t audioBuffer[512];
size_t bytesWritten;

int16_t ultimovalor=0;

float volumen=0.4;

int limitepulsacion=5;


int sonidocanal[4];
boolean sonidocargado[3];

byte sonido[3][25000];
int tamsonido[4];

float frecuenciasonido[4][39];
float possonido[4][39];

float frecuenciawav[4];
boolean cargarsonido[4];

boolean play=false;

int instante;

int canalseleccionado=0;

boolean teclas[4][39];
int tiempoteclas[4][39];

byte valorteclas[4][39];

float volumenteclas[4][39];

int notasenteclas[4][39];
byte musicanotas[4][250];
uint16_t musicatiempoinicio[4][250];
uint16_t musicatiempofin[4][250];

int musicaposicion[4];
int tiempomusica[4];
int musicatam[4];

int musicadelay[4];

int tiempoinicio=0;

int menu=0;
int tiempomenu;

String archivocargado[4];
int color[4];
byte rcolor[4];
byte gcolor[4];
byte bcolor[4];

int colordefondo=0;

boolean nivelteclas[4];
float efectochorus[4];

boolean grabando=false;
boolean playing[4];

boolean menuplaying=false;

void cargasonido(int numsonido){

  if(posarchivos[numsonido]==0){ 
    
      archivocargado[numsonido]="None";   
      if(sonidocanal[numsonido]!=-1){  sonidocargado[sonidocanal[numsonido]]=false; }  
     // if(sonidocanal[numsonido]!=-1){ Serial.print("SonidoCanal ");Serial.print(sonidocanal[numsonido]);Serial.println(" Libre !!!"); }
      sonidocanal[numsonido]=-1;

      
  }else if(posarchivos[numsonido]==1){ 
    
      archivocargado[numsonido]="PIANO";       
      frecuenciawav[numsonido]=523.25;

      if(sonidocanal[numsonido]!=-1){  sonidocargado[sonidocanal[numsonido]]=false; }  
    //  if(sonidocanal[numsonido]!=-1){ Serial.print("SonidoCanal ");Serial.print(sonidocanal[numsonido]);Serial.println(" Libre !!!"); }
      sonidocanal[numsonido]=-1;

      
      for(int i=0;i<39;i++){   possonido[numsonido][i]=pianotam;  }
      
  }else {  //Se lee tarjeta SD y se mete en sonido[][]
      
    archivocargado[numsonido]=archivos[posarchivos[numsonido]-2];    
    
    File file2 = SD.open(archivocargado[numsonido],"r");  
  
    if (!file2) { Serial.println("Failed to open file for reading"); return; }  
  
    byte header[44];file2.read(header,44);

    if(sonidocanal[numsonido]==-1){      
    
      if(!sonidocargado[0]){ sonidocanal[numsonido]=0;sonidocargado[0]=true; }
      else if(!sonidocargado[1]){ sonidocanal[numsonido]=1;sonidocargado[1]=true; }
      else if(!sonidocargado[2]){ sonidocanal[numsonido]=2;sonidocargado[2]=true; }
      else{ 
        archivocargado[numsonido]="None";   
        if(sonidocanal[numsonido]!=-1){  sonidocargado[sonidocanal[numsonido]]=false; }  
        sonidocanal[numsonido]=-1;
        posarchivos[numsonido]=0;
        menupantalla();
        
        return; 
      }

    }
    
   // Serial.print("SonidoCanal ");Serial.print(sonidocanal[numsonido]);Serial.println(" Ocupado !!!");
      
    tamsonido[numsonido]=file2.read(sonido[sonidocanal[numsonido]],25000);

    for(int i=0;i<39;i++){
        possonido[numsonido][i]=tamsonido[numsonido];
    }
             
    frecuenciawav[numsonido]=523.25;
    
    file2.close();          

  }
  
  menupantalla();
  
}

void teclasluces(int i,boolean estado){
    if(teclas[i][0]){  led(i,0,estado);  }
    if(teclas[i][2]){  led(i,1,estado);  }
    if(teclas[i][4]){  led(i,2,estado);  }
    if(teclas[i][5]){  led(i,3,estado);  }
    if(teclas[i][7]){  led(i,4,estado);  }
    if(teclas[i][9]){  led(i,5,estado);  }
    if(teclas[i][11]){ led(i,6,estado);  }
    
    if(teclas[i][12]){  led(i,7,estado);  }
    if(teclas[i][14]){  led(i,8,estado);  }
    if(teclas[i][16]){  led(i,9,estado);  }
    if(teclas[i][17]){  led(i,10,estado);  }
    if(teclas[i][19]){  led(i,11,estado);  }
    if(teclas[i][21]){  led(i,12,estado);  }
    if(teclas[i][23]){  led(i,13,estado);  }
    
    if(teclas[i][24]){  led(i,14,estado);  }
    if(teclas[i][26]){  led(i,15,estado);  }
    if(teclas[i][28]){  led(i,16,estado);  }
    if(teclas[i][29]){  led(i,17,estado);  }
    if(teclas[i][31]){  led(i,18,estado);  }
    if(teclas[i][33]){  led(i,19,estado);  }
    if(teclas[i][35]){  led(i,20,estado);  }
    
    if(teclas[i][36]){  led(i,21,estado);  }
    if(teclas[i][38]){  led(i,22,estado);  }
  
}

void luces(){

  for(int i=0;i<23;i++){ ledfondo(i); }

  for(int i=0;i<4;i++){  teclasluces(i,true);}
  teclasluces(canalseleccionado,true);
  
  pixels.show();
  
}

void ledfondo(int i){

    byte r,g,b;
    
    if(colordefondo==0) { r=0;g=0;b=0;}
    else if(colordefondo==1){ r=0;g=0;b=200;}
    else if(colordefondo==2){ r=200;g=0;b=0;}
    else if(colordefondo==3){ r=0;g=200;b=0;}
    else if(colordefondo==4){ r=200;g=100;b=50;}
    else if(colordefondo==5){ r=200;g=20;b=20;}
    else if(colordefondo==6){ r=50;g=50;b=50;}  
    else if(colordefondo==7){          

        if(i==0){ r=0;g=0;b=200;}
        else if(i==1){ r=200;g=0;b=0;}
        else if(i==2){ r=0;g=200;b=0;}
        else if(i==3){ r=200;g=100;b=50;}
        else if(i==4){ r=200;g=20;b=20;}
        else if(i==5){ r=0;g=0;b=200;}
        else if(i==6){ r=200;g=0;b=0;}
        else if(i==7){ r=0;g=200;b=0;}
        else if(i==8){ r=200;g=100;b=50;}
        else if(i==9){ r=200;g=20;b=20;}
        else if(i==10){ r=0;g=0;b=200;}
        else if(i==11){ r=200;g=0;b=0;}
        else if(i==12){ r=0;g=200;b=0;}
        else if(i==13){ r=200;g=100;b=50;}
        else if(i==14){ r=200;g=20;b=20;}
        else if(i==15){ r=0;g=0;b=200;}
        else if(i==16){ r=200;g=0;b=0;}
        else if(i==17){ r=0;g=200;b=0;}
        else if(i==18){ r=200;g=100;b=50;}
        else if(i==19){ r=200;g=20;b=20;}
        else if(i==20){ r=0;g=0;b=200;}
        else if(i==21){ r=200;g=0;b=0;}
        else if(i==22){ r=0;g=200;b=0;}
      
  
    }
    
    pixels.setPixelColor(i,pixels.Color(r,g,b));  
    pixels.setPixelColor(45-i,pixels.Color(r,g,b));  
    pixels.setPixelColor(i+46,pixels.Color(r,g,b));  
    
}

void led(int j,int i,boolean encendido){

    byte r,g,b;
    if(!encendido){ r=0;g=0;b=0;}
    else{
        
        if(color[j]==6){
            if(i==0){ r=0;g=0;b=200;}
            else if(i==1){ r=200;g=0;b=0;}
            else if(i==2){ r=0;g=200;b=0;}
            else if(i==3){ r=200;g=100;b=50;}
            else if(i==4){ r=200;g=20;b=20;}
            else if(i==5){ r=0;g=0;b=200;}
            else if(i==6){ r=200;g=0;b=0;}
            else if(i==7){ r=0;g=200;b=0;}
            else if(i==8){ r=200;g=100;b=50;}
            else if(i==9){ r=200;g=20;b=20;}
            else if(i==10){ r=0;g=0;b=200;}
            else if(i==11){ r=200;g=0;b=0;}
            else if(i==12){ r=0;g=200;b=0;}
            else if(i==13){ r=200;g=100;b=50;}
            else if(i==14){ r=200;g=20;b=20;}
            else if(i==15){ r=0;g=0;b=200;}
            else if(i==16){ r=200;g=0;b=0;}
            else if(i==17){ r=0;g=200;b=0;}
            else if(i==18){ r=200;g=100;b=50;}
            else if(i==19){ r=200;g=20;b=20;}
            else if(i==20){ r=0;g=0;b=200;}
            else if(i==21){ r=200;g=0;b=0;}
            else if(i==22){ r=0;g=200;b=0;}                 
        }else{
            r=rcolor[j];
            g=gcolor[j];
            b=bcolor[j];                                    
        }
    }
    
    pixels.setPixelColor(i,pixels.Color(r,g,b));  
    pixels.setPixelColor(45-i,pixels.Color(r,g,b));  
    pixels.setPixelColor(i+46,pixels.Color(r,g,b));  
    
}
  


void pintaplaying(){

  display.clearDisplay();
  String texto="Playing ";
  texto+=(canalseleccionado+1);
  drawCentreString(texto,64,12,2); 
  texto="Ret:";texto+=musicadelay[canalseleccionado];texto+="ms";
  drawCentreString(texto,64,42,2); 
  display.display();
  menuplaying=true;
}

void recogeteclas(){

  byte numeros[4][39];


  //////////////////////////////////////////////////////////////////
  //////Se Recogen los valores que nos envian los ESP32 esclavos.///
  //////////////////////////////////////////////////////////////////
  for(int j=0;j<4;j++){
    for(int i=0;i<39;i++){ 
      numeros[j][i]=0; 
    }
  }
  
  //Serial.println("Miramos teclas");
  
  Wire.requestFrom(I2C_SLAVE_ADDR1,15);
  int contador=0;
  
  
  int instante=millis();
  while(contador<15){
    if(Wire.available()>0){
      numeros[canalseleccionado][contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);

  Wire.requestFrom(I2C_SLAVE_ADDR2,13);
  instante=millis();
  while(contador<28){
    if(Wire.available()>0){
      numeros[canalseleccionado][contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);
  
  Wire.requestFrom(I2C_SLAVE_ADDR3,11);
  instante=millis();
  while(contador<39){
    if(Wire.available()>0){
      numeros[canalseleccionado][contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);

  /////////////////////////////////



  for(int j=0;j<4;j++){

  
      //Comprobamos si play y se tocan las notas solas
      if(playing[j]){
    
         ///Dentro del las teclas pulsadas se comprueba si es el momento de soltarla 
        
        for(int i=0;i<39;i++){
            if(notasenteclas[j][i]!=-1){
              if((millis()-tiempomusica[j])>(musicatiempofin[j][notasenteclas[j][i]]+musicadelay[j])){
                  numeros[j][i]=0;                           
                  notasenteclas[j][i]=-1;
                  //String texto="Tecla ";texto+=i;texto+=" OFF";Serial.println(texto);
                  //ponerenpantalla(texto);              
               
              }else{
                  numeros[j][i]=130;      
              }
            }    
        }
    
    
        //Se comprueba si es el momento de pulsar las teclas almacenadas
        
        if(musicaposicion[j]<musicatam[j]){     
    
          if((millis()-tiempomusica[j])>(musicatiempoinicio[j][musicaposicion[j]]+musicadelay[j])){
            
              int tecla=musicanotas[j][musicaposicion[j]];
                
              //String texto="Tecla ";texto+=tecla;texto+=" ON ";Serial.println(texto);
              //ponerenpantalla(texto);
              
              valorteclas[j][tecla]=100;
              numeros[j][tecla]=130;  
              notasenteclas[j][tecla]=musicaposicion[j];
              musicaposicion[j]++;
                
          }
      
        }
    
        boolean termino=true;
        for(int i=0;i<39;i++){  if(notasenteclas[j][i]!=-1){ termino=false; } }
    
        if(termino){
            if(musicaposicion[j]>=musicatam[j]){ 
              tiempomusica[j]=millis();
              musicaposicion[j]=0;
            }       
        }
          
      
      }
    
      
        
      float f=130.81;
        
      for(byte i=0;i<39;i++){
    
        if(!teclas[j][i]){  //Tecla no pulsada   
          if((millis()-tiempoteclas[j][i])>50){         
            
            if(numeros[j][i]>120){
    
              if((grabando)&&(j==canalseleccionado)){
                  if(musicatam[canalseleccionado]==0){ tiempoinicio=millis();musicadelay[canalseleccionado]=0;}
                  notasenteclas[canalseleccionado][i]=musicatam[canalseleccionado];      
                  musicanotas[canalseleccionado][musicatam[canalseleccionado]]=i;
                  int tiempon=millis()-tiempoinicio;      
                  musicatiempoinicio[canalseleccionado][musicatam[canalseleccionado]]=tiempon;                
                  musicatam[canalseleccionado]++; 
              }
          
              int rampa=(((int)valorteclas[j][i])-((int)numeros[j][i]))/2;
              if(rampa>10){ rampa=10; }
              volumenteclas[j][i]=0.1*rampa;          
              
              frecuenciasonido[j][i]=f;
              possonido[j][i]=0; 
    
              if(efectochorus[j]!=0.0){
                if(i<38){frecuenciasonido[j][i+1]=(f+efectochorus[j]);possonido[j][i+1]=0; }
                if(i>0){frecuenciasonido[j][i-1]=(f-efectochorus[j]);possonido[j][i-1]=0; }                
              }
                        
              teclas[j][i]=true; 
              tiempoteclas[j][i]=millis();
            }  
          }          
        }else{  //Tecla no pulsada
          if((millis()-tiempoteclas[j][i])>50){
            
            if(numeros[j][i]<120){
              if(posarchivos[j]==1){possonido[j][i]=pianotam; }
              else{ possonido[j][i]=tamsonido[j]; }
              
              teclas[j][i]=false;  
              
              if((grabando)&&(j==canalseleccionado)){                         
                  int tiempon=millis()-tiempoinicio;      
                  musicatiempofin[j][notasenteclas[j][i]]=tiempon;                                          
              }
              
              tiempoteclas[j][i]=millis();      
            }  
          }    
        }
           
        f=f*1.06;    
        
      }
    
      for(byte i=0;i<39;i++){ valorteclas[j][i]=numeros[j][i]; }

  
  }
  
}

void actualizacolor(){

  definecolor(canalseleccionado,color[canalseleccionado]);

  menupantalla();

  compruebaluces();
  
}

boolean pulsacionbotones(){

  int valor=analogRead(36); //Boton Derecha
  //Serial.println(valor);
  if(valor<255){ 
    menu++;
    if(menu>6){ menu=0;}
    menupantalla();
    
  }
  
  valor=analogRead(39); //Boton Izquierda
  if(valor<255){ 
    menu--;
    if(menu<0){ menu=6;}
    menupantalla();    
  }

  
  valor=analogRead(34);if(valor<255){  //Boton Abajo
     if(menuplaying){
        musicadelay[canalseleccionado]-=10;
        if(musicadelay[canalseleccionado]<0){musicadelay[canalseleccionado]=0;}
        pintaplaying();
     }else{
      
       if(menu==0){
          posarchivos[canalseleccionado]--;
          if(posarchivos[canalseleccionado]<0){ posarchivos[canalseleccionado]=numarchivos+1; }
          cargarsonido[canalseleccionado]=true;
          delay(200);
       }else if(menu==1){
          color[canalseleccionado]--;
          if(color[canalseleccionado]<0){ color[canalseleccionado]=6; }
          actualizacolor();
       }else if(menu==2){
          colordefondo--;
          if(colordefondo<0){ colordefondo=7; }          
          menupantalla();                    
       }else if(menu==3){        
          frecuenciawav[canalseleccionado]*=1.06;
          menupantalla();  
       }else if(menu==4){        
          volumen-=0.05;
          if(volumen<0.05){ volumen=0.05;}
          menupantalla();  
       }else if(menu==5){        
          nivelteclas[canalseleccionado]=!nivelteclas[canalseleccionado];
          menupantalla();  
       }else if(menu==6){        
          efectochorus[canalseleccionado]-=0.1;
          if(efectochorus[canalseleccionado]<=0.0){ efectochorus[canalseleccionado]=0.0;}
          menupantalla();  
       }       
     }
     
  }
  
  valor=analogRead(35);if(valor<255){ //Boton Arriba

     if(menuplaying){
        musicadelay[canalseleccionado]+=10;
        if(musicadelay[canalseleccionado]>5000){musicadelay[canalseleccionado]=5000;}
        pintaplaying();
     }else{
       if(menu==0){
          posarchivos[canalseleccionado]++;
          if(posarchivos[canalseleccionado]>=numarchivos+2){ posarchivos[canalseleccionado]=0; }
          cargarsonido[canalseleccionado]=true;  
          delay(200);      
       }else if(menu==1){
          color[canalseleccionado]++;
          if(color[canalseleccionado]>6){ color[canalseleccionado]=0; }
          actualizacolor();
       }else if(menu==2){
          colordefondo++;
          if(colordefondo>7){ colordefondo=0; }   
          menupantalla();          
       }else if(menu==3){
          frecuenciawav[canalseleccionado]/=1.06;
          menupantalla();  
       }else if(menu==4){        
          volumen+=0.05;
          if(volumen>1.0){ volumen=1.0;}
          menupantalla();  
       }else if(menu==5){        
          nivelteclas[canalseleccionado]=!nivelteclas[canalseleccionado];
          menupantalla();  
       }else if(menu==6){        
          efectochorus[canalseleccionado]+=0.1;
          if(efectochorus[canalseleccionado]>10.0){ efectochorus[canalseleccionado]=10.0;}
          menupantalla();  
       }
       
     }
  }
  
  valor=analogRead(32);if(valor<255){ ponerenpantalla("Boton 5"); }
  
    
    
  if(digitalRead(33)==LOW){ //Chanel --
      canalseleccionado--;
      if(canalseleccionado<0){ canalseleccionado=3; }
      menu=0;
      menupantalla();
  }
  
  if(digitalRead(13)==LOW){ //Chanel ++
      canalseleccionado++;
      if(canalseleccionado>=4){ canalseleccionado=0; }
      menu=0;
      menupantalla();
  }
  
  if(digitalRead(15)==LOW){ //Play y Stop Notas guardadas
      if(!grabando){
        delay(500);
        playing[canalseleccionado]=!playing[canalseleccionado];
        if(playing[canalseleccionado]){

            pintaplaying();
            
            tiempomusica[canalseleccionado]=millis();
            musicaposicion[canalseleccionado]=0;
            for(int i=0;i<39;i++){ notasenteclas[canalseleccionado][i]=-1; }
            
            if(musicatam[canalseleccionado]==0){
                playing[canalseleccionado]=false;
                menupantalla();  
            }
        }else{
            menuplaying=false;
            menupantalla();  
        }
      }
      
  }

  
  if(digitalRead(4)==LOW){  ///Boton de Grabacion
      if(!playing[canalseleccionado]){
        if(!grabando){          
            display.clearDisplay();
            String texto="Record ";
            texto+=(canalseleccionado+1);
            drawCentreString(texto,64,32,2); 
            display.display();           
            musicatam[canalseleccionado]=0;
            /*
            for(int i=0;i<1000;i++){
                  musicatiempoinicio[i]=0;
                  musicatiempofin[i]=0;
            }  
            */                
            grabando=true;
            
        }else{          
            grabando=false;               
            menupantalla(); 
        }
        delay(500); 
      }
    
  }
      

  return pulsado;
    
}



void leeficheros() {
  
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
  for(int i=0;i<4;i++){
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
    
    for(int i=0;i<4;i++){
      
      if(posarchivos[i]!=0){  

        for(int jj=0;jj<39;jj++){

          float tam=tamsonido[i]-50;
          if(posarchivos[i]==1){ tam=pianotam; }
            
          if(possonido[i][jj]<tam){
  
            consonido=true;
            
            float vel=frecuenciasonido[i][jj]/frecuenciawav[i];    
            vel=vel*2;
    
            int ii=(int)possonido[i][jj];
            if((ii%2)!=0){ ii++; }
            
            int16_t dato1;   
            if(posarchivos[i]==1){ dato1 = piano[ii+1]<<8 | piano[ii]; }
            else { dato1 = sonido[sonidocanal[i]][ii+1]<<8 | sonido[i][ii]; }

            int16_t dato2;      
            if(posarchivos[i]==1){ dato2 = piano[ii+3]<<8 | piano[ii+2]; }
            else { dato2 = sonido[sonidocanal[i]][ii+3]<<8 | sonido[i][ii+2]; }

            float dat1 = (float)dato1;
            float dat2 = (float)dato2;

            float p=possonido[i][jj]-ii;

            float dat=(dat1*(1-p))+(dat2*p);
            
            if(!nivelteclas[i]){ volumenteclas[i][jj]=0.5;}
            
            dat=dat*volumenteclas[i][jj];
  
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

    if((cargarsonido[0])||(cargarsonido[1])||(cargarsonido[2])||(cargarsonido[3])){ 
       //Serial.println("Se sale de reproduce para cargar sonido");
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
    String texto=recortatexto(archivocargado[canalseleccionado]);
    drawCentreString(texto,64,52,1);
   
  }else if(menu==1){
    display.drawBitmap(39,0,colores,50,50, WHITE);  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    String texto;
    if(color[canalseleccionado]==0){ texto=recortatexto("AZUL");}
    else if(color[canalseleccionado]==1){ texto=recortatexto("ROJO");}
    else if(color[canalseleccionado]==2){ texto=recortatexto("VERDE");}
    else if(color[canalseleccionado]==3){ texto=recortatexto("NARANJA");}
    else if(color[canalseleccionado]==4){ texto=recortatexto("ROSA");}
    else if(color[canalseleccionado]==5){ texto=recortatexto("BLANCO");}
    else if(color[canalseleccionado]==6){ texto=recortatexto("MULTICOLOR");}
    
    drawCentreString(texto,64,52,1);
    
  }else if(menu==2){
    display.drawBitmap(39,0,colorfondo,50,50, WHITE);  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    String texto;
    if(colordefondo==0){ texto=recortatexto("NEGRO");}
    else if(colordefondo==1){ texto=recortatexto("AZUL");}
    else if(colordefondo==2){ texto=recortatexto("ROJO");}
    else if(colordefondo==3){ texto=recortatexto("VERDE");}
    else if(colordefondo==4){ texto=recortatexto("NARANJA");}
    else if(colordefondo==5){ texto=recortatexto("ROSA");}
    else if(colordefondo==6){ texto=recortatexto("BLANCO");}
    else if(colordefondo==7){ texto=recortatexto("MULTICOLOR");}
    
    drawCentreString(texto,64,52,1);
    
  }else if(menu==3){
    display.drawBitmap(39,0,pitch,50,50, WHITE);  
    String texto="";
    texto+=frecuenciawav[canalseleccionado];    
    texto+=" Hz";
    drawCentreString(texto,64,52,1);    
  }else if(menu==4){
    display.drawBitmap(39,0,altavoz,50,50, WHITE);  
    String texto="";
    texto+=(volumen*100);    
    texto+=" %";
    drawCentreString(texto,64,52,1);    
  }else if(menu==5){
    display.drawBitmap(39,0,sensorhall,50,50, WHITE);  
    String texto="ON";
    if(!nivelteclas[canalseleccionado]){ texto="OFF"; }
    drawCentreString(texto,64,52,1);    
  }else if(menu==6){
    display.drawBitmap(39,0,chorus,50,50, WHITE);  
    
    String texto="";
    texto+=efectochorus[canalseleccionado];
    if(efectochorus[canalseleccionado]<0.1){ texto="OFF"; }
    drawCentreString(texto,64,52,1);    
  }
     
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(4,2);
  display.println((canalseleccionado+1));
  
  
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

void definecolor(int j,int colordefinido){

  if(colordefinido==0){ rcolor[j]=0;gcolor[j]=0;bcolor[j]=200;}
  else if(colordefinido==1){ rcolor[j]=200;gcolor[j]=0;bcolor[j]=0;}
  else if(colordefinido==2){ rcolor[j]=0;gcolor[j]=200;bcolor[j]=0;}
  else if(colordefinido==3){ rcolor[j]=200;gcolor[j]=100;bcolor[j]=50;}
  else if(colordefinido==4){ rcolor[j]=200;gcolor[j]=20;bcolor[j]=20;}
  else if(colordefinido==5){ rcolor[j]=50;gcolor[j]=50;bcolor[j]=50;}
  
}

void compruebaluces(){
  
  pixels.begin();
  delay(5);
  for(int i=0;i<69;i++){
    if(color[canalseleccionado]==6){
          int c=random(0,5);
          definecolor(canalseleccionado,c);                    
    }
    pixels.setPixelColor(i,pixels.Color(rcolor[canalseleccionado],gcolor[canalseleccionado],bcolor[canalseleccionado]));   
    delay(5);
    pixels.show();
  }    

  for(int i=68;i>=0;i--){
    pixels.setPixelColor(i,pixels.Color(0,0,0));      
    delay(5);
    pixels.show(); 
  }
  
}

void inicializavariables(){
  
   for(int i=0;i<3;i++){ sonidocargado[i]=false; }
   
   for(int i=0;i<4;i++){     
      sonidocanal[i]=-1; 
      cargarsonido[i]=false; 
      posarchivos[i]=0;
      nivelteclas[i]=false;
      efectochorus[i]=0.0;   
      archivocargado[i]="None"; 
      musicatam[i]=0;
      musicaposicion[i]=0;

      if(i==0){  color[i]=0;rcolor[i]=0;gcolor[i]=0;bcolor[i]=200; }
      else if(i==1){  color[i]=1;rcolor[i]=200;gcolor[i]=0;bcolor[i]=0; }
      else if(i==2){  color[i]=2;rcolor[i]=0;gcolor[i]=200;bcolor[i]=0; }
      else if(i==3){  color[i]=4;rcolor[i]=200;gcolor[i]=20;bcolor[i]=20; }

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

  int tiemposd=millis();
  
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
    if((millis()-tiemposd)>10000){ break;}
  }
  
  leeficheros();

  //Serial.println("Comenzamos las pulsaciones");

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */ 


  inicializavariables();
 
  tiempomenu=millis();

  cargasonido(canalseleccionado);
  
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
