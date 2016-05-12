//Creado por Edgar Catarino López Ramos
//cerradura electronica
//@edgarslr
#include <Keypad.h> // Controla el teclado
#include <LiquidCrystal.h>  //controla el LCD

#include <EEPROM.h>  //COntrola E/S EEPROM
#include <SPI.h>
#include <Ethernet.h>

//*****************************************
// Configuracion del Ethernet Shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xEE}; // Direccion MAC
byte ip[] = { 192,168,1,100 }; // Direccion IP del Arduino
byte server[] = { 192,168,1,22 }; // Direccion IP del servidor
EthernetClient client; 


LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //display LCD
const byte filas = 4;
const byte columnas = 4;



byte pinsFilas[filas] = {43, 44, 45, 46};
byte pinsColumnas[columnas] = {47, 48, 49, 42};
char teclas[filas][columnas] =  {
  {'1', '2', '3', 'A'},                       // Declaración del teclado
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
Keypad teclado = Keypad(makeKeymap(teclas), pinsFilas, pinsColumnas, filas, columnas);
char password[5]; //almacena la contraseña en eeprom
char ingreso;  //ingreso del usuario
char passUser[4];
char confirmPass[4];  //confirmacion de la contraseña
int aprobacion=0;
//char ca[7] = {'3', '7', '1', '9', '2', 'A'}; //clave admin establecida
//char cal[7];   // Clave admin leida
//int contador = 0; //Lleva la posicion del array
//int cursorr = 6; //Lleva la posicion del cursor
//int comp;  // comparacion entre 2 arrays
int i=0;
int a; //aux
void setup() {
   // LEDS QUE INDICAN ABIERTO(13) O CERRADO(19)
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  //digitalWrite(19, HIGH); //enciende el led de cerrado 
  //digitalWrite(5, HIGH);
  lcd.begin(16, 2);   //Configuracion lcd 16X2 (columnas,fila)

  lcd.setCursor(0, 0);
  lcd.print(" --CONTRASENA-- ");
  lcd.setCursor(5, 1);
  lcd.print("____");
  //LEER CONTRASEÑA DE LA EEPROM 

Ethernet.begin(mac, ip); // Inicializamos el Ethernet Shield
  delay(1000); // Esperamos 1 segundo de cortesia

//        error if(password[4]!='Z'){     
    //correción
    if(EEPROM.read(4)!='Z'){
    EEPROM.write(0,'1');
    EEPROM.write(1,'2');
    EEPROM.write(2,'3');
     EEPROM.write(3,'4');
    EEPROM.write(4,'Z'); 
  }
   for (int i = 0; i <= 4; i++) { 
    password[i] = EEPROM.read(i);
  }              
  
}//fin del setup

void loop() {
    leerIngreso(1);
}
 




////////////////////////////////////////////

void leerIngreso(int a) {
  ingreso = teclado.getKey();
  if (ingreso != NO_KEY)
    switch (ingreso) {
      case 'A':    // es como el "enter" para introducir la password
        if(evaluar(1)==1)
        correcto();
        else{
        msgError();
        }
        reset();
        break;
        
      case 'B':
        informacion();  //muestra en el lcd las opcionesde la caja fuerte
        reset();
        break;
        
      case 'C':
        cambioPass();
        reset();
        break;
      case 'D':
        lcd.setCursor(0, 0);
        lcd.noDisplay(); 
        
        reset();
        break;
      case '*':
        lcd.setCursor(0, 0);
        lcd.display(); 
        reset();
        break;
      case '#':
        lcd.setCursor(0, 0);
        lcd.print("Presionaste #");
        delay(3000);
        reset();
        break;
      default: //si es un numero debe imprimirlo en el LCD y ademas guardarlo en el arreglo passUser
        if(a==1){
        passUser[i] = ingreso;
        printPass(passUser[i], 5 + i, 1);
        }
        if(a==2){
        confirmPass[i] = ingreso;
        printPass(confirmPass[i], 5 + i, 1);
        }
        i++;
        if (i > 3)
          i = 0;
   
}
}

void cleanlcd() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}


void printPass(char a, int columna, int fila ) {
  lcd.setCursor(columna, fila);
  lcd.print(a);
  delay(100);
  lcd.setCursor(columna, fila);
  lcd.print("*");
}

int evaluar(int a) {
  int j = 0;
  if (a==1){
  for (int i = 0; i <= 3; i++) {
    if (password[i] == passUser[i]) {
        j++;
    }
  }
  }
  if(a==2){
  for (int i = 0; i <= 3; i++) {
    if (passUser[i] == confirmPass[i]) {
        j++;
    }
  }
  }
  if (j == 4) {
    return j=1;
  }
  else {
    return j=0;
  }
}


