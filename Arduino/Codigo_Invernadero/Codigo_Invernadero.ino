#include <Keypad.h> // libreria necesaria para KeyPad
#include <Wire.h>  // libreria necesaria para bus I2C
#include <DHT.h> //Incluimos librería para sensor DHT 
#include <LiquidCrystal_I2C.h> //libreria necesaria para display I2C
#include <EEPROM.h> // Controla E/S EEPROM
/*
Para la comunicación serial entre Arduino y Python, tomaremos en cuenta los siguientes casos:

0  --> Error
1  --> Temperatura
2  --> HumedadSuelo
3  --> HumedadRelativa
4  --> Modificación del Estandar liHs
5  --> Modificación del Estandar lsHs
6  --> Modificación del Estandar liTemp
7  --> Modificación del Estandar lsTemp
8  --> Modificación del Estandar liHa
9  --> Modificación del Estandar lsHa
10 --> Modificación del estado del Rociador--
11 --> Modificación del estado del Riego por goteo--

*/

#define DHTPIN 10 //Definimos el pin digital donde se conecta el sensor
#define DHTTYPE DHT11 //Dependiendo del tipo de sensor

//Declaración de variables locales
const int sensorPinT = A0;
const int releGoteo = 11;
const int releRocio = 12;
const byte ROWS = 4;
const byte COLS = 4;
bool riegoGoteo = false, rociador = false, estadoActuador;
int i, valT, u, w, x, y, z, contliTemp, contlsTemp, contliHa, contliHs, contlsHa, contlsHs;
float t, h;
char key, liHs, lsHs, liTemp, lsTemp, liHa, lsHa, val;
char liTemperatura[3], lsTemperatura[7], liHsuelo[11], lsHsuelo[15], liHaire[19], lsHaire[23]; //almacena los caracteres en EEPROM
char keys[ROWS][COLS] = {
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
byte rowPins[ROWS] = {5,4,3,2}; //Filas (pines del 5 al 2)
byte colPins[COLS] = {9,8,7,6}; //Columnas (pines del 9 al 6)

LiquidCrystal_I2C lcd20x4(0x23,20,4);  // configuramos el LCD en la direccion de bus I2C que es 0x27 y el tamaño columnas x filas 20x4
LiquidCrystal_I2C lcd16x2(0x27,16,2);  // configuramos el LCD en la direccion de bus I2C que es 0x27 y el tamaño columnas x filas 16x2
DHT dht(DHTPIN, DHTTYPE); //Inicializamos el sensor DHT11
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //Inicializamos la keypad

void setup(){
  Serial.begin(9600);
  Wire.begin(); // Unimos este dispositivo al bus I2C
  dht.begin();

  lcd16x2.init(); // initializa el lcd 16x2
  lcd20x4.init(); // initializa el lcd 20x4
  
  lcd16x2.backlight(); // enciende la luz de fondo del display
  lcd20x4.backlight(); // enciende la luz de fondo del display

  MenuPrincipal();
}

void loop(){
    Temperatura(); // Metodo para la lectura de la tempeartura
    //HumedadRelativa(); // Metodo para la lectura de la humedad relativa
    //HumedadSuelo(); // Metodo para la lectura de la humedad en suelo
    MenuLecturaOpciones();
   
}

void MenuPrincipal(){
  lcd20x4.clear();
  lcd20x4.begin(20,4);
  lcd20x4.setCursor(0,0);
  lcd20x4.print("MENU");
  lcd20x4.setCursor(0,1);
  lcd20x4.print("[A] Modificar Stnd.");
  lcd20x4.setCursor(0,2);
  lcd20x4.print("[B] Activar actuador.");
}

void MenuLecturaOpciones(){
  //Temperatura(); // Metodo para la lectura de la tempeartura
  //HumedadRelativa(); // Metodo para la lectura de la humedad relativa
  //HumedadSuelo(); // Metodo para la lectura de la humedad en 
  z = 0;
  key = keypad.getKey(); //se guarda en la variable key el caracter de la tecla presionada
  
  if(key){
    switch (key){
      case 'A':
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("[1] Modificar Temp");
        lcd20x4.setCursor(0,1);
        lcd20x4.print("[2] Modificar HS");
        lcd20x4.setCursor(0,2);
        lcd20x4.print("[3] Modificar HR");
        lcd20x4.setCursor(0,3);
        lcd20x4.print("[C] Cancelar");
        delay(1000);
        MenuModificarStnd(); //Metodo para mostrar el menu del caso A "Modificar Stnd."
        z++;
      break;
      case 'B':
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("[1] Riego por goteo");
        lcd20x4.setCursor(0,1);
        lcd20x4.print("[2] Rociadores");
        lcd20x4.setCursor(0,2);
        lcd20x4.print("[C] Cancelar");
        delay(1000);
        MenuActivarActuador(riegoGoteo, rociador); //Metodo para mostrar el menu del caso B "Activar actuador"
        z++;
      break;
    }
  }
}

void MenuModificarStnd(){
  y = 0, contliTemp = 0, contlsTemp = 4, contliHs = 8, contlsHs = 12, contliHa = 16, contlsHa = 20;
  while(z<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ // Modificar Temp (temperatura)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Temperatura (*C) ");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limite inf:");
      lecturaLiTemperatura();
      lcd20x4.print(" sup:");
      lecturaLsTemperatura();
      lcd20x4.setCursor(0,2);
      lcd20x4.print("[1] Modif Limite inf");
      lcd20x4.setCursor(0,3);
      lcd20x4.print("[2] Modif Limite sup");
      liTemperatura[contliTemp] = key;
      lsTemperatura[contlsTemp] = key;
      EEPROM.write(contliTemp, liTemperatura[contliTemp]);
      EEPROM.write(contlsTemp, lsTemperatura[contlsTemp]);
      contliTemp++;
      contlsTemp++;
      MenuNuevoEstandar(contliTemp, contlsTemp, liTemperatura[3], lsTemperatura[3]); // Metodo para capturar el nuevo valor 
      y++;
    }
    if(key == '2'){ // Modificar HS (humedad del suelo)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Humedad suelo (%)");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limite inf:");
      lecturaLiHsuelo();
      lcd20x4.print(" sup:");
      lecturaLsHsuelo();
      lcd20x4.setCursor(0,2);
      lcd20x4.print("[1] Modif Limite inf");
      lcd20x4.setCursor(0,3);
      lcd20x4.print("[2] Modif Limite sup");
      liHsuelo[contliHs] = key;
      lsHsuelo[contlsHs] = key;
      EEPROM.write(contliHs, liHsuelo[contliHs]);
      EEPROM.write(contlsHs, lsHsuelo[contlsHs]);
      contliHs++;
      contlsHs++;
      MenuNuevoEstandar(contliHs, contlsHs, liHsuelo[3], lsHsuelo[3]); // Metodo para capturar el nuevo valor
      y++;
    }
    if(key == '3'){ // Modificar HA (humedad relativa)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Humedad relativa (%)");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limite inf:");
      lecturaLiHaire();
      lcd20x4.print(" sup:");
      lecturaLsHaire();
      lcd20x4.setCursor(0,2);
      lcd20x4.print("[1] Modif Limite inf");
      lcd20x4.setCursor(0,3);
      lcd20x4.print("[2] Modif Limite sup");
      liHaire[contliHa] = key;
      lsHaire[contlsHa] = key;
      EEPROM.write(contliHa, liHaire[contliHa]);
      EEPROM.write(contlsHa, lsHaire[contlsHa]);
      contliHa++;
      contlsHa++;
      MenuNuevoEstandar(contliHa, contlsHa, liHaire[3], lsHaire[3]);
      y++;
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); //Metodo para mostrar el menu principal
      MenuLecturaOpciones();
      z++;
    }
  }
}

