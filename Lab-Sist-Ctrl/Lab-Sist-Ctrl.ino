// Defino las librerias a utilizar
#include <ArduinoJson.h> // Para el manejo y conversión de JSON a vartiables
#include <UIPEthernet.h> // Para el manejo del Shield Ethernet
//----------------------------------------------//
// Defino el servidor y el Puerto
#define server_port 22 // 80
EthernetServer server = EthernetServer(server_port);
//----------------------------------------------//
// Defino variables para Json
#define const_mje 256// Mensaje recibido por ethernet. (Comando + JSON)
#define const_valores 236 // JSON recibido.
//----------------------------------------------//
//------- Defino mensajes de error predeterminado
uint8_t Errores = 0;
// 0 - Sin errores
// 1 - Laboratorio Parado
// 2 - Laboratorio incorrecto.
// 3 - Error de comunicación UART
//----------------------------------------------//
//------- Defino variables globales
  #define Numero_laboratorio 1
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
  #define limite_arrays 200
//////// VAriables Locales de json //////////////
// Estado
  int num_Lab=Numero_laboratorio;
  bool subLab=0;// false = Aliassing / true= Control de posición
  bool iniLab=false; // false = desactivado / true= activado
// - Sub Lab 1
// Habilitadores
  bool hab_agua=false;
  bool hab_luz=false;
//Frecuencias
  int  frec_agua=0;
  int  frec_luz=0;
// - Sub Lab 2
// Arrays 
 uint32_t Array_Velocidad_ang[limite_arrays]={0};
 uint32_t Array_Posicion[limite_arrays]={0};
 uint32_t Array_Tiempo[limite_arrays]={0};
// Valores
 uint32_t rx_Exitacion_tipo=0;
 uint32_t rx_Exitacion_frec=0;
 uint32_t rx_Regulacion_P=0;
 uint32_t rx_Regulacion_I=0;
 uint32_t rx_Regulacion_D=0;
 uint32_t rx_Planta_P0=0;
 uint32_t rx_Planta_P1=0;
 uint32_t rx_Planta_P2=0;
 uint32_t rx_Planta_P3=0;
 uint32_t rx_Planta_C0=0;
 uint32_t rx_Planta_C1=0;
 uint32_t rx_Planta_C2=0;
 uint32_t rx_Planta_K=0;
// Valores de control de posicion
 uint32_t rx_Velocidad_ang=0;
 uint32_t rx_Posicion=0;
 uint32_t rx_Tiempo=0;
 bool envio=0;
 int indice=0;
// UART
  char* input_ether_char={0};
  String input_ether_string=""; // variable que se usa para guardar el mje recibido de ethernet.
  String input_ctrl_string=""; //  variable que se usa para guardar el mje recibido del arduino del Lab de control de posición.
//--- Variables auxiliares ---//
// banderas
  // bool bandera_rep=0; // bandera para limitar la cantidad de repeticiones de mensaje de lab incorrecto
  // bool sentido=0;
  // int repeticiones=0; // contador para repeticiones.
