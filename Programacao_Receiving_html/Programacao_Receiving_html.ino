//Entega do Código do Projeto de Controle de Nível - CT Embarcados

//Inclusão da bibliotecas:
#include <Ultrasonic.h> //Biblioteca para realizar calcular distância com o sensor ultrassônico 
#include <Adafruit_BME280.h> //Bilbioteca utilizada para realizar as medições de temperatura, pressão e umidade com o sensor BME280
#include <LiquidCrystal.h> //Biblioteca que torna mais simples utilizar o display LCD
/*********************************************************** Página HTML *****************************************************/
#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <HTTP_Method.h>
#include <WebServer.h>

////////////////////////////////
#include <AsyncTCP.h>
#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>
#include <Arduino.h>
/////////////////////////////////

///////////////////////////////////
const char* PARAM_INPUT_1 = "input1";
float vazaoHTML; //= atoi(input1);
//const char* PARAM_INPUT_2 = "input2";
bool ligaHTML; //= atoi(input2);
const char* PARAM_INPUT_3 = "input3";
//////////////////////////////////////

/*COLOCAR O NOME DA REDE WIFI E A SENHA!!!:*/
const char* ssid = "NOME_REDE";  // Enter SSID here
const char* password = "NOME_SENHA";  //Enter Password here

WebServer server(80);
/*********************************************************** Página HTML *****************************************************/

const int S5=33, S6=35, S7=34, S8=10, LEDG1=21, LEDY2=22, LEDY3=23, LEDY4=19, LEDY5=18, LEDY6=5, LEDB7=32, LEDB8=39, LEDB9=11, LEDB10=4, LEDB11=16, releCafe=12,
releTV=13, releVal=9, pwmb=17; //Atribuição dos pinos dos dispositivos aos respectivos números de porta na ESP32
//CORREÇÃO: de acordo com o esquemático, os pino do sensor BME280 seriam slc=11, sda=12; contudo o código agora segue a ligação em que o sensor está ligado às saídas I2C da ESP 32

LiquidCrystal lcd(6,7,8,15,2,0); //Declaração dos pinos do LCD para a função

Ultrasonic sensorUltra (26, 25); //Declaração dos pinos do ultrassônico para a função

//Definição de variáveis
int i, j = 0;
double tempo = 0;
byte limNivel = 0;
float distancia, vazaoLitros, temperatura, umidade, pressao = 0;
String estado;

#define SEALEVELPRESSURE_HPA (1013.25) //Pressão ao nível do mar
Adafruit_BME280 bme; // I2C

const int pwmbPin = 16;  //Pino do PWM da bomba

// setting PWM properties
const int freq = 10000;
const int pwmbChannel = 0;
const int resolution = 8;

//Funções criadas
bool pinBotaoS5Retencao();
byte pinBotaoS7Apertado();
void controleVazao();
void medicaoNivel();                     
void medeAmb();
void cronometro();


void setup() 
  {
    pinMode(S5, INPUT_PULLUP); //BOTAO S5 (Liga a máquina)
    pinMode(S6, INPUT_PULLUP); //BOTAO S6 (Desliga a máquina)
    pinMode(S7, INPUT_PULLUP); //BOTAO S7 (+Vazão)
    pinMode(S8, INPUT_PULLUP); //BOTAO S8 (-Vazão)
    pinMode(LEDG1, OUTPUT); //LED GREEN 1
    //LEDS que correspondem a vazão
    pinMode(LEDY2, OUTPUT); //LED YELLOW 2
    pinMode(LEDY3, OUTPUT); //LED YELLOW 3
    pinMode(LEDY4, OUTPUT); //LED YELLOW 4
    pinMode(LEDY5, OUTPUT); //LED YELLOW 5
    pinMode(LEDY6, OUTPUT); //LED YELLOW 6
    //LEDS que correspondem ao nível do reservatório 
    pinMode(LEDB7, OUTPUT); //LED BLUE 7
    pinMode(LEDB8, OUTPUT); //LED BLUE 8
    pinMode(LEDB9, OUTPUT); //LED BLUE 9
    pinMode(LEDB10, OUTPUT); //LED BLUE 10
    pinMode(LEDB11, OUTPUT); //LED BLUE 11
    pinMode(releCafe, OUTPUT); //RELÉ CAFETEIRA
    pinMode(releTV, OUTPUT); //RELÉ TV
    pinMode(releVal, OUTPUT); //RELÉ VÁLVULA
    pinMode(pwmb, OUTPUT); //PWM BOMBA

    // configure LED PWM functionalitites
    ledcSetup(pwmbChannel, freq, resolution);
    
    // attach the channel to the GPIO to be controlled
    ledcAttachPin(pwmb, pwmbChannel);
  }
  