void MenuNuevoEstandar(int contli, int contls, char linf[3], char lsup[3]){
  x = 0;
  while(y<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ //Nuevo Estandar para limite inferior
      linf[contli] = key;//se guarda caracter por caracter en el arreglo limite[]
      EEPROM.write(contli, linf[contli]);
      contli++;
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Nuevo Limite inf:");
      CapturarEstandar(contli, linf[3]);
      x++;
    }
    if(key == '2'){ //Nuevo Estandar para limite superior
      lsup[contls] = key; //se guarda caracter por caracter en el arreglo limite[]
      EEPROM.write(contls, lsup[contls]);
      contls++;
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Nuevo Limite sup:");      
      CapturarEstandar(contls, lsup[3]);
      x++;
    }
  }
}

void CapturarEstandar(int contlim, char limite[3]){
  x = 0, i = 0;
  while(i<2){
    key = keypad.getKey();
    delay(100);
    if(key){
      if((key != 'A') and (key != 'B') and (key != 'C') and (key != 'D') and (key != '*' ) and (key != '#')){
        limite[contlim] = key; //se guarda caracter por caracter en el arreglo limite[]
        EEPROM.write(contlim, limite[contlim]);
        lcd20x4.print(key);
        contlim++;
        i++;
      }
    }
  }
  GuardarValor();
  x++;
}

