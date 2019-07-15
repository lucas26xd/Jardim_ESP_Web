#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define VALVULA 2
#define SSID "LUCAS"
#define PSWD "889161639397"
#define URL_API "http://dataservice.accuweather.com/forecasts/v1/daily/1day/32239?apikey=aZQSo11M9j9Vi2rlYB8zYHiZAyWe9dwW&language=pt-BR"
#define PAGE "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"><meta name=\"author\" content=\"Lucas Santos\"><meta name=\"generator\" content=\"Atom, Arduino IDE\"><meta name=\"description\" content=\"Página para controle de irrigação de um jardim vertical.\"><meta name=\"keywords\" content=\"Jardim, HTML, CSS, Arduino, ESP 01, ESP8266\"><title>Controle de Jardim</title><link rel=\"shortcut icon\" href=\"https://cdn.icon-icons.com/icons2/681/PNG/512/plant_icon-icons.com_61040.png\"><style type=\"text/css\">html {clear: both; margin: 0 auto; font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif; font-size: 2.5vw; color: white; text-align: center; background-color: #B9F6CA; padding: 5px; background: url(http://data.1freewallpapers.com/download/growing-plant.jpg) no-repeat; background-size: 105%;}button {position: absolute; left: 0; border: none; color: white; width: 97%; padding: 1.8vw 3vw; margin: 0.5vw; text-align: center; text-decoration: none; font-size: 16px;}#liga {top: 2.8vw; background-color: #4CAF50;}#desliga {top: 10.8vw; background-color: #F44336;}#liga:hover{background-color: #3C9F40; cursor: pointer;}#desliga:hover{background-color: #E43326; cursor: pointer;}#texto {position: absolute; left: 2vw;}#regar, #clima {display: inline-block; position: absolute; top: 0; padding: 12vw; margin: 3vw; border-width: 2px; border-color: black; border-style: solid; width:5vw; height:8vw; border-radius: 10px 20px;}#regar {left: 0;}#clima {right: 0;}footer {position:absolute; top: 44vw; width:99%;}</style></head><body><br><br><main role=\"main\" class=\"container\"><div id=\"fundo\"><div id=\"regar\"><a href=\"/ligar\"><button id=\"liga\">LIGA</button></a><a href=\"/desligar\"><button id=\"desliga\">DESLIGA</button></a><br><br>"

DynamicJsonBuffer jsonBuffer;

IPAddress IP(10, 0, 0, 12);
IPAddress GATEWAY(10, 0, 0, 1);
IPAddress MASCARA(255, 255, 255, 0);

ESP8266WebServer SERVER(80);

boolean ligada = false;
int icon = 3, temperatura = 32;
String ano = "2018", statusPrevisao = "Parcialmente ensolarado",
       urlPrevisao = "https://m.accuweather.com/pt/br/sobral/32239/daily-weather-forecast/32239?day=1&lang=pt-br";

