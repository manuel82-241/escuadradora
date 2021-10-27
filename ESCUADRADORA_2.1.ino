#include "Nextion.h"    //incluir libreria nexion
#include "SoftwareSerial.h"   //incluimos libreria puerto serie para bluetooth
//VARIABLES EJE X
int pinActivacionBobinasMotor = 2;          //PIN ACTIVACION BOBINA EJE X(PUL)2
int pinSentidoGiroMotor  = 5;               //PIN ACTIVACION SENTIDO GIRO EJE X(DIR)5
int  pinActivacionDriverMotor  = 8;         //PIN HABILITAR DRIVERS MOTORES 8
int pinFinalDeCarrera = 9;                  //PIN FINAL DE CARRERA EJE X 9
boolean Homing_Hecho = false;               //HOMING EJE X HECHO
float ConversionPasos = 0;                  //VARIABLE PARA CONVERTIR MILIMETROS A PASOS
float Distancia_Maxima_X = 1700;            //DISTANCIA MAXIMA DE RECORRIDO EJE X
float Posicion_Eje_X = Distancia_Maxima_X;  //VARIABLE PARA POSICIONAR EL EJE X AL HACER HOMING
float Distancia_Minima_X = 206;             //DISTANCIA MINIMA OFFSET PARA EJE X EN MILIMETROS
char ejeX = "X";
int PasosPorMilimetro = 2560;
unsigned long VelocidadGiroMotor = 10;  //variación de la velocidad de giro
 
//VARIABLES EJE Y
int pinActivacionBobinasMotor_Y = 3;          //PIN ACTIVACION BOBINA EJE Y(PUL)3
int pinSentidoGiroMotor_Y  = 6;               //PIN ACTIVACION SENTIDO GIRO EJE Y(DIR)6
int pinFinalDeCarrera_Y = 10;                 //PIN FINAL DE CARRERA EJE Y 10
boolean Homing_Hecho_Y = false;             //HOMING EJE Y HECHO
float ConversionPasos_Y = 0;                //VARIABLE PARA CONVERTIR MILIMETROS A PASOS
float Distancia_Maxima_Y = 970;            //DISTANCIA MAXIMA DE RECORRIDO EJE Y
float Posicion_Eje_Y = Distancia_Maxima_Y;  //VARIABLE PARA POSICIONAR EL EJE Y AL HACER HOMING
float Distancia_Minima_Y = -2;             //DISTANCIA MINIMA OFFSET PARA EJE Y EN MILIMETROS
char ejeY = "Y";
int PasosPorMilimetro_Y = 1600;             //PASOS POR MILIMETRO MOTOR EJE Y
unsigned long VelocidadGiroMotor_Y = 4;      //VELOCIDAD GIRO MOTOR EJE Y
 
/* RELACION DE VARIABLES CON LOS BOTONES DE PANTALLA NEXTION */
//BOTONES EJE X
NexButton botRetro05X = NexButton(1,5,"botRetro05X");     //parámetros de identifiacion de los botones en la pantalla nexion
NexButton botAv05X = NexButton(1,6,"botAv05X");
NexButton botRetro1X = NexButton(1,7,"botRetro1X");
NexButton botAv1X = NexButton(1,8,"botAv1X");
NexButton botHomingX = NexButton(1,9,"botHomingX");
NexButton botonEnviarX = NexButton(1,4,"botonEnviarX");
NexText introDistX = NexText(1,3,"introDistX");
NexText muestraDistX = NexText(1,2,"muestraDistX");
 
//BOTONES EJE Y
NexButton botRetro05Y = NexButton(2,5,"botRetro05Y");    
NexButton botAv05Y = NexButton(2,6,"botAv05Y");
NexButton botRetro1Y = NexButton(2,7,"botRetro1Y");
NexButton botAv1Y = NexButton(2,8,"botAv1Y");
NexButton botHomingY = NexButton(2,9,"botHomingY");
NexButton botonEnviarY = NexButton(2,4,"botonEnviarY");
NexText introDistY = NexText(2,3,"introDistY");
NexText muestraDistY = NexText(2,2,"muestraDistY");
 
