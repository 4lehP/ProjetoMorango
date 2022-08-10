

/*************ln("sream begin error, %s\n\n", stream.errorReason().c_str());E COISAS
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
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#define ValvulaLigada LOW
#define ValvulaDesligada HIGH

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#define RELAY_PIN 3

int relayGPIOsteste[RELAY_PIN] = {15, 13, 2};
String StringPortax[RELAY_PIN] = {"D1", "D2", "D3"};
String relayCodigoT[RELAY_PIN] =       {"VAgua", "VRetAdb1", "VAdb1"};
//---------PARTE DAS CONFIGURAÇÕES DAS ENTRADAS DAS VálvulaS----------//

const byte      LED_ON                  = HIGH;
const byte      LED_OFF                 = LOW;

#define NUM_RELAYS  12
#define NUM_RELAYS2  2

int relayGPIOs[NUM_RELAYS] =          {2, 13, 14, 27, 26, 25, 33, 32, 16, 17, 4, 15};
String relayDescricao[NUM_RELAYS] =  {"Válvula Agua", "Válvula Retorno Adubo 1", "Válvula Adubo 1", "Válvula Retorno Adubo 2", "Válvula Adubo 2", "Válvula Canteiro 6", "Válvula Canteiro 1", "Válvula Canteiro 2", "Válvula Canteiro 3", "Válvula Canteiro 4", "Válvula Canteiro 5", "Bomba 1"};
String relayCodigo[NUM_RELAYS] =       {"VAgua", "VRetAdb1", "VAdb1", "VRetAdb2", "VAdb2", "VL6", "VL1", "VL2", "VL3", "VL4", "VL5", "VBomba"};
String relayCodigoD [NUM_RELAYS] = {"D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12"};

static time_t horas;
int atualizaAgenda = 10;
int horaMais;
int setflag = 1;
// Tamanho do Objeto JSON
//const   size_t    JSON_SIZE            = 404; declarado em def.h

//--------------------FIREBASE-----------------------------------------//
FirebaseData firebaseData;
FirebaseData firebaseDataRelatorio;
FirebaseData stream;
FirebaseData streamStatus;
FirebaseData streamConfig;
FirebaseData streamAgendamento;
FirebaseData streamRelatorio;
FirebaseConfig config;
FirebaseJson jsonL;
FirebaseJson jsonD;
FirebaseJson jsonS;
FirebaseJson jsonCheck;
FirebaseJson json;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseData fare;
unsigned long sendDataPrevMilli = 0;
unsigned long countin = 0;
byte bFn;
int releSetFlag = 99;           //A flag para atualizar o status no firebase quando muda o estado no webserver local do esp32
String fireStatus[NUM_RELAYS2] = {"Desligado", "Desligado"};
unsigned long sendDataPrevMillis = 0;
unsigned long dataMillis = 0; // Variaveis para os firebases  set/status/config
int count = 0;
int counts = 0;
String uid;
uint32_t idleTimeForStream = 15000;


#define USER_EMAIL "morangos@gmail.com"   // email para conectar com o firebase
#define USER_PASSWORD "123456mo"          // senha do email para o firebase

// As variáveis para o projeto do firebase
#define API_KEY "AIzaSyB5BAbxcc0IemgmzU0yILI3OAiv5JQbj1I"
#define DATABASE_URL "https://cursofb-d8836-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "DATABASE_SECRET"

static time_t hora;  // variavel para pegar a hora atual


//----------------PROTÓTIPO DAS FUNÇÕES---------------------------------------------//

String softwareStr(); // Retorna nome do software
void  configReset(); // As configurações padrão
boolean configRead(); // Ler as configurações
boolean configSave(); // Salvar as mudanças de configurações
void handleHome(); // As info que vai ser mostradas na pagina Home
void handleRelay(); //
void handleRelayStatus(); //Leitura dos estados dos Reles
void handleRelaySet(); //Recebimento da mudança de estado dos reles
void handleConfig(); //Era aonde acontecia a atualização dos dados da configuração
void handleConfigSave(); //Grava as configurações
void handleReconfig(); // Reinicia a config
void handleReboot(); //Le o tanto de reboot que foi dado
void handleFileList();// Abre o arquivo pra ler
void handleLog(); //
void handleLogGet(); // Via baixar o arquivo
void handleLogFileGet(); //
void handleLogReset(); //
void handleLogFileReset(); //
void handleCSS(); //
void FireBaseSet();// A cada pedido de mudança no FireBase é atualizado os estados dos reles
void  FireBaseStatus();//Atualização do estado dos reles
void ConexaoFireBase(); //Atualizado quando o esp32 esta online


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
char              email[30];     // Rede email
char              senha[30];       // Senha do email
char              pw[30];       // Senha da Rede WiFi
char              agendamento[5000];  //#sched# adendamento de rotinas
char              configuracao[5000]; //mostra as ultimas config
char            horarioAtualiza[20]; // hora que vai dar o reboot no esp

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


// Funções de Configuração ------------------------------
void  configReset() {
  // Define configuração padrão
  strlcpy(id, "IF_CATARINENSE", sizeof(id));
  strlcpy(ssid, "ifcatarinense", sizeof(ssid));
  strlcpy(pw, " ", sizeof(pw));
  strlcpy(email, " ", sizeof(email));
  strlcpy(senha, " ", sizeof(senha));
  strlcpy(referencia, "00,00,00,00,00,0000", sizeof(referencia));
  ledOn     = false;
  bootCount = 0;
  fuso = 3;
  strlcpy(horamanual, "00,00,00,00,00,0000", sizeof(horamanual));
  autenticacao = true;
  softap = true;
  strlcpy(agendamento, "0000", sizeof(agendamento)); //agendamento
  strlcpy(configuracao, " ", sizeof(configuracao));
  strlcpy(horarioAtualiza, "14:00", sizeof(horarioAtualiza));

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
    strlcpy(email, jsonConfig["email"]  | "", sizeof(email));
    strlcpy(pw, jsonConfig["pw"]      | "", sizeof(pw));
    strlcpy(senha, jsonConfig["senha"]      | "", sizeof(senha));
    strlcpy(referencia, jsonConfig["referencia"]      | "", sizeof(referencia));
    ledOn     = jsonConfig["led"]     | false;
    bootCount = jsonConfig["boot"]    | 0;
    fuso =  jsonConfig["fuso"]    | 13;
    strlcpy(horamanual, jsonConfig["horamanual"]      | "", sizeof(horamanual));
    autenticacao = jsonConfig["autenticacao"]    | true;
    softap = jsonConfig["softap"]    | true;
    strlcpy(agendamento, jsonConfig["agendamento"]      | "", sizeof(agendamento));
    strlcpy(configuracao, jsonConfig["configuracao"]      | "", sizeof(configuracao));
    strlcpy(horarioAtualiza, jsonConfig["hora de reboot"]      | "", sizeof(horarioAtualiza));

    file.close();

    log(F("\nLendo config:"));
    logFire(F("Config"), F("Lendo config:"));
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
    jsonConfig["boot"]  = bootCount;
    jsonConfig["ssid"]  = ssid;
    jsonConfig["email"]  = email;
    jsonConfig["pw"]    = pw;
    jsonConfig["senha"]    = senha;
    jsonConfig["referencia"]    = referencia;
    //jsonConfig["softap"] = softap;
    jsonConfig["agendamento"] = agendamento;
    jsonConfig["configuracao"] = configuracao;
    jsonConfig["horario de reboot"] = horarioAtualiza;

    serializeJsonPretty(jsonConfig, file);
    file.close();

    log(F("\nGravando config:"));
    logFire(F("Config"), F("Gravando config:"));
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
    s.replace(F("#reference#")       , referencia);
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
    logFire(F("Home"), F("Home - ERRO lendo arquivo"));
  }
}

//--------------------Relays-Server--------------------------------------

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
      logFire(F("WebRelay"), F("ERRO lendo arquivo"));
    }
  }
}



void handleRelayStatus() {   // Atualização dos status a cada 5 segundos no site
  // Relay status
  if (!pwdNeeded() || chkWebAuth()) {

    String s = dateTimeStr(now())   + "&" ;

    for (int i = 0; i < NUM_RELAYS; i++) {
      s += String(digitalRead(relayGPIOs[i]))  + "&" ; //Leitura de todas as Válvulas

    }
    s += lastEvent;

    server.send(200, F("text/plain"), s);

    log(F("WebRelayStatus"), "Cliente: " + ipStr(server.client().remoteIP()) +
        " [" + s + "]"); //enviar todos reles em uma msg

  }

}


void handleRelaySet() {   //Mudança de acordo com o botão
  // Set Relay status
  if (!pwdNeeded() || chkWebAuth()) {
    String s = server.arg("set");
    String LigarDesligar = s.substring(1, 2);//primeira parte para liga/desliga = 0 e 1
    String IndexRele = s.substring(2); //Qual a posição do vetor
    if (LigarDesligar.toInt()) {
      digitalWrite(relayGPIOs[IndexRele.toInt()] , 0);
    } else {
      digitalWrite(relayGPIOs[IndexRele.toInt()] , 1);


    }

    lastEvent = " " + dateTimeStr(now()) + " " + relayGPIOs[IndexRele.toInt()]  + " " + LigarDesligar + " ";


    releSetFlag = IndexRele.toInt();

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



//------------------------------------------------------------------------------------------


void handleConfig() {
  // Config
  //  FireBaseSetConfig();
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
    s.replace(F("#configuracao#")    , configuracao);

    // Send data
    server.send(200, F("text/html"), s);
    log("Config - Cliente: " + ipStr(server.client().remoteIP()));

  } else {
    server.send(500, F("text/plain"), F("Config - ERROR 500"));
    log(F("Config - ERRO lendo arquivo"));
    logFire(F("Config"), F("Config - ERRO lendo arquivo"));
  }
}

void handleConfigSave() {
  // Grava Config
  // Verifica número de campos recebidos
  Serial.println(server.args());
  // ESP32 envia apenas os 4 campos
  if (server.args() >= 1 && server.args() <= 5 ) {
    String s;

    // Grava id
    s = server.arg("adress");
    s.trim();
    if (s == "") {
      s = email;
    }
    strlcpy(email, s.c_str(), sizeof(email));
    // Grava id
    s = server.arg("senha");
    s.trim();
    if (s == "") {
      s = senha;
    }
    strlcpy(email, s.c_str(), sizeof(email));


    s.trim();
    if (s == "") {
      s = email;
    }
    strlcpy(email, s.c_str(), sizeof(email));
    // Grava id
    s = server.arg("senha");
    s.trim();
    if (s == "") {
      s = senha;
    }
    strlcpy(senha, s.c_str(), sizeof(senha));

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
    //    s = server.arg("horamanual");
    //    s.trim();
    //    if (s != "") {
    //      strlcpy(horamanual, s.c_str(), sizeof(horamanual));
    //    }

    //    fuso = server.arg("fuso").toInt();
    //    autenticacao = server.arg("autenticacao").toInt();
    //    softap = server.arg("softap").toInt();

    // Grava agendamento
    //    s = server.arg("agendamento");
    //    s.trim();
    //    if (s != "") {
    //      // Atualiza senha, se informada
    //      strlcpy(agendamento, s.c_str(), sizeof(agendamento));
    //    }

    // Grava configuração
    if (configSave()) {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração salva.');history.back()</script></html>"));
      log("ConfigSave - Cliente: " + ipStr(server.client().remoteIP()));

    } else {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Falha salvando configuração.');history.back()</script></html>"));
      log(F("ConfigSave - ERRO salvando Config"));
      logFire(F("ConfigSave"), F("ConfigSave - ERRO salvando Config"));
    }
  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>"));
  }
}

void handleReconfig() {
  // Reinicia Config
  configReset();



  // Grava configuração
  if (configSave()) {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração reiniciada.');window.location = '/'</script></html>"));
    log("Reconfig - Cliente: " + ipStr(server.client().remoteIP()));

  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Falha reiniciando configuração.');history.back()</script></html>"));
    log(F("Reconfig - ERRO reiniciando Config"));
    logFire(F("Reconfig"), F("Reconfig - ERRO reiniciando Config"));
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
    logFire(F("Reboot"), F("Reboot - ERRO lendo arquivo"));
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
      logFire(F("WebFileList"), F("ERRO lendo arquivo"));
    }

  }
}

void handleLog() {
  // Log
  String files[DIRECTORY_MAX_FILES];
  String f;
  // if (chkWebAuth()) {
  File file = SPIFFS.open(F("/Log.htm"), "r");
  if (file) {
    file.setTimeout(500);
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
    logFire(F("WebLogList"), F("ERRO lendo arquivo"));

  }
  // }
}

void handleLogGet() {
  // Memory Log download
  //if (chkWebAuth()) {
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

  // }
}

void handleLogFileGet() {
  // File Log download
  //if (chkWebAuth()) {
  String s = "1";
  String IP = ipStr(server.client().remoteIP());
  //if (s != "") {
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
    logFire(F("WebLogFileGet"), F("ERRO lendo arquivo"));

  }
  //    } else {
  //      server.send(500, F("text/plain"), F("LogFileGet - ERROR Bad parameter 500"));
  //      log(F("WebLogFileGet"), F("ERRO parametro incorreto"));
  //    }
  //}
}
void handleLogReset() {
  // Memory Log reset
  // if (chkWebAuth()) {
  // Delete log
  logDelete();
  // Send data
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Log em Memória excluído.');window.location = 'log';</script></html>"));
  log(F("WebLogReset"), "Cliente: " + ipStr(server.client().remoteIP()));
  logFile(F("WebLogReset"), "Cliente: " + ipStr(server.client().remoteIP()));

  // }
}

void handleLogFileReset() {
  // File Log reset
  //if (chkWebAuth()) {
  // Delete log files
  logFileDelete();
  // Send data
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Log em Arquivo excluído.');window.location = 'log';</script></html>"));
  log(F("WebLogFileReset"), "Cliente: " + ipStr(server.client().remoteIP()));
  logFile(F("WebLogFileReset"), "Cliente: " + ipStr(server.client().remoteIP()));

  // }
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
//---------------------------FIREBASE--------------------------------

void logFire(const String &type, const String &msg) {
  FirebaseJson jsonl;
  FirebaseData fbdoo;
  String dataDeAgora = dateTimeStr(now());
  String hora = dataDeAgora.substring(11);
  String dataCalendario = dataDeAgora.substring(0, 11);

  jsonl.add(dataCalendario,  hora + ";" + type + ";" + msg);
  if (WiFi.status() == WL_CONNECTED  && Firebase.ready() )
  {
    Firebase.push(fbdoo, "Config/Relatorio/json", dataCalendario + " " +  hora + ";" + type + ";" + msg);
  }
}

void  FireBaseStatus() {
  if (WiFi.status() == WL_CONNECTED && releSetFlag < 12) {
    String p;

    jsonD.set("Estado/", "Desligado");
    jsonL.set("Estado/", "Ligado");


    int i = releSetFlag;


    if (digitalRead(relayGPIOs[i]) == 0) {
      Firebase.updateNode(firebaseData, "/Digitais/" + relayCodigo[i] , jsonD);
    } else {
      Firebase.updateNode(firebaseData, "/Digitais/" + relayCodigo[i], jsonL);
    }


    releSetFlag = 99;
  }
}


void FireBaseConfig() {
  jsonS.set("Estado", "Online");
  if (Firebase.ready() && (millis() - sendDataPrevMillis > idleTimeForStream || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    count++;
  }
  if (Firebase.ready())
  {
    if (!Firebase.readStream(streamConfig)) Serial.printf("sream read error, %s\n\n", streamConfig.errorReason().c_str());

    if (streamConfig.streamTimeout())
    {
      Serial.println("stream timed out, resuming...\n");

      if (!streamConfig.httpConnected()) {
        Serial.printf("error code: %d, reason: %s\n\n", streamConfig.httpCode(), streamConfig.errorReason().c_str());
      }

    }
    if (streamConfig.streamAvailable())
    {

      Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\nvalue, %s\n\n",
                    streamConfig.streamPath().c_str(),
                    streamConfig.dataPath().c_str(),
                    streamConfig.dataType().c_str(),
                    streamConfig.eventType().c_str(),
                    streamConfig.stringData().c_str());

      String caminho = streamConfig.streamPath().c_str();
      String mudanca = streamConfig.dataPath().c_str();
      String estado = streamConfig.stringData().c_str();

      for ( int i = 0; i < NUM_RELAYS; i++) {
        if (mudanca.indexOf(relayCodigo[i]) > 0) {
          if (estado.indexOf("Desligado") > 0)digitalWrite(relayGPIOs[i] , LOW);
          else if (estado.indexOf("Ligado") > 0)digitalWrite(relayGPIOs[i] , HIGH);

        }
      }
      if (mudanca.indexOf("Pull") > 0) {
        strlcpy(agendamento, estado.c_str(), sizeof(agendamento));
        schedule = agendamento;
        Serial.print(schedule);
        schedule.trim();
        schedule.toUpperCase();
        scheduleSet(schedule);
        logFire(F("Agendamento"), F(agendamento));
      }

      if (estado.indexOf("Offline") > 0) {
        Firebase.updateNode(firebaseData, "Config/ESP32", jsonS);
      }

    }
  }
}


void ConfigSchedule() {
  //-------------------ParteDoAgendamento--------------------------------

  // Schedule entries
  schedule = scheduleGet();   // pega os comandos salvos do schedule anterior
  // SET SCHEDULE ENTRIES - DEBUG ONLY
  time_t t = now() + 61;

  for (int i = 0; i < RELAY_PIN; i++) {
    schedule = "SH" + dateTimeStr( t      , false).substring(0, 16) + relayCodigo[i]  +
               "\nSL" +  dateTimeStr( t + 60 , false).substring(0, 16) + relayCodigo[i] +
               "\nMH" + dateTimeStr( t + 120, false).substring(8, 16) + relayCodigo[i] +
               "\nML" + dateTimeStr( t + 180, false).substring(8, 16) + relayCodigo[i] +
               "\nWH" +    weekday( t + 240) + " " + dateTimeStr( t + 240, false).substring(11, 16) + relayCodigo[i] +
               "\nWL" +    weekday( t + 300) + " " + dateTimeStr( t + 300, false).substring(11, 16) + relayCodigo[i] +
               "\nDH" + dateTimeStr( t + 360, false).substring(11, 16) + relayCodigo[i] +
               "\nDL" +  dateTimeStr( t + 420, false).substring(11, 16) + relayCodigo[i] +
               "\nIH" + "00:01" + relayCodigo[i] + "\nIL" + "00:01" + relayCodigo[i];
  }
  log(F("Boot"), F("Agendamento Ok"));
  logFire(F("Boot"), F("Agendamento Ok"));
}


// ---------------------Setup -------------------------------------------

void setup() {

  // Velocidade para ESP32
  Serial.begin(115200);

  //------------------ Configura WiFi para ESP32---------------------//

  WiFi.setHostname(deviceID().c_str());
  WiFi.softAP(deviceID().c_str(), "ESP32ESP32");
  log("WiFi AP " + deviceID() + " - IP " + ipStr(WiFi.softAPIP()));

  // Habilita roteamento DNS
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNSSERVER_PORT, "*", WiFi.softAPIP());

  // WiFi Station
  WiFi.begin("IF_CATARINENSE", "ifcatarinense");
  log("Conectando WiFi " + String(ssid));
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
    log("WiFi conectado (" + String(WiFi.RSSI()) + ") IP " + ipStr(WiFi.localIP()));
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    Firebase.begin(&config, &auth);

    // Comment or pass false value when WiFi reconnection will control by your code or third party library
    Firebase.reconnectWiFi(true);

    Firebase.setDoubleDigits(7);
    setSyncProvider(timeNTP);
    setSyncInterval(NTP_INT);

    if (timeStatus() != timeSet) {
      log(F("Boot"), F("Data/Hora ERRO"));
    }
  } else {
    // Soft AP mode, ignore date/time
    log(F("Boot"), F("Data/Hora nao definida"));
    logFire(F("Boot"), F("Data/Hora nao definida"));
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


  // LED

  for (int i = 0; i <= RELAY_PIN; i++) {
    pinMode(relayGPIOsteste[i], OUTPUT);

  }

  delay(500);
  for (int i = 0; i <= RELAY_PIN; i++) {
    digitalWrite(relayGPIOsteste[i], ValvulaDesligada); //Inicia todas portas em HIGH pro sistema desligar os reles. HIGH relé DESLIGADO

  }



  //---------------------FIREBASE-------------------------------------------

  if (!Firebase.beginStream(streamConfig, "/Config/")) {
    Serial.printf("sream config begin error, %s\n\n", streamConfig.errorReason().c_str());
  } else {
    Serial.println("sream config begin complete");
  }

  //---------------------------------WATCHDOG---------------------------//
  //configureWatchdog();

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
  //  server.on(F("/logSet")    , handleLogSet);
  server.on(F("/LogReset"), handleLogReset);
  server.on(F("/LogGet")      , handleLogGet);
  server.on(F("/LogFileGet")  , handleLogFileGet);
  server.on(F("/style.css")       , handleCSS);

  server.onNotFound(handleHome);
  server.collectHeaders(WEBSERVER_HEADER_KEYS, 1);
  server.begin();



  // Pronto
  log(F("Pronto"));
  logFire(F("ESP32"), F("Pronto"));
  //  timeNTP();
  //  hold(1000);
  ConfigSchedule();
}



// ------------------Loop --------------------------------------------

void loop() {

  yield();

  // Web ---------------------------------------------
  server.handleClient();



  // DNS ---------------------------------------------
  dnsServer.processNextRequest();

  //Firebase pegando os status-----------------------
  FireBaseStatus(); // Mudança no estado da valvula do server local
  FireBaseConfig(); // configurar o horario de reboot, agendamento, se o esp ta online

  for (int i = 0; i < RELAY_PIN; i++) {
    String s   = scheduleChk(schedule, relayGPIOsteste[i], StringPortax[i]); //StringPortax[i] //StringPortax[i] String que contem o nome da porta testada no schedule
    delay(500);
    if (s != "") {
      // Event detected
      lastEvent = (digitalRead(relayGPIOsteste[i]) ? "Ligado " : "Desligado ") +
                  s + " - " + dateTimeStr(now());
      log(F("Agendamento"), lastEvent);
      logFire(F("Agendamento"), lastEvent);
    }
  }

  // WatchDog ----------------------------------------
  hora = now();
  String horas = "";
  if (hour(hora) < 10) {
    s += '0';
  }
  horas += String(hour(hora)) + ':';
  if (minute(hora) < 10) {
    s += '0';
  }
  horas += String(minute(hora));

  if ( horas == horarioAtualiza ) {
    reboot();
  } else if (WiFi.status() == WL_CONNECTED && timeStatus() != timeSet) {
    reboot();
  }
}
