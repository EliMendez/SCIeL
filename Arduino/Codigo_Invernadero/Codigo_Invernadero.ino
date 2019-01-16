#include <Keypad.h> // libreria necesaria para KeyPad
#include <Wire.h>  // libreria necesaria para bus I2C
#include <DHT.h> //Incluimos librería para sensor DHT 
#include <LiquidCrystal_I2C.h> //libreria necesaria para display I2C
#include <EEPROM.h> // Controla E/S EEPROM

#define DHTPIN 10 //Definimos el pin digital donde se conecta el sensor
#define DHTTYPE DHT11 //Dependiendo del tipo de sensor

//Declaración de variables locales
struct MEDICION{
  int invernadero;
  int parametro;
  int sensor;
  int actuador;
  float medicion;
  bool estado;
} toma_medicion;

//Instanciando objetos
MEDICION temperatura;
MEDICION humedadRelativa;
MEDICION humedadSuelo;

const int sensorPinT = A0;
const int releGoteo = 11;
const int releRocio = 12;
const byte ROWS = 4;
const byte COLS = 4;
bool riegoGoteo = false, rociador = false, estadoActuador;
int i, u, w, x, y, z, t, h, valT, cont, contliTemp, contlsTemp, contliHa, contliHs, contlsHa, contlsHs;
char key, liHs, lsHs, liTemp, lsTemp, liHa, lsHa;
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
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); //Inicializamos la keypad

void setup(){
  //Inicializamos objetos
  temperatura.invernadero = 1; //GreenHouse
  temperatura.sensor = 1; //DHT11
  temperatura.actuador = 1; //Rociador
  temperatura.parametro = 1;  //Temperatura
  humedadRelativa.invernadero = 1; //GreenHouse
  humedadRelativa.sensor = 1; //DHT11
  humedadRelativa.actuador = 1; //Rociador
  humedadRelativa.parametro = 2;  //Humedad Relativa
  humedadSuelo.invernadero = 1; //GreenHouse
  humedadSuelo.sensor = 2; //Suelo
  humedadSuelo.actuador = 2; //Riego por Goteo
  humedadSuelo.parametro = 3;  //Humedad del Suelo

  Serial.begin(9600);
  Wire.begin(); // Unimos este dispositivo al bus I2C
  dht.begin();

  lcd16x2.init(); // initializa el lcd 16x2
  lcd20x4.init(); // initializa el lcd 20x4
  lcd16x2.backlight(); // enciende la luz de fondo del display
  lcd20x4.backlight(); // enciende la luz de fondo del display
  
  Lecturas();
  MenuPrincipal();
}

void loop(){
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
        MenuActivarActuador(riegoGoteo, rociador); //Metodo para mostrar el menu del caso B "Activar actuador"
        z++;
      break;
    }
  }
}

void MenuModificarStnd(){
  y = 0, contliTemp = 0, contlsTemp = 4, contliHs = 8, contlsHs = 12, contliHa = 16, contlsHa = 20;
  Lecturas();
  
  while(z<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ // Modificar Temp (temperatura)
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Temperatura (*C) ");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limite inf:");
      LecturaLiTemp_LCD();
      lcd20x4.print(" sup:");
      LecturaLsTemp_LCD();
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
      LecturaLiHs_LCD();
      lcd20x4.print(" sup:");
      LecturaLsHs_LCD();
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
      LecturaLiHa_LCD();
      lcd20x4.print(" sup:");
      LecturaLsHa_LCD();
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

void CapturarEstandar(int cont, char limite[3]){
  x = 0, i = 0;
  while(i<2){
    key = keypad.getKey();
    delay(100);
    if(key){
      if((key != 'A') and (key != 'B') and (key != 'C') and (key != 'D') and (key != '*' ) and (key != '#')){
        limite[cont] = key; //se guarda caracter por caracter en el arreglo limite[]
        EEPROM.write(cont, limite[cont]);
        lcd20x4.print(key);
        cont++;
        i++;
      }
    }
  }
  GuardarValor();
  x++;
}

char GuardarValor(){
  delay(50);
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
  Lecturas();
  MenuPrincipal();
  MenuLecturaOpciones();
  y++;
  z++;
}

// Caso B del Menu Principal
void MenuActivarActuador(bool riegoGoteo, bool rociador){
  y = 0;
  Lecturas();
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
  x=0;
  while(y<1){
    key = keypad.getKey();
    delay(100);
    if(key == '1'){
      if(estadoActuador == true){ // Condicion si el estado del actuador está encendido (rociador)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        ApagarRociador(); // Metodo para apagar rociador
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Rociador Apagado");
        x++;
      }
      else{ // Condicion si el estado del actuador está apagado (rociador)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        EncenderRociador(); // Metodo para encender rociador
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Rociador Encendido");
        x++;
      }
    }
  }
  Lecturas();
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
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Riego Apagado");
        x++;
      }
      else{ //Condicion si el estado del actuador está apagado (riego por goteo)
        lcd20x4.clear();
        lcd20x4.setCursor(0,0);
        lcd20x4.print("Nuevo Estado:");
        EncenderRiegoPorGoteo(); // Metodo para encender el riego por goteo
        lcd20x4.setCursor(0,1);
        lcd20x4.print("Riego Encendido");
        x++;
      }
    }
  }
  Lecturas();
  MenuPrincipal();
  MenuLecturaOpciones();
  riegoGoteo = estadoActuador;
  z++;
}

