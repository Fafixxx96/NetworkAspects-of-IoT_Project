#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h> 
#include "WiFi.h"
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

#define CALIBRATION_FILE "/TouchCalData3"
#define REPEAT_CAL false

const char PASSWORD[] = "Capparelli96"; //same password for all networks
const char mqttBROKER[] = "192.168.1.146"; //ip of my PC with mosquitto broker
const char mqttCLIENT[] = "player2";

volatile uint8_t caso, nNet, protocol;
String ssidNet[2], msgIn, mossaOut;
volatile boolean flag = false, turn; //opposto player1

volatile char segno = '-';
volatile char matrix[3][3];

//for mqtt
WiFiClient espClient; 
PubSubClient client(espClient);

//for coap
WiFiUDP udp;
Coap coap(udp);

TFT_eSPI tft = TFT_eSPI();

void setup() {
  caso = 0;
  
  tft.init();
  tft.setRotation(3); //horizontal
  touch_calibrate();

   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   delay(100);

   Serial.begin(9600);   
   emptyMatrix();
   
   defaultScreen();
}

void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y))
       eventScreen(caso, x, y);
  eventScreen(caso, -1, -1);
}

void defaultScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 155, 0, 3, TFT_OLIVE);
  setBtn("Scan WiFi", 90, 50, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  setBtn("About Project", 90, 100, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);

  setText(" NETWORK ASPECTS OF IOT - Fabio Capparelli -  214490", 0, 230, 1, TFT_OLIVE);
}


void aboutProjectScreen(){
    tft.fillScreen(TFT_ORANGE);
    setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
    setText("About Project", 170, 0, 3, TFT_OLIVE);
    setText("The following has as main aim to develop a minigame", 1, 60, 1, TFT_OLIVE);
    setText("TIC TAC TOE 1vs1, by giving the possibility to choose", 1, 80, 1, TFT_OLIVE);
    setText("the comunication protocol.", 1, 100, 1, TFT_OLIVE);
    setText("You can choose MQTT or CoAP, two fundamental protocols", 1, 120, 1, TFT_OLIVE);
    setText("in IoT world, it's importat that the two devices ", 1, 140, 1, TFT_OLIVE);
    setText("have to be agree about the choosen protocol.", 1, 160, 1, TFT_OLIVE);
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
  //setBtn("HTTP", 90, 180, 120, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
}

void mqttScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 170, 0, 3, TFT_OLIVE);
  setText("MQTT protocol: ", 170, 50, 2, TFT_OLIVE);
  
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
  setBtn("Connect to Broker", 90, 100, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  //setBtn("About Protocol", 90, 150, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
}

