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
10 --> Modificación del estado del Rociador
11 --> Modificación del estado del Riego por goteo

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
int i, valT, x, y, z, contliTemp, contlsTemp, contliHa, contliHs, contlsHa, contlsHs, liHs = 0, lsHs = 100, liTemp = 0, lsTemp = 50, liHa = 20, lsHa = 95;
float t, h;
char key;
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

  //Lectura de los nuevos estandares de la Temperatura que estan almacendados en EEPROM
  Serial.println("\n***Estandar Temp***");
  Serial.println("Limite inferior: ");
  //Serial.println("6");
  for(contliTemp=2; contliTemp<=sizeof(liTemperatura); contliTemp++){
    liTemperatura[contliTemp] = char(EEPROM.read(contliTemp));
    Serial.print(liTemperatura[contliTemp]);
  }
  Serial.println("\nLimite superior: ");
  //Serial.println("7");
  for(contlsTemp=6; contlsTemp<=sizeof(lsTemperatura); contlsTemp++){
    lsTemperatura[contlsTemp] = char(EEPROM.read(contlsTemp));
    Serial.print(lsTemperatura[contlsTemp]);
  }
  
  //Lectura de los nuevos estandares de la Humedad en suelo que estan almacendados en EEPROM
  Serial.println("\n***Estandar Hs***");
  Serial.println("Limite inferior: ");
  //Serial.println("4");
  for(contliHs=10; contliHs<=sizeof(liHsuelo); contliHs++){
    liHsuelo[contliHs] = char(EEPROM.read(contliHs));
    Serial.print(liHsuelo[contliHs]);
  }  
  Serial.println("\nLimite superior: ");
  //Serial.println("5");
  for(contlsHs=14; contlsHs<=sizeof(lsHsuelo); contlsHs++){
    lsHsuelo[contlsHs] = char(EEPROM.read(contlsHs));
    Serial.print(lsHsuelo[contlsHs]);
  }
  
  //Lectura de los nuevos estandares de la humedad relativa que estan almacendados en EEPROM
  Serial.println("\n***Estandar Ha***");
  Serial.println("Limite inferior: ");
  //Serial.println("8");
  for(contliHa=18; contliHa<=sizeof(liHaire); contliHa++){
    liHaire[contliHa] = char(EEPROM.read(contliHa));
    Serial.print(liHaire[contliHa]);
  }  
  Serial.println("\nLimite superior: ");
  //Serial.println("9");
  for(contlsHa=22; contlsHa<=sizeof(lsHaire); contlsHa++){
    lsHaire[contlsHa] = char(EEPROM.read(contlsHa));
    Serial.print(lsHaire[contlsHa]);
  }
}

void loop(){
  //Temperatura(t); // Metodo para la lectura de la tempeartura
  //HumedadRelativa(h); // Metodo para la lectura de la humedad relativa
  //delay(1000);
  //HumedadSuelo(valT); // Metodo para la lectura de la humedad en suelo
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
      MenuLimiteIyS(liTemp, lsTemp, contliTemp, contlsTemp, liTemperatura[3], lsTemperatura[3]); 
      y++;
    }
    if(key == '2'){ // Modificar HS (humedad del suelo)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Humedad suelo (%)");
      MenuLimiteIyS(liHs, lsHs, contliHs, contlsHs, liHsuelo[3], lsHsuelo[3]);
      y++;
    }
    if(key == '3'){ // Modificar HA (humedad relativa)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Humedad relativa (%)");
      MenuLimiteIyS(liHa, lsHa, contliHa, contlsHa, liHaire[3], lsHaire[3]);
      y++;
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); //Metodo para mostrar el menu principal
      MenuLecturaOpciones();
      z++;
    }
  }
}

