#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h> 
#include "WiFi.h"
#include <PubSubClient.h>


#define CALIBRATION_FILE "/TouchCalData3"
#define REPEAT_CAL false


const char PASSWORD[] = "Capparelli96"; //same password for all networks
const char mqttBROKER[] = "192.168.1.146"; //ip of my PC
const char mqttCLIENT[] = "player2";

volatile uint8_t caso = 0, nNet, protocol;
String ssidNet[2], msgIn;
volatile boolean flag = false, turn; //opposto player2

volatile char segno = '-';
volatile char matrix[3][3];

WiFiClient espClient; 
PubSubClient client(espClient);

TFT_eSPI tft = TFT_eSPI();

void defaultScreen(){
  tft.fillScreen(TFT_ORANGE);
  //delay(1000);
  setText("Tic Tac Toe", 155, 0, 3, TFT_OLIVE);
  setBtn("Scan WiFi", 90, 50, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("About Project", 90, 100, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);

  setText(" NETWORK ASPECTS OF IOT - Fabio Capparelli -  214490", 0, 230, 1, TFT_OLIVE);
}

void wifiScreen(){
  tft.fillScreen(TFT_ORANGE);
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setText("Connect to WiFi", 170, 0, 3, TFT_OLIVE);
  
  setText("Scanning      ", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("Scanning .    ", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("Scanning . .  ", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("Scanning . . .", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("         . . .", 170, 40, 2, TFT_ORANGE);
  delay(200);
  setText("Scanning      ", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("Scanning .    ", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("Scanning . .  ", 170, 40, 2, TFT_OLIVE);
  delay(200);
  setText("Scanning . . .", 170, 40, 2, TFT_OLIVE);
  delay(200);

  uint8_t nNetworks = WiFi.scanNetworks();
  if (nNetworks == 0) 
  {
    setText("Scanning . . .", 170, 40, 2, TFT_ORANGE);
    setText("no networks found", 170, 40, 1, TFT_OLIVE);
  }
  else 
  {
    setText("Scanning . . .", 170, 40, 2, TFT_ORANGE);
    setText(" " + String(nNetworks) + " networks found", 170, 40, 1, TFT_OLIVE);
    setText(" YOU CAN CONNECT ONLY TO fastweb_Capparelli NETWORK", 0, 230, 1, TFT_OLIVE);
   
    uint8_t c=0;
    for (int i = 0; i < nNetworks; ++i) 
    {
      String ssid = WiFi.SSID(i);
      if(ssid.startsWith("fastweb_Capparelli 2"))
      {
        ssidNet[c] = ssid; 
        setText(String(c+1) + ": " + ssid, 0, (c+3)*20, 1, TFT_OLIVE);
        setBtn("Connect", 230, (c+3)*18, 80, 13, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
        c++;
      }
    }
    nNet = c;
    
    for (int i = 0; i < nNetworks; ++i) 
    {
      String ssid = WiFi.SSID(i);
      if(!ssid.startsWith("fastweb_Capparelli 2"))
      {
        setText(String(c+1) + ": " + ssid, 0, (c+3)*20, 1, TFT_OLIVE);
        c++;
      }
    }
    
  }  
}

void connectingScreen(String ssid, String password){
  tft.fillScreen(TFT_ORANGE);

  setText(" SSID: " + ssid, 0, 70, 1, TFT_OLIVE);
  setText(" PASSWORD: " + password, 0, 100, 1, TFT_OLIVE);

  setText("Connecting      ", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("Connecting .    ", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("Connecting . .  ", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("Connecting . . .", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("           . . .", 170, 0, 3, TFT_ORANGE);
  delay(200);
  setText("Connecting      ", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("Connecting .    ", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("Connecting . .  ", 170, 0, 3, TFT_OLIVE);
  delay(200);
  setText("Connecting . . .", 170, 0, 3, TFT_OLIVE);
  
 

  delay(1000);
}

void connectedScreen(String ssid, String password, String ip, String gip, String sm){
  tft.fillScreen(TFT_ORANGE);

  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("->", 230, 180, 60, 40, 2, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);

  setText("CONNECTED", 170, 0, 3, TFT_OLIVE);
  setText(" SSID: " + ssid, 0, 70, 1, TFT_OLIVE);
  setText(" PASSWORD: " + password, 0, 100, 1, TFT_OLIVE);
  setText(" local_IP: " + ip, 0, 130, 1, TFT_OLIVE);  
  setText(" gateway_IP: " + gip, 0, 160, 1, TFT_OLIVE); 
  setText(" subnet_MASK: " + sm, 0, 190, 1, TFT_OLIVE); 
}


void protocolScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 170, 0, 3, TFT_OLIVE);
  setText("choose the protocol: ", 170, 50, 2, TFT_OLIVE);
  
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
  setBtn("MQTT", 90, 80, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("CoAP", 90, 130, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("HTTP", 90, 180, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
}

void mqttScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 170, 0, 3, TFT_OLIVE);
  setText("MQTT protocol: ", 170, 50, 2, TFT_OLIVE);
  
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
  setBtn("Connect to Broker", 90, 100, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("About Protocol", 90, 150, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
}

void coapScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 170, 0, 3, TFT_OLIVE);
  setText("CoAP protocol: ", 170, 50, 2, TFT_OLIVE);
  
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
  setBtn("Connect to CoAP server", 90, 100, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("About Protocol", 90, 150, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
}

void httpScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 170, 0, 3, TFT_OLIVE);
  setText("HTTP protocol: ", 170, 50, 2, TFT_OLIVE);
  
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
  setBtn("Connect to HTTP server", 90, 100, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("About Protocol", 90, 150, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
}

void signScreen(){
  tft.fillScreen(TFT_BLACK);
  setText("Tic Tac Toe", 170, 0, 3, TFT_WHITE);
  setBtn("<-", 1, 1, 20, 20, 1, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
  
  switch(protocol){
    case 1: //MQTT
        if(segno == '-'){
          setText("choose O or X", 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          setBtn("X", 170, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
        }if(segno == 'O'){
          setText("you are: " + String(segno), 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          setBtn("X", 170, 100, 50, 40, 3, TFT_BLACK, TFT_BLACK, TFT_BLACK);
        }if(segno == 'X'){
          setText("you are: " + String(segno), 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_BLACK, TFT_BLACK, TFT_BLACK);
          setBtn("X", 170, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
        }
        break;
    case 2: //CoAP
        if(segno == '-'){
          setText("choose O or X", 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          setBtn("X", 170, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
        }if(segno == 'O'){
          setText("you are: " + String(segno), 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          setBtn("X", 170, 100, 50, 40, 3, TFT_BLACK, TFT_BLACK, TFT_BLACK);
        }if(segno == 'X'){
          setText("you are: " + String(segno), 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_BLACK, TFT_BLACK, TFT_BLACK);
          setBtn("X", 170, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
        }
        break;
    case 3: //HTTP
        if(segno == '-'){
          setText("choose O or X", 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          setBtn("X", 170, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
        }if(segno == 'O'){
          setText("you are: " + String(segno), 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          setBtn("X", 170, 100, 50, 40, 3, TFT_BLACK, TFT_BLACK, TFT_BLACK);
        }if(segno == 'X'){
          setText("you are: " + String(segno), 170, 50, 2, TFT_WHITE);
          setBtn("O", 120, 100, 50, 40, 3, TFT_BLACK, TFT_BLACK, TFT_BLACK);
          setBtn("X", 170, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
        }
        break;    
  }
}

void gameScreen(){
  tft.fillScreen(TFT_BLACK);
  setText("Tic Tac Toe", 170, 0, 2, TFT_WHITE);
  setText("you are: " + String(segno), 170, 30, 2, TFT_WHITE);
  setBtn("<-", 1, 1, 20, 20, 1, TFT_ORANGE, TFT_BLACK, TFT_WHITE);

  for(int i=0; i<3;i++)
    for(int j=0; j<3;j++)
         setBtn("", (i*50)+95, (j*50)+60, 50, 50, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
}

void winScreen(String s){
  
  tft.fillScreen(TFT_BLACK);
  setText("Tic Tac Toe", 170, 0, 3, TFT_WHITE);
  setText(s + " (" + String(segno) + ")", 170, 80, 3, TFT_WHITE);
  setBtn("PLAY", 110, 100, 100, 50, 2, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
  
}
void setup() {
  // put your setup code here, to run once:
  tft.init();

  tft.setRotation(1); //horizontal
  touch_calibrate();

   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   delay(100);

   Serial.begin(9600);
   
   emptyMatrix();
   scanMatrix();
   
   defaultScreen();
  
}

void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y))
       eventScreen(caso, x, y);
  eventScreen(caso, -1, -1);
}

void eventScreen(int i, uint16_t x, uint16_t y){

  switch(i){
     case 0: //HOME
        if( (x<=210 && x>=90) && (y<=90 && y>=50) ){
            wifiScreen(); 
            caso = 1;
        }
        break;
         
     case 1: //WIFI SCREEN
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            defaultScreen(); 
            caso = 0;
        }
        for (int i = 0; i<nNet; i++){
            if( (x<=310 && x>=230) && (y<=((i+3)*18)+13 && y>=(i+3)*18) ){

              if(WiFi.status() == WL_CONNECTED) WiFi.disconnect();
              char ssid[ssidNet[i].length()+1];
              ssidNet[i].toCharArray(ssid, ssidNet[i].length()+1);
              WiFi.begin(ssid, PASSWORD);
              while (WiFi.status() != WL_CONNECTED){
                delay(500);
                connectingScreen(String(ssid), String(PASSWORD));
              }
              connectedScreen(String(ssid), String(PASSWORD), WiFi.localIP().toString(), WiFi.gatewayIP().toString(), WiFi.subnetMask().toString());
              caso = 2;
          }
        }
        break;

      case 2: //CONNECTED SCREEN
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            wifiScreen();
            caso = 1;
        }
        if( (x<=290 && x>=230) && (y<=220 && y>=180) ){
             protocolScreen();
             caso = 3;
        }
        break;
        
      case 3: //PROTOCOL SCREEN
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            connectedScreen(WiFi.SSID(), String(PASSWORD), WiFi.localIP().toString(), WiFi.gatewayIP().toString(), WiFi.subnetMask().toString()); 
            delay(1000);
            caso = 2;
        }
        if(x<=210 && x>=90){
          if (y<=120 && y>=80) {
            mqttScreen();
            caso = 4;
          }
          if (y<=170 && y>=130) {
            coapScreen();
            caso = 5;
          }
          if (y<=220 && y>=180) {
            httpScreen();
            caso = 6;
          }
        }
        break;
        
      case 4: //MQTT SCREEN
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
        }
         if( (x<=230 && x>=90) && (y<=140 && y>=100) ){
            client.setServer(mqttBROKER, 1883);
            client.setCallback(callback);
            setBtn("Connected", 90, 100, 140, 40, 1, TFT_GREEN, TFT_BLACK, TFT_WHITE);
            setBtn("PLAY", 110, 200, 100, 30, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
            flag = true;
            caso = 4;
        }
        if(flag == true && ((x<=210 && x>=110) && (y<=230 && y>=200)) ){
              protocol = 1;
              caso = 7;
              signScreen();
              flag = false;
            }
       break;
       
       case 5: //CoAP SCREEN
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
        }
         if( (x<=230 && x>=90) && (y<=140 && y>=100) ){
            //client.setServer(mqttBROKER, 1883);
            setBtn("Connected", 90, 100, 140, 40, 1, TFT_GREEN, TFT_BLACK, TFT_WHITE);
            setBtn("PLAY", 110, 200, 100, 30, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
            flag = true;
            caso = 5;
        }
         if(flag == true && ((x<=210 && x>=110) && (y<=230 && y>=200)) ){
              protocol = 2;
              signScreen();
              caso = 7;
              flag = false;
        }
        break;
        
       case 6: //HTTP SCREEN
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
        }
         if( (x<=230 && x>=90) && (y<=140 && y>=100) ){
            //client.setServer(mqttBROKER, 1883);
            setBtn("Connected", 90, 100, 140, 40, 1, TFT_GREEN, TFT_BLACK, TFT_WHITE);
            setBtn("PLAY", 110, 200, 100, 30, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
            flag = true;
            caso = 6;
        }
         if(flag == true && ((x<=210 && x>=110) && (y<=230 && y>=200)) ){
              protocol = 3;
              signScreen();
              caso = 7;
              flag = false;
        }
        break;

      case 7: //SIGN SCREEN
       if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
            segno = '-';
            client.disconnect();
        }
        switch(protocol){
          case 1: //MQTT
          
            if (!client.connected()) reconnectMqtt("segno");
            client.loop(); 
            
            if(segno == '-' && msgIn == "O") {
              segno = 'X';
              msgIn = "";
              caso = 8;
              turn = false; //opponent turn
              gameScreen();
              client.disconnect();
            }
            if(segno == '-' && msgIn == "X") {
              segno = 'O';
              msgIn = "";
              caso = 8;
              turn = false; //opponent turn
              gameScreen();
              client.disconnect();
            }
            
            if(segno == '-' && (x<=170 && x>=120) && (y<=140 && y>=100)){
              publishMessage("segno", "O");
              msgIn="";
              segno = 'O';
              caso = 8;
              turn = true; //my turn
              gameScreen();
              client.disconnect();
            }
            
            if(segno == '-' && (x<=220 && x>=170) && (y<=140 && y>=100)){
              publishMessage("segno", "X");
              msgIn="";
              segno = 'X';
              caso = 8;
              turn = true; //my turn
              gameScreen();
              client.disconnect();
            }
            
            if(segno == 'X' && (x<=220 && x>=170) && (y<=140 && y>=100)){
              segno = 'X';
              gameScreen();
              client.disconnect();
              caso = 8;
            }
            if(segno == 'O' && (x<=170 && x>=120) && (y<=140 && y>=100)){
              segno = 'O';
              msgIn="";
              gameScreen();
              client.disconnect();
              caso = 8;
            }
            break;
        }
      break;

      case 8: //gameScreen
       if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
            segno = '-';
            client.disconnect();
            emptyMatrix();
        }
        switch(protocol){
          case 1:

            char tpc1[] = "mossa ";  //Own Topic
            tpc1[5] = segno;

            char tpc2[] = "mossa ";  //Opponent Topic
            tpc2[5] = (segno=='O')?'X':'O';
            
            if (!client.connected()) reconnectMqtt(tpc2);
            client.loop();

            //OPPONENT MOVES
            uint8_t r,c;  //row column sign, y=row , x=column
            char s; 
            if(!turn && msgIn != ""){
              r = uint8_t(msgIn[0])-48;
              c = uint8_t(msgIn[1])-48;
              s = char(msgIn[2]);
              
              msgIn="";
           
              if(matrix[r][c] != 'X' && (matrix[r][c] != 'O')){
                matrix[r][c] = s;
                setBtn(String(s), (c*50)+95, (r*50)+60, 50, 50, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
                turn = true;
                if(win()) {
                  winScreen("You Lost");
                  caso = 9;
                }
              }
            }

            if(turn){
              for(int i=0; i<3; ++i)
                for(int j=0; j<3; ++j){
                  
                  if( ((matrix[i][j] != 'X') && (matrix[i][j] != 'O')) && ( x<=((j+1)*50)+95 && x>=(j*50)+95 && y<=((i+1)*50)+60 && y>=(i*50)+60 ) ){
                    
                      String msgS = String(i) + String(j) + String(segno);
                      char msg[msgS.length()+1];
                      msgS.toCharArray(msg, msgS.length()+1); 
                      publishMessage(tpc1, msg);
                      matrix[i][j] = segno;
                      setBtn(String(segno), (j*50)+95, (i*50)+60, 50, 50, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
                      turn = false;
                      if(win()) {
                         winScreen("You Won");
                         caso = 9;
                      }
                    }
                  }
                 }

            break; 
        }
        break;

        case 9:
           if( (x<=210 && x>=110) && (y<=150 && y>=100) ){
            protocolScreen();
            caso = 3;
            segno = '-';
            client.disconnect();
            emptyMatrix();
        }
        break;
          
  }
}


void setText(String text, int x, int y, int s,  int color){
  tft.setTextColor(color);
  tft.setTextSize(s);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(text, x, y);
}


void setBtn(String label, int x, int y, int w, int h, int s, int fillColor, int labelColor, int cornerColor)
{
  tft.drawRect(x-1, y-1, w+2, h+2, cornerColor);
  tft.fillRect(x, y, w, h, fillColor);
  tft.setTextColor(labelColor);
  tft.setTextSize(s);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(label, x + (w/2), y + (h / 2));
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

//for MQTT
void callback(char topic[], byte payload[], unsigned int length) {
  msgIn = "";
  for (int i = 0; i < length; i++) 
    msgIn = msgIn + ((char)payload[i]); 
  subscribeMessage(topic); 
 }


void publishMessage(char topic[], char msg[]){
  client.publish(topic, msg);
}

void reconnectMqtt(char topic[]) {
  while (!client.connected()) {
    if (client.connect(mqttCLIENT)) {
        client.subscribe(topic);
    }else
      delay(5000);
  }
}

void subscribeMessage(char topic[]){
   client.subscribe(topic);
}

void emptyMatrix(){
  for(int i = 0; i<3; ++i)
    for(int j = 0; j<3; ++j)
      matrix[i][j] = '-';
}


void scanMatrix(){
  for(int i = 0; i<3; ++i){
      for(int j = 0; j<3; ++j){
        Serial.print(String(matrix[i][j]) + " [" + String(i) + "," + String(j) + "]");
        Serial.print(" | ");
      }
      Serial.println("");
     }
}

boolean win(){
  Serial.println("");
  scanMatrix();
     
  char c = '-';
  for(int i = 0; i<3; ++i)
      if(String(matrix[i][0]).equals(String(matrix[i][1])) && String(matrix[i][0]).equals(String(matrix[i][2]))) {
        c = matrix[i][2];
        return (c=='O' || c=='X')?true:false;
      }

  for(int j = 0; j<3; ++j)
      if(String(matrix[0][j]).equals(String(matrix[1][j])) && String(matrix[0][j]).equals(String(matrix[2][j]))){ 
        c = matrix[2][j];
        return (c=='O' || c=='X')?true:false;
      }
      
  if(String(matrix[0][0]).equals(String(matrix[1][1])) && String(matrix[0][0]).equals(String(matrix[2][2]))){
    c = matrix[2][2];
    return (c=='O' || c=='X')?true:false;
  }
  

  if(String(matrix[0][2]).equals(String(matrix[1][1])) && String(matrix[0][2]).equals(String(matrix[2][0]))){
    c = matrix[0][2];
    return (c=='O' || c=='X')?true:false;
  }
  
 
  return false;

}