void ApagarRiegoPorGoteo(){
  digitalWrite(releGoteo, LOW);
  riegoGoteo = false;
  humedadSuelo.estado = riegoGoteo; //Registrando el cambio de estado
}

void EncenderRiegoPorGoteo(){
  digitalWrite(releGoteo, HIGH);
  riegoGoteo = true;
  humedadSuelo.estado = riegoGoteo; //Registrando el cambio de estado
}

void ApagarRociador(){
  digitalWrite(releRocio, LOW);
  rociador = false;
  temperatura.estado = rociador; //Registrando el cambio de estado
  humedadRelativa.estado = rociador; //Registrando el cambio de estado
}

void EncenderRociador(){
  digitalWrite(releRocio, HIGH);
  rociador = true;
  temperatura.estado = rociador; //Registrando el cambio de estado
  humedadRelativa.estado = rociador; //Registrando el cambio de estado
}

char LimiteInferiorTemp(){
  char liTemperatura[3];
  int cont;
  Serial.print("{\"LiTemp\": ");
  for(cont=2; cont<=sizeof(liTemperatura); cont++){
    liTemperatura[cont] = char(EEPROM.read(cont));
    Serial.print(liTemperatura[cont]);
  }
  Serial.println("}\n");
}

char LimiteSuperiorTemp(){
  char lsTemperatura[7];
  int cont;
  Serial.print("{\"LsTemp\": ");
  for(cont=6; cont<=sizeof(lsTemperatura); cont++){
    lsTemperatura[cont] = char(EEPROM.read(cont));
    Serial.print(lsTemperatura[cont]);
  }
  Serial.println("}\n");
}

char LimiteInferiorHs(){
  char liHsuelo[11];
  int cont;  
  Serial.print("{\"LiHs\": ");
  for(cont=10; cont<=sizeof(liHsuelo); cont++){
    liHsuelo[cont] = char(EEPROM.read(cont));
    Serial.print(liHsuelo[cont]);
  }
  Serial.println("}\n");
}

char LimiteSuperiorHs(){
  char lsHsuelo[15];
  int cont;
  Serial.print("LsHs: ");
  for(cont=14; cont<=sizeof(lsHsuelo); cont++){
    lsHsuelo[cont] = char(EEPROM.read(cont));
    Serial.print(lsHsuelo[cont]);
  }
  Serial.println("}\n");
}

char LimiteInferiorHa(){
  char liHaire[19];
  int cont; 
  Serial.print("{\"LiHa\": "); 
  for(cont=18; cont<=sizeof(liHaire); cont++){
    liHaire[cont] = char(EEPROM.read(cont));
    Serial.print(liHaire[cont]);
  }
  Serial.println("}\n");
}

char LimiteSuperiorHa(){
  char lsHaire[23];
  int cont;
  Serial.print("{\"LsHa\": ");
  for(cont=22; cont<=sizeof(lsHaire); cont++){
    lsHaire[cont] = char(EEPROM.read(cont));
    Serial.print(lsHaire[cont]);
  }
  Serial.println("}\n");
}

void Temperatura(){
  t = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  liTemp = LimiteInferiorTemp();
  lsTemp = LimiteSuperiorTemp();
  temperatura.medicion = t; //Registrando la lectura
  
  //Validando lecturas de Temperatura
  if (isnan(t)) {
    lcd16x2.clear();
    lcd16x2.setCursor(0,0);
    lcd16x2.print("Temp: ¡Error!");
  }
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("T:");
  lcd16x2.print(t);

  if(h<liTemp){
    EncenderRociador();
    temperatura.estado = true; //Registrando el estado
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Rociador Encendido");
    delay(500);
  }
  if(h>lsTemp){
    ApagarRociador();
    temperatura.estado = false; //Registrando el estado
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Rociador Apagado");
    delay(500);
  }
  delay(800);
}

void HumedadRelativa(){
  h = dht.readHumidity(); // Leemos la humedad relativa
  liHa = LimiteInferiorHa();
  lsHa = LimiteSuperiorHa();
  humedadRelativa.medicion = h; //Registrando lectura
  //Validando lecturas de Humedad Relativa
  if (isnan(h)) {
    lcd16x2.clear();
    lcd16x2.setCursor(0,0);
    lcd16x2.print("HA: ¡Error!");
  }
  //Desplegando datos en pantalla
  lcd16x2.setCursor(5,0);
  lcd16x2.print("HA:");
  lcd16x2.print(h);
    
  if(h<liHa){
    EncenderRociador();
    humedadRelativa.estado = true; //Registrando el estado
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Rociador Encendido");
    delay(500);
  }
  if(h>lsHa){
    ApagarRociador();
    humedadRelativa.estado = false; //Registrando el estado
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Rociador Apagado");
    delay(500);
  }  
  delay(800);
}