//--- FUNCIONES --- //
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
        indice=0;
        StaticJsonDocument<256> doc;
        JsonArray Estado = doc.createNestedArray("Estado");
          Estado.add(num_Lab);
          Estado.add(subLab);
          Estado.add(iniLab);
        if(!subLab){
          JsonArray Habilitadores = doc.createNestedArray("Habilitadores");
            Habilitadores.add(hab_agua);
            Habilitadores.add(hab_luz);
          JsonArray Frecuencias = doc.createNestedArray("Frecuencias");
            Frecuencias.add(frec_agua);
            Frecuencias.add(frec_luz);
        }
        else{
          JsonArray Posicion = doc.createNestedArray("Posicion");
          JsonArray Velocidad_ang = doc.createNestedArray("Velocidad_ang");
          JsonArray Tiempo = doc.createNestedArray("Tiempo");
          for(int i=0;i<sizeof(limite_arrays);i++){
            Posicion.add(Array_Posicion[i]);
            Tiempo.add(Array_Tiempo[i]);
            Velocidad_ang.add(Array_Velocidad_ang[i]);
          }            
        }
        doc["Errores"] = Errores;
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
        if(envio) envio=false; // Solo se envia una vez los comandos al disp de control de posición.
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
          subLab = Estado[1]; // 0 [Sub Lab 1], 1 [SubLab 2]
          iniLab = Estado[2]; // 1 [Inicia Experimento], 0 [Finaliza Experimento]
        if(num_Lab==Numero_laboratorio){ // Si es el numero de laboratorio asigno variables recibidas.
          if(!subLab){
            JsonArray Habilitadores = doc.createNestedArray("Habilitadores");
              hab_agua = Habilitadores[0];
              hab_luz = Habilitadores[1];
            JsonArray Frecuencias = doc.createNestedArray("Frecuencias");
              frec_agua = Frecuencias[0];
              frec_luz = Frecuencias[1];
          }else{
            JsonArray Exitacion = doc.createNestedArray("Exitacion");
              rx_Exitacion_tipo = Exitacion[0];
              rx_Exitacion_frec = Exitacion[1];
            JsonArray Regulacion = doc.createNestedArray("Regulacion");
              rx_Regulacion_P = Regulacion[0];
              rx_Regulacion_I = Regulacion[1];
              rx_Regulacion_D = Regulacion[2];
            JsonArray Planta = doc.createNestedArray("Planta");
              rx_Planta_P0 = Planta[0];
              rx_Planta_P1 = Planta[1];
              rx_Planta_P2 = Planta[2];
              rx_Planta_P3 = Planta[3];
              rx_Planta_C0 = Planta[4];
              rx_Planta_C1 = Planta[5];
              rx_Planta_C2 = Planta[6];
              rx_Planta_K = Planta[7];
          }
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
  if(num_Lab==Numero_laboratorio){ // Si es el lab correcto - Control de numero de sublab.
    if (!subLab and iniLab){ // Aliassing, submuestreo y efecto stroboscópico.
      Serial.println("Sub-Lab 1");
      if(hab_agua and frec_agua>0){ // parlante.
        digitalWrite(Pin_Agua,HIGH);
        delay(500/frec_agua);
        digitalWrite(Pin_Agua,LOW);
        delay(500/frec_agua);
      }
      if(hab_luz and frec_luz>0){ // led.
        digitalWrite(Pin_Luz,HIGH);
        delay(500/frec_luz);
        digitalWrite(Pin_Luz,LOW);
        delay(500/frec_luz);        
      }
    }else if (subLab and iniLab){ // Control de posicion
      Serial.println("Sub-Lab 2");
      if(!envio){
      Serial.println("Envío data una vez");
      StaticJsonDocument<256> doc;
      JsonArray Exitacion = doc.createNestedArray("Exitacion");
        Exitacion.add(rx_Exitacion_tipo);
        Exitacion.add(rx_Exitacion_frec);
      JsonArray Regulacion = doc.createNestedArray("Regulacion");
        Regulacion.add(rx_Regulacion_P);
        Regulacion.add(rx_Regulacion_I);
        Regulacion.add(rx_Regulacion_D);
      JsonArray Planta = doc.createNestedArray("Planta");
        Planta.add(rx_Planta_P0);
        Planta.add(rx_Planta_P1);
        Planta.add(rx_Planta_P2);
        Planta.add(rx_Planta_P3);
        Planta.add(rx_Planta_C0);
        Planta.add(rx_Planta_C1);
        Planta.add(rx_Planta_C2);
        Planta.add(rx_Planta_K);
      // Serial.print(F("Sending: "));
      // serializeJson(doc, Serial);
      // Serial.println();
      // Write response headers
      Serial1.println(F("HTTP/1.0 200 OK"));
      Serial1.println(F("Content-Type: application/json"));
      Serial1.print(F("Content-Length: "));
      Serial1.println(measureJsonPretty(doc));
      Serial1.println();
      // Write JSON document
      serializeJsonPretty(doc, Serial1);
      envio=1;
      }
    }
    else{ // Laboratorio parado.
      Serial.println("Laboratorio Parado");
      Errores =1;
    }
  }else{ // Laboratorio incorrecto
      Serial.println("Laboratorio incorrecto");
      Errores=2;
  }
  if(Serial1.available()){ //Controlo lo que se envía por el serial 1.
  //  Serial1.readBytesUntil('\r', input_ctrl_string, sizeof(input_ctrl_string)); // Tomo el mensaje recibido.
    input_ctrl_string=Serial1.readString();
    Serial.println("Se recibio de serial 1: " + input_ctrl_string);
    Serial.println("Datos json guardados");
    // ver posible bandera.
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, input_ctrl_string); // Deserializo
    if (error){ // Analizo posibles errores.
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    JsonArray Exitacion = doc.createNestedArray("Exitacion");
      rx_Exitacion_tipo = Exitacion[0];
      rx_Exitacion_frec = Exitacion[1];
    if(indice<limite_arrays){
      Array_Posicion[indice]= doc["Posicion"];
      Array_Tiempo[indice]= doc["Tiempo"];
      Array_Velocidad_ang[indice] = doc["Velocidad"];
      indice++;
    }
  }
}