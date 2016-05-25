/*****************************************************************************************
**Control d'un hivernacle de manera automàtica o manual: Obertura o tancament del toldo **
**de l'ombrejat, obertura de finestres per eccès de temperatura, regulació de la humitat**         
** sistema de reg per goteig, control de la temperatura i encesa automàtica dels llums. **  
*****************************************************************************************/ 

//******  Includes  ********************************************************************** 

#define rele1 12 //Connexió del relè 1 (obertura de finestra) al pin 12.
#define rele2 11 //Connexió del relè 2 (ombrejat) al pin 11.
#define rele3 10 //Connexió del relè 3 (inversió de gir finestra) al pin 10.
#define rele4 9  //Connexió del relè 4 (inversió de gir ombrejat) al pin 9 .
#define rele5 8  //Connexió del relè 5 (sistema de calefacció) al pin 8.
#define rele6 7  //Connexió del relè 6 (electrovàlvula goteig) al pin 7.
#define rele7 6  //Connexió del relè 7 (electrovàlvula difusors) al pin 6.
#define rele8 5  //Connexió del relè 8 (llums) al pin 5 .
#define rele9 4  //Connexió del relè 9 (llum de senyalització) al pin 4.
#include "DHT.h" //Llibreria de control del sensor.
#define DHTPIN 2 //Pin de la connexió.
#define DHTTYPE DHT11 //Tipus de sensor.

DHT dht(DHTPIN, DHTTYPE);//Configuració de la llibreria del sensor.

const int LDR1 = A0;//Connexió del LDR1 (principal) al pin A0.
const int LDR2 = A1;//Connexió del LDR2 (inici toldo) al pin A1.
const int LDR3 = A2; //Connexió del LDR3 (fi toldo) al pin A2.
//const int finalDeCursaSuperior = A3 //Connexió final de cursa finestra al pin A3.
//const int finalDeCursaInferior = A4 //Connexió final de cursa finestra al pin A4.

int temperatura;//Determina la variable Temperatura.
int humitat;    //Determina la variable huitat.

int lluminositatExterior;//Determina la variable Lluminositat Exterior.
int finalDeCursaInici;   //Determina la variable Final de Cursa Inici.
int finalDeCursaFi;      //Determina la variable Final de Cursa Fí.

float n1, n2;// Variables amb coma flotant (punt decimal) per l'entrada manual.

//******  Setup  *************************************************************************

void setup() {
  pinMode (rele1, OUTPUT); //Configuració del relè 1 com a sortida.
  pinMode (rele2, OUTPUT); //Configuració del relè 2 com a sortida.
  pinMode (rele3, OUTPUT); //Configuració del relè 3 com a sortida.
  pinMode (rele4, OUTPUT); //Configuració del relè 4 com a sortida.
  pinMode (rele5, OUTPUT); //Configuració del relè 5 com a sortida.
  pinMode (rele6, OUTPUT); //Configuració del relè 6 com a sortida.
  pinMode (rele7, OUTPUT); //Configuració del relè 7 com a sortida.
  pinMode (rele8, OUTPUT); //Configuració del relè 8 com a sortida.
  pinMode (rele9, OUTPUT); //Configuració del relè 9 com a sortida.

  pinMode (LDR1, INPUT); //Configuració del LDR1 com a entrada.
  pinMode (LDR2, INPUT); //Configuració del LDR2 com a entrada.
  pinMode (LDR3, INPUT); //Configuració del LDR3 com a entrada.

  Serial.begin(9600); //Inicialitza el port serie.
  
  dht.begin();     //Inicialitza la llibreria dht.
  
  Serial.println("Per tal d'activar un element de forma manual:)");
                   //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("Introdueix primer un nombre per indicar l'acció a realitzar");
  //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("Accions:");//Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("1 = Visualització de la temperatura actual.");//Escriu el text i salta 
                   //de línea.
  Serial.println("2 = Visualització de la humitat actual.");//Escriu el text i salta de 
                   //línea.
  Serial.println("3 = Llums.");         //Escriu el text i salta de línea.
  Serial.println("4 = Reg per goteig.");//Escriu el text i salta de línea.
  Serial.println("5 = Difusors.");      //Escriu el text i salta de línea.
  Serial.println("6 = Calefacció" );    //Escriu el text i salta de línea.
  Serial.println();                     //Deixa un espai en blanc.
  Serial.println("I un segón nombre separat d'una coma per encendre o apagar-lo");
                   //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("1 = Activa l'acció.");//Escriu el text i salta de línea.
  Serial.println("0 = Atura l'acció."); //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
}

