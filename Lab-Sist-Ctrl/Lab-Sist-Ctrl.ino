// Defino las librerias a utilizar
#include <ArduinoJson.h> // Para el manejo y conversión de JSON a vartiables
#include <UIPEthernet.h> // Para el manejo del Shield Ethernet
#include <Servo.h> // Para el manejo de los Servos.
//----------------------------------------------//
// Defino el servidor y el Puerto
#define server_port 22 // 80
EthernetServer server = EthernetServer(server_port);
//----------------------------------------------//
// Defino variables para Json
#define const_mje 256
#define const_valores 236

//char Mensaje_recibido[const_mje] = {0}; // Mensaje recibido por ethernet. (Comando + JSON)
//char valores_recibidos[const_valores] = {0}; // JSON recibido.
//----------------------------------------------//
//------- Defino mensajes de error predeterminado
// una variable error int
int Errores = 0;
// 0 - Sin errores
// 1 - Laboratorio Parado
// 2 - Laboratorio incorrecto.
// 3 - Error de comunicación UART
//----------------------------------------------//
//------- Defino variables globales
  #define Numero_laboratorio 0
// Nombres para los pines GPIO
  // Bomba de agua
  #define Pin_Bomba 9
  //  Habilitadores
  #define Pin_Agua 8
  #define Pin_Luz 7
  // Leds
  #define Led_0 10 // Indicador 0
  #define Led_1 11 // Indicador 1
  // Manejo del serial
  #define velocidad_serial_default 9600

//////// VAriables Locales de json //////////////
// Estado
  int num_Lab=Numero_laboratorio;
  bool subLab=0;// false = Aliassing / true= Control de posición
  bool iniLab=false; // false = desactivado / true= activado
// Velocidad
  uint32_t velocidad_serial=9600;
// Llaves
  bool llave_0=false;
  bool llave_1=false;
//Analógicos
  int  analogico_0=0;
  int  analogico_1=0;
  int  analogico_2=0;

// UART
  char* input_ether_char={0};
  String input_ether_string="Holis rx"; // variable que se usa para guardar el mje recibido de ethernet.
  String input_ctrl_string=""; //  variable que se usa para guardar el mje recibido del arduino del Lab de control de posición.
//--- Variables auxiliares ---//
// banderas
  // bool bandera_rep=0; // bandera para limitar la cantidad de repeticiones de mensaje de lab incorrecto
  // bool sentido=0;
  // int repeticiones=0; // contador para repeticiones.

//--- FUNCIONES --- //
  void asignarPines(int pul_0,int pul_1,int pul_2,int pul_3,int led_0,int led_1,int led_2,int led_3);
  void Comunicacion(bool Serial_out);
  void Control();

//----------------------------------------------//
// Realizo las configuraciones iniciales
void setup() {
  uint8_t myMAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // defino mac del dispositivo.
  IPAddress myIP(192,168,1,108); // 172.20.5.140 Defino IP Address del dispositivo.
  Serial.begin(velocidad_serial_default); // Inicializo Puerto serial 0
  Serial1.begin(velocidad_serial_default); // Inicializo Puerto serial 1 - Arduino Ctrl de posición - GPIO 19 tx 18 rx
  while (!Serial) continue;
  Ethernet.begin(myMAC,myIP);  // Inicializo libreria Ethernet
  server.begin(); // Start to listen
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
  Serial.println("Port:" + (String)server_port);
// ------- Defino GPIO MODE (INPUT/OUTPUT)--------  //
  input_ctrl_string.reserve(200);
  pinMode(Led_0,OUTPUT);
  pinMode(Led_1,OUTPUT);
  pinMode(Pin_Agua,OUTPUT);
  pinMode(Pin_Bomba,OUTPUT);
  pinMode(Pin_Luz,OUTPUT); 
  //------ Definir estados iniciales ------//
}