void MenuLimiteIyS(int li, int ls, int contli, int contls, char linf[3], char lsup[3]){
  linf[contli] = key;
  lsup[contls] = key;
  EEPROM.write(contli, linf[contli]);
  EEPROM.write(contls, lsup[contls]);
  Serial.print(linf[contli]);
  Serial.print(lsup[contls]);
  contli++;
  contls++;
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Limite inf:");
  lcd20x4.print(li);
  lcd20x4.print(" sup:");
  lcd20x4.print(ls);
  lcd20x4.setCursor(0,2);
  lcd20x4.print("[1] Modif Limite inf");
  lcd20x4.setCursor(0,3);
  lcd20x4.print("[2] Modif Limite sup");
  MenuNuevoEstandar(contli, contls, linf[3], lsup[3]); // Metodo para capturar el nuevo valor  
}

void MenuNuevoEstandar(int contli, int contls, char linf[3], char lsup[3]){
  x = 0;
  while(y<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ //Nuevo Estandar para limite inferior
      linf[contli] = key;//se guarda caracter por caracter en el arreglo limite[]
      EEPROM.write(contli, linf[contli]);
      Serial.print(linf[contli]);
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
      Serial.print(lsup[contls]);
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
        Serial.print(limite[contlim]);
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
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Riego Apagado");
  //Registrando el apagado del riego por goteo
  Serial.println("11");
  Serial.println("0");
}

void EncenderRiegoPorGoteo(){
  digitalWrite(releGoteo, HIGH);
  riegoGoteo = true;
  lcd20x4.setCursor(0,1);
  lcd20x4.println("Riego Encendido");
  //Registrando la activación del riego por goteo
  Serial.println("11");
  Serial.println("1");
}

void ApagarRociador(){
  digitalWrite(releRocio, LOW);
  rociador = false;
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Rociador Apagado");
  //Registrando el apagado del rociador
  Serial.println("10");
  Serial.println("0");
}

void EncenderRociador(){
  digitalWrite(releRocio, HIGH);
  rociador = true;
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Rociador Encendido");
  //Registrando la activación del rociador
  Serial.println("10");
  Serial.println("1");
}

void Temperatura(){
  t = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  
  //Validando lecturas de Temperatura
  if (isnan(t)) {
    //Cuando haya un error, enviará un cero por el puerto serial, esto evitará que python capture datos inválidos
    Serial.println("0");
    Serial.println("Error obteniendo los datos de la Temperatura 'sensor DHT11'");
  }
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("Temp: ");
  lcd16x2.print(t);
  lcd16x2.print(" *C");

  //Enviando la temperatura por el puerto Serial
  Serial.println("1");
  Serial.println(t);
  //delay(1000);
}

void HumedadSuelo(int liHs, int lsHs){
  valT = map(analogRead(sensorPinT), 0, 1023, 100, 0); //Trunca un valor del rango de (1023, 0) ajustandolo a los porcentajes de (0 y 100) (Humedad en suelo)
  valT = constrain(valT, 0, 100); //Restringe un número a estar dentro del porcentaje de 0 y 100
  
  //Desplegando datos de la Humedad en el suelo
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HS:");
  lcd16x2.print(valT);

  //Enviando la humedad del suelo por el puerto serial
  Serial.println("2");
  Serial.println(valT);
      
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

void HumedadRelativa(int liHa, int lsHa){
  h = dht.readHumidity(); // Leemos la humedad relativa
  
  //Validando lecturas de Humedad Relativa
  if (isnan(h)) {
    Serial.println("0");
    Serial.println("Error obteniendo los datos de la humedad relativa 'sensor DHT11'");
  }
  //Desplegando datos en pantalla
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HA: ");
  lcd16x2.print(h);
  lcd16x2.print(" %");

  //Enviando la humedad del aire por el puerto serial
  Serial.println("3");
  Serial.println(h);
  
  if(h<liHa){
    EncenderRociador();
    delay(1000);
  }
  if(h>lsHa){
    ApagarRociador();
    delay(1000);
  }
  //delay(1000);
}
