#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_task_wdt.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR1 0x04
#define I2C_SLAVE_ADDR2 0x05
#define I2C_SLAVE_ADDR3 0x06

#define PIN 12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(69, PIN, NEO_GRB + NEO_KHZ800);


//GPIO's
//#define BCK  25
//#define LRC  27
//#define DOU  26


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

boolean botones[14];
int valorbotones[14];
int tiempobotones[14];

int tiempob;
boolean pulsado=false;

// Configuración del bus I2S PCM5102
i2s_config_t i2sConfig;

// Buffer de salida de audio
int16_t audioBuffer[512];
size_t bytesWritten;

int16_t ultimovalor=0;

float volumen=0.4;

int limitepulsacion=5;

byte sonido[4][25000];
int tamsonido[4];

float frecuenciasonido[39];
float possonido[39];

boolean sonidorelleno[4];
boolean cargarsonido[4];
int loopsonido[4];
int looptiempo[4];

boolean play=false;

int instante;

int botonseleccionado=0;

int tiempoluces;
int tiempo;

boolean teclas[39];
int tiempoteclas[39];


void cargasonido(int numsonido){

  String nombrearchivo=archivos[posarchivos];

 // Serial.print("Se va abrir el archivo : ");
 // Serial.println(nombrearchivo);    
    
  File file2 = SD.open(nombrearchivo,"r");  
  
  posarchivos++;
  if(posarchivos>=numarchivos){ posarchivos=0; }

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

  frecuenciasonido[numsonido]=261.63;
  loopsonido[numsonido]=0;
  possonido[numsonido]=0;
  
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

  if(teclas[0]){  led(0,0,0,200);  }else {  led(0,0,0,0);  }
  if(teclas[2]){  led(1,0,0,200);  }else {  led(1,0,0,0);  }
  if(teclas[4]){  led(2,0,0,200);  }else {  led(2,0,0,0);  }
  if(teclas[5]){  led(3,0,0,200);  }else {  led(3,0,0,0);  }
  if(teclas[7]){  led(4,0,0,200);  }else {  led(4,0,0,0);  }
  if(teclas[9]){  led(5,0,0,200);  }else {  led(5,0,0,0);  }
  if(teclas[11]){ led(6,0,0,200);  }else {  led(6,0,0,0);  }
  
  if(teclas[12]){  led(7,0,0,200);  }else {  led(7,0,0,0);  }
  if(teclas[14]){  led(8,0,0,200);  }else {  led(8,0,0,0);  }
  if(teclas[16]){  led(9,0,0,200);  }else {  led(9,0,0,0);  }
  if(teclas[17]){  led(10,0,0,200);  }else {  led(10,0,0,0);  }
  if(teclas[19]){  led(11,0,0,200);  }else {  led(11,0,0,0);  }
  if(teclas[21]){  led(12,0,0,200);  }else {  led(12,0,0,0);  }
  if(teclas[23]){  led(13,0,0,200);  }else {  led(13,0,0,0);  }
  
  if(teclas[24]){  led(14,0,0,200);  }else {  led(14,0,0,0);  }
  if(teclas[26]){  led(15,0,0,200);  }else {  led(15,0,0,0);  }
  if(teclas[28]){  led(16,0,0,200);  }else {  led(16,0,0,0);  }
  if(teclas[29]){  led(17,0,0,200);  }else {  led(17,0,0,0);  }
  if(teclas[31]){  led(18,0,0,200);  }else {  led(18,0,0,0);  }
  if(teclas[33]){  led(19,0,0,200);  }else {  led(19,0,0,0);  }
  if(teclas[35]){  led(20,0,0,200);  }else {  led(20,0,0,0);  }
  
  if(teclas[36]){  led(21,0,0,200);  }else {  led(21,0,0,0);  }
  if(teclas[38]){  led(22,0,0,200);  }else {  led(22,0,0,0);  }
  
  pixels.show();
  
}

void led(int i,int r,int g,int b){
  
    pixels.setPixelColor(i,pixels.Color(r,g,b));  
    pixels.setPixelColor(45-i,pixels.Color(r,g,b));  
    pixels.setPixelColor(i+46,pixels.Color(r,g,b));  
    
}
  
  
void lectura(int puerto,int pos){

  if((millis()-tiempobotones[pos])<100){ botones[pos]=false; return; }      

  int valor=analogRead(puerto);

  //Serial.println(valor);

  if(valor>limitepulsacion){ 

    delayMicroseconds(100);
    valor=analogRead(puerto);
    
    if(valor>limitepulsacion){ 

      botones[pos]=true;
      pulsado=true;    
      valorbotones[pos]=valor; 

      tiempobotones[pos]=instante; 
      
      /*
      Serial.print("Boton ");
      Serial.print(pos);      
      Serial.print(" : ");
      Serial.println(valor);
      */
    }else{     
      botones[pos]=false;            
    }
    
  }else{     
    botones[pos]=false;         
  }
  
    
}