void requisitaAPI() {
  //HTTPClient client;
  //client.begin(URL_API);

  //if (client.GET() > 0) {
    //String payload = client.getString();
    String payload = "{\"Headline\":{\"EffectiveDate\":\"2018-07-25T07:00:00-03:00\",\"EffectiveEpochDate\":1532512800,\"Severity\":7,\"Text\":\"Menos úmido amanhã\",\"Category\":\"humidity\",\"EndDate\":\"2018-07-25T19:00:00-03:00\",\"EndEpochDate\":1532556000,\"MobileLink\":\"http://m.accuweather.com/pt/br/sobral/32239/extended-weather-forecast/32239?lang=pt-br\",\"Link\":\"http://www.accuweather.com/pt/br/sobral/32239/daily-weather-forecast/32239?lang=pt-br\"},\"DailyForecasts\":[{\"Date\":\"2018-07-24T07:00:00-03:00\",\"EpochDate\":1532426400,\"Temperature\":{\"Minimum\":{\"Value\":69.0,\"Unit\":\"F\",\"UnitType\":18},\"Maximum\":{\"Value\":94.0,\"Unit\":\"F\",\"UnitType\":18}},\"Day\":{\"Icon\":2,\"IconPhrase\":\"Predominantemente ensolarado\"},\"Night\":{\"Icon\":34,\"IconPhrase\":\"Predominantemente aberto\"},\"Sources\":[\"AccuWeather\"],\"MobileLink\":\"http://m.accuweather.com/pt/br/sobral/32239/daily-weather-forecast/32239?day=1&lang=pt-br\",\"Link\":\"http://www.accuweather.com/pt/br/sobral/32239/daily-weather-forecast/32239?day=1&lang=pt-br\"}]}";

    JsonObject& json = jsonBuffer.parseObject(payload);
    urlPrevisao = json["Headline"]["MobileLink"].as<String>();
    temperatura = int((json["DailyForecasts"][0]["Temperature"]["Maximum"]["Value"].as<int>() - 32) / 1.8);
    statusPrevisao = json["DailyForecasts"][0]["Day"]["IconPhrase"].as<String>();
    icon = json["DailyForecasts"][0]["Day"]["Icon"].as<int>();
    ano = json["Headline"]["EffectiveDate"].as<String>();
    ano = ano.substring(0, ano.indexOf("-"));
  //}
  //client.end();
}

void montaPagina() {
  requisitaAPI();

  String page = "<h3 id=\"texto\">Status da válvula: <span style=\"color:";
  page += (ligada ? "green" : "red");
  page += "\">";
  page += (ligada ? "Ligada" : "Desligada");
  page += "</span></h3>";

  page += "</div><div id=\"clima\"><h4 id=\"texto\" style=\"top: 0\">Previsão do Tempo</h4><br>";

  page += "<img src=\"https://developer.accuweather.com/sites/default/files/";
  page += (icon < 10 ? "0" + String(icon) : String(icon));
  page += "-s.png\" alt=\"\" style=\"position: absolute; top: 7vw; left: 8vw; width: 10vw\">";

  String page1 = "<h5 style=\"position: absolute; top: 12vw; left: 2vw;\"><span style=\"color:#80D8FF\">";
  page1 += statusPrevisao;
  page1 += "</span></h5>";

  page1 += "<h4 id=\"texto\" style=\"top: 18vw\">Temperatura: <span style=\"color:#FFE57F\">";
  page1 += temperatura;
  page1 += "ºC</span></h4>";
  /*
    page1 += "<h4 id=\"texto\" style=\"top: 20vw\">Umidade: <span style=\"color:#CFD8DC\">";
    page1 += umidade;
    page1 += "%</span></h4>";
  */
  page1 += "<h6 style=\"font-size: 1.1vw; position: absolute; bottom: 0; right: 1vw\"><a target=\"_blank\" href=\"";
  page1 += urlPrevisao;
  page1 += "\">Análise Detalhada -></a></h4>";

  String page2 = "</div><br><br><footer><p><small>Desenvolvido por <b>Lucas Santos</b> &copy; ";
  page2 += ano;
  page2 += " - Todos os direitos reservados.</small></p></footer></div></main></body></html>";;

  SERVER.send(200, "text/html", PAGE + page + page1 + page2);
}

void ligar() {
  ligada = true;
  digitalWrite(VALVULA, HIGH);
  montaPagina();
}

void desligar() {
  ligada = false;
  digitalWrite(VALVULA, LOW);
  montaPagina();
}

void setup() {
  pinMode(VALVULA, OUTPUT);
  digitalWrite(VALVULA, LOW);

  WiFi.config(IP, GATEWAY, MASCARA);
  WiFi.begin(SSID, PSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //digitalWrite(VALVULA, !digitalRead(VALVULA));
  }

  SERVER.on("/", montaPagina);
  SERVER.on("/ligar", ligar);
  SERVER.on("/desligar", desligar);
  SERVER.begin();
}

void loop() {
  SERVER.handleClient();
}
