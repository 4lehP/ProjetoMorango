/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-relay-module-ac-web-server/

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
/********
 * OBS:
    utilizei o exemplo no endereço acima o qual fornece autorização de uso do mesmo desde que 
    seja incluido o texto acima nos projetos e susas alterações 
    Melquior Panini.
************/

// Importar bibliotecas necessárias
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

// Defina como verdadeiro para definir o relé como normalmente aberto (NÃO)
#define RELAY_NO    true

// Defina o número de relés
#define NUM_RELAYS  12
#define NUM_RELAYS2  12

// Atribui cada GPIO a um relé
int relayGPIOs[NUM_RELAYS] = {2, 13, 14, 27, 26, 25, 33, 32, 16, 17, 4, 15};
String relayGPIOs2[NUM_RELAYS2] = {"Valvula Água", "Valvula Circulação Adubo 1", "Valvula Adubo 1", "Valvula Circulção Adubo 2", "Valvula Adubo 2", "VALVULA GERAL", "Valvula Canteiro 1", "Valvula Canteiro 2", "Valvula Canteiro 3", "Valvula Canteiro 4", "Valvula Canteiro 5", "Bomba 1"};

// Substitua por suas credenciais de rede
const char* ssid = "XXXXXX";
const char* password = "XXXXXXX";
const char *ip = "192.168.100.201";

//Rede Access Point
const char *ssidAP = "ESP32AP";
const char *passwordAP = "";

const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";
const int pinButton = 12; //pino do botão para simular um travamento

// Criar objeto AsyncWebServer na porta 80
AsyncWebServer server(80);

//Watchdog
//faz o controle do temporizador (interrupção por tempo)
hw_timer_t *timer = NULL;
//função que o temporizador irá chamar, para reiniciar o ESP32
void IRAM_ATTR resetModule() {
  ets_printf("(watchdog) reiniciar\n"); //imprime no log
  esp_restart(); //reinicia o chip
}
//Watchdog
//função que o configura o temporizador
void configureWatchdog() {
  timer = timerBegin(0, 80, true); //timerID 0, div 80
  //timer, callback, interrupção de borda
  timerAttachInterrupt(timer, &resetModule, true);
  //timer, tempo (us), repetição
  timerAlarmWrite(timer, 5000000, true);//Tempo para resetar caso fique travado ou o btn reset pressionado
  timerAlarmEnable(timer); //habilita a interrupção //enable interrupt
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="utf-8">
  <meta http-equiv='refresh' content='5;URL=/'>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #0abb1e}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .packet1 {color: #bebebe; font-size: 15px;}
  </style>
</head>
<body>
  <h2>ESPERIMENTAL IFC</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
</body>
<br><br><br><br>
<h3>ESQUEMÁTICO DO SISTEMA</h3>

<img src="Esq_Valv" width=84%>

<p class="packet1">© Sistema Desenvolvido por: Melquior Panini & Lucas Kelvin Elias, Orientação Prof. Eduardo Bidese Puhl. IFC RSL SC.
<br>Foi Seguido o exemplo de Rui Santos com autorização do mesmo conforme está em seu site:</br>
<br>Complete project details at https://RandomNerdTutorials.com/esp32-esp-now-wi-fi-web-server</br>
<br>Permission is hereby granted, free of charge, to any person obtaining a copy</br>
<br>of this software and associated documentation files.</br>
<br>The above copyright notice and this permission notice shall be included in all</br>
<br>copies or substantial portions of the Software.</br>
</p>
</html>
)rawliteral";

// Substitui o espaço reservado pela seção de botão em sua página da web
String processor(const String& var){
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    for(int i=1; i<=NUM_RELAYS; i++){
      String relayStateValue = relayState(i);
      buttons+= "<h4>"  + relayGPIOs2[i-1] +"</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  return String();
}

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}

void setup(){
    configureWatchdog();
    pinMode(pinButton, INPUT_PULLUP);
    
  // Serial port for debugging purposes
  Serial.begin(115200);
  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;}
  // Defina todos os relés para desligados quando o programa iniciar-se definido
  // para Normalmente aberto, o relé é desligado quando você define o relé para HIGH 
  for(int i=1; i<=NUM_RELAYS; i++){
    pinMode(relayGPIOs[i-1], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i-1], HIGH);
    }
    else{
      digitalWrite(relayGPIOs[i-1], LOW);
    }
  }
  
// Conecte-se ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  //Configura o IP
  IPAddress ipAddress;
  ipAddress.fromString(ip);
  WiFi.config(ipAddress, WiFi.gatewayIP(), WiFi.subnetMask());
  // Imprime o endereço de IP e starta o web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

//Configura o IP modo Access Point
  Serial.println("Configuração do access point...");
  WiFi.softAP(ssidAP, passwordAP);
  delay(100);
  Serial.println("Set softAPConfig");
  IPAddress Ip(192, 168, 0, 200);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  IPAddress myIP = WiFi.softAPIP();
  // Imprimir endereço IP AP Local
  Serial.print("IP AP address: ");
  Serial.println(myIP);
  Serial.println("");
  
  // Rota para raiz / página da web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Envie uma solicitação GET para <ESP_IP> / update? Relay = <inputMessage> & state = <inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET o valor input1 em <ESP_IP> / update? Relay = <inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if(RELAY_NO){
        Serial.print("NO ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        Serial.print("NC ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  //Pertence a chamada da imagem 1
    server.on("/Esq_Valv", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Esq_Valv.png", "image/png");
  });
  // Iniciar servidor
  server.begin();
}
  
void loop() {
  //Watchdog
  //reseta o temporizador (alimenta o watchdog)
  timerWrite(timer, 0); //reseta o temporizador (alimenta o watchdog)
  long tme = millis(); //tempo inicial do loop
  //fica preso no loop enquanto o botão estiver pressionado
  while (digitalRead(pinButton)) {
    Serial.println("botão pressionado: " + String(millis() - tme));
    delay(500);
  }
}