//BOTONES EJE Z
NexTouch *nex_listen_list[]=                    //cargar lista de botones y leer los cambios en los botones
{
  //EJE X
  &botRetro05X,
  &botAv05X,
  &botRetro1X,
  &botAv1X,
  &botHomingX,
  &botonEnviarX,
  &introDistX,
  //EJE Y
  &botRetro05Y,
  &botAv05Y,
  &botRetro1Y,
  &botAv1Y,
  &botHomingY,
  &botonEnviarY,
  &introDistY,
  //EJE Z
  NULL
};
 
/*DECLARACION DE FUCNIONES EJE X*/
 
void mostrarDistancia(float parDistancia)     //funcion para mostrar la distancia por pantalla como variable texto
{
  char bufer[10]={0};  //Cadena donde almacenaremos el número convertido
  muestraDistX.setText(dtostrf(parDistancia, 6, 1, bufer));
}
 
void introducirDistancia()          //funcion para introducir la medida en mm por pantalla y valorar para saber en que sentido tiene que girar el motor
{
  if(Homing_Hecho==true)
  {
    char bufer[10]={0};  //Cadena donde almacenaremos el número convertido
    introDistX.getText(bufer,sizeof(bufer));   //pasamos el texto y lo dimensionamos
    float distanciaIntroducida = atof(bufer);   //pasamos a float la variable char bufer
    if((distanciaIntroducida < Posicion_Eje_X) && (distanciaIntroducida > Distancia_Minima_X))   //si la distancia introducida es menor a la posicion del eje x y es mayor que 100
    {
      float diferenciaEntreDistanciaIntroducidayPosicionEjeX = Posicion_Eje_X - distanciaIntroducida;       //hacemos la diferencia en float entre la distanciaIntroducida y la posicion del eje X
      diferenciaEntreDistanciaIntroducidayPosicionEjeX = diferenciaEntreDistanciaIntroducidayPosicionEjeX *(-1);    //multiplicamos por -1 para volverla negativa para mover el motor hacia el punto de inicio
      Mover_Eje(ejeX,diferenciaEntreDistanciaIntroducidayPosicionEjeX);   //enviamos a la funcion mover eje el parametro obtenido de la diferencia
      //MuestraDist.setText(dtostrf(distanciaIntroducida, 6, 1, bufer));    //mostramos en pantalla la distancia introducida que coincide con la posicion del eje X una vez llevado el motor a esa posicion 
    mostrarDistancia(distanciaIntroducida);
  }
    else if((distanciaIntroducida > Posicion_Eje_X) && (distanciaIntroducida < Distancia_Maxima_X))    //si la distancia introducida es mayor a la posicion del eje x y menor a la distancia maxima (1700)
    {
      float diferenciaEntreDistanciaIntroducidayPosicionEjeX = distanciaIntroducida - Posicion_Eje_X;   //hacemos la diferencia en float entre la distanciaIntroducida y la posicion del eje X
      Mover_Eje(ejeX,diferenciaEntreDistanciaIntroducidayPosicionEjeX);       //enviamos a la funcion mover eje el parametro obtenido de la diferencia
      //MuestraDist.setText(dtostrf(distanciaIntroducida, 6, 1, bufer));    //mostramos en pantalla la distancia introducida que coincide con la posicion del eje X una vez llevado el motor a esa posicion
      mostrarDistancia(distanciaIntroducida);
  }
    else if((distanciaIntroducida < Distancia_Minima_X) || (distanciaIntroducida > Distancia_Maxima_X) || (distanciaIntroducida == Posicion_Eje_X))   //si la distancia introducida esta fuera de rango es menor a 100 o mayor a 1700 es invalida por lo tanto solo mostramos en pantalla la posicion del eje X
    {
       mostrarDistancia(Posicion_Eje_X);
     //MuestraDist.setText(dtostrf(Posicion_Eje_X, 6, 1, bufer));
    }     
  }
}
 
