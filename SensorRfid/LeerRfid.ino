#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h> 

#define buzzer 6      //Pin para el buzzer
#define RST_PIN  9    //Pin para el reset del RC522
#define SS_PIN  8   //Pin para el SS (SDA) del RC522

MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el MFRC522
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // direccion MAC del Ethernet Shield

EthernetClient client;
int    HTTP_PORT   = 80;              // puerto del servidor web apache
String HTTP_METHOD = "GET";           // Metodo Http
char   HOST_NAME[] = "192.168.1.72";  // Direccion IP donde esta alojado el servidor web, PC
String PATH_NAME   = "/insertdatos.php"; //Directorio del archivo PHP para insertar Datos
String queryString = "?num='";    // Inicio del query


void setup() {
  Serial.begin(9600); //Iniciamos la comunicación  serial
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos  el MFRC522
  pinMode(buzzer, OUTPUT); //Configurar el buzzer para OUTPUT

  if(Ethernet.begin(mac) == 0){     // Inicia conexion Ethernet, Error si El dhcp no se puede acceder
    Serial.println("Error");
  }

}



void loop() {
  
  // perparar la llave, Todas las llaves se establecen a FFFFFFFFFFFFh desde fabric
  MFRC522::MIFARE_Key key; 
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  //inicio de Variables del llave
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Resetear el Loop ai no se detecta una tarjeta. Esto ahorra recursos si esta inactiva el sensor
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Selecciona una de los tarjetas
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("Tarjeta Detectada"));

  //-------------------------------------------
  
  Serial.print(F("Nombre: "));
      
  byte buffer1[18]; //arreglo de 18 bytes
  String cardname="";
  block = 4;  
  len = 18; 

  //-------------------------------------------Nombre
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //Al chile no entiendo porque es necesrio esto
  status = mfrc522.MIFARE_Read(block, buffer1, &len); // Leer (18) bytes del bloque (4) de la tarjeta, y guardarlo en el arreglo buffer1
   
  
  for (uint8_t i = 0; i < 16; i++)
  {
    if ((buffer1[i] != 32) && (buffer1[i] != 10))
    {
      cardname=cardname+String(char(buffer1[i]));
      Serial.write(buffer1[i]);
    }
  }
  cardname=cardname+"_";
  Serial.print(" ");

  //---------------------------------------- Apellido

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  status = mfrc522.MIFARE_Read(block, buffer2, &len);


  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] ); 
    if ((buffer2[i] != 32) && (buffer2[i] != 10)){
    cardname=cardname+String(char(buffer2[i]));
    }
  }

  //----------------------------------------

  queryString="?num='"+cardname+"'";

    // conectar al servidor web
    if(client.connect(HOST_NAME, HTTP_PORT)){
      // Si esta conectada:
      // enviar query
      client.println(HTTP_METHOD + " " + PATH_NAME + queryString + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      //cerrar conexion
      client.println("Connection: close");
      client.println(); // Cerrar header HTTP
      
      digitalWrite(buzzer, HIGH);   // aprender buzzer
      delay(100);            
      digitalWrite(buzzer, LOW);   // apagar buzzer     
    }
  queryString = "?num='";
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