char GuardarValor(){
  delay(100);
  lcd20x4.setCursor(0,1);
  lcd20x4.print("[A] Guardar valor");
  lcd20x4.setCursor(0,2);
  lcd20x4.print("[C] Cancelar");
  while(x<1){
    key = keypad.getKey();
    delay(100);
    if(key == 'A'){ //Guardar nuevo estandar
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Nuevo stnd. Guardado");
      lcd20x4.setCursor(0,2);
      lcd20x4.print("                    ");
      delay(1500);
      x++;
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); //Metodo para mostrar el menu principal
      MenuLecturaOpciones();
      x++;
    }
  }
  MenuPrincipal();
  MenuLecturaOpciones();
  y++;
  z++;
}

// Caso B del Menu Principal
void MenuActivarActuador(bool riegoGoteo, bool rociador){
  estadoActuador = false;
  y = 0;
  while(z<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ // Riego por goteo
      if(riegoGoteo){ // Condicion si el riego por goteo está apagado
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Estado actual:");
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Riego Apagado");
        estadoActuador = false;
        lcd20x4.setCursor(0,2);
        lcd20x4.print("[1] Modificar Estado");
        ModificarRiegoPorGoteo(estadoActuador);
        y++;
      }
      else{ // Condicion si el riego por goteo está encendido
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Estado actual:");
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Riego Encendido");
        estadoActuador = true;
        lcd20x4.setCursor(0,2);
        lcd20x4.print("[1] Modificar Estado");
        ModificarRiegoPorGoteo(estadoActuador);
        y++;
      }
    }
    if(key == '2'){ // Rociador
      if(rociador){ // Condicion si el rociador está apagado
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Estado actual:");
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Rociador Apagado");
        estadoActuador = false;
        lcd20x4.setCursor(0,2);
        lcd20x4.print("[1] Modificar Estado");
        ModificarRociador(estadoActuador);
        y++;
      }
      else{ // Condicion si el rociador está encendido
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Estado actual:");
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Rociador Encendido");
        estadoActuador = true;
        lcd20x4.setCursor(0,2);
        lcd20x4.print("[1] Modificar Estado");
        ModificarRociador(estadoActuador);
        y++;
      }
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); // Metodo para mostrar el menu principal
      MenuLecturaOpciones();
      z++;
    }
  }
}

void ModificarRociador(bool estadoActuador){
  while(y<1){
    key = keypad.getKey();
    delay(100);
    if(key == '1'){
      if(estadoActuador == true){ // Condicion si el estado del actuador está encendido (rociador)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        ApagarRociador(); // Metodo para apagar rociador
        y++;
      }
      else{ // Condicion si el estado del actuador está apagado (rociador)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        EncenderRociador(); // Metodo para encender rociador
        y++;
      }
    }
  }
  MenuPrincipal();
  MenuLecturaOpciones();
  rociador = estadoActuador;
  z++;
}

