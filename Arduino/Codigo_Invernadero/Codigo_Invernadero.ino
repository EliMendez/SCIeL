#include <Keypad.h> // libreria necesaria para KeyPad
#include <Wire.h>  // libreria necesaria para bus I2C
#include <DHT.h> //Incluimos librería para sensor DHT 
#include <LiquidCrystal_I2C.h> //libreria necesaria para display I2C
#include <EEPROM.h> // Controla E/S EEPROM
//#include <WiFi.h> // libreria necesaria para el modulo Wifi ESP8266

#define DHTPIN 10 //Definimos el pin digital donde se conecta el sensor
#define DHTTYPE DHT11 //Dependiendo del tipo de sensor

//Declaración de variables locales
const int sensorPinT = A0;
const int releGoteo = 11;
const int releRocio = 12;
const byte ROWS = 4;
const byte COLS = 4;
bool riegoGoteo = false, rociador = false, estadoActuador;
int valT, x, y, z, cont, liHs = 0, lsHs = 100, liTemp = 0, lsTemp = 50, liHa = 20, lsHa = 95;
float t, h;
char key;
char teclasGuardadas[3];//almacena los caracteres en EEPROM
char teclas[3]; //almacena los caracteres en una variable
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

  for(cont=0; cont<=sizeof(teclasGuardadas); cont++){
    teclasGuardadas[cont] = char(EEPROM.read(cont));
    Serial.print(teclasGuardadas[cont]);
  }
  if(teclasGuardadas[0] == '1'){
    Serial.println("\nEstandar Temp: ");
    for(cont=2; cont<=sizeof(teclasGuardadas); cont++){
      teclas[cont] = char(EEPROM.read(cont));
      Serial.print(teclas[cont]);
    }
  }
  if(teclasGuardadas[0] == '2'){
    Serial.println("\nEstandar Hs: ");
    for(cont=2; cont<=sizeof(teclasGuardadas); cont++){
      teclas[cont] = char(EEPROM.read(cont));
      Serial.print(teclas[cont]);
    }
  }
  if(teclasGuardadas[0] == '3'){
    Serial.println("\nEstandar Hr: ");
    for(cont=2; cont<=sizeof(teclasGuardadas); cont++){
      teclas[cont] = char(EEPROM.read(cont));
      Serial.print(teclas[cont]);
    }
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

// Caso A del Menu Principal
void MenuModificarStnd(){
  cont = 0;
  y = 0;
  while(z<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ // Modificar Temp (temperatura)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Temperatura (*C) ");
      MenuLimiteIyS(liTemp, lsTemp, cont);
      cont++;
      y++;
    }
    if(key == '2'){ // Modificar HS (humedad del suelo)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Humedad suelo (%)");
      MenuLimiteIyS(liHs, lsHs, cont);
      cont++;
      y++;
    }
    if(key == '3'){ // Modificar HA (humedad relativa)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Humedad relativa (%)");
      MenuLimiteIyS(liHa, lsHa, cont);
      cont++;
      y++;
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); //Metodo para mostrar el menu principal
      MenuLecturaOpciones();
      z++;
    }
  }
}

void MenuLimiteIyS(int li, int ls, int cont){
  teclas[cont] = key;//se guarda caracter por caracter en el arreglo teclas[]
  teclasGuardadas[cont] = teclas[cont];
  EEPROM.write(cont, teclasGuardadas[cont]);
  cont++;
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Limit inferior: ");
  lcd20x4.print(li);
  lcd20x4.setCursor(0,2);
  lcd20x4.print("Limit superior: ");
  lcd20x4.print(ls);
  lcd20x4.setCursor(0,3);
  lcd20x4.print("[1] Nuevo Estandar");
  MenuNuevoEstandar(cont); // Metodo para capturar el nuevo valor  
}

void MenuNuevoEstandar(int cont){
  while(y<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ //Nuevo Estandar
      teclas[cont] = key;//se guarda caracter por caracter en el arreglo teclas[]
      teclasGuardadas[cont] = teclas[cont];
      EEPROM.write(cont, teclasGuardadas[cont]);
      cont++;
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Nuevo Estandar:");
      CapturarEstandar(cont);
    }
  }
}

void CapturarEstandar(int cont){
  int conta = cont;
  x = 0;
  int i = 0;
  char valor[2];
  while(i<2){
    key = keypad.getKey();
    delay(100);
    if(key){
      if((key != 'A') and (key != 'B') and (key != 'C') and (key != 'D') and (key != '*' ) and (key != '#')){
        //Comparar si el valor esta en el intervalo de temp, hs y hr dependiendo la opcion que se haya seleccionado
        teclas[conta] = key;//se guarda caracter por caracter en el arreglo teclas[]
        teclasGuardadas[conta] = teclas[conta];
        EEPROM.write(conta, teclasGuardadas[conta]);
        lcd20x4.print(key);
        conta++;
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
}

void EncenderRiegoPorGoteo(){
  digitalWrite(releGoteo, HIGH);
  riegoGoteo = true;
  lcd20x4.setCursor(0,1);
  lcd20x4.println("Riego Encendido");
}

void ApagarRociador(){
  digitalWrite(releRocio, LOW);
  rociador = false;
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Rociador Apagado");
}

void EncenderRociador(){
  digitalWrite(releRocio, HIGH);
  rociador = true;
  lcd20x4.setCursor(0,1);
  lcd20x4.print("Rociador Encendido");
}

void Temperatura(){
  t = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  
  //Validando lecturas de Temperatura
  if (isnan(t)) {
    Serial.println("Error obteniendo los datos de la Temperatura 'sensor DHT11'");
    //return;
  }
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("Temp: ");
  lcd16x2.print(t);
  lcd16x2.print(" *C");
  //delay(1000);
}

void HumedadSuelo(){
  valT = map(analogRead(sensorPinT), 0, 1023, 100, 0); //Trunca un valor del rango de (1023, 0) ajustandolo a los porcentajes de (0 y 100) (Humedad en suelo)
  valT = constrain(valT, 0, 100); //Restringe un número a estar dentro del porcentaje de 0 y 100
  
  //Desplegando datos de la Humedad en el suelo
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HS:");
  lcd16x2.print(valT);
      
  if((valT >= 0) and (valT <= 33)) {
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Suelo seco");
    digitalWrite(releGoteo, HIGH);
    riegoGoteo = true;
    delay(1000);
    lcd16x2.clear();
    lcd16x2.print("Riego Encendido");
  }else{ 
    if((valT >=34) and (valT <= 66)) {
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo humedo");
    }else{
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo mojado");
      digitalWrite(releGoteo, LOW);
      riegoGoteo = false;
      delay(1000);
      lcd16x2.clear();
      lcd16x2.print("Riego Apagado");
    }
  }
  delay(1000);
}

void HumedadRelativa(){
  h = dht.readHumidity(); // Leemos la humedad relativa
  
  //Validando lecturas de Humedad Relativa
  if (isnan(h)) {
    Serial.println("Error obteniendo los datos de la humedad relativa 'sensor DHT11'");
    //return;
  }
  //Desplegando datos en pantalla
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HA: ");
  lcd16x2.print(h);
  lcd16x2.print(" %");
  
  if(h<40){
      digitalWrite(releRocio, HIGH);
      rociador = true;
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Rociador Encendido");
      delay(1000);
    }
    if(h>60){
      digitalWrite(releRocio, LOW);
      rociador = false;
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Rociador Apagado");
      delay(1000);
    }
  //delay(1000);
}
