
/********************************************************
   CANAL INTERNET E COISAS
   Configuracao com JSON e SPIFFS
   Multiplataforma ESP32 e ESP8266
   12/2019 - Andre Michelon
   andremichelon@internetecoisas.com.br
   https://internetecoisas.com.br
*/
//------------------------------BIBLIOTECAS--------------------------------//

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "IeCESPReleV4Lib.h"

//------------------------------------------------------------------------//
// ESP8266 possui pino padrão de LED



//---------PARTE DAS CONFIGURAÇÕES DAS ENTRADAS DAS VÁLVULAS----------//
const byte      LED_ON                  = HIGH;
const byte      LED_OFF                 = LOW;

#define NUM_RELAYS  12
#define NUM_RELAYS2  12

int relayGPIOs[NUM_RELAYS] = {2, 13, 14, 27, 26, 25, 33, 32, 16, 17, 4, 15};
String relayLABEL[NUM_RELAYS2] = {"Valvula Água", "Valvula Circulação Adubo 1", "Valvula Adubo 1", "Valvula Circulção Adubo 2", "Valvula Adubo 2", "VALVULA GERAL", "Valvula Canteiro 1", "Valvula Canteiro 2", "Valvula Canteiro 3", "Valvula Canteiro 4", "Valvula Canteiro 5", "Bomba 1"};

//---------------------------------------------------------------------//

// Tamanho do Objeto JSON
//const   size_t    JSON_SIZE            = 404; declarado em def.h

//--------------------FIREBASE-----------------------------------------//
FirebaseData firebaseData;
FirebaseJson json;
bool releSetFlag=0;
String fireStatus[NUM_RELAYS2] = {"Desligado", "Desligado"};
String statusAnterior[NUM_RELAYS2] = {"Desligado", "Desligado"};
String fireMandaStatus1 = "";
String fireMandaStatus2 = "";

//---------------------------------------------------------------------//

StaticJsonDocument<320> doc;

// Variáveis Globais ------------------------------------
char              id[30];       // Identificação do dispositivo
boolean           ledOn;        // Estado do LED
word              bootCount;    // Número de inicializações
int               fuso;         // Fuso horário
char              horamanual[30];//horario manual
char              referencia[30];//referencia Nome do dispositivo
bool              autenticacao; //Senpre solicitar Autenticação HIGH
bool              softap;       // Modo SoftAP sempre ativo HIGH
char              ssid[30];     // Rede WiFi
char              pw[30];       // Senha da Rede WiFi
char              agendamento[30];  //#sched# adendamento de rotinas

// Funções Genéricas ------------------------------------

String softwareStr() {
  // Retorna nome do software
  return String(__FILE__).substring(String(__FILE__).lastIndexOf("\\") + 1);
}

String longTimeStr(const time_t &t) {
  // Retorna segundos como "d:hh:mm:ss"
  String s = String(t / SECS_PER_DAY) + ':';
  if (hour(t) < 10) {
    s += '0';
  }
  s += String(hour(t)) + ':';
  if (minute(t) < 10) {
    s += '0';
  }
  s += String(minute(t)) + ':';
  if (second(t) < 10) {
    s += '0';
  }
  s += String(second(t));
  return s;
}

void ledSet() {
  // Define estado do LED
  for (int i = 0; i < NUM_RELAYS; i++) {
    digitalWrite(relayGPIOs[i], ledOn ? LED_ON : LED_OFF);
  }

}

// Funções de Configuração ------------------------------
void  configReset() {
  // Define configuração padrão
  strlcpy(id, "IeC Device", sizeof(id));
  strlcpy(ssid, "IF_CATARINENSE", sizeof(ssid));
  strlcpy(pw, "ifcatarinense", sizeof(pw));
  strlcpy(referencia, "00,00,00,00,00,0000", sizeof(referencia));
  ledOn     = false;
  bootCount = 0;
  fuso = 3;
  strlcpy(horamanual, "00,00,00,00,00,0000", sizeof(horamanual));
  autenticacao = true;
  softap = true;
  strlcpy(agendamento, "0000", sizeof(agendamento));

}

