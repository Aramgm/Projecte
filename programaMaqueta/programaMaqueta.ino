/*****************************************************************************************
**Control d'un hivernacle de manera automàtica o manual: Obertura o tancament del toldo **
**de l'ombrejat, obertura de finestres per eccès de temperatura, regulació de la humitat**         
** sistema de reg per goteig, control de la temperatura i encesa automàtica dels llums. **  
*****************************************************************************************/ 

//******  Includes  ********************************************************************** 

#define DHTPIN 2 //Pin de la connexió.
#define DHTTYPE DHT11 //Tipus de sensor.

#include <Time.h>//Llibreria per al control de temps (programador de reg).
#include "DHT.h" //Llibreria de control del sensor.

DHT dht(DHTPIN, DHTTYPE);//Configuració de la llibreria del sensor.

byte horaInici = 12, minutInici = 02;//Hora i minuts d'inici del reg (Goteig).
byte horaFi = 12, minutFi = 03;      //Hora i minuts de la finalització del reg.

int estatRele[9]={0,0,0,0,0,0,0,0,0};/*Array que actua com a memòria i determina que 
les variables dels Relès 1, 2, 3, 4, 5, 6, 7, 8, 9 estàn a 0.*/

int rele[9]={12,11,10,9,8,7,6,5,4};/*Array que determina la connexió del relè 1  
(inversió de gir finestra)al pin 1, del relè 2 (inversió de gir ombrejat) al pin 
11, del relè 3 (obertura de finestra) al pin 10,del relè 4 (ombrejat) al pin 9, 
del relè 5 (sistema de calefacció) al pin 8, del relè 6 (electrovàlvula goteig) al 
pin 7, del relè 7(electrovàlvula difusors) al pin 6, del relè 8 (llums) al pin 5 i  
del relè 9 (llum de senyalització) al pin 4.*/

int temperatura;//Determina la variable Temperatura.
int humitat;    //Determina la variable huitat.

int lluminositatExterior;//Determina la variable Lluminositat Exterior.
int finalDeCursaInici;   //Determina la variable Final de Cursa Inici.
int finalDeCursaFi;      //Determina la variable Final de Cursa Fí.

int inici = (horaInici * 60)+ minutInici; //Temps en minuts del inici de reg.
int fi = (horaFi * 60) + minutFi;         //Temps en minuts de la finalització del reg.

const int LDR1 = A0;//Connexió del LDR1 (principal) al pin A0.
const int LDR2 = A1;//Connexió del LDR2 (inici toldo) al pin A1.
const int LDR3 = A2; //Connexió del LDR3 (fi toldo) al pin A2.

const int finalDeCursaSuperior = 1; //Connexió final de cursa finestra al pin 1.
const int finalDeCursaInferior = 3; //Connexió final de cursa finestra al pin 3.

float n1, n2;// Variables amb coma flotant (punt decimal) per l'entrada manual.

//******  Setup  *************************************************************************