void coapScreen(){
  tft.fillScreen(TFT_ORANGE);

  setText("Tic Tac Toe", 170, 0, 3, TFT_OLIVE);
  setText("CoAP protocol: ", 170, 50, 2, TFT_OLIVE);
  
  setBtn("<-", 1, 1, 20, 20, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  
  setBtn("Connect to CoAP server", 90, 100, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
  //setBtn("About Protocol", 90, 150, 140, 40, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
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
        }
        break;
    case 2: //CoAP
        if(segno == '-'){
          setText("choose O or X", 170, 50, 2, TFT_WHITE);80
          setBtn("O", 120, 100, 50, 40, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
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
  delay(500);
  tft.fillScreen(TFT_BLACK);
  setText("Tic Tac Toe", 170, 0, 3, TFT_WHITE);
  setText(s + " (" + String(segno) + ")", 170, 80, 3, TFT_WHITE);
  setBtn("PLAY", 110, 100, 100, 50, 2, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
}


void eventScreen(int i, uint16_t x, uint16_t y){

  switch(i){
     case 0: //HOME
        if( (x<=210 && x>=90) && (y<=90 && y>=50) ){
            wifiScreen(); 
            caso = 1;
        }
        if( (x<=210 && x>=90) && (y<=140 && y>=100) ){
            aboutProjectScreen(); 
            caso = 10;
        }
        break;

     case 10: 
        if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            defaultScreen(); 
            caso = 0;
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
            WiFi.disconnect();
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
            setBtn("PLAY", 110, 150, 100, 30, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
            flag = true;
            caso = 4;
        }
        if(flag == true && ((x<=210 && x>=110) && (y<=180 && y>=150)) ){
              protocol = 1;
              caso = 6;
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
            coap.server(callback_segno, "segno");
            coap.server(callback_mossa, "mossa");
            coap.response(callback_response);
            coap.start(5680); //port for UDP connection
            
            setBtn("Connected", 90, 100, 140, 40, 1, TFT_GREEN, TFT_BLACK, TFT_WHITE);
            setBtn("PLAY", 110, 150, 100, 30, 1, TFT_DARKGREY, TFT_WHITE, TFT_WHITE);
            flag = true;
            caso = 5;
        }
         if(flag == true && ((x<=210 && x>=110) && (y<=180 && y>=150)) ){
              protocol = 2;
              signScreen();
              caso = 6;
              flag = false;
        }
        break;
        
      case 6: //SIGN SCREEN
       if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
            segno = '-';
            client.disconnect();
        }
        switch(protocol){
          case 1: //MQTT
            if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
              protocolScreen();
              caso = 3;
              segno = '-';
              client.disconnect();
              }
            
            if (!client.connected()) reconnectMqtt("segno");
            client.loop(); 
            
            if(segno == '-' && msgIn == "O") {
              segno = 'X';
              msgIn = "";
              caso = 8;
              turn = false; //opponent turn
              client.unsubscribe("segno");
              gameScreen();
              client.disconnect();
            }
            if(segno == '-' && msgIn == "X") {
              segno = 'O';
              msgIn = "";
              caso = 8;
              turn = false; //opponent turn
              client.unsubscribe("segno");
              gameScreen();
              client.disconnect();
            }
            
            if(segno == '-' && (x<=170 && x>=120) && (y<=140 && y>=100)){
              client.unsubscribe("segno");
              publishMessage("segno", "O");
              msgIn="";
              segno = 'O';
              caso = 8;
              turn = true; //my turn
              gameScreen();
              client.disconnect();
            }
            
            if(segno == '-' && (x<=220 && x>=170) && (y<=140 && y>=100)){
              client.unsubscribe("segno");
              publishMessage("segno", "X");
              msgIn="";
              segno = 'X';
              caso = 8;
              turn = true; //my turn
              gameScreen();
              client.disconnect();
            }
            
            break;

            case 2: //CoAP
              if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
                protocolScreen();
                caso = 3;
                segno = '-';
                msgIn="";
                mossaOut="";
                coap.end();
               
              }
              
              //se non scelgo io, chiedo al mio server coap il mio segno, cioè se l'avversario me lo ha caricato.
              if( (x>320 || x<0) && (y>240 || y<0) ){
                
                int getid = coap.get(IPAddress(192, 168, 1, 127), 5680, "segno", COAP_CON);
     
                if(segno == '-' && msgIn == "O") {
                  segno = 'X';
                  caso = 8;
                  turn = false; //opponent turn
                  gameScreen();
                }
                if(segno == '-' && msgIn == "X") {
                  segno = 'O'; 
                  caso = 8;
                  turn = false; //opponent turn
                  gameScreen();
                }
              }
              
              //se scelgo io, carico nel server coap dell'avversario il mio segno.
              if(segno == '-' && (x<=170 && x>=120) && (y<=140 && y>=100)){
                int putid = coap.put(IPAddress(192, 168, 1, 101), 5680, "segno", "O", COAP_CON);
                caso = 8;
                turn = true; //my turn
                segno = 'O';
                gameScreen();
                x = -1;
                y = -1;
               
            }
             if(segno == '-' && (x<=220 && x>=170) && (y<=140 && y>=100)){
                int putid = coap.put(IPAddress(192, 168, 1, 101), 5680, "segno", "X", COAP_CON);
                caso = 8;
                segno = 'X';
                turn = true; //my turn
                gameScreen();
                x = -1;
                y = -1;
               
            }
            coap.loop();
            break;   
        }
      break;

      case 8: //gameScreen
       if( (x<=20 && x>=0) && (y<=20 && y>=0) ){
            protocolScreen();
            caso = 3;
            segno = '-';
            msgIn="";
            mossaOut="";
            emptyMatrix();
            client.disconnect();
            coap.end();
        }
        switch(protocol){
          case 2: //CoAP
           
              opponentMoves(true);
              
              if(turn)
                for(int i=0; i<3; ++i)
                  for(int j=0; j<3; ++j)
                    if( ((matrix[i][j] != 'X') && (matrix[i][j] != 'O')) && ( x<=((j+1)*50)+95 && x>=(j*50)+95 && y<=((i+1)*50)+60 && y>=(i*50)+60 ) ){
                      String msgS = String(i) + String(j) + String(segno);
                      char msg[msgS.length()+1];
                      msgS.toCharArray(msg, msgS.length()+1); 
                      
                      int putid = coap.put(IPAddress(192, 168, 1, 101), 5680, "mossa", msg, COAP_CON);
                      
                      setBtn(String(segno), (j*50)+95, (i*50)+60, 50, 50, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
                      matrix[i][j] = segno;
                 }
              coap.loop();
              
              break;
          case 1: //MQTT

            char tpc1[] = "mossa ";  //Own Topic
            tpc1[5] = segno;

            char tpc2[] = "mossa ";  //Opponent Topic
            tpc2[5] = (segno=='O')?'X':'O';
            
            if (!client.connected()) reconnectMqtt(tpc2);
            client.loop();

           opponentMoves(false);

            if(turn)
              for(int i=0; i<3; ++i)
                for(int j=0; j<3; ++j)
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
                      if(parity()){
                         winScreen("No one won");
                         caso = 9;
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
            mossaOut="";
            msgIn="";
            emptyMatrix();
            client.disconnect();
            coap.end();
        }
        break;
          
  }
}

void opponentMoves(boolean isCoap){
    //OPPONENT MOVES
   uint8_t r,c;  //row column sign, y=row , x=column
   char s; 
   if(!turn)
     if(isCoap) int getid = coap.get(IPAddress(192, 168, 1, 127), 5680, "mossa", COAP_CON);
    
       if(msgIn.length() == 3){
       r = uint8_t(msgIn[0])-48;
       c = uint8_t(msgIn[1])-48;
       s = char(msgIn[2]);
              
      if(matrix[r][c] != 'X' && (matrix[r][c] != 'O') && (matrix[r][c] == '-') ){
          matrix[r][c] = s;
          setBtn(String(s), (c*50)+95, (r*50)+60, 50, 50, 3, TFT_ORANGE, TFT_BLACK, TFT_WHITE);
          turn = true;
          if(win()) {
             winScreen("You Lost");
             caso = 9;
          }
          if(parity()){
             winScreen("No one won");
             caso = 9;
          }
       }
       msgIn="";
       mossaOut="";
   }
}


void setText(String text, int x, int y, int s,  int color){
  tft.setTextColor(color);
  tft.setTextSize(s);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(text, x, y);
}


void setBtn(String label, int x, int y, int w, int h, int s, int fillColor, int labelColor, int cornerColor){
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
  //subscribeMessage(topic); 
 }


void publishMessage(char topic[], char msg[]){ client.publish(topic, msg); }


void reconnectMqtt(char topic[]) {
  while (!client.connected()) {
    if (client.connect(mqttCLIENT)) {
        client.subscribe(topic);
    }else
      delay(5000);
  }
}

void subscribeMessage(char topic[]){ client.subscribe(topic, 1); }

//FOR CoAP
//when server receives the request
void callback_segno(CoapPacket &packet, IPAddress ip, int port) {
 
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  String message(p);
  if(message.equals("X")) segno = 'X';
   // coap.sendResponse(ip, port, packet.messageid, "X"); //forse inutile
  else if(message.equals("O")) segno = 'O';
    // coap.sendResponse(ip, port, packet.messageid, "O"); //forse inutile
  else{
    if(segno == 'X') coap.sendResponse(ip, port, packet.messageid, "X");
    else if(segno == 'O') coap.sendResponse(ip, port, packet.messageid, "O");
    else coap.sendResponse(ip, port, packet.messageid, "-");
  }
}


void callback_mossa(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  String message(p);

  if(String(message[2]).equals(String(segno))){
      mossaOut = message;
      
      if(win()) {
         winScreen("You Won");
         caso = 9;
         }
      if(parity()){
         winScreen("No one won");
         caso = 9;
      }
      turn = false;
  }else {
    char msg[mossaOut.length()+1];
    mossaOut.toCharArray(msg, mossaOut.length()+1); 
    coap.sendResponse(ip, port, packet.messageid, msg);
  }
}

//when client receives the response
void callback_response(CoapPacket &packet, IPAddress ip, int port){
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  msgIn = String(p);
  //Serial.println(" 2: " + msgIn);
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
  //scanMatrix();
     
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

boolean parity(){
 // scanMatrix();
   for(int i = 0; i<3; ++i)
      for(int j = 0; j<3; ++j)
         if(String(matrix[i][j]).equals("-")) return false;
   return true;
}