void reset() {
  lcd.setCursor(0, 0);
  lcd.print(" --CONTRASENA-- ");
  lcd.setCursor(0, 1);
  lcd.print("-----____-----");
  for(int i =0;i<=3;i++){
  passUser[i]=NO_KEY;
  confirmPass[i]=NO_KEY;
  }
  i=0;
}

void msgError(){
  
        lcd.setCursor(0, 0);
        lcd.print("      ERROR      ");
        delay(500);
        servidor1();
        digitalWrite(5, LOW);
        delay(10);
        digitalWrite(5, HIGH);
        delay(1000);
      
    
  }
///////////////////////////////servidor
void servidor() {
  // Proceso de envio de muestras al servidor
  Serial.println("Connecting...");

  lcd.setCursor(0, 0);
  lcd.print("  conectando...  ");
  delay(1000);
  if (client.connect(server, 80)>0) {  // Conexion con el servidor
    client.print("GET /entrada.php?var="); // Enviamos los datos por GET
    client.print(aprobacion);
    client.println(" HTTP/1.0");
    client.println("User-Agent: Arduino 1.0");
    client.println();
    lcd.print("Conectado");
  } else {
    lcd.print("Fallo en la conexion");
  }
  if (!client.connected()) {
    lcd.setCursor(0, 0);
    lcd.print("Desconectado");
  }
  client.stop();
  client.flush();
   
}


//////
void servidor1() {
  // Proceso de envio de muestras al servidor
  Serial.println("Connecting...");

  lcd.setCursor(0, 0);
  lcd.print("  conectando...  ");
  delay(1000);
  if (client.connect(server, 80)>0) {  // Conexion con el servidor
    client.print("GET /fail.php?var="); // Enviamos los datos por GET
    client.print(aprobacion);
    client.println(" HTTP/1.0");
    client.println("User-Agent: Arduino 1.0");
    client.println();
    lcd.print("Conectado");
  } else {
    lcd.print("Fallo en la conexion");
  }
  if (!client.connected()) {
    lcd.setCursor(0, 0);
    lcd.print("desconectado");
  }
  client.stop();
  client.flush();
   
}
/////////


  

void correcto(){
        
        lcd.setCursor(0, 0);
        lcd.print("     CORRECTO      ");
        delay(500);
        servidor();
        //Abrir servomotor
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        delay(100);
        digitalWrite(5, HIGH);
        delay(100);
        digitalWrite(5, LOW);
        delay(100);
        digitalWrite(5, HIGH);
        delay(100);
        digitalWrite(5, LOW);
        delay(100);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        delay(1000);
}

  
void informacion() {
  lcd.setCursor(0, 0);
  lcd.print("'A' para introdu");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("cir la pass        ");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("'C' para cambiar");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print(" la pass             ");
  delay(1000);
}



void cambioPass() {
   lcd.setCursor(0, 0);
  lcd.print("Cambio de pass    ");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Introduce pass     ");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("anterior        ");
  delay(2000);  
  reset();
   while(passUser[3]==NO_KEY){
    leerIngreso(1);}
  if (evaluar(1) == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Introduce           ");
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("la pass nueva            ");
    delay(2000);
    reset();
    while(passUser[3]==NO_KEY){
      leerIngreso(1);
  }
    lcd.setCursor(0, 0);
    lcd.print("Vuelve a intro     ");
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("cirla           ");
    delay(2000);    
    lcd.setCursor(0, 0);
    lcd.print("  **PASSWORD**      ");
    lcd.setCursor(5, 1);
    lcd.print("____");
    lcd.setCursor(0, 0);
    i=0;
    while(confirmPass[3]==NO_KEY){
      leerIngreso(2);
  }
    if(evaluar(2)==1){
      // funcion de EEPROM
      for (int i = 0; i <= 3; i++) { 
         EEPROM.write(i, passUser[i]);
    }
      for (int i = 0; i <= 3; i++) { 
         password[i] = EEPROM.read(i);
  }
      lcd.setCursor(0, 0);
      lcd.print("Contraseña cam  ");
      delay(2000);
      lcd.setCursor(0, 0);
      lcd.print("biada           ");
      delay(2000);
    }
    else{
      lcd.setCursor(0, 0);
      lcd.print("Error las pass   ");
      delay(2000);
      lcd.setCursor(0, 0);
      lcd.print("no coinciden        ");
      delay(2000);
    }
    
  }
  else {
    msgError();
  }
  reset();
}

void passChange(){
 for(int i=0;i<=3;i++){
   password[i]=passUser[i];
 }
  
}