void sentidoMotorHaciaElCero()      //funcion para definir el sentido de giro del motor para mover el tope hacia el cero
{
  digitalWrite(pinSentidoGiroMotor,LOW);
}
 
void sentidoMotorHaciaDistanciaMaxima()   //funcion para definir el sentido de giro del motor para mover el tope hacia la distancia máxima
{
  digitalWrite(pinSentidoGiroMotor,HIGH);
}
 
void BotonHaciaMaximo1()      //funcion para mover el tope hacia la distancia máxima 1mm con cada pulsacion del boton
{
  if(Homing_Hecho==true)
  {
    Mover_Eje(ejeX,1);   //pasamos a la funcion mover_eje dos parametros eje x y -1
  }
}
 
void BotonHaciaMinimo1()      //funcion para mover el tope hacia el punto 0 1mm con cada pulsacion del boton
{
  if(Homing_Hecho==true)
  {
     Mover_Eje(ejeX,-1);        //pasamos a la funcion mover_eje dos parametros eje x y 1
  }
}
 
void BotonHaciaMaximo05()     //funcion para mover el tope hacia la distancia máxima 0.5mm con cada pulsacion del boton
{
  if(Homing_Hecho==true)
  {
    Mover_Eje(ejeX,0.5);       //pasamos a la funcion mover_eje dos parametros eje x y -0.5
  }
}
 
void BotonHaciaMinimo05()     //funcion para mover el tope hacia el punto 0 0.5mm con cada pulsacion del boton
{
  if(Homing_Hecho==true)
  {
    Mover_Eje(ejeX,-0.5);        //pasamos a la funcion mover_eje dos parametros eje x y 0.5
  }
}
 
void Mover_Eje(char Parametro_Eje,float Distancia)      //funcion para mover el eje y valorar la distancia para saber cuanto se tiene que mover y en que sentido
{                      
  if((Parametro_Eje == ejeX) 
  && ((Posicion_Eje_X + Distancia) <= Distancia_Maxima_X) 
  && ((Posicion_Eje_X + Distancia) >= Distancia_Minima_X))     //valoramos que estemos en el ejeX y que nunca se supere la distancia maxima y siempre la distancia sea mayor o igual a 100mm es lo maximo que permite acercar el tope 
  {
 
    if(Distancia < 0)         //si le pasamos una distancia negativa
      {
        sentidoMotorHaciaElCero();       
        Posicion_Eje_X = Posicion_Eje_X + Distancia;
    Distancia=Distancia*(-1);                     //pasamos la distancia a postiiva para el bucle del for hacerlo en valor absoluto
        Mover_Motor(Distancia);
      }
 
  else if(Distancia > 0)      
    {
    sentidoMotorHaciaDistanciaMaxima();
      Posicion_Eje_X = Posicion_Eje_X + Distancia;
      Mover_Motor(Distancia);
    }
  }
      mostrarDistancia(Posicion_Eje_X);                 
 
}
 
void Mover_Motor(float Distancia)       //funcion que convierte los milimetros de la distancia en los pasos que tiene que dar el motor para recorrer esa distancia
{
  ConversionPasos=Distancia*PasosPorMilimetro;
  /*if(Homing_Hecho==false)
  {
  for(float x=0;x<ConversionPasos && digitalRead(pinFinalDeCarrera)==1;x++)
  {
    digitalWrite(pinActivacionBobinasMotor,HIGH);
    delayMicroseconds(VelocidadGiroMotor);
    digitalWrite(pinActivacionBobinasMotor,LOW);
    delayMicroseconds(VelocidadGiroMotor);
  }
  }
    else
  {
  for(float x=0;x<ConversionPasos;x++)
  {
    digitalWrite(pinActivacionBobinasMotor,HIGH);
    delayMicroseconds(VelocidadGiroMotor);
    digitalWrite(pinActivacionBobinasMotor,LOW);
    delayMicroseconds(VelocidadGiroMotor);
  }
  }*/
  for(float x=0;x<ConversionPasos;x++)
  {
    digitalWrite(pinActivacionBobinasMotor,HIGH);
    delayMicroseconds(VelocidadGiroMotor);
    digitalWrite(pinActivacionBobinasMotor,LOW);
    delayMicroseconds(VelocidadGiroMotor);
  }
  digitalWrite(pinActivacionBobinasMotor,0);
  digitalWrite(pinActivacionDriverMotor,0);
}
 