void setup() {
  for (int i=0; i < 9; i++){
    pinMode(rele[i],OUTPUT);//Configuració dels relès com a sortida.
  }
  
  pinMode (LDR1, INPUT); //Configuració del LDR1 com a entrada.
  pinMode (LDR2, INPUT); //Configuració del LDR2 com a entrada.
  pinMode (LDR3, INPUT); //Configuració del LDR3 com a entrada.

  Serial.begin(9600); //Inicialitza el port serie.
  
  dht.begin();     //Inicialitza la llibreria dht.

  setTime (12,00,00,30,04,2016);/*Hora, minut, segón, dia, mes i any definit al carregar
  el programa al Arduino.*/

  for (int e=0; e < 9; e++){
  estatRele[e]=0;/*Determina que la variables Relè 1, 2, 3, 4, 5, 6, 7, 8, 9 estàn a 0. 
  Actua com a memòria.*/
  }
  
  for (int a=0; a < 8; a++){
    digitalWrite (rele[a], HIGH);/*Apaga els relès "funcionen al revès": 1 (Obertura de 
    finestra), 2 (Obertura ombrejat), 3 (Inversor de gir finestra), 4 (inversió de gir 
    ombrejat), 5 (Calefacció), 6 (Goteig), 7 (Difusors) i 8 (Llums).Funcionen al revés*/
  }
  
  /*El mòdul de relès (8 Relay module) funcionen inversament: S'encenen amb LOW i s'apaguen 
  amb HIGH. El nové Relè es independent i funcioa de manera positiva (normal)*/
  
  digitalWrite (rele[8], LOW);//S'apaga el relè 9 (Llum indicació)

  Serial.println("Per activar un element de forma manual:");
                   //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("1-Introdueix primer un nombre per indicar l\47acci\363 a realitzar");
  //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("  Accions:");//Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.println("  1 = Visualitzaci\363 de la temperatura actual.");/*Escriu el text i 
  salta de línea.*/
  Serial.println("  2 = Visualitzaci\363 de la humitat actual.");/*Escriu el text i salta 
  de línea.*/
  Serial.println("  3 = Llums.");         //Escriu el text i salta de línea.
  Serial.println("  4 = Reg per goteig.");//Escriu el text i salta de línea.
  Serial.println("  5 = Difusors.");      //Escriu el text i salta de línea.
  Serial.println("  6 = Calefacci\363" ); //Escriu el text i salta de línea.
  Serial.println("  7 = Ombrejat" );      //Escriu el text i salta de línea.
  Serial.println("  8 = Finestra" );      //Escriu el text i salta de línea.
  Serial.println("  9 = Hora actual" );   //Escriu el text i salta de línea.
  Serial.println();                       //Deixa un espai en blanc.
  Serial.print("2-Posa un segon nombre separat del primer amb una coma per encendre o ");
  //Escriu el text.
  Serial.print("apagar l'acci\363");
  Serial.println();//Deixa un espai en blanc.             
  Serial.println();//Deixa un espai en blanc.
  Serial.println("  1 = Activar acci\363.");//Escriu el text i salta de línea.
  Serial.println("  0 = Aturar acci\363."); //Escriu el text i salta de línea.
  Serial.println();//Deixa un espai en blanc.
  Serial.print("______________________________________________________________________");
  Serial.println("________________");//Separació visual pantalla.
  Serial.println();//Deixa un espai en blanc.
  Serial.print("                                    Hora d\47inici: ");//Escriu el text.
  Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
  hora,els : els minuts, : i els segons.*/
  Serial.println();//Deixa un espai en blanc.

}//Tancament de void setup.

//******  Loop  **************************************************************************