boolean configRead() {

  /*
     https://arduinojson.org/v6/assistant/
     https://www.youtube.com/watch?v=uxSuNrA4P1Q&ab_channel=InterneteCoisas
     size: 176+228 = 404

     {
    "id": "123456789012345678901234567890",
    "led": true,
    "bool": 9999,
    "ssid": "123456789012345678901234567890",
    "pw": "123456789012345678901234567890",
    "boot": 0,
    "fuso": 13,
    "horamanual": "123456789012345678901234567890",
    "autenticacao": 9999,
    "softap": 9999,
    "agendamento": "123456789012345678901234567890"
    }



  */
  // Lê configuração
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open(F("/Config.json"), "r");
  if (deserializeJson(jsonConfig, file)) {
    // Falha na leitura, assume valores padrão
    configReset();

    log(F("Falha lendo CONFIG, assumindo valores padrão."));
    return false;
  } else {
    // Sucesso na leitura
    strlcpy(id, jsonConfig["id"]      | "", sizeof(id));
    strlcpy(ssid, jsonConfig["ssid"]  | "", sizeof(ssid));
    strlcpy(pw, jsonConfig["pw"]      | "", sizeof(pw));
    strlcpy(referencia, jsonConfig["referencia"]      | "", sizeof(referencia));
    ledOn     = jsonConfig["led"]     | false;
    bootCount = jsonConfig["boot"]    | 0;
    fuso =  jsonConfig["fuso"]    | 13;
    strlcpy(horamanual, jsonConfig["horamanual"]      | "", sizeof(horamanual));
    autenticacao = jsonConfig["autenticacao"]    | true;
    softap = jsonConfig["softap"]    | true;
    strlcpy(agendamento, jsonConfig["agendamento"]      | "", sizeof(agendamento));

    file.close();

    log(F("\nLendo config:"));
    serializeJsonPretty(jsonConfig, Serial);
    log("");

    return true;
  }
}

boolean configSave() {
  // Grava configuração
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open(F("/Config.json"), "w+");
  if (file) {
    // Atribui valores ao JSON e grava
    jsonConfig["id"]    = id;
    jsonConfig["led"]   = ledOn;
    jsonConfig["boot"]  = bootCount;
    jsonConfig["ssid"]  = ssid;
    jsonConfig["pw"]    = pw;
    jsonConfig["referencia"]    = referencia;
    jsonConfig["fuso"]  = fuso;
    jsonConfig["horamanual"]  =  horamanual;
    jsonConfig["autenticacao"] = autenticacao;
    jsonConfig["softap"] = softap;
    jsonConfig["agendamento"] = agendamento;

    serializeJsonPretty(jsonConfig, file);
    file.close();

    log(F("\nGravando config:"));
    serializeJsonPretty(jsonConfig, Serial);
    log("");

    return true;
  }
  return false;
}