void BotonHoming()        //función para hacer homing cada vez que se quiera
{
  Posicion_Eje_X = Distancia_Maxima_X;  //cada vez que hacemos un homing es porque no sabemos donde esta el tope o por lo que sea no está en la posicion correcta por eso hacemos esta asignación
  Homing_Hecho == false; //sirve para hacer homing varias veces tras la inicial
  while(digitalRead(pinFinalDeCarrera)==1)
  {
    Mover_Eje(ejeX,-0.5);    //pasamos el parametro X y la distancia en el sentido de giro del homing
  }
 
  //delay(500);     
  //Mover_Eje(ejeX,1);    //pasamos el parametro X y la distancia en el sentido de giro contrario al homing
  //delay(500);
  //Mover_Eje(ejeX,-1);
  Homing_Hecho = true;
  Posicion_Eje_X = Distancia_Minima_X;
  mostrarDistancia(Posicion_Eje_X);
}
 
/*DECLARACION DE FUNCIONES EJE Y*/
 
void mostrarDistanciaEjeY(float parDistancia)     //funcion para mostrar la distancia por pantalla como variable texto
{
  char bufer[10]={0};  //Cadena donde almacenaremos el número convertido
  muestraDistY.setText(dtostrf(parDistancia, 6, 1, bufer));
}
 
void introducirDistanciaEjeY()          //funcion para introducir la medida en mm por pantalla y valorar para saber en que sentido tiene que girar el motor
{
  if(Homing_Hecho_Y==true)
  {
    char bufer[10]={0};  //Cadena donde almacenaremos el número convertido
    introDistY.getText(bufer,sizeof(bufer));   //pasamos el texto y lo dimensionamos
    float distanciaIntroducida = atof(bufer);   //pasamos a float la variable char bufer
    if((distanciaIntroducida < Posicion_Eje_Y) && (distanciaIntroducida > Distancia_Minima_Y))   //si la distancia introducida es menor a la posicion del eje Y y es mayor que 100
    {
      float diferenciaEntreDistanciaIntroducidayPosicionEjeY = Posicion_Eje_Y - distanciaIntroducida;       //hacemos la diferencia en float entre la distanciaIntroducida y la posicion del eje Y
      diferenciaEntreDistanciaIntroducidayPosicionEjeY = diferenciaEntreDistanciaIntroducidayPosicionEjeY *(-1);    //multiplicamos por -1 para volverla negativa para mover el motor hacia el punto de inicio
      Mover_EjeY(ejeY,diferenciaEntreDistanciaIntroducidayPosicionEjeY);   //enviamos a la funcion mover eje el parametro obtenido de la diferencia
      //MuestraDist.setText(dtostrf(distanciaIntroducida, 6, 1, bufer));    //mostramos en pantalla la distancia introducida que coincide con la posicion del eje Y una vez llevado el motor a esa posicion 
    mostrarDistanciaEjeY(distanciaIntroducida);
  }
    else if((distanciaIntroducida > Posicion_Eje_Y) && (distanciaIntroducida < Distancia_Maxima_Y))    //si la distancia introducida es mayor a la posicion del eje Y y menor a la distancia maxima Y(1000)
    {
      float diferenciaEntreDistanciaIntroducidayPosicionEjeY = distanciaIntroducida - Posicion_Eje_Y;   //hacemos la diferencia en float entre la distanciaIntroducida y la posicion del eje Y
      Mover_EjeY(ejeY,diferenciaEntreDistanciaIntroducidayPosicionEjeY);       //enviamos a la funcion mover eje el parametro obtenido de la diferencia
      //MuestraDist.setText(dtostrf(distanciaIntroducida, 6, 1, bufer));    //mostramos en pantalla la distancia introducida que coincide con la posicion del eje Y una vez llevado el motor a esa posicion
      mostrarDistanciaEjeY(distanciaIntroducida);
  }
    else if((distanciaIntroducida < Distancia_Minima_Y) || (distanciaIntroducida > Distancia_Maxima_Y) || (distanciaIntroducida == Posicion_Eje_Y))   //si la distancia introducida esta fuera de rango es menor a 100 o mayor a 10000 es invalida por lo tanto solo mostramos en pantalla la posicion del eje X
    {
       mostrarDistanciaEjeY(Posicion_Eje_Y);
     //MuestraDist.setText(dtostrf(Posicion_Eje_Y, 6, 1, bufer));
    }     
  }
}
 