void revisaloop(){
  int instanteloop=millis();
  
  for(int i=0;i<4;i++){
    if((sonidorelleno[i])&&(loopsonido[i]!=0)){  
      if((instanteloop-looptiempo[i])>loopsonido[i]){
        //Serial.println("Se pulsa boton por loop");
        possonido[i]=0;    
        looptiempo[i]=instanteloop;    
      }         
    }    
  } 
  
}

void recogeteclas(){
    
  //Serial.println("Miramos teclas");
   
  Wire.requestFrom(I2C_SLAVE_ADDR1,15);
  int contador=0;
  byte numeros[39];
  
  int instante=millis();
  while(contador<15){
    if(Wire.available()>0){
      numeros[contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);

  Wire.requestFrom(I2C_SLAVE_ADDR2,15);
  instante=millis();
  while(contador<30){
    if(Wire.available()>0){
      numeros[contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);
  
  Wire.requestFrom(I2C_SLAVE_ADDR3,9);
  instante=millis();
  while(contador<39){
    if(Wire.available()>0){
      numeros[contador] = Wire.read();      
      contador++;
    } 
    if((millis()-instante)>1000) { return; }   
  }
  delay(1);
  
  /*
  //Serial.print("Esclavo 1 :");
  for(byte i=0;i<15;i++){
      //Serial.print(numeros[i]);
      //Serial.print(",");      
      delay(1);
  }
  //Serial.println("");

  //Serial.print("Esclavo 2 :");
  for(byte i=15;i<30;i++){
      //Serial.print(numeros[i]);
      //Serial.print(",");      
      delay(1);
  }
  //Serial.println("");

  //Serial.print("Esclavo 3 :");
  for(byte i=30;i<39;i++){
      //Serial.print(numeros[i]);
      //Serial.print(",");      
      delay(1);
  }
  //Serial.println("");

  */
  
  float f=130.61;
    
  for(byte i=0;i<39;i++){

    if(!teclas[i]){  //Tecla no pulsada   
      if((millis()-tiempoteclas[i])>50){         
        //if(numeros[i]>80){
        if(numeros[i]>120){
          frecuenciasonido[i]=f;
          possonido[i]=0; 
          teclas[i]=true; 
          tiempoteclas[i]=millis();
        }  
      }          
    }else{  //Tecla pulsada
      if((millis()-tiempoteclas[i])>50){
        //if(numeros[i]<180){
        if(numeros[i]<120){
          possonido[i]=tamsonido[botonseleccionado];          
          teclas[i]=false;  
          tiempoteclas[i]=millis();      
        }  
      }    
    }
       
    f=f*1.06;    
    
  }
        
}


boolean pulsacionbotones(){
  
    pulsado=false;
    instante=millis();
    
    digitalWrite(22,HIGH);delay(2);
    lectura(39,0);
    lectura(34,1);
    lectura(35,2);
    lectura(36,3);
    digitalWrite(22,LOW);delay(2);
    
    digitalWrite(4,HIGH);delay(2);
    lectura(39,4);
    lectura(34,5);
    lectura(35,6);
    lectura(36,7);
    digitalWrite(4,LOW);delay(2);


    digitalWrite(13,HIGH);delay(2);
    lectura(39,8);
    lectura(34,9);
    lectura(35,10);
    lectura(36,11);
    digitalWrite(13,LOW);delay(2); 

    digitalWrite(15,HIGH);delay(2);
    lectura(36,12);
    lectura(34,13);
    digitalWrite(15,LOW);delay(2);
    

   /*
    
    for(int i=0;i<14;i++){
      if(botones[i]){
        Serial.print("Boton ");
        Serial.print(i);      
        Serial.print(" : ");
        Serial.println(valorbotones[i]);
      }      
    }
    
   */
     
    return pulsado;
    
}


void control(){

  if(botones[9]) { play=!play; delay(300);}

  if(botones[13]) { ///VOLUMEN +  
        volumen+=0.05;
        if(volumen>1.0){ volumen=1.0;}
  }else if(botones[12]) { ///VOLUMEN - 
        volumen-=0.05;
        if(volumen<0.05){ volumen=0.05;}
  }

  if(!play) { /// Se configuran los botones y las parametros del sonido

    if(botones[0]) {       botonseleccionado=0;possonido[0]=0;   }
    else if(botones[1]) {  botonseleccionado=1;possonido[1]=0;   }
    else if(botones[2]) {  botonseleccionado=2;possonido[2]=0;   }    
    else if(botones[3]) {  botonseleccionado=3;possonido[3]=0;  }  

    
    if(botones[11]) { 
      cargarsonido[botonseleccionado]=true; 
     // while(cargarsonido[botonseleccionado]){ delay(1); }      
    }    

    if(botones[10]) { ///PITCH +  de botonseleccionado
        frecuenciasonido[botonseleccionado]+=2.0;
        possonido[botonseleccionado]=0;
    }else if(botones[8]) { ///PITCH -  de botonseleccionado
        frecuenciasonido[botonseleccionado]-=2.0;
        possonido[botonseleccionado]=0;
    }  

    if(botones[5]) { ///LOOP +  de botonseleccionado

        looptiempo[botonseleccionado]=millis();
       
        if(loopsonido[botonseleccionado]==0){ loopsonido[botonseleccionado]=500; }
        else{ 
            loopsonido[botonseleccionado]-=50; 
            if(loopsonido[botonseleccionado]<100){ loopsonido[botonseleccionado]=0;}
        }
        
    }else if(botones[7]) { ///LOOP -  de botonseleccionado
      
        looptiempo[botonseleccionado]=millis();
      
        if(loopsonido[botonseleccionado]==0){ loopsonido[botonseleccionado]=500; }
        else{ 
            loopsonido[botonseleccionado]+=50; 
            if(loopsonido[botonseleccionado]>3000){ loopsonido[botonseleccionado]=0;}
        }
    }  

    
  }else{

    if(botones[0]) {  possonido[0]=0;  }
    if(botones[1]) {  possonido[1]=0;  }
    if(botones[2]) {  possonido[2]=0;  }    
    if(botones[3]) {  possonido[3]=0;  }  

  }
  
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

void compruebaplayer(){

  //Serial.println("Se comprueba player");
  
  for(int i=0;i<4;i++){
     if((sonidorelleno[i])&&(possonido[i]==0)) {       
        reproduce();         
     }    
  }

  if(ultimovalor!=0){ reproduce();}  
  
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

    consonido=false;

    int16_t valormuestra=0;
    
    for(int i=0;i<4;i++){
      
      if(sonidorelleno[i]){  

        for(int jj=0;jj<39;jj++){
                
          if(possonido[jj]<(tamsonido[i]-50)){
  
            consonido=true;
            
            float frecuenciawav=523.25;
      
            float vel=frecuenciasonido[jj]/frecuenciawav;    
            vel=vel*2;
    
            int ii=(int)possonido[jj];
            if((ii%2)!=0){ ii++; }
            
            int16_t dato1;      
            dato1 = sonido[i][ii+1]<<8 | sonido[i][ii];

            int16_t dato2;      
            dato2 = sonido[i][ii+3]<<8 | sonido[i][ii+2];

            float dat1 = ((float)dato1)*volumen;
            float dat2 = ((float)dato2)*volumen;

            float p=possonido[jj]-ii;

            float dat=(dat1*(1-p))+(dat2*p);
  
            valormuestra   +=  (int16_t)dat;
                       
            possonido[jj]+=vel;
            
          }
          
        }  
               
      }  
           
    }

    boolean sigue=true;
    int16_t dato2=valormuestra;
    
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
    
    audioBuffer[j] = dato2;
    audioBuffer[j+1] = dato2;
       
    

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
       Serial.println("Se sale de reproduce para cargar sonido");
       break; 
    }
    
  }

 // Serial.println("Sale de reproducir");
  
}




void setup() {

  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);
  Wire.setClock(400000);
    
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
  pinMode(22,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(15,OUTPUT);

  pinMode(34,INPUT);
  pinMode(35,INPUT);
  pinMode(36,INPUT);
  pinMode(39,INPUT);
 
  analogReadResolution(8);
  
  pixels.begin();

  delay(2000);

  tiempob=millis();
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
  pinConfig.bck_io_num = 27;    // Pin de reloj de bit
  pinConfig.ws_io_num = 14;     // Pin de selección de palabra
  pinConfig.data_out_num = 26;  // Pin de datos de salida
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

  for(int i=0;i<4;i++){ sonidorelleno[i]=false;cargarsonido[i]=false; }

  cargarsonido[botonseleccionado]=true;
  
}

void Task1code( void * pvParameters ){    // en este Core recogemos las peticiones web

    
  for(;;){   
    
    //pulsacionbotones();
    recogeteclas();
    revisaloop();
    //control();
    //if((millis()-tiempob)>5000){cargarsonido[botonseleccionado]=true;}
    luces(); 
        
    esp_task_wdt_reset();      
    delay(5);    
  } 
}


void loop() {
    
    actualizasonidos();    
    compruebaplayer();
    esp_task_wdt_reset();         
  
}
