#include <Keypad.h> // libreria necesaria para KeyPad
#include <Wire.h>  // libreria necesaria para bus I2C
#include <DHT.h> //Incluimos librería
#include <LiquidCrystal_I2C.h> //libreria necesaria para display I2C

#define DHTPIN 10 //Definimos el pin digital donde se conecta el sensor
#define DHTTYPE DHT11 //Dependiendo del tipo de sensor

//Declaración de variables
const int sensorPinT = A0;
const int releGoteo = 11;
const int releRocio = 12;
const byte ROWS = 4;
const byte COLS = 4;
bool riegoGoteo = false, rociador = false;
int y, z, liHs = 0, lsHs = 100, liTemp = 0, lsTemp = 50;
float liHa = 20, lsHa = 95;
char key;
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

  lcd16x2.init(); // initialza el lcd 16x2
  lcd20x4.init(); // initialza el lcd 20x4
  
  lcd16x2.backlight(); // enciende la luz de fondo del display
  lcd20x4.backlight(); // enciende la luz de fondo del display
  MenuPrincipal();
}

void loop(){
  z = 0;
  y = 0;
  key = keypad.getKey();
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
        MenuActivarActuador(); //Metodo para mostrar el menu del caso B "Activar actuador"
        z++;
      break;
    }
  }
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

void MenuModificarStnd(){
  y = 0;
  while(z<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ // Modificar Temp (temperatura)
      Temperatura(); // Metodo para la lectura de la tempeartura
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Limit inferior: ");
      lcd20x4.print(liTemp);
      lcd20x4.print("*C");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limit superior: ");
      lcd20x4.print(lsTemp);
      lcd20x4.print("*C");
      lcd20x4.setCursor(0,2);
      lcd20x4.print("[1] Nuevo Estandar");
      MenuNuevoEstandar(); // Metodo para capturar el nuevo estandar
      y++;
    }
    if(key == '2'){ // Modificar HS (humedad del suelo)
      HumedadSuelo(); // Metodo para la lectura de la humedad en suelo
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Limite inferior: ");
      lcd20x4.print(liHs);
      lcd20x4.print(" %");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limite superior: ");
      lcd20x4.print(lsHs);
      lcd20x4.print(" %");
      lcd20x4.setCursor(0,2);
      lcd20x4.print("[1] Nuevo Estandar");
      MenuNuevoEstandar(); // Metodo para capturar el nuevo estandar
      y++;
    }
    if(key == '3'){ // Modificar HA (humedad relativa)
      HumedadRelativa(); // Metodo para la lectura de la humedad relativa
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Limite inferior: ");
      lcd20x4.print(liHa);
      lcd20x4.print(" %");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Limite superior: ");
      lcd20x4.print(lsHa);
      lcd20x4.print(" %");
      lcd20x4.setCursor(0,2);
      lcd20x4.print("[1] Nuevo Estandar");
      MenuNuevoEstandar(); // Metodo para capturar el nuevo valor
      y++;
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); //Metodo para mostrar el menu principal
    }
  }
}

void MenuActivarActuador(){
  y = 0;
  while(z<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){ // Riego por goteo
      //if(riegoGoteo){
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Estado actual:");
      MenuRiegoPorGoteo();
      //}
      y++;
    }
    if(key == '2'){ // Rociador
      //if(rociador){
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Estado actual:");
      MenuRociador();
      //}
      y++;
    }
    if(key == 'C'){ // Cancelar
      MenuPrincipal(); //Metodo para mostrar el menu principal
    }
  }
}

void MenuNuevoEstandar(){
  while(y<1){
    key = keypad.getKey(); //Leyendo Keypad
    delay(100);
    if(key == '1'){
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Nuevo Estandar:");
      CapturarEstandar();
    }
  }
}

void CapturarEstandar(){
  int i = 0;
  while(i<2){
    key = keypad.getKey();
    if(key){
      lcd20x4.print(key);
      i++;
    }
  }
  delay(1000);
  lcd20x4.setCursor(0,1);
  lcd20x4.print("[A] Guardar valor");
  lcd20x4.setCursor(0,2);
  lcd20x4.print("[C] Cancelar");

  //FALTA TERMINAR
}
float lecturaTemp(){
  float t = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  return t;
}
int lecturaHS(){
  int valT;
  //Capturando datos
  valT = map(analogRead(sensorPinT), 0, 1023, 100, 0); //Trunca un valor del rango de (1023, 0) ajustandolo a los porcentajes de (0 y 100)
  valT = constrain(valT, 0, 100); //Restringe un número a estar dentro del porcentaje de 0 y 100
  return valT;
}

float lecturaHR(){
  float h = dht.readHumidity(); // Leemos la humedad relativa
  return h;
}

void Temperatura(){
  float t = lecturaTemp();
  //Validando lecturas de Temperatura
  if (isnan(t)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    //return;
  }
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("Temp: ");
  lcd16x2.print(t);
  lcd16x2.print(" *C");
  delay(1500);
}

void HumedadSuelo(){
  int valT = lecturaHS();
  
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
    delay(3000);
    lcd16x2.clear();
    lcd16x2.print("Riego por goteo on");
  }else{ 
    if((valT >=34) and (valT <= 66)) {
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo humedo");
    }else{
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo mojado");
      digitalWrite(releGoteo, LOW);
      riegoGoteo = false;
      delay(3000);
      lcd16x2.clear();
      lcd16x2.print("Riego por goteo off");
    }
  }
  delay(1500);
}

void HumedadRelativa(){
  float h = lecturaHR();
  //Validando lecturas de Humedad Relativa
  if (isnan(h)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    //return;
  }
  //Desplegando datos en pantalla y puerto Serial
  lcd16x2.clear();
  lcd16x2.setCursor(0,0);
  lcd16x2.print("HA: ");
  lcd16x2.print(h);
  lcd16x2.print(" %");
  delay(1500);
}

void MenuRiegoPorGoteo(){
  int valT = lecturaHS();
  if((valT >= 0) and (valT <= 33)) {
    lcd16x2.setCursor(0,1);
    lcd16x2.print("Suelo seco");
    digitalWrite(releGoteo, HIGH);
    riegoGoteo = true;
    Serial.println("Riego por goteo encendido");
  }else{ 
    if((valT >=34) and (valT <= 66)) {
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo humedo");
    }else{
      lcd16x2.setCursor(0,1);
      lcd16x2.print("Suelo mojado ");
      digitalWrite(releGoteo, LOW);
      riegoGoteo = false;
      Serial.println("Riego por goteo apagado");
    }
  }
}

void MenuRociador(){
  float h = lecturaHR();
  HumedadRelativa();
  while(y<1){
    if(h<40){
      digitalWrite(releRocio, HIGH);
      rociador = true;
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Estado Actual");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Rociador encendido");
      delay(6000);
    }
    if(h>60){
      digitalWrite(releRocio, LOW);
      rociador = false;
      lcd20x4.clear();
      lcd20x4.setCursor(0,0);
      lcd20x4.print("Estado Actual");
      lcd20x4.setCursor(0,1);
      lcd20x4.print("Rociador apagado");
      delay(6000);
    }
  }
}