void sentidoMotorHaciaElCeroEjeY()      //funcion para definir el sentido de giro del motor para mover el tope hacia el cero
{
  digitalWrite(pinSentidoGiroMotor_Y,LOW);
}
 
void sentidoMotorHaciaDistanciaMaximaEjeY()   //funcion para definir el sentido de giro del motor para mover el tope hacia la distancia máxima
{
  digitalWrite(pinSentidoGiroMotor_Y,HIGH);
}
 
void BotonHaciaMaximo1EjeY()      //funcion para mover el tope hacia la distancia máxima 1mm con cada pulsacion del boton
{
  if(Homing_Hecho_Y==true)
  {
    Mover_EjeY(ejeY,1);   //pasamos a la funcion mover_eje dos parametros eje x y -1
  }
}
 
void BotonHaciaMinimo1EjeY()      //funcion para mover el tope hacia el punto 0 1mm con cada pulsacion del boton
{
  if(Homing_Hecho_Y==true)
  {
     Mover_EjeY(ejeY,-1);        //pasamos a la funcion mover_eje dos parametros eje x y 1
  }
}
 
void BotonHaciaMaximo05EjeY()     //funcion para mover el tope hacia la distancia máxima 0.5mm con cada pulsacion del boton
{
  if(Homing_Hecho_Y==true)
  {
    Mover_EjeY(ejeY,0.5);       //pasamos a la funcion mover_eje dos parametros eje x y -0.5
  }
}
 
void BotonHaciaMinimo05EjeY()     //funcion para mover el tope hacia el punto 0 0.5mm con cada pulsacion del boton
{
  if(Homing_Hecho_Y==true)
  {
    Mover_EjeY(ejeY,-0.5);        //pasamos a la funcion mover_eje dos parametros eje x y 0.5
  }
}
 
void Mover_EjeY(char Parametro_Eje,float Distancia)      //funcion para mover el eje y valorar la distancia para saber cuanto se tiene que mover y en que sentido
{                      
  if((Parametro_Eje == ejeY) 
  && ((Posicion_Eje_Y + Distancia) <= Distancia_Maxima_Y) 
  && ((Posicion_Eje_Y + Distancia) >= Distancia_Minima_Y))     //valoramos que estemos en el ejeX y que nunca se supere la distancia maxima y siempre la distancia sea mayor o igual a 100mm es lo maximo que permite acercar el tope 
  {
 
    if(Distancia < 0)         //si le pasamos una distancia negativa
      {
        sentidoMotorHaciaElCeroEjeY();       
        Posicion_Eje_Y = Posicion_Eje_Y + Distancia;
    Distancia=Distancia*(-1);                     //pasamos la distancia a postiiva para el bucle del for hacerlo en valor absoluto
        Mover_MotorY(Distancia);
      }
 
  else if(Distancia > 0)      
    {
    sentidoMotorHaciaDistanciaMaximaEjeY();
      Posicion_Eje_Y = Posicion_Eje_Y + Distancia;
      Mover_MotorY(Distancia);
    }
  }
      mostrarDistanciaEjeY(Posicion_Eje_Y);                 
 
}
 