void loop() 
 {
  server.handleClient(); //função padrão do Webserver
 }

//Caso, conexão com a Internet tenha sido efetuada com sucesso:
void handle_OnConnect() 
 {
   if (pinBotaoS5Retencao() || ligaHTML == 1) //Se a retenção do botão liga for pressionado ou o sistema for ligado pela página HTML
    {
      bool estadoBotaoS6 = digitalRead(S6); //declaração da varável de estado para o botão S6
      if(estadoBotaoS6 == LOW && ligaHTML != 1) //Se o botão de desligar for acionado, interrompe o programa (sempre sai da função de loop) e desliga o LED verde
       {
        digitalWrite(LEDG1,LOW);
        estado = "Desligado";
        return;
       }
      else //Se o botão de desligar não estiver sendo pressionado, 
       {
        digitalWrite(LEDG1,HIGH); //liga o LED verde
        estado = "Ligado";
        controleVazao(); //introduz a função que controla a vazão da bomba
        medicaoNivel(); // instroduz a função de medição do nível do reservatório
        medeAmb(); //introduz afunção que mede as variáveis do ambiente (temperatura, pressão e umidade)
       }
    }
   else
    {
      digitalWrite(LEDG1,LOW);
      estado = "Desligado";
      return;
    }
  
    server.send(200, "text/html", SendHTML(estado, vazaoLitros, distancia, temperatura, pressao, umidade, tempo)); //Envia para a página HTML os valores das variáveis especificadas
 }



//Caso, conexão com a Internet NÃO tenha sido efetuada com sucesso:
void handle_NotFound()
 {
  server.send(404, "text/plain", "Not found");
 }
 
   
bool pinBotaoS5Retencao() //função que faz a característica de retensão do botão de ligar (S5)
 {
   #define tempoDebounce 50 //(tempo para eliminar o efeito Bounce EM MILISEGUNDOS)

   bool estadoBotao;
   static bool estadoBotaoAnt; 
   static bool estadoRet = true;
   static unsigned long delayBotao = 0;

   if ( (millis() - delayBotao) > tempoDebounce ) 
    {
       estadoBotao = digitalRead(S5);
       if ( estadoBotao && (estadoBotao != estadoBotaoAnt) ) 
       {
          estadoRet = !estadoRet;
          delayBotao = millis();
       }
       estadoBotaoAnt = estadoBotao;
    }

   return estadoRet;
  }