void ModificarRiegoPorGoteo(bool estadoActuador){
  while(y<1){
    key = keypad.getKey();
    delay(100);
    if(key == '1'){
      if(estadoActuador == true){ //Condicion si el estado del actuador está encendido (riego por goteo)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        ApagarRiegoPorGoteo(); // Metodo para apagar el riego por goteo
        y++;
      }
      else{ //Condicion si el estado del actuador está apagado (riego por goteo)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        EncenderRiegoPorGoteo(); // Metodo para encender el riego por goteo
        y++;
      }
    }
  }
  MenuPrincipal();
  MenuLecturaOpciones();
  riegoGoteo = estadoActuador;
  z++;
}

void ApagarRiegoPorGoteo(){
  digitalWrite(releGoteo, LOW);
  riegoGoteo = false;
  lcd16x2.setCursor(0,1);
  lcd16x2.print("Riego Apagado");
  
  //Registrando el apagado del riego por goteo
  Serial.print("{\"Estado_riego\":");
  Serial.print(riegoGoteo);
  Serial.println("}\n");
}

void EncenderRiegoPorGoteo(){
  digitalWrite(releGoteo, HIGH);
  riegoGoteo = true;
  lcd16x2.setCursor(0,1);
  lcd16x2.print("Riego Encendido");
  
  //Registrando la activación del riego por goteo
  Serial.print("{\"Estado_riego\":");
  Serial.print(riegoGoteo);
  Serial.println("}\n");
}

void ApagarRociador(){
  digitalWrite(releRocio, LOW);
  rociador = false;
  lcd16x2.setCursor(0,1);
  lcd16x2.print("Rociador Apagado");
  
  //Registrando el apagado del rociador
  Serial.print("{\"Estado rociador\":");
  Serial.print(rociador);
  Serial.println("}\n");
}

void EncenderRociador(){
  digitalWrite(releRocio, HIGH);
  rociador = true;
  lcd16x2.setCursor(0,1);
  lcd16x2.print("Rociador Encendido");
  
  //Registrando la activación del rociador
  Serial.print("{\"Estado rociador\":");
  Serial.print(rociador);
  Serial.println("}\n");
}

void Temperatura(){
  t = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  
  //Validando lecturas de Temperatura
  /*if (isnan(t)) {
    //Cuando haya un error, enviará un cero por el puerto serial, esto evitará que python capture datos inválidos
    Serial.println("0");
    Serial.println("Error obteniendo los datos de la Temperatura 'sensor DHT11'");
  }*/
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("Temp: ");
  lcd16x2.print(t);
  lcd16x2.print(" *C");

  //Enviando la temperatura por el puerto Serial
  Serial.print("{\"Temp\": ");
  Serial.print(t);
  Serial.println("}\n");
  delay(1000);
}

void HumedadSuelo(){
  liHs = lecturaLiHsuelo();
  lsHs = lecturaLsHsuelo();
  
  valT = map(analogRead(sensorPinT), 0, 1023, 100, 0); //Trunca un valor del rango de (1023, 0) ajustandolo a los porcentajes de (0 y 100) (Humedad en suelo)
  valT = constrain(valT, 0, 100); //Restringe un número a estar dentro del porcentaje de 0 y 100
  
  //Desplegando datos de la Humedad en el suelo
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HS:");
  lcd16x2.print(valT);

  //Enviando la humedad del suelo por el puerto serial
  Serial.print("{\"Hs\": ");
  Serial.print(valT);
  Serial.println("}\n");
      
  if((valT >= 0) and (valT <= liHs)) { 
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Suelo seco");
    delay(1000);
    EncenderRiegoPorGoteo();
  }else{ 
    if((valT > liHs) and (valT <= lsHs)) {
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo humedo");
    }else{
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo mojado");
      delay(1000);
      ApagarRiegoPorGoteo();
    }
  }
  delay(1000);
}