void HumedadSuelo(){
  valT = map(analogRead(sensorPinT), 0, 1023, 100, 0); //Trunca un valor del rango de (1023, 0) ajustandolo a los porcentajes de (0 y 100) (Humedad en suelo)
  valT = constrain(valT, 0, 100); //Restringe un número a estar dentro del porcentaje de 0 y 100
  liHs = LimiteInferiorHs();
  lsHs = LimiteSuperiorHs();
  humedadSuelo.medicion = valT; //Registrando la lectura
  
  //Desplegando datos de la Humedad en el suelo
  lcd16x2.setCursor(11,0);
  lcd16x2.print("HS:");
  lcd16x2.print(valT);
      
  if((valT >= 0) and (valT <= liHs)) { 
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Suelo seco      ");
    delay(900);
    humedadSuelo.estado = true; //Registrando el estado
    EncenderRiegoPorGoteo();
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Riego Encendido");
  }else{ 
    if((valT > liHs) and (valT <= lsHs)) {
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo humedo    ");
    }else{
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo mojado    ");
      delay(1000);
      humedadSuelo.estado = false; //Registrando el estado
      ApagarRiegoPorGoteo();
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Riego Apagado");
    }
  }
  delay(1000);
}

char LecturaLiTemp_LCD(){
  char liTemperatura[3];
  int cont;
  for(cont=2; cont<=sizeof(liTemperatura); cont++){
    liTemperatura[cont] = char(EEPROM.read(cont));
    lcd20x4.print(liTemperatura[cont]);
  }
}

char LecturaLsTemp_LCD(){
  char lsTemperatura[7];
  int cont;
  for(cont=6; cont<=sizeof(lsTemperatura); cont++){
    lsTemperatura[cont] = char(EEPROM.read(cont));
    lcd20x4.print(lsTemperatura[cont]);
  }
}

char LecturaLiHs_LCD(){
  char liHsuelo[11];
  int cont;  
  for(cont=10; cont<=sizeof(liHsuelo); cont++){
    liHsuelo[cont] = char(EEPROM.read(cont));
    lcd20x4.print(liHsuelo[cont]);
  }
}

char LecturaLsHs_LCD(){
  char lsHsuelo[15];
  int cont;
  for(cont=14; cont<=sizeof(lsHsuelo); cont++){
    lsHsuelo[cont] = char(EEPROM.read(cont));
    lcd20x4.print(lsHsuelo[cont]);
  }
}

char LecturaLiHa_LCD(){
  char liHaire[19];
  int cont;  
  for(cont=18; contliHa<=sizeof(liHaire); cont++){
    liHaire[cont] = char(EEPROM.read(cont));
    lcd20x4.print(liHaire[cont]);
  }
}

char LecturaLsHa_LCD(){
  char lsHaire[23];
  int cont;
  for(cont=22; cont<=sizeof(lsHaire); cont++){
    lsHaire[cont] = char(EEPROM.read(cont));
    lcd20x4.print(lsHaire[cont]);
  }
}

void sendData(MEDICION med){
  Serial.println("1");
  Serial.println("{\"invernadero\": ");
  Serial.print(med.invernadero);
  Serial.print(",");
  Serial.print("\"parametro\": ");
  Serial.print(med.parametro);
  Serial.print(",");
  Serial.print("\"sensor\": ");
  Serial.print(med.sensor);
  Serial.print(",");
  Serial.print("\"actuador\": ");
  Serial.print(med.actuador);
  Serial.print(",");
  Serial.print("\"medicion\": ");
  Serial.print(med.medicion);
  Serial.print(",");
  Serial.print("\"estado\": ");
  Serial.print(med.estado);
  Serial.println("}\n");
}

void sendModificacion(int id, float li, float ls){
  Serial.println("2");
  Serial.println("{\"id:\": ");
  Serial.print(id);
  Serial.print(",");
  Serial.print("\"li:\": ");
  Serial.print(li);
  Serial.print(",");
  Serial.print("\"ls:\": ");
  Serial.print(ls);
  Serial.println("}\n");
}

void Lecturas(){
  Temperatura(); // Metodo para la lectura de la tempeartura
  HumedadRelativa(); // Metodo para la lectura de la humedad relativa
  HumedadSuelo(); // Metodo para la lectura de la humedad en suelo

  //Despues de leer los datos, los enviamos por el puerto Serial
  sendData(humedadSuelo);
  sendData(humedadRelativa);
  sendData(temperatura);
}