void loop(){
  //////////// Strings de comunicación /////////////
  char Mensaje_recibido[const_mje] = {0}; // Mensaje recibido por ethernet. (Comando + JSON)
  char valores_recibidos[const_valores] = {0}; // JSON recibido.  // Wait for an incomming connection
  EthernetClient client = server.available();
  if(client){ // Si tengo un cliente conectado
    while (client.available()){
      client.readBytesUntil('\r', Mensaje_recibido, sizeof(Mensaje_recibido)); // Tomo el mensaje recibido.
      strncpy(valores_recibidos,&Mensaje_recibido[15],(sizeof(Mensaje_recibido)-15));
      Serial.print("Mensaje Recibido: " + (String)Mensaje_recibido);
      //----------- GET ----------- //
      if (strstr(Mensaje_recibido, "GET /HTTP/1.1") != NULL) { // Compruebo si llega un GET, respondo valores actuales
        StaticJsonDocument<256> doc;
        JsonArray Estado = doc.createNestedArray("Estado");
          Estado.add(num_Lab);
          Estado.add(subLab);
          Estado.add(iniLab);
        JsonArray Llaves = doc.createNestedArray("Llaves");
          Llaves.add(llave_0);
          Llaves.add(llave_1);
        JsonArray Analogicos = doc.createNestedArray("Analogicos");
          Analogicos.add(analogico_0);
          Analogicos.add(analogico_1);
          Analogicos.add(analogico_2);        
        doc["velocidad"] = velocidad_serial;
        doc["Posicion"] = input_ctrl_string;
        
        doc["Error"] = Errores;

        Serial.print(F("Sending: "));
        serializeJson(doc, Serial);
        Serial.println();
  // Write response headers
        client.println(F("HTTP/1.0 200 OK"));
        client.println(F("Content-Type: application/json"));
  // client.println(F("Connection: close"));
        client.print(F("Content-Length: "));
        client.println(measureJsonPretty(doc));
        client.println();
  // Write JSON document
        serializeJsonPretty(doc, client);
      }
      //------------ POST ----------//
      if (strstr(Mensaje_recibido, "POST /HTTP/1.1") !=NULL) { // Compruebo si llega un POST y respondo, habilito banderas.
        Errores=0;
//        if(bandera_rep==1)bandera_rep=0;//reinicio bandera de repetición cuando tengo un mje nuevo.
        Serial.println("Solicitud de escritura recibida");
        client.println();
        client.println(F("HTTP/1.1 200 OK"));
        client.println();
        StaticJsonDocument<256> doc; // Creo un doc de json
        DeserializationError error = deserializeJson(doc, valores_recibidos); // Deserializo
        if (error){ // Analizo posibles errores.
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        JsonArray Estado = doc["Estado"];
          num_Lab = Estado[0]; // 0 [Sist Dig], 1 [Sist Control], 2[Telecomunicaciones], 3[Fisica]
          subLab = Estado[1]; // 0 [Sub Lab 1 con Cyclone II], 1 [SubLab 2 con Cyclone IV]
          iniLab = Estado[2]; // 1 [Inicia Experimento], 0 [Finaliza Experimento]
        if(num_Lab==Numero_laboratorio){ // Si es el numero de laboratorio asigno variables recibidas.
          velocidad_serial = doc["velocidad"];
          JsonArray Pulsadores = doc["Pulsadores"];
            pulsador_0 = Pulsadores[0];
            pulsador_1 = Pulsadores[1];
            pulsador_2 = Pulsadores[2];
            pulsador_3 = Pulsadores[3];
          input_ether_char = doc["Serial"];
          input_ether_string = (String)input_ether_char;
        }
      }
    }
  }
  else{ // Si no está el cliente enviando algo, sigo haciendo lo que corresponde.
    Control();
  }
}

/**
 * @brief Funcion utilizada para el control de laboratorios. "Se analiza las variables de entrada y se elije la placa a utilizar"
 *
 */
void Control(){
  if(num_Lab==Numero_laboratorio and bandera_rep==0){ // Si es el lab correcto - Control de numero de sublab.
    if (subLab and iniLab){ // UART
      Serial.println("Sub-Lab: UART");
      Serial1.begin(velocidad_serial); // asigno la velocidad al serial 1
      asignarPines(Pulsador_II_0,Pulsador_II_1,Pulsador_II_2,Pulsador_II_3,Led_II_0,Led_II_1,Led_II_2,Led_II_3);
      Serial1.print(input_ether_string);
      bandera_rep=1;
    }
    else if (!subLab and iniLab){ // I2C
      Serial.println("Sub-Lab: I2C");
      Serial2.begin(velocidad_serial); // asigno la velocidad al serial 2
      asignarPines(Pulsador_IV_0,Pulsador_IV_1,Pulsador_IV_2,Pulsador_IV_3,Led_IV_0,Led_IV_1,Led_IV_2,Led_IV_3);
      Serial2.print(input_ether_string);
      bandera_rep=1;
    }
    else{ // Laboratorio parado.
      if(bandera_rep==0){
        Serial.println("Laboratorio Parado");
        bandera_rep=1;
        Errores =1;
      }
    }
  }
  else{ // Laboratorio incorrecto
    if(bandera_rep==0){
      Serial.println("Laboratorio incorrecto");
      bandera_rep = 1;
      Errores=2;
    }
  }
  if(Serial1.available()){ //Controlo lo que se envía por el serial 1.
    input_ctrl_string=Serial1.readString();
    Serial.println("Se recibio de serial 1: " + input_ctrl_string);
  }
  else if(Serial2.available()){ //Controlo lo que se envía por el serial 1.
    input_ctrl_string=Serial2.readString();
    Serial.println("Se recibio de serial 2: " + input_ctrl_string);
  }
  if(repeticiones<10){// Se busca que no se repita mas de 10 veces una acción.
    repeticiones++;
  }
  else{
    bandera_rep=1;
    repeticiones=0;
  }
}

/**
 * @brief Selecciona las salidas y entradas en base a la placa elegida
 *
 * @param pul_0 - Pulsador 0
 * @param pul_1 - Pulsador 1
 * @param pul_2 - Pulsador 2
 * @param pul_3 - Pulsador 3
 * @param led_0 - Indicador 0
 * @param led_1 - Indicador 1
 * @param led_2 - Indicador 2
 * @param led_3 - Indicador 3
 */
void asignarPines(int pul_0,int pul_1,int pul_2,int pul_3,int led_0,int led_1,int led_2,int led_3){
//    Serial.println("Escribo pulsadores");
    digitalWrite(pul_0,pulsador_0);
    digitalWrite(pul_1,pulsador_1);
    digitalWrite(pul_2,pulsador_2);
    digitalWrite(pul_3,pulsador_3);
//    Serial.println("Leo indicadores");
    indicador_0 = digitalRead(led_0);
    indicador_1 = digitalRead(led_1);
    indicador_2 = digitalRead(led_2);
    indicador_3 = digitalRead(led_3);
}