char HumedadRelativa(){
  h = dht.readHumidity(); // Leemos la humedad relativa
  liHa = lecturaLiHaire();
  lsHa = lecturaLsHaire();
  
  //Validando lecturas de Humedad Relativa
  /*if (isnan(h)) {
    Serial.println("0");
    Serial.println("Error obteniendo los datos de la humedad relativa 'sensor DHT11'");
  }*/
  //Desplegando datos en pantalla
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HA: ");
  lcd16x2.print(h);
  lcd16x2.print(" %");

  //Enviando la humedad del aire por el puerto serial
  Serial.print("{\"Ha\": ");
  Serial.print(h);
  Serial.println("}\n");
  
  if(h<liHa){
      EncenderRociador();
      delay(1000);
    }
    if(h>lsHa){
      ApagarRociador();
      delay(1000);
    }
  delay(1000);
}

char lecturaLiTemperatura(){
  char liTemperatura[3];
  int contliTemp;
  Serial.print("{\"LiTemp\": ");
  for(contliTemp=2; contliTemp<=sizeof(liTemperatura); contliTemp++){
    liTemperatura[contliTemp] = char(EEPROM.read(contliTemp));
    lcd20x4.print(liTemperatura[contliTemp]);
    Serial.print(liTemperatura[contliTemp]);
  }
  Serial.println("}\n");
  delay(100);
}

char lecturaLsTemperatura(){
  char lsTemperatura[7];
  int contlsTemp;
  Serial.print("{\"LsTemp\": ");
  for(contlsTemp=6; contlsTemp<=sizeof(lsTemperatura); contlsTemp++){
    lsTemperatura[contlsTemp] = char(EEPROM.read(contlsTemp));
    lcd20x4.print(lsTemperatura[contlsTemp]);
    Serial.print(lsTemperatura[contlsTemp]);
  }
  Serial.println("}\n");
  delay(100);
}

char lecturaLiHsuelo(){
  char liHsuelo[11];
  int contliHs;  
  Serial.print("{\"LiHs\": ");
  for(contliHs=10; contliHs<=sizeof(liHsuelo); contliHs++){
    liHsuelo[contliHs] = char(EEPROM.read(contliHs));
    lcd20x4.print(liHsuelo[contliHs]);
    Serial.print(liHsuelo[contliHs]);
  }
  Serial.println("}\n");
  delay(100);
}

char lecturaLsHsuelo(){
  char lsHsuelo[15];
  int contlsHs;
  Serial.print("{\"LsHs\": ");
  for(contlsHs=14; contlsHs<=sizeof(lsHsuelo); contlsHs++){
    lsHsuelo[contlsHs] = char(EEPROM.read(contlsHs));
    lcd20x4.print(lsHsuelo[contlsHs]);
    Serial.print(lsHsuelo[contlsHs]);
  }
  Serial.println("}\n");
  delay(100);
}

char lecturaLiHaire(){
  char liHaire[19];
  int contliHa;  
  Serial.print("{\"LiHa\": ");
  for(contliHa=18; contliHa<=sizeof(liHaire); contliHa++){
    liHaire[contliHa] = char(EEPROM.read(contliHa));
    lcd20x4.print(liHaire[contliHa]);
    Serial.print(liHaire[contliHa]);
  }
  Serial.println("}\n");
  delay(100);
}

char lecturaLsHaire(){
  char lsHaire[23];
  int contlsHa;
  Serial.print("{\"LsHa\": ");
  for(contlsHa=22; contlsHa<=sizeof(lsHaire); contlsHa++){
    lsHaire[contlsHa] = char(EEPROM.read(contlsHa));
    lcd20x4.print(lsHaire[contlsHa]);
    Serial.print(lsHaire[contlsHa]);
  }
  Serial.println("}\n");
  delay(100);
}