void controleVazao()
    {
     int cont; //contador que determina a opção de vazão
     int vetLEDY[5]= {LEDY2, LEDY3, LEDY4, LEDY5, LEDY6}; //Vetor de LEDs que indicam a vazão
     static unsigned long delayBotao; //delay utilizado para evitar várias somas ao contador com um único pressionamento do botão
     
       //FUNCIONAMENTO DOS BOTÕES +/- PWM
       if(digitalRead(S7) == LOW && (cont < 6)) //Condção para incremento do valor do contador
        {
          if((millis()- delayBotao) == 200) //Inscrementa o contator só no instante t = 200millis
            cont++;
          if((millis()- delayBotao) > 400) //Reinicia a contagem do tempo após 400 millis
            delayBotao = millis();
        }
        
       else if(cont >= 6) //Limita o contador as onze opções de vazão (0 a 10), reiniciando a contagem do 0 em caso de ultrapassagem
          cont = 0;
        
       if(digitalRead(S8) == LOW && (cont >= 1)) //Condição para decremento do valor do contador
        {
          if((millis()- delayBotao) == 200) //Decrementa o contator só no instante t = 200millis
            cont--;
          if((millis()- delayBotao) > 400) //Reinicia a contagem do tempo após 400 millis
            delayBotao = millis();
        }


       if(limNivel == 1) //Condição para o controle automático do nível no limite do tanque
        {
          static unsigned long delayTV;
            
          digitalWrite(releVal, HIGH); //Liga a válvula de esvaziamento 
          ledcWrite(pwmbChannel, 0); //Desliga a bomba de enchimento
          
          if(i >= 6)
            i = 0;
           else
            {
             digitalWrite(vetLEDY[i], LOW); //Apaga os LEDs de vazão
             i++;
            }

          if((millis()- delayTV) < 1200000) //Permanece com a TV ligada, desde do momento de echimento do tanque, por 20min
            digitalWrite(releTV, HIGH); //Liga o relé da Televisão
          else
            digitalWrite(releTV, LOW); //Adequação para novo ciclo
            delayTV = millis();  //Desliga o relé da TV
            
        }

       else //Se o nível está OK, faz um dos dois modos de controle
        {
         if(limNivel == 0)
          {
           static unsigned long delayCafe;
           if((millis()- delayCafe) < 600000) //Permanece com a Cafeteira ligada por 10min desde o momento do nivel minimo
             digitalWrite(releCafe, HIGH); //Liga o relé da Cafeteira
           else
            {
             digitalWrite(releCafe, LOW); //Desliga o relé da Cafeteira
             delayCafe = millis();  //Adequação para novo ciclo
            }
          }
          
         if(vazaoHTML != 0.00) //CONTROLE PELA PÁGINA HTML se houver informações sendo passadas
          {
           float valPWM = (vazaoHTML*2.55);  //((vazaoHTML(%)*255)/100)
           vazaoLitros = (vazaoHTML*0.045); //((vazaoHTML(%)*4,5L/min)/100)
           ledcWrite(pwmbChannel, valPWM); //Coloca no PWM a porcentagem de vazão especificada pela página HTML
           cronometro(); //Inicia a cronometragem, em tempo real, do funcionamento da bomba pela ativação na página HTML
          } 
          
          
         else //CONTROLE PELOS BOTÕES +/- PWM
          {
           switch (cont) 
           //Análise do valor final do contador e criação de casos para cada uma das 11 opções, de modo 
           //que o primeiro caso corresponde a nenhuma vazão, os que estão entre o primeiro e o último são os que tem saída crescente
           //linearmente varíavel e o último caso corresponde a vazão máxima da bomba
              {
                case 0:                    
                 if(i >= 6)
                    i = 0;
  
                 else
                  {
                   digitalWrite(vetLEDY[i], LOW);
                   i++; 
                  }
  
                 ledcWrite(pwmbChannel, 0);
                 vazaoLitros = 0.0;
                break;
              
                case 1:
                 digitalWrite(LEDY2, HIGH);
                 ledcWrite(pwmbChannel, 51);
                 cronometro(); //Inicia a cronometragem da operação da bomba, pelo acionamento dos botões, em tempo real
                 vazaoLitros = 0.9; //Atribui a vazão em L/min para o envio para a página HTML
                break;
                
                case 2:
                 digitalWrite(LEDY2, HIGH);
                 digitalWrite(LEDY3, HIGH);
                 ledcWrite(pwmbChannel, 102);
                 cronometro();
                 vazaoLitros = 1.8;
                break;
    
                case 3:
                 digitalWrite(LEDY2, HIGH);
                 digitalWrite(LEDY3, HIGH);
                 digitalWrite(LEDY4, HIGH);
                 ledcWrite(pwmbChannel, 153);
                 cronometro(); 
                 vazaoLitros = 2.7;
                break;
    
                case 4:
                 digitalWrite(LEDY2, HIGH);
                 digitalWrite(LEDY3, HIGH);
                 digitalWrite(LEDY4, HIGH);
                 digitalWrite(LEDY5, HIGH);
                 ledcWrite(pwmbChannel, 204);
                 cronometro(); 
                 vazaoLitros = 3.6;
                break;
    
                case 5:
                 digitalWrite(LEDY2, HIGH);
                 digitalWrite(LEDY3, HIGH);
                 digitalWrite(LEDY4, HIGH);
                 digitalWrite(LEDY5, HIGH);
                 digitalWrite(LEDY6, HIGH);
                 ledcWrite(pwmbChannel, 255);
                 cronometro(); 
                 vazaoLitros = 4.5;
                break;
              }
           
          }
       }
    }