// Requisições Web --------------------------------------
void handleHome() {
  // Home
  File file = SPIFFS.open(F("/Home.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();

    // Atualiza conteúdo dinâmico
    s.replace(F("#id#")       , id);
    s.replace(F("#led#")      , ledOn ? F("Ligado") : F("Desligado"));
    s.replace(F("#bootCount#"), String(bootCount));
#ifdef ESP8266
    s.replace(F("#serial#") , hexStr(ESP.getChipId()));
#else
    s.replace(F("#serial#") , hexStr(ESP.getEfuseMac()));
#endif
    s.replace(F("#software#") , softwareStr());
    s.replace(F("#sysIP#")    , ipStr(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : WiFi.softAPIP()));
    s.replace(F("#clientIP#") , ipStr(server.client().remoteIP()));
    s.replace(F("#active#")   , longTimeStr(millis() / 1000));
    s.replace(F("#userAgent#"), server.header(F("User-Agent")));

    // Envia dados
    server.send(200, F("text/html"), s);
    log("Home - Cliente: " + ipStr(server.client().remoteIP()) +
        (server.uri() != "/" ? " [" + server.uri() + "]" : ""));
  } else {
    server.send(500, F("text/plain"), F("Home - ERROR 500"));
    log(F("Home - ERRO lendo arquivo"));
  }
}

void handleRelay() {
  // Relay
  if (!pwdNeeded() || chkWebAuth()) {
    File file = SPIFFS.open(F("/Relay.htm"), "r");
    if (file) {
      file.setTimeout(100);
      String s = file.readString();
      file.close();
      // Replace markers
      s.replace("#reference#" , reference());
      // Send data
      server.send(200, F("text/html"), s);
      log(F("WebRelay"), "Cliente: " + ipStr(server.client().remoteIP()));
    } else {
      server.send(500, F("text/plain"), F("Relay - ERROR 500"));
      logFile(F("WebRelay"), F("ERRO lendo arquivo"), true);
    }
  }
}

void handleRelayStatus() {
  // Relay status
  if (!pwdNeeded() || chkWebAuth()) {

    String s = dateTimeStr(now())   + "&" ;

    for (int i = 0; i < NUM_RELAYS; i++) {
      s += String(digitalRead(relayGPIOs[i]))  + "&" ; //Leitura de todas as Valvulas

    }
    s += lastEvent;

    server.send(200, F("text/plain"), s);

    log(F("WebRelayStatus"), "Cliente: " + ipStr(server.client().remoteIP()) +
        " [" + s + "]"); //enviar todos reles em uma msg



  }

}


void handleRelaySet() {
  // Set Relay status
  if (!pwdNeeded() || chkWebAuth()) {
    String s = server.arg("set");
    String LigarDesligar = s.substring(1, 2);//primeira parte para liga/desliga = 0 e 1
    String IndexRele = s.substring(2); //Qual a posição do vetor

    digitalWrite(relayGPIOs[IndexRele.toInt()] , LigarDesligar.toInt());

    lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[IndexRele.toInt()]  + " " + LigarDesligar + " ";

    
    
  
        
       
//    }
//    if(digitalRead(relayGPIOs[1]==0)){
//      Firebase.set(firebaseData, "/Digitais/VRetAdb1/Estado/", "Desligado");
//    }else{
//       Firebase.set(firebaseData,"/Digitais/VRetAdb1/Estado/", "Ligado");
//    }
    releSetFlag=1;

    //   if(fireStatus1=="Desligado"){
    //    digitalWrite(relayGPIOs[0],LOW);
    //  }
    //  if(fireStatus1=="Ligado"){
    //    digitalWrite(relayGPIOs[0],HIGH);
    //  }
    //
    //  if(fireStatus2=="Desligado"){
    //    digitalWrite(relayGPIOs[1],LOW);
    //  }
    //  if(fireStatus2=="Ligado"){
    //    digitalWrite(relayGPIOs[1],HIGH);
    //  }
    //

    //
    //            if(fireStatus1=="Desligado"){
    //              digitalWrite(relayGPIOs[0] , LOW);
    //              lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[0]  + " " + LigarDesligar + " ";
    //              handleRelayStatus();
    //            }
    //            else if(fireStatus1=="Ligado"){
    //              digitalWrite(relayGPIOs[0] , HIGH);
    //              lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[0]  + " " + LigarDesligar + " ";
    //              handleRelayStatus();
    //            }
    //
    //
    //              if(fireStatus2=="Desligado"){
    //              digitalWrite(relayGPIOs[1] , LOW);
    //             lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[1]  + " " + LigarDesligar + " ";
    //              handleRelayStatus();
    //            }
    //            else if(fireStatus2=="Ligado"){
    //              digitalWrite(relayGPIOs[1] , HIGH);
    //              lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[1]  + " " + LigarDesligar + " ";
    //              handleRelayStatus();
    //            }



    //    fireStatus1 = Firebase.getString("Digitais/ VAdb1 / Estado");
    //    fireStatus2 = Firebase.getString("Digitais/ VRetAdb1 / Estado");

    //    if (LigarDesligar == "0") {
    //      digitalWrite(relayGPIOs[IndexRele.toInt()] , LOW);
    //      lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[IndexRele.toInt()]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //        if(digitalRead(relayGPIOs[0])){
    //           Firebase.setString(firebaseData,F("/Digitais/VAdb1/Estado"), "Desligado");
    //        }
    //        if(digitalRead(relayGPIOs[1])){
    //           Firebase.setString(firebaseData,F("/Digitais/ VRetAdb1 / Estado"), "Desligado");
    //        }
    //
    //    }
    //    if (LigarDesligar == "1") {
    //      digitalWrite(relayGPIOs[IndexRele.toInt()] , HIGH);
    //      lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[IndexRele.toInt()]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //       if(digitalRead(relayGPIOs[1])){
    //           Firebase.setString(firebaseData,F("/Digitais/VAdb1/Estado"), "Ligado");
    //        }
    //        if(digitalRead(relayGPIOs[1])){
    //           Firebase.setString(firebaseData,F("/Digitais/ VRetAdb1 / Estado"), "Ligado");
    //        }
    //    }
    //----------Parte da comunicação com Firebase---------------------------//
    //  do{
    //      if(LigarDesligar=="0"||fireStatus1 == "Desligado" && ){
    //      digitalWrite(relayGPIOs[0],LOW);
    //      lastEvent = " " + dateTimeStr(now())+ " " + relayGPIOs[0]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //      }
    //      if(LigarDesligar=="1"||fireStatus1 == "Ligado"){
    //      digitalWrite(relayGPIOs[0],HIGH);
    //      lastEvent = " " + dateTimeStr(now())+ " " + relayGPIOs[0]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //      }
    //      if(LigarDesligar=="0"||fireStatus2 == "Desligado"){
    //      digitalWrite(relayGPIOs[1],LOW);
    //      lastEvent = " " + dateTimeStr(now())+ " " + relayGPIOs[0]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //      }
    //      if(LigarDesligar=="1"||fireStatus2 == "Ligado"){
    //      digitalWrite(relayGPIOs[1],HIGH);
    //      lastEvent = " " + dateTimeStr(now())+ " " + relayGPIOs[0]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //      }
    //  }while(IndexRele.toInt()<2);
    //
    //  if(IndexRele.toInt()<2){
    //
    //    if (LigarDesligar == "1") {
    //      digitalWrite(relayGPIOs[IndexRele.toInt()] , HIGH);
    //      lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[IndexRele.toInt()]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //    }
    //    if (LigarDesligar == "0") {
    //      digitalWrite(relayGPIOs[IndexRele.toInt()] , LOW);
    //      lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[IndexRele.toInt()]  + " " + LigarDesligar + " ";
    //      handleRelayStatus();
    //    }
    //   }


    server.send(200, "text/plain", String(digitalRead (relayGPIOs[IndexRele.toInt()])));
    log("WebRelaySet", "Cliente: " + ipStr(server.client().remoteIP()) +
        " [" + s + "]");
  }
}

// Reset Schedule intervals
//    scheduleChk("", 0);
//    s = String(digitalRead(IndexRele.toInt()))  + "&" +
//        dateTimeStr(now())              + "&" +
//        lastEvent;
//    server.send(200, F("text/plain"), s);
//    log(F("WebRelaySet"), "Cliente: " + ipStr(server.client().remoteIP()) +
//        " [" + s + "]");





void handleConfig() {
  // Config
  File file = SPIFFS.open(F("/Config.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();

    // Atualiza conteúdo dinâmico
    s.replace(F("#id#")     , id);
    s.replace(F("#ledOn#")  ,  ledOn ? " checked" : "");
    s.replace(F("#ledOff#") , !ledOn ? " checked" : "");
    s.replace(F("#ssid#")   , ssid);
    s.replace(F("#pw#")   , pw);
    s.replace(F("#referencia#")   , referencia);
    s.replace(F("#horamanual#")   , horamanual);
    s.replace(F("#fuso#")   , String(fuso));
    s.replace(F("#autenticacao#")      , (fuso ? " checked" : ""));
    s.replace(F("#softap#")       , (softap ? " checked" : ""));
    s.replace(F("#agendamento#")    , agendamento);


    // Send data
    server.send(200, F("text/html"), s);
    log("Config - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, F("text/plain"), F("Config - ERROR 500"));
    log(F("Config - ERRO lendo arquivo"));
  }
}

void handleConfigSave() {
  // Grava Config
  // Verifica número de campos recebidos
  Serial.println(server.args());
#ifdef ESP8266
  // ESP8266 gera o campo adicional "plain" via post
  if (server.args() == 10) {
#else
  // ESP32 envia apenas os 4 campos
  if (server.args() == 9) {
#endif
    String s;

    // Grava id
    s = server.arg("id");
    s.trim();
    if (s == "") {
      s = id;
    }
    strlcpy(id, s.c_str(), sizeof(id));

    // Grava ssid
    s = server.arg("ssid");
    s.trim();
    strlcpy(ssid, s.c_str(), sizeof(ssid));

    // Grava pw
    s = server.arg("pw");
    s.trim();
    if (s != "") {
      strlcpy(pw, s.c_str(), sizeof(pw));
    }

    // Grava referencia
    s = server.arg("referencia");
    s.trim();
    if (s != "") {
      strlcpy(referencia, s.c_str(), sizeof(referencia));
    }

    // Grava horamanual
    s = server.arg("horamanual");
    s.trim();
    if (s != "") {
      strlcpy(horamanual, s.c_str(), sizeof(horamanual));
    }

    fuso = server.arg("fuso").toInt();
    autenticacao = server.arg("autenticacao").toInt();
    softap = server.arg("softap").toInt();

    // Grava agendamento
    s = server.arg("agendamento");
    s.trim();
    if (s != "") {
      // Atualiza senha, se informada
      strlcpy(agendamento, s.c_str(), sizeof(agendamento));
    }

    // Grava ledOn
    ledOn = server.arg("led").toInt();

    // Atualiza LED
    ledSet();

    // Grava configuração
    if (configSave()) {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração salva.');history.back()</script></html>"));
      log("ConfigSave - Cliente: " + ipStr(server.client().remoteIP()));
    } else {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Falha salvando configuração.');history.back()</script></html>"));
      log(F("ConfigSave - ERRO salvando Config"));
    }
  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>"));
  }
}

void handleReconfig() {
  // Reinicia Config
  configReset();

  // Atualiza LED
  ledSet();

  // Grava configuração
  if (configSave()) {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração reiniciada.');window.location = '/'</script></html>"));
    log("Reconfig - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Falha reiniciando configuração.');history.back()</script></html>"));
    log(F("Reconfig - ERRO reiniciando Config"));
  }
}

void handleReboot() {
  // Reboot
  File file = SPIFFS.open(F("/Reboot.htm"), "r");
  if (file) {
    server.streamFile(file, F("text/html"));
    file.close();
    log("Reboot - Cliente: " + ipStr(server.client().remoteIP()));
    delay(100);
    ESP.restart();
  } else {
    server.send(500, F("text/plain"), F("Reboot - ERROR 500"));
    log(F("Reboot - ERRO lendo arquivo"));
  }
}

void handleFileList() {
  // File list
  if (!pwdNeeded() || chkWebAuth()) {
    File file = SPIFFS.open(F("/FileList.htm"), "r");
    if (file) {
      file.setTimeout(100);
      String  s = file.readString(),
              sort = "",
              files[DIRECTORY_MAX_FILES];
      file.close();
      File dir = SPIFFS.open("/");   //   Dir dir = SPIFFS.openDir("/");
      File file = dir.openNextFile();
      byte b = 0;
      while (file) {
        files[b] = "<li>" + String(file.name()) + " - " + String(file.size() / 1024.0, 2) + "kb" + "</li>";
        b++;
        yield();
        file = dir.openNextFile();
      }
      // Sort entries
      sortArray(files, sort);
      // Replace markers
      s.replace(F("#files#")   , "<ul>" + sort + F("</ul>"));
      s.replace(F("#fsSpace#") , fsSpaceStr());
      // Send data
      server.send(200, F("text/html"), s);
      log(F("WebFileList"), "Cliente: " + ipStr(server.client().remoteIP()));
    } else {
      server.send(500, F("text/plain"), F("FileList - ERROR 500"));
      logFile(F("WebFileList"), F("ERRO lendo arquivo"), true);
    }
  }
}

void handleLog() {
  // Log
  String files[DIRECTORY_MAX_FILES];
  String f;
  if (chkWebAuth()) {
    File file = SPIFFS.open(F("/Log.htm"), "r");
    if (file) {
      file.setTimeout(100);
      String s = file.readString();
      file.close();
      File dir = SPIFFS.open(F("/Log/"));//Dir dir = SPIFFS.openDir(F("/Log/"));
      byte b = 0;
      while (dir) {  // while (dir.next()) {
        f = String(dir.name()).substring(5);
        files[b] = "<li><a href='logFileGet?l=" + f.substring(3, 4) + "'>" + f + F("</a> - ") +
                   String(dir.size() / 1024.0, 2) + F("kb ") +
                   (logDay() == f.substring(3, 4).toInt() ? "(A)" : "") + F("</li>");
        b++;
      }
      String sort;
      if (files[0] == "") {
        // No entries
        sort = F("<li><i>Nenhum arquivo</i></li>");
      } else {
        // Sort entries
        sortArray(files, sort);
      }
      // Replace markers
      s.replace(F("#logFiles#"), "<ul>" + sort + F("</ul>"));
      s.replace(F("#fsSpace#") , fsSpaceStr());
      // Send data
      server.send(200, F("text/html"), s);
      log(F("WebLog"), "Client: " + ipStr(server.client().remoteIP()));
    } else {
      server.send(500, F("text/plain"), F("LogList - ERROR 500"));
      logFile(F("WebLogList"), F("ERRO lendo arquivo"), true);
    }
  }
}

void handleLogGet() {
  // Memory Log download
  if (chkWebAuth()) {
    byte bFn;
    String s = deviceID() +
               F(" - Log em Memoria\r\nData/Hora;Tipo;Mensagem\r\n");
    for (bFn = logIndex; bFn < LOG_ENTRIES; bFn++) {
      if (logStr[bFn] != "") {
        s += logStr[bFn] + F("\r\n");
      }
    }
    for (bFn = 0; bFn < logIndex; bFn++) {
      if (logStr[bFn] != "") {
        s += logStr[bFn] + F("\r\n");
      }
    }
    server.sendHeader(F("Content-Disposition"), "filename=\"" +
                      deviceID() + F("LogMemoria.csv\""));
    server.send(200, F("text/csv"), s);
    log(F("WebLogGet"), "Client: " + ipStr(server.client().remoteIP()));
  }
}

void handleLogFileGet() {
  // File Log download
  if (chkWebAuth()) {
    String s = server.arg("l");
    if (s != "") {
      File file = SPIFFS.open("/Log/Dia" + s + F(".csv"), "r");
      if (file) {
        server.sendHeader(F("Content-Disposition"), "filename=\"" +
                          deviceID() + F("LogDia") + s + F(".csv\""));
        server.streamFile(file, "text/csv");
        file.close();
        log(F("WebLogFileGet"), "Client: " + ipStr(server.client().remoteIP()));
      } else {
        server.send(500, F("text/plain"), F("LogFileGet - ERROR 500"));
        log(F("WebLogFileGet"), F("ERRO lendo arquivo"));
      }
    } else {
      server.send(500, F("text/plain"), F("LogFileGet - ERROR Bad parameter 500"));
      log(F("WebLogFileGet"), F("ERRO parametro incorreto"));
    }
  }
}

void handleLogReset() {
  // Memory Log reset
  if (chkWebAuth()) {
    // Delete log
    logDelete();
    // Send data
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Log em Memória excluído.');window.location = 'log';</script></html>"));
    log(F("WebLogReset"), "Cliente: " + ipStr(server.client().remoteIP()));
    logFile(F("WebLogReset"), "Cliente: " + ipStr(server.client().remoteIP()));
  }
}

void handleLogFileReset() {
  // File Log reset
  if (chkWebAuth()) {
    // Delete log files
    logFileDelete();
    // Send data
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Log em Arquivo excluído.');window.location = 'log';</script></html>"));
    log(F("WebLogFileReset"), "Cliente: " + ipStr(server.client().remoteIP()));
    logFile(F("WebLogFileReset"), "Cliente: " + ipStr(server.client().remoteIP()));
  }
}

void handleCSS() {
  // Arquivo CSS
  File file = SPIFFS.open(F("/Style.css"), "r");
  if (file) {
    // Define cache para 3 dias
    server.sendHeader(F("Cache-Control"), F("public, max-age=172800"));
    server.streamFile(file, F("text/css"));
    file.close();
    log("CSS - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, F("text/plain"), F("CSS - ERROR 500"));
    log(F("CSS - ERRO lendo arquivo"));
  }
}

//--------------------------------

void FireBaseSet() {

  if (WiFi.status() == WL_CONNECTED) {
    if (Firebase.getString(firebaseData, "/Digitais/VAdb1/Estado")) {
      fireStatus[0] = firebaseData.stringData();
      if (fireStatus[0] == "Desligado") {
        digitalWrite(relayGPIOs[0] , LOW);
      } else if (fireStatus[0] == "Ligado") {
        digitalWrite(relayGPIOs[0] , HIGH);
      }

      if (Firebase.getString(firebaseData, "/Digitais/VRetAdb1/Estado")) {
        fireStatus[1] = firebaseData.stringData();
        if (fireStatus[1] == "Desligado") {
          digitalWrite(relayGPIOs[1] , LOW);
        } else if (fireStatus[1] == "Ligado") {
          digitalWrite(relayGPIOs[1] , HIGH);
        }
      }
    }
  }
}

   void  FireBaseStatus() {
 if (WiFi.status() == WL_CONNECTED && releSetFlag==1) {
      String p;

       if(digitalRead(relayGPIOs[0])==0){
      json.set("Estado/", "Desligado");
      Firebase.updateNode(firebaseData, "/Digitais/VAdb1/", json);
      }else{json.set("Estado/", "Ligado");
      Firebase.updateNode(firebaseData, "/Digitais/VAdb1/", json);
      }

   if(digitalRead(relayGPIOs[1])==0){
      json.set("Estado/", "Desligado");
      Firebase.updateNode(firebaseData, "/Digitais/VRetAdb1/", json);
      }else{json.set("Estado/", "Ligado");
      Firebase.updateNode(firebaseData, "/Digitais/VRetAdb1/", json);
      }
  releSetFlag=0;
  }
}






// Setup -------------------------------------------
void setup() {

  // Velocidade para ESP32
  Serial.begin(115200);

  Firebase.setDoubleDigits(5);

  // Configura WiFi para ESP32
  WiFi.setHostname(deviceID().c_str());
  WiFi.softAP(deviceID().c_str(), "ESP32ESP32");
  log("WiFi AP " + deviceID() + " - IP " + ipStr(WiFi.softAPIP()));

  // Habilita roteamento DNS
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNSSERVER_PORT, "*", WiFi.softAPIP());

  // WiFi Station
  WiFi.begin("IF_CATARINENSE", "ifcatarinense");
  log("Conectando WiFi " + String("IF_CATARINENSE"));
  byte b = 0;
  while (WiFi.status() != WL_CONNECTED && b < 20) {
    b++;
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    // WiFi Station conectado
    Serial.println("");
    Serial.print("WiFi conected. IP: ");
    Serial.println(WiFi.localIP());
    Firebase.begin("https://cursofb-d8836-default-rtdb.firebaseio.com/", "BIcBAhezSHlG0xtoeJqCPa6zmSWySypFjLPowkeh");
    log("WiFi conectado (" + String(WiFi.RSSI()) + ") IP " + ipStr(WiFi.localIP()));

  } else {
    log(F("WiFi não conectado"));
  }


  // SPIFFS                     SPI Flash File System
  if (!SPIFFS.begin()) {
    log(F("SPIFFS ERRO"));
    while (true);
  }

  // Lê configuração
  configRead();

  // Incrementa contador de inicializações
  bootCount++;
  //configReset(); // testes
  // Salva configuração
  configSave();

  // LED

  for (int i = 0; i <= NUM_RELAYS; i++) {
    pinMode(relayGPIOs[i], OUTPUT);
    //digitalWrite(relayGPIOs[i], LOW);
  }
  ledSet();



  // WebServer
  server.on(F("/Relay.htm")       , handleRelay);
  server.on(F("/relayStatus") , handleRelayStatus);
  server.on(F("/relaySet")    , handleRelaySet);

  server.on(F("/Config.htm")    , handleConfig);
  server.on(F("/FileList.htm")    , handleFileList);
  server.on(F("/ConfigSave"), handleConfigSave);
  server.on(F("/Reconfig.htm")  , handleReconfig);
  server.on(F("/Reboot.htm")    , handleReboot);
  server.on(F("/Log.htm")         , handleLog);
  server.on(F("/LogReset")    , handleLogReset);
  server.on(F("/LogFileReset"), handleLogFileReset);
  server.on(F("/LogGet")      , handleLogGet);
  server.on(F("/LogFileGet")  , handleLogFileGet);
  server.on(F("/style.css")       , handleCSS);

  server.onNotFound(handleHome);
  server.collectHeaders(WEBSERVER_HEADER_KEYS, 1);
  server.begin();

  // Pronto
  log(F("Pronto"));
  //  timeNTP();
  //  hold(1000);

}

// Loop --------------------------------------------
void loop(){
  // WatchDog ----------------------------------------
  yield();

  // DNS ---------------------------------------------
  dnsServer.processNextRequest();

  // Web ---------------------------------------------
  server.handleClient();

  //Firebase pegando os status-----------------------


  FireBaseStatus();
  FireBaseSet();
  

 
  //      if (LigarDesligar.toInt() == 0) {
  //        statusAnterior[IndexRele.toInt()] = "Desligado";
  //      } else {
  //        statusAnterior[IndexRele.toInt()] = "Ligado";
  //      }

}