//******  Loop  **************************************************************************

void loop() {
  
  int humitat = dht.readHumidity(); //Iguala el valor intern humitat a dht. 
                                    //readHumidyty (lector de la humitat).
  int temperatura = dht.readTemperature();//Iguala el valor intern temeratura
                          //a dht. readTemperature (lector de la temperatura).
  int lluminositatExterior = analogRead (LDR1);//Iguala el valor intern
                          //lluminositatExterior al valor que llegeixi de LDR1.
  int finalDeCursaInici = analogRead (LDR2);//Iguala el valor intern
                          //finalDeCursaInici al valor que llegeixi de LDR2.
  int finalDeCursaFi = analogRead (LDR3);//Iguala el valor intern
                          //finalDeCursaFi al valor que llegeixi de LDR3.
   
   //Activació de la calefacció depenent de la temperatura.*******************************

  if (temperatura < 20)//Si la variable temperatura és menor a 20º:
    digitalWrite (rele5, LOW); //Activa el relè 5 (Calefacció).
  if (temperatura > 25)//Quan la variable temperatura és superior a 25º:
    digitalWrite (rele5, HIGH);//Apaga el relè 5 (Calefacció).
  //if ((temperatura > 30)&&(FinalDeCursaInferior))// Si la variable temperatura és
    //major a 30º i el final de cursa inferior està actiu:
    //digitalWrite (Rele1, LOW); //Activa el relè 1 (Obertura de finestra).
  //if (finalDeCursaSup)// Quan arriba al final de cursa superior:
    //digitalWrite (Rele1, HIGH); //Apaga el relè 1 (Obertura de finestra).
  //if ((temperatura < 50) && (temperatura >=27)&& (finalDeCursaSuperior))//Si la 
    //temperatura és menor que 50º, sup. o igual a 27º i el Final de cursa sup. està actiu:
    //digitalWrite (Rele3, LOW); //Activa el relè 3 (inversor de gir d'obertura de 
    //finestra.
    //digitalWrite (Relè1,LOW); //Activa el relè 1 (Tancament de finestra).
  //if /finalDeCursaInferior)//Quan s'activa el final de cursa Inferior:
    //digitalWrite (Relè1, HIGH);//Apaga el relè 3 (inversor de gir d'obertura de 
    //finestra.
    //digitalWrite (Relè3, HIGH);//Apaga el relè 1 (Tancament de finestra).

  //Ombrejat automàtic.*******************************************************************

  if ((lluminositatExterior<225)&& (finalDeCursaFi<225))//Si la variable LLuminositat 
    //i la variable Final de cursa fí, és més petita que 225:
    digitalWrite (rele2, LOW); //Activa el relè 2 (Obertura ombrejat).
  if (finalDeCursaFi>225)//Si la variable final de cursa fi és més gran que 225:
    digitalWrite (rele2, HIGH);//S'apaga el relè 2.
  if ((lluminositatExterior>500) && (finalDeCursaFi>570) && (finalDeCursaInici>570))
    //Si la variable  LLuminositat és major a 500 i el Final de cursa fí i Final de Cursa 
    //Inici són més grans que 570:
    digitalWrite (rele4, LOW); //Activa el relè 4 (inversió de gir ombrejat).
    digitalWrite (rele2, LOW); //Activa el relè 2 (Tancameent obrejat).
  if ((lluminositatExterior<600) && (finalDeCursaInici>500) && (finalDeCursaInici<600))
      digitalWrite (rele2, HIGH); //Desactiva el relè 2 (Tancameent obrejat).
      digitalWrite (rele4, LOW);//Desactiva el relè 4 (inversió de gir ombrejat).

  //Llums automàtics.*********************************************************************
    
  if (lluminositatExterior>2000) //Si la variable LLuminositat es superior a 2000:
    digitalWrite (rele8, LOW); //S'encèn el relè 8 (LLums).
    digitalWrite (rele9, LOW); //S'encèn el relè 9 (Senyalització).
    delay(1000);               //S'espera 1 segón.
    digitalWrite (rele9, HIGH);//S'apaga el relè 9.

  if (lluminositatExterior<1000) //Si la variable LLuminositat es més petita que 1000:
    digitalWrite (rele8, HIGH);//S'apaga el relè 8.
    digitalWrite (rele9, HIGH);//S'apaga el relè 9.

  //Difusors automàtics.******************************************************************
    
  if ((lluminositatExterior>800) && (humitat<30))
    digitalWrite (rele7, LOW);//S'encèn el relè 7 (Difusors).
    delay (10000);//S'espera 10 segons.
    digitalWrite (rele7, HIGH); //S'apaga el relè 7.

  //Reg Automàtic.************************************************************************

  //Control manual.***********************************************************************
  while (Serial.available() > 0) {// Si la variable es més gran que zero càrrega el codi.
    
    n1 = Serial.parseInt();//Llegeix el primer nómbre sencer de la posició en la que es
    // troba i el guarda a n1.
    n2 = Serial.parseInt();//Llegeix el primer nómbre sencer de la posició en la que es
    // troba i el guarda a n2.

    if (n1==1)
      Serial.print("Temperatura actual: ");//Imprimeix el tex "Temperatura actual:"
      Serial.print(temperatura); //Imprimeix la variable temperatura.
      Serial.print("ºC");//Imprimeix el símbol graus centigrads i salta de línea.

    if (n1==2)
      Serial.print("Humitat actual: ");//Imprimeix el text "Humitat actual:"
      Serial.print (humitat); //Imprimeix la variable humitat.
      Serial.println ("%"); //Imprimeix el símbol percentatge i salta de línea.

    if ((n1==3)&&(n2==1))
      digitalWrite (rele8, LOW);//S'encèn el relè 8 (Llums).
      Serial.println("Llums: ON ");//Imprimeix el text "Llums:ON" i salta de línea.
      
    if ((n1==3)&&(n2==0))
      digitalWrite (rele8, HIGH);//S'apaga el relè 8 (Llums).
      Serial.println("Llums: OFF ");//Imprimeix el text "Llums:OFF "i salta de línea
      
    if ((n1==4)&&(n2==1))
      digitalWrite (rele6, LOW);//S'encèn el relè 6 (Reg per goteig).
      Serial.println("Reg de goteig: ON ");//Imprimeix el text "Reg de goteig:ON" i 
      //salta de línea.
      
    if ((n1==4)&&(n2==0))
      digitalWrite (rele6, HIGH);//S'apaga el relè 6 (Reg per oteig).
      Serial.println("Reg de goteig: OFF ");//Imprimeix el text "Reg de goteig:OFF" 
      //i salta de línea.
      
    if ((n1==5)&&(n2==1))
      digitalWrite (rele7, LOW);//S'encèn el relè 7 (Difusors).
      Serial.println("Difusors: ON ");//Imprimeix el text "Difusors:ON" i salta de 
      //línea.
      
    if ((n1==5)&&(n2==0))
      digitalWrite (rele7, HIGH);//S'apaga el relè 7 (Difusors).
      Serial.println("Difusors: OFF ");//Imprimeix el text "Difusors:OFF" i salta de 
      //línea.

    if ((n1==6)&&(n2==1))
      digitalWrite (rele5, LOW);//S'encèn el relè 5 (Calefacció).
      Serial.println("Calefactor: ON ");//Imprimeix el text "Calefactor:ON" i salta de 
      //línea.
      
    if ((n1==6)&&(n2==0))
      digitalWrite (rele5, HIGH);//S'apaga el relè 5 (Calefacció).
      Serial.println("Calefactor: OFF ");//Imprimeix el text "Calefactor:OFF" i salta
      //de línea.
  
    if (Serial.read() == '\n') { //Comproba el final de línea.


        }
    }
}