void medicaoNivel()
   {
      distancia = sensorUltra.Ranging(CM); //Medida pela função da distância em cm
      int vetLEDB[5]= {LEDB7, LEDB8, LEDB9, LEDB10, LEDB11}; //Vetor de LEDs que indicam o nível
      //Foram divididos 5 intervalos para o nível do tanque, acionado LEDs na sequência em que elas aumentam
      if(distancia < 5.8 && distancia != 0) 
       {
        digitalWrite(LEDB7, HIGH);
        limNivel = 2;
       }
      if(distancia >= 5.8 && distancia < 11.6)
       {
        digitalWrite(LEDB7, HIGH);
        digitalWrite(LEDB8, HIGH);
        limNivel = 2;
       }
      if(distancia >= 11.6 && distancia < 17.4)
       {
        digitalWrite(LEDB7, HIGH);
        digitalWrite(LEDB8, HIGH);
        digitalWrite(LEDB9, HIGH);
        limNivel = 2;
       }
      if(distancia >= 17.4 && distancia < 23.2)
       {
        digitalWrite(LEDB7, HIGH);
        digitalWrite(LEDB8, HIGH);
        digitalWrite(LEDB9, HIGH);
        digitalWrite(LEDB10, HIGH);
        limNivel = 2;
       }
      if(distancia >= 23.2 && distancia <= 29)
       {
        digitalWrite(LEDB7, HIGH);
        digitalWrite(LEDB8, HIGH);
        digitalWrite(LEDB9, HIGH);
        digitalWrite(LEDB10, HIGH);
        digitalWrite(LEDB11, HIGH);
        limNivel = 2;
       }
      if(distancia > 29)
        limNivel = 1;
      
      if(distancia == 0)
       {
        limNivel = 0;
        
        if(j >= 6)
         j = 0;
         
        else
         {
          digitalWrite(vetLEDB[j], LOW);
          j++; 
         }
       }
   }



void medeAmb()
  {
    temperatura = (bme.readTemperature()); //Medida da temperatura
    pressao = (bme.readPressure()/100.0F); //Medida da pressão
    umidade = (bme.readHumidity()); //Medida da umidade
  }

void cronometro()
  {
    static unsigned long delayTempo;
    if(millis() - delayTempo == 1000)
     {
      tempo++;
      lcd.clear();
      lcd.print("Cronometro");
      lcd.setCursor(0, 1);
      lcd.print("Tempo: ");
      lcd.setCursor(7, 1);
      lcd.print(tempo);
      delayTempo = millis();
     }
  }