void loop() {
  
  int humitat = dht.readHumidity(); /*Iguala el valor intern humitat a dht. 
  readHumidyty (lector de la humitat).*/
  int temperatura = dht.readTemperature();/*Iguala el valor intern temeratura a dht. 
  readTemperature (lector de la temperatura).*/
  int lluminositatExterior = analogRead (LDR1);/*Iguala el valor intern lluminositatExterior
  al valor que llegeixi de LDR1.*/
  int finalDeCursaInici = analogRead (LDR2);/*Iguala el valor intern finalDeCursaInici al 
  valor que llegeixi de LDR2.*/
  int finalDeCursaFi = analogRead (LDR3);/*Iguala el valor intern finalDeCursaFi al valor 
  que llegeixi de LDR3.*/
   
  //Activació de la calefacció depenent de la temperatura.********************************
  
  if ((temperatura < 15)&&(estatRele[4]==0)){/*Si la variable temperatura és menor a 15º i 
    el relè5 està aturat:*/
    digitalWrite (rele[4], LOW); //Activa el relè 5 (Calefacció).Funciona al revés.
    Serial.print ("Calafacci\363 autom\340tica");/*Imprimeix el tex "Calefacció 
    automàtica" i salta de línea.*/
    Serial.print ("                             ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[4]=2;//Posa la memória del relè5 a memòria 2 (procés automàtic).
    
  }//Tancament de l'if.
  
  if ((temperatura>20)&&(estatRele[4]==2)){/*Quan la variable temperatura és superior a 20º
    i el relè5 està en memòria de procés automàtic:*/
    digitalWrite (rele[4], HIGH);//Apaga el relè 5 (Calefacció).Funciona al revés.
    Serial.print ("Aturada autom\340tica del Calefactor");/*Imprimeix el tex "Aturada 
    automàtica del calefactor.*/
    Serial.print ("                 ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[4]=0;//Posa la memória del relè5 a 0.
    
  }//Tancament de l'if.

  //Control de la finestra.***************************************************************
  
  if ((temperatura > 30)&&(finalDeCursaInferior)&&( estatRele[2]==0 )){/* Si la variable 
    temperatura és major a 30º, el final de cursa inferior està actiu i el relè 3 està
    apagat:*/
    digitalWrite (rele[2], LOW); //Activa el relè 3 (Obertura de finestra).Funciona al revés.
    estatRele[2]=2;//Posa la memória del relè3 a 2 (procès automàtic).
    
  }//Tancament de l'if.
    
  if ((finalDeCursaSuperior)&&(estatRele[2]==2)){/*Quan arriba al final de cursa superior i 
    el relè 3 està en memòria de procés automàtic:*/
    digitalWrite (rele[2], HIGH); //Apaga el relè 3 (Obertura de finestra).Funciona al revés.
    estatRele[2]=0;//Posa la memória del relè1 a 0.
    
  }//Tancament de l'if.
    
  if ((temperatura<30)&&(temperatura>=20)&&(finalDeCursaSuperior)&&(estatRele[2]==0)&&(estatRele[0]==0)){
    /*Si la temperatura és menor que 50º, sup. o igual a 27º i el Final de cursa sup. està
    actiu i els relès 3 i 1 estàn aturats:*/
    digitalWrite (rele[0], LOW); /*Activa el relè 1 (inversor de gir d'obertura de finestra.
    Funciona al revés.*/
    digitalWrite (rele[2],LOW); /*Activa el relè 3 (Tancament de finestra). Funciona al 
    revés.*/
    estatRele[0]=1;//Posa la memória del relè1 a 1.
    estatRele[2]=1;//Posa la memória del relè3 a 1.
    
  }//Tancament de l'if.
    
  if ((finalDeCursaInferior)&&(estatRele[0]==1)&&(estatRele[2]==1)){/*Quan s'activa el final 
    de cursa Inferior i estàn actius els relès 3 i 1:*/
    digitalWrite (rele[2], HIGH);//Apaga el relè 3 (Tancament de finestra).Funciona al revés.
    digitalWrite (rele[0], HIGH);/*Apaga el relè 1 (inversor de gir d'obertura de finestra).
    Funciona al revés.*/
    estatRele[2]=0;//Posa la memória del relè3 a 0.
    estatRele[0]=0;//Posa la memória del relè1 a 0.
    
  }//Tancament de l'if.
    
  //Ombrejat automàtic.*******************************************************************

  if ((lluminositatExterior>350)&&(finalDeCursaFi>350)&&(estatRele[3]==0)){/*Si la variable 
    LLuminositat i la variable Final de cursa fí són més petites que 2 i el relè4 està 
    apagat:*/
    digitalWrite (rele[3], LOW); //Activa el relè 4 (Obertura ombrejat).Funciona al revés.
    Serial.print ("Ombrejat autom\340tic");/*Imprimeix el tex "Ombrejat automàtic" i 
    salta de línea*/
    Serial.print ("                                ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[3]=1;//Posa la memória del relè4 a 1.
    
  }//Tancament de l'if.
  
  else{ //Si no coincideix amb la variable if anterior:
    digitalWrite (rele[3], HIGH);//S'apaga el relè 4.Funciona al revés.
    estatRele[3]=0;//Posa la memória del relè4 a 0.
    
  }//Tancament de l'else.
  
  if ((lluminositatExterior<250)&&(lluminositatExterior>230)&&(finalDeCursaFi<230)&&(finalDeCursaInici<225)&&(estatRele[3]==0)&&(estatRele[1]==0)){
    /*Si la variable  LLuminositat és major a 2 i més petita que tres i el Final de 
    cursa fí el Final de Cursa Inici són més petits que 2 i els relès 2 i 4 estàn 
    apagats:*/
    digitalWrite (rele[1], LOW); /*Activa el relè 2 (inversió de gir ombrejat).Funciona al 
    revés.*/
    digitalWrite (rele[3], LOW); //Activa el relè 4 (Tancament obrejat). Funciona al revés.
    Serial.print ("Recollida autom\340tica del ombrejat");/*Imprimeix el tex "Recollida 
    automàtica del ombrejat" i salta de línea.*/
    Serial.print ("                 ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[1]=1;//Posa la memória del relè2 a 1.
    estatRele[3]=1;//Posa la memória del relè4 a 1.
    
  }//Tancament de l'if.
  
  else{ //Si no coincideix amb la variable if anterior:
     digitalWrite (rele[3], HIGH);/*Desactiva el relè 4 (Tancament obrejat).Funciona al 
     revés.*/
     digitalWrite (rele[1], HIGH);/*Desactiva el relè 2 (inversió de gir ombrejat).Funciona 
     al revés.*/
     estatRele[1]=0;//Posa la memória del relè2 a 0.
     estatRele[3]=0;//Posa la memória del relè4 a 0.
     
   }//Tancament de l'else.

  //Llums automàtics.*********************************************************************
    
  if ((lluminositatExterior<5) && (estatRele[7]==0)){ /*Si la variable LLuminositat es
    inferior a 1 i el relè8 està apagat:*/
    digitalWrite (rele[7], LOW); //S'encèn el relè 8 (LLums).Funciona al revés.
    Serial.print ("Conexi\363 de llums autom\340tics"); /*Imprimeix el tex "Llums 
    automàtics".*/
    Serial.print ("                       ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[7]=2;//Posa la memória del relè8 a memòria 2.
    
  }//Tancament de l'if.
  
  if (estatRele[7]==2){ //Si el relè8 està està en memòria de procés automàtic:
    digitalWrite (rele[8], HIGH); //S'encèn el relè 9 (Senyalització).
    delay(1000);               //S'espera 1 segón.
    digitalWrite (rele[8], LOW);//S'apaga el relè 9.
    
  }//Tancament de l'if.

  if ((lluminositatExterior>20 ) && (estatRele[7]==2)){ /*Si la variable 
    LLuminositat es més gran que 2 i el relè 8 està en memòria de procés automàtic:*/
    digitalWrite (rele[7], HIGH);//S'apaga el relè 8.Funciona al revés.
    digitalWrite (rele[8], LOW);//S'apaga el relè 9.
    Serial.print ("Desconexi\363 de llums autom\340tics"); /*Imprimeix el tex "Llums 
    automàtics".*/
    Serial.print ("                    ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[7]=0;//Posa la memória del relè8 a 0.
    
  }//Tancament de l'if.

  //Difusors automàtics.******************************************************************
    
  if ((lluminositatExterior<3) && (humitat<30)&&(estatRele[6]==0)){/*Quan la lluminositat
    exterior és més petita que 3, la humitat és inferior a 30 i el relè7 està apagat:*/
    digitalWrite (rele[6], LOW);//S'encèn el relè 7 (Difusors). Funciona al revés.
    Serial.print ("Connexi\363 de Difusors autom\340tics");/*Imprimeix el tex "Conexió
    de difusors automàtics".*/
    Serial.print ("                   ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    estatRele[6]=2;//Posa la memória del relè7 a 2 (funcionament automàtic).
    
  }//Tancament de l'if.
  
  if (estatRele[6]==2){  
    delay (10000);              //S'espera 10 segons.
    digitalWrite (rele[6], HIGH); //S'apaga el relè 7. Funciona al revés.
    Serial.print ("Desconnexi\363 autom\340tica de Difusors");/*Imprimeix el tex 
    "Desconexió automàtica de difusors".*/
    Serial.print ("                ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    estatRele[6]=0;//Posa la memória del relè7 a 0.
    
  }//Tancament de l'if.
  
  //Reg Automàtic.************************************************************************

  if (((hour()*60+minute())>=inici)&&((hour()*60+minute())< fi)&&(estatRele[5]==0)){/*Quan 
    el temps actual(en minuts) és més gran o igual al temps (en minuts) de l'hora d'inici i 
    al mateix temps més petit que la hora de finalització de reg (també en minuts) "fi" i 
    el relè 6 està apagat:*/
    digitalWrite (rele[5], LOW);  //S'encèn el relè 6 (Goteig). Funciona al revés.
    Serial.print ("Reg autom\340tic");//Imprimeix el tex "Reg automàtic" i salta de línea.
    Serial.print ("                                     ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    Serial.println();//Deixa un espai en blanc.
    estatRele[5]=2;//Posa la memória del relè6 a 2 (funcionament automàtic).
    
  }//Tancament de l'if.
  
  if (((hour() * 60 + minute())==fi) && (estatRele[5]==2)){/*Si la hora actual és igual a 
    la hora de finalització de reg "fi" i el relè6 està en memòria de procés automàtic:*/
    digitalWrite (rele[5],HIGH);  //S'apaga el relè 6 (Goteig). Funciona al revés.
    Serial.print ("Aturada autom\340tica del reg");//Imprimeix el tex "Aturada
    //automàtica del reg" i salta de línea.
    Serial.print ("                        ");//Deixa un espai.
    Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
    hora,els : els minuts, : i els segons.*/
    estatRele[5]=0;//Posa la memória del relè6 a 0.
    
  }//Tancament de l'if.
  
  //Control manual.***********************************************************************
  
  while (Serial.available() > 0) {// Si la variable es més gran que zero càrrega el codi. 
       
    n1 = Serial.parseInt();/*Llegeix el primer nómbre sencer de la posició en la que es
    troba i el guarda a n1.*/
    n2 = Serial.parseInt();/*Llegeix el primer nómbre sencer de la posició en la que es
    troba i el guarda a n2.*/

    if (n1==1){//Si n1 es igual a 1:
      Serial.print("  Temperatura actual: ");//Imprimeix el tex "Temperatura actual:"
      Serial.print(temperatura); //Imprimeix la variable temperatura.
      Serial.println("\52C");//Imprimeix el símbol graus centigrads i salta de línea.
      Serial.println();//Deixa un espai en blanc.
      
    }//Tancament de l'if.
    
    if (n1==2){//Si n1 es igual a 2:
      Serial.print("  Humitat actual: ");//Imprimeix el text "Humitat actual:"
      Serial.print (humitat); //Imprimeix la variable humitat.
      Serial.println ("%"); //Imprimeix el símbol percentatge i salta de línea.
      Serial.println();//Deixa un espai en blanc.
      
    }//Tancament de l'if.
    
    if ((n1==3)&&(n2==1) && (estatRele[7]==0)){/*Si n1 es igual a 3, n2 es igual a 1 i el
      relè8 es troba apagat:*/
      digitalWrite (rele[7], LOW);//S'encèn el relè 8 (Llums). Funciona al revés.
      Serial.println("  Llums: ON ");//Imprimeix el text "Llums:ON" i salta de línea.
      Serial.println();//Deixa un espai en blanc.
      estatRele[7]=1;//Posa la memória del relè8 a 1.
      
    }//Tancament de l'if.
    
    if ((n1==3)&&(n2==0) && (estatRele[7]==1)){/*Si n1 es igual a 3, n2 es igual a 0 i el
      relè8 es troba actiu:*/
      digitalWrite (rele[7], HIGH);//S'apaga el relè 8 (Llums). Funciona al revés.
      Serial.println("  Llums: OFF ");//Imprimeix el text "Llums:OFF "i salta de línea
      Serial.println();//Deixa un espai en blanc.
      estatRele[7]=0;//Posa la memória del relè8 a 0.
      
    }//Tancament de l'if.
    
    if ((n1==4)&&(n2==1) && (estatRele[5]==0)){/*Si n1 es igual a 4, n2 es igual a 1 i el
      relè6 es troba apagat:*/
      digitalWrite (rele[5], LOW);//S'encèn el relè 6 (Reg per goteig). Funciona al revés.
      Serial.println("  Reg de goteig: ON ");/*Imprimeix el text "Reg de goteig:ON" i 
      salta de línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[5]=1;//Posa la memória del relè6 a 1.
      
    }//Tancament de l'if.
    
    if ((n1==4)&&(n2==0) && (estatRele[5]==1)){/*Si n1 es igual a 4, n2 es igual a 0 i el
      relè6 es troba actiu:*/
      digitalWrite (rele[5], HIGH);//S'apaga el relè 6 (Reg per goteig). Funciona al revés.
      Serial.println("  Reg de goteig: OFF ");/*Imprimeix el text "Reg de goteig:OFF" i 
      salta de línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[5]=0;//Posa la memória del relè6 a 0.
      
    }//Tancament de l'if.
    
    if ((n1==5)&&(n2==1) && (estatRele[6]==0)){/*Si n1 es igual a 5, n2 es igual a 1 i el
      relè7 es troba apagat:*/
      digitalWrite (rele[6], LOW);//S'encèn el relè 7 (Difusors). Funciona al revés.
      Serial.println("  Difusors: ON ");//Imprimeix el text "Difusors:ON" i salta de línea.
      Serial.println();//Deixa un espai en blanc.
      estatRele[6]=1;//Posa la memória del relè7 a 1.
      
    }//Tancament de l'if.
    
    if ((n1==5)&&(n2==0) && (estatRele[6]==1)){/*Si n1 es igual a 5, n2 es igual a 0 i el
      relè7 es troba actiu:*/
      digitalWrite (rele[6], HIGH);//S'apaga el relè 7 (Difusors). Funciona al revés.
      Serial.println("  Difusors: OFF ");/*Imprimeix el text "Difusors:OFF" i salta de 
      línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[6]=0;//Posa la memória del relè7 a 0.
      
    }//Tancament de l'if.
    
    if ((n1==6)&&(n2==1) && (estatRele[4]==0)){/*Si n1 es igual a 6, n2 es igual a 1 i el
      relè5 es troba apagat:*/
      digitalWrite (rele[4], LOW);//S'encèn el relè 5 (Calefacció). Funciona al revés.
      Serial.println("  Calefactor: ON ");/*Imprimeix el text "Calefactor:ON" i salta de 
      línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[4]=1;//Posa la memória del relè5 a 1.
      
    }//Tancament de l'if.
    
    if ((n1==6)&&(n2==0) && (estatRele[4]==1)){/*Si n1 es igual a 6, n2 es igual a 0 i el
      relè5 es troba actiu:*/
      digitalWrite (rele[4], HIGH);//S'apaga el relè 5 (Calefacció). Funciona al revés.
      Serial.println("  Calefactor: OFF ");/*Imprimeix el text "Calefactor:OFF" i salta
      de línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[4]=0;//Posa la memória del relè5 a 0.
      
      }//Tancament de l'if.

    if ((n1==7)&&(n2==1) && (estatRele[3]==0)){/*Si n1 es igual a 7, n2 es igual a 1 i el
      relè4 es troba apagat:*/
      digitalWrite (rele[3], LOW);//S'encèn el relè 4 (Ombrejat). Funciona al revés.
      Serial.println("  Ombrejat: ON ");//Imprimeix el text "Ombrejat:ON" i salta de línea.
      Serial.println();//Deixa un espai en blanc.
      estatRele[3]=1;//Posa la memória del relè2 a 1.
      
    }//Tancament de l'if.

    if ((n1==7)&&(n2==2) && (estatRele[1]==0) && (estatRele[3]==0)){/*Si n1 es igual a 7, 
      n2 es igual a 1 i els reles 2 i 4 estàn apagats:*/
      digitalWrite (rele[1], LOW);/*S'encèn el relè 2 (Inversor de gir de l'Ombrejat). F
      unciona al revés.*/
      digitalWrite (rele[3], LOW);//S'encèn el relè 4 (Ombrejat). Funciona al revés.
      Serial.println("  Ombrejat: OFF ");/*Imprimeix el text "Ombrejat:OFF" i salta de 
      línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[1]=1;//Posa la memória del relè2 a 1.
      estatRele[3]=1;//Posa la memória del relè4 a 1.
      
    }//Tancament de l'if.

    if ((n1==7)&&(n2==0)){//Si n1 es igual a 7, n2 es igual a 0:
      digitalWrite (rele[3], HIGH);//S'apaga el relè 4 (Ombrejat). Funciona al revés.
      digitalWrite (rele[1], HIGH);/*S'apaga el relè 2 (Inversió de gir de l'Ombrejat).
      Funciona al revés.*/
      estatRele[1]=0;//Posa la memória del relè2 a 0.
      estatRele[3]=0;//Posa la memória del relè4 a 0.
      
    }//Tancament de l'if.

    if ((n1==8) && (n2==1) && (estatRele[2]==0)){/*Si n1 es igual a 8, n2 es igual a 1 i el
      relè 3 està apagat:*/
      digitalWrite (rele[2], LOW);/*S'encèn el relè 3 (Inversor de gir de l'Ombrejat).
      Funciona al revés.*/
      Serial.println("  Finestra: ON ");/*Imprimeix el text "Finestra: ON" i salta de 
      línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[2]=1;//Posa la memória del relè3 a 1.
      
    }//Tancament de l'if.

    if ((n1==8)&&(n2==2) && (estatRele[0]==0) && (estatRele[2]==0)){/*Si n1 es igual a 8, n2 
      es igual a 2 i els relès 1 i 3 estàn apagats:*/
      digitalWrite (rele[0], LOW);/*S'encèn el relè 1 (Inversor de gir de Finestra).Funciona 
      al revés.*/
      digitalWrite (rele[2], LOW);//S'encèn el relè 3 (Finestra). Funciona al revés.
      Serial.println("  Finestra: OFF ");/*Imprimeix el text "Finestra:OFF" i salta de 
      línea.*/
      Serial.println();//Deixa un espai en blanc.
      estatRele[2]=1;//Posa la memória del relè3 a 1.
      estatRele[4]=1;//Posa la memória del relè1 a 1.
      
    }//Tancament de l'if.

    if ((n1==8)&&(n2==0)){//Si n1 es igual a 7, n2 es igual a 0:
      digitalWrite (rele[2], HIGH);//S'apaga el relè 3 (Finestra). Funciona al revés.
      digitalWrite (rele[0], HIGH);/*S'apaga el relè 1 (Inversor de gir finestra). Funciona 
      al revés.*/
      estatRele[2]=0;//Posa la memória del relè3 a 0.
      estatRele[0]=0;//Posa la memória del relè1 a 0.
      
    }//Tancament de l'if.
    
    if (n1==9){//Si n1 es igual a 9:
      Serial.print("                                     Hora actual: ");//Escriu el text.
      Serial.println(String(hour())+":"+String(minute())+":"+ String(second()));/*Mostra la
      hora,els : els minuts, : i els segons.*/
      Serial.println();//Deixa un espai en blanc.
      
    }//Tancament de l'if.
    
  }//Tancament del while.
  
}//Tancament del void loop.




