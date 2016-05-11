#include <SoftwareSerial.h>
SoftwareSerial Wifi(3, 2); // RX | TX

const String ssid = "undefined";
const String pass = "k1818285";
const String host = "192.168.43.105";
const String port = "1338";

const int digitalOutput = 13;
const int digitalInput = 10;
const String idSensor = "3";

String inputString = "";
boolean stringComplete = false;
void setup()
{
  Serial.begin(9600);
  Wifi.begin(9600);
  
  inputString.reserve(200);
  writeCommand("ATE0");
  writeCommand("AT+CWMODE=1");
  writeCommand("AT+CIPMUX=0");
  writeCommand("AT+CIPCLOSE");
  tryTcpConection();
  pinMode(digitalOutput,OUTPUT);
  pinMode(digitalInput,INPUT);
}
bool prevState=false;
int readPin;
bool firstMsg = true;
int aux;
void loop()
{

  if(firstMsg){
    delay(1000);
    digitalWrite(digitalOutput,LOW);
    sendInfo(0);
    firstMsg = false;
  }
  else{
    if(Wifi.available())
    {
      readResponse();
      if(inputString.indexOf("@1")!=-1){
        digitalWrite(digitalOutput,HIGH); 
        sendInfo(1);
      }else if(inputString.indexOf("@0")!=-1){
        digitalWrite(digitalOutput,LOW); 
        sendInfo(0);
      }
    }
    readPin = digitalRead(digitalInput);
    if(readPin!=prevState){
      prevState = readPin;
      digitalWrite(digitalOutput,prevState?HIGH:LOW);
      sendInfo(prevState);
    }
    if (Serial.available())
    {
      char c = Serial.read();
      Wifi.print(c);
    }
  }
}
void writeCommand(String command){
  Wifi.println(command);
  Serial.println(command);
  delay(100);
  readResponse();
}
void readResponse(){
  inputString="";
  stringComplete = false;
  do{
    if(Wifi.available()){
      char inChar = (char)Wifi.read();
      inputString += inChar;
      if(inChar=='\n') stringComplete = true;
    }
  }while(!stringComplete);
  Serial.print(inputString);
  //Serial.print(inputString.length());
}
void sendInfo(int state){
  boolean first = true;
  do{
    if(!first) tryTcpConection(); //fallo conexion tcp
    writeCommand("AT+CIPSEND=5");
    first = false;
  }while(inputString.indexOf("not")!=-1);
  writeCommand(idSensor+","+String(state)+"\r\n");
}
void tryWifiConection(){
  boolean first = true;
  int trys = 0;
  do{
    if(!first) delay(5000); //si no es el primer intento, espera 5 segundos para volver a intentar
    writeCommand("AT+CWJAP=\""+ssid+"\",\""+pass+"\"");
    first = false;
    trys++;
  }while(inputString.indexOf("OK")==-1 && trys<10); //intentara la comunicacion 10 veces, de no lograrlo se pasa a estado error
}
void tryTcpConection(){
  //antes verifica que se tiene una ip
  writeCommand("AT+CIFSR");
  if(inputString.indexOf("0.0.0.0")!=-1) tryWifiConection(); //primero resolver error wifi antes de establecer comunicacion tcp
  boolean first = true;
  int trys = 0;
 do{
    if(!first) delay(2000); //si no es el primer intento, espera 2 segundos para volver a intentar
    writeCommand("AT+CIPSTART=\"TCP\",\""+host+"\","+port);
    first = false;
    trys++;
  }while(inputString.indexOf("ERROR")!=-1 && trys<10); //intentara la comunicacion 10 veces, de no lograrlo se pasa a estado error tcp
}