String SendHTML(String a, float b, float c, float d, float e, float f, int g) //Página HTML
 {
    String ptr = "<!DOCTYPE html>";
    ptr +="<html>";
    ptr +="<script>\n";
    ptr +="setInterval(loadDoc,1000);\n";
    ptr +="function loadDoc() {\n";
    ptr +="var xhttp = new XMLHttpRequest();\n";
    ptr +="xhttp.onreadystatechange = function() {\n";
    ptr +="if (this.readyState == 4 && this.status == 200) {\n";
    ptr +="document.body.innerHTML =this.responseText}\n";
    ptr +="};\n";
    ptr +="xhttp.open(\"GET\", \"/\", true);\n";
    ptr +="xhttp.send();\n";
    ptr +="}\n";
    ptr +="</script>\n";
    ptr +="<head>";
    //Título da Página:
    ptr +="<title>Controle de Nível</title>";
    ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
    ptr +="<style>";
    ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
    ptr +="body{margin: 0px;} ";
    ptr +="h1 {margin: 50px auto 30px;} ";
    ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
    ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
    ptr +=".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  
    //Nomes das variáveis e as cores dos textos
    ptr +=".estado .reading{color: #000000;}";
    ptr +=".vazaoLitros .reading{color: #000000;}";
    ptr +=".distancia .reading{color: #000000;}";
    ptr +=".temperatura .reading{color: #000000;}";
    ptr +=".umidade .reading{color: #000000;}";
    ptr +=".pressao .reading{color: #000000;}";
    ptr +=".tempo .reading{color: #000000;}";
   
    ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
    ptr +=".data{padding: 10px;}";
    ptr +=".container{display: table;margin: 0 auto;}";
    ptr +=".icon{width:65px}";
    ptr +="</style>";
    ptr +="</head>";
    ptr +="<body>";

    ptr +="<h1>Controle de Nível</h1>";
    
    //////////////////////////////////////////
    ptr +="<form action="/get">";
    ptr +="Vazão <input type="text" name="vazaoHTML">";
    ptr +="<input type="submit" value="Enviar">";
    ptr +="</form><br>";
    ptr +="<form action="/get">";
    ptr +="On(1)/Off(0) <input type="text" name="ligaHTML">";
    ptr +="<input type="submit" value="Enviar">";
    ptr +="</form><br>";
    ptr +="<form action="/get">";
    ////////////////////////////////////////
    
    //Título da Página, que aparece no título da "BODY":
    ptr +="<div class='container'>";
  
    //Escrita de "div"s que mostram os valores lidos pelas variáveis:
    ptr +="<div class='data estado'>";                    //Nome da class.
    ptr +="<div class='side-by-side text'>Estado:</div>";  //Nome da variável que aparece na leitura.
    ptr +="<div class='side-by-side reading'>";
    ptr +=(String)estado;                            //Envio da variável, para ela poder ser lido o valor dela.
    ptr +="<span class='superscript'></span></div>"; //Unidade da Variável
    ptr +="</div>";
  
    ptr +="<div class='data vazaoLitros'>";                    
    ptr +="<div class='side-by-side text'>Vazão:</div>";  
    ptr +="<div class='side-by-side reading'>";
    ptr +=(float)vazaoLitros;                                    
    ptr +="<span class='superscript'>L/min</span></div>"; 
    ptr +="</div>";

    ptr +="<div class='data distancia'>";                 
    ptr +="<div class='side-by-side text'>Distância:</div>";  
    ptr +="<div class='side-by-side reading'>";
    ptr +=(float)distancia;                                   
    ptr +="<span class='superscript'>cm</span></div>"; 
    ptr +="</div>";

    ptr +="<div class='data temperatura'>";                   
    ptr +="<div class='side-by-side text'>Temperatura:</div>";  
    ptr +="<div class='side-by-side reading'>";
    ptr +=(float)temperatura;                                
    ptr +="<span class='superscript'>ºC</span></div>"; 
    ptr +="</div>";

    ptr +="<div class='data umidade'>";                   
    ptr +="<div class='side-by-side text'>Umidade:</div>";  
    ptr +="<div class='side-by-side reading'>";
    ptr +=(float)umidade;                                  
    ptr +="<span class='superscript'>%</span></div>"; 

    ptr +="<div class='data pressao'>";                    
    ptr +="<div class='side-by-side text'>Pressão:</div>";  
    ptr +="<div class='side-by-side reading'>";
    ptr +=(float)pressao;                                  
    ptr +="<span class='superscript'>hPa</span></div>"; 
    ptr +="</div>";

    ptr +="<div class='data tempo'>";                  
    ptr +="<div class='side-by-side text'>Tempo:</div>"; 
    ptr +="<div class='side-by-side reading'>";
    ptr +=(double)tempo;                                   
    ptr +="<span class='superscript'>s</span></div>"; 
    ptr +="</div>";
    ptr +="</div>";
    ptr +="</body>";
    ptr +="</html>";
    return ptr;
 }