void Mover_MotorY(float Distancia)       //funcion que convierte los milimetros de la distancia en los pasos que tiene que dar el motor para recorrer esa distancia
{
  ConversionPasos_Y=Distancia*PasosPorMilimetro_Y;
  if(Homing_Hecho_Y==false)
  {
  for(float x=0;x<ConversionPasos_Y && digitalRead(pinFinalDeCarrera_Y)==1;x++)
  {
    digitalWrite(pinActivacionBobinasMotor_Y,HIGH);
    delayMicroseconds(VelocidadGiroMotor_Y);
    digitalWrite(pinActivacionBobinasMotor_Y,LOW);
    delayMicroseconds(VelocidadGiroMotor_Y);
  }
  }
  else
  {
  for(float x=0;x<ConversionPasos_Y;x++)
  {
    digitalWrite(pinActivacionBobinasMotor_Y,HIGH);
    delayMicroseconds(VelocidadGiroMotor_Y);
    digitalWrite(pinActivacionBobinasMotor_Y,LOW);
    delayMicroseconds(VelocidadGiroMotor_Y);
  }
  }
  digitalWrite(pinActivacionBobinasMotor_Y,0);
  digitalWrite(pinActivacionDriverMotor,0);
}
 
void BotonHomingEjeY()        //función para hacer homing cada vez que se quiera
{
  Posicion_Eje_Y = Distancia_Maxima_Y;  //cada vez que hacemos un homing es porque no sabemos donde esta el tope o por lo que sea no está en la posicion correcta por eso hacemos esta asignación
  Homing_Hecho_Y = false; //sirve para hacer homing varias veces tras la inicial
  
    Mover_EjeY(ejeY,-970);    //pasamos el parametro X y la distancia en el sentido de giro del homing
  
 
  //delay(500);     
  //Mover_Eje(ejeX,1);    //pasamos el parametro X y la distancia en el sentido de giro contrario al homing
  //delay(500);
  //Mover_Eje(ejeX,-1);
  Homing_Hecho_Y = true;
  Posicion_Eje_Y = Distancia_Minima_Y;
  mostrarDistanciaEjeY(Posicion_Eje_Y);
}
 
void setup() {          
  Serial.begin(9600);     //iniciamos el puerto serie a 9600 baudios igual que la pantalla
  nexInit();
  //EJE X
  pinMode(pinActivacionBobinasMotor,OUTPUT);                   //declaración de pines como salida
  pinMode(pinSentidoGiroMotor,OUTPUT);
  pinMode(pinActivacionDriverMotor , OUTPUT);
  pinMode(pinFinalDeCarrera, INPUT_PULLUP);           //declaración como entrada y activando resistencia de pullup para el 0 lógico
  //EJE Y
  pinMode(pinActivacionBobinasMotor_Y,OUTPUT);                   //declaración de pines como salida
  pinMode(pinSentidoGiroMotor_Y,OUTPUT);
  pinMode(pinFinalDeCarrera_Y, INPUT_PULLUP);           //declaración como entrada y activando resistencia de pullup para el 0 lógico
  botRetro05X.attachPop(BotonHaciaMaximo05);             //declarar que hace al pulsar cada botón
  botAv05X.attachPop(BotonHaciaMinimo05);
  botRetro1X.attachPop(BotonHaciaMaximo1);
  botAv1X.attachPop(BotonHaciaMinimo1);
  botonEnviarX.attachPop(introducirDistancia);
  botHomingX.attachPop(BotonHoming);
  //EJE Y
  botRetro05Y.attachPop(BotonHaciaMinimo05EjeY);             //declarar que hace al pulsar cada botón
  botAv05Y.attachPop(BotonHaciaMaximo05EjeY);
  botRetro1Y.attachPop(BotonHaciaMinimo1EjeY);
  botAv1Y.attachPop(BotonHaciaMaximo1EjeY);
  botonEnviarY.attachPop(introducirDistanciaEjeY);
  botHomingY.attachPop(BotonHomingEjeY);
}
 
void loop() {
  nexLoop(nex_listen_list); 
}
