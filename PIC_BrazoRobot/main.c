#include <p18f4553.h>
#include <stdio.h>
#include <stdlib.h> 
#include "interface.h"
#include "command.h"
#include "AtWareGraphics.h"
#include "teclado.h"

//////////////////////////////////
//			Robot Arm   		//
//////////////////////////////////

int g_nESB = 0;
int g_nPSB = 0;
int g_nES1 = 0;
int g_nPS1 = 0;
int g_nES2 = 0;
int g_nPS2 = 0;
int g_nES3 = 0;
int g_nPS3 = 0;

void ServoB(int nESB, int nPSB);
void Servo1(int nES1, int nPS1);
void Servo2(int nES2, int nPS2);
void Servo3(int nES3, int nPS3);

float PosicionMenor = 0;
float PosicionMayor = 0;

Encoder()
{	
	PORTD = 0x00;
	TRISD = 0x3f;
	
	PosicionMenor = 5.625 * PORTD;
	PosicionMayor = (5.625 * PORTD) + 5.625;

	PORTD = 0x00;
	TRISD = 0x00;

	//Limpiar el GLCD con el color de brocha
	ATGClear();							
																			
	g_Column = 2;
	g_Line	 = 2;

	printf("Base: (%d - %d)", (int) PosicionMenor, (int) PosicionMayor);

	g_Column = 2;
	g_Line	 = 3;

	printf("Servo 1: %d ", g_nPS1);

	g_Column = 2;
	g_Line	 = 4;

	printf("Servo 2: %d ", g_nPS2);

	g_Column = 2;
	g_Line	 = 5;

	printf("Servo 3: %d ", g_nPS3);
	//ATGClear();
}	

#pragma interrupt high_interrupt_handler
void high_interrupt_handler()
{
	//si se encendio la bandera de interrupcion del timer0
	if(INTCONbits.TMR0IF)
	{
		//borra la bandera de interrupcion del timer0
		INTCONbits.TMR0IF = 0;
		//deshabilita el permiso de interrupcion por timer0
		//INTCONbits.TMR0IE = 0;

		//unsigned short timer;

		//(unsigned char*)&timer = TMR0L;
		//*((unsigned char*)&timer +1) = TMR0H;
		//timer = 15536-timer;

		//TMR0H = *((unsigned char*)&timer+1);
		//TMR0L = *((unsigned char*)&timer);
	}

	//si se encendio la bandera de interrupcion del timer1
	if(PIR1bits.TMR1IF)
	{
		//borra la bandera de interrupcion del timer1
		PIR1bits.TMR1IF = 0;
		//deshabilita el permiso de interrupcion por timer1
		//PIE1bits.TMR1IE = 1;
	}
}

#pragma interruptlow low_interrupt_handler
void low_interrupt_handler()
{
}

//****************************************************************************//
//                        Vector de interrupciones                            //
//****************************************************************************//
/*#pragma code high_interrupt_vector = 0x002008
void high_interrupt_vector()
{
	_asm
		goto high_interrupt_handler
	_endasm
}

#pragma code low_interrupt_vector = 0x002018
void low_interrupt_vector()
{
	_asm
		goto low_interrupt_handler
	_endasm
}*/

//////////////////////////////////
//			Graficos			//
//////////////////////////////////
/*
Esta función es invocada por todas las funciones de salida estándar <stdio.h>
*/
int _user_putc(char c)
{
	//Realizar la escritura del caracter proveniente de STDOUT mediante la biblioteca gráfica.
	//El cursor de caracteres GLCD_Column y GLCD_Line puede cambiarse en cualquier momento a manera de gotoxy antes de escribir
	return ATGConsolePutChar(c);
}

/*
 ----------- GRAPHICS LCD HARDWARE ABSTRACTION LAYER --------------------
 By Cornejo.
 
 Este es un ejemplo de HAL funcionando en mi configuración local. MPU 48MHz, GLCD, a máxima velocidad estable.
 
 Indico pues, será necesario ver los sincrogramas y los tiempos necesarios que deberán cumplirse para 
 que la comunicación sea estable. Factores como la longitud de los cables y las capacitancias parásitas
 pueden aumentar los requerimientos de temporización. Por favor, experimenten. Traten con 6 nops, y vayan reduciendo
 hasta lograr un mínimo estable. Esto debido a que los chips son ensamblados por diferentes fabricantes de módulos.
 
 No usar ESS_DelayMS dentro de la implementación de una HAL un milisegundo es una eternidad.... Las operaciones gráficas son de alta frecuencia y 
 computacionalmente costosos. A menos que quieran ver como se escribe pixel por pixel.
 
*/

//BUS DE CONTROL (PINOUT DEFINITION)
#define GLCD_CS1 PORTBbits.RB7   
#define GLCD_CS2 PORTBbits.RB6
#define GLCD_DI  PORTBbits.RB5
#define GLCD_RW  PORTBbits.RB4
#define GLCD_E	 PORTBbits.RB3
#define GLCD_TRIS_CS1 TRISBbits.TRISB7
#define GLCD_TRIS_CS2 TRISBbits.TRISB6
#define GLCD_TRIS_DI  TRISBbits.TRISB5
#define GLCD_TRIS_RW  TRISBbits.TRISB4
#define GLCD_TRIS_E	  TRISBbits.TRISB3
#define GLCD_TRIS_OUTPUT_MASK 0x07      //Bits 7:3 del puerto B como salidas
//BUS DE DATOS (PINOUT DEFINITION)
#define GLCD_TRIS_DATA TRISD
#define GLCD_PORT_DATA PORTD


//HAL de lectura de datos del GLCD
void User_ATGReadData(void)
{
	do {User_ATGReadStatus();} while(g_ucGLCDDataIn&GLCD_STATUSREAD_BUSY);
	GLCD_RW=1;
	GLCD_DI=1;
	GLCD_E=1;
	Nop();
	GLCD_E=0;
	do {User_ATGReadStatus();} while(g_ucGLCDDataIn&GLCD_STATUSREAD_BUSY);
	GLCD_RW=1;
	GLCD_DI=1;
	GLCD_E=1;
	Nop();
	Nop();
	g_ucGLCDDataIn=GLCD_PORT_DATA;
	GLCD_E=0;
	GLCD_CS1=1;
	GLCD_CS2=1;
}
//HAL  de escritura de datos al GLCD
void  User_ATGWriteData(void)
{
	do {User_ATGReadStatus();} while(g_ucGLCDDataIn&GLCD_STATUSREAD_BUSY);
	GLCD_PORT_DATA=g_ucGLCDDataOut;
	GLCD_TRIS_DATA=0x00;
	GLCD_RW=0;
	GLCD_DI=1;
	GLCD_E=1;
	Nop();
	GLCD_E=0;
	GLCD_CS1=1;
	GLCD_CS2=1;
}
//HAL de escritura de comandos al GLCD
void  User_ATGWriteCommand(void)
{
	do {User_ATGReadStatus();} while(g_ucGLCDDataIn&GLCD_STATUSREAD_BUSY);
	GLCD_PORT_DATA=g_ucGLCDDataOut;
	GLCD_TRIS_DATA=0x00;
	GLCD_RW=0;
	GLCD_DI=0;
	GLCD_E=1;
	Nop();
	GLCD_E=0;
	GLCD_CS1=1;
	GLCD_CS2=1;
}
//HAL de lectura del bit de BUSY del GLCD(g_ulGLCDDevChild)
void User_ATGReadStatus(void)
{
	GLCD_TRIS_DATA=0xff;
	if(g_ucGLCDDevChild)
		GLCD_CS1=0;
	else 
		GLCD_CS2=0;
	GLCD_RW=1;
	GLCD_DI=0;
	GLCD_E=1;
	Nop();
	Nop();
	g_ucGLCDDataIn=GLCD_PORT_DATA;
	GLCD_E=0;
}

//////////////////////////////////////////
//			Comunicación USB			//
//////////////////////////////////////////

void HighPriorityISR(void);
void LowPriorityISR(void);

#pragma code HIGH_INTERRUPT_VECTOR=0x2008

void HighInterruptHook(void)
{
	_asm goto HighPriorityISR _endasm 
} 
#pragma code LOW_INTERRUPT_VECTOR=0x2018
void LowInterruptHook(void)
{
	_asm goto LowPriorityISR _endasm
}

#pragma code
#pragma interrupt HighPriorityISR
void HighPriorityISR(void)
{

}
#pragma interruptlow LowPriorityISR
void LowPriorityISR(void)
{
}

char USBOverlayCommandHandler(struct TRANSCEIVER_STATE_MACHINE *pM)
{
	//Puntero a la tabla virtual en rom del transmisor/receptor USB (véase "command.h")
	static rom struct TRANSCEIVER_VIRTUAL_CONST_TABLE* s_vtbl;
	//Puntero al comando recibido
	static struct PICCOMMAND* s_pPicCmd;
	
	//Extraer el puntero a la tabla virtual
	s_vtbl=pM->m_vtbl;
	//Extraer el puntero al comando
	s_pPicCmd=s_vtbl->m_pPicCmd;
	
	//Decodificar el comando
	switch(s_pPicCmd->m_bCommandCode)
	{
		//Procesar comandos definidos por el usuario.
		
		/*
			Supóngase que existe un comando con código 0x33 definido por usted,
			en alguna parte de su proyecto así:
			
			#define COMMAND_PIC_MYCOMMAND 0x33
			
			También supóngase que se necesita retorna un valor entero de 32 bits como respuesta
			cuyo valor se encuentra en una variable declarada como "long lRespuesta" en alguna parte.
			
			Luego, impleméntese dentro de ésta selección múltiple (switch) siguiendo el procedimiento
			ya descrito el código para notificar la respuesta al host:
			
			case COMMAND_PIC_MYCOMMAND:
				s_pPicCmd->m_bCommandCode=ERROR_PIC_SUCCESS;    		//Paso 1: código de suceso
				s_pPicCmd->m_bParamsBufferSize=sizeof(long);			//Paso 2: sizeof(long) es 4 bytes
				*((long*)(s_pPicCmd->m_abParamsBuffer))=lRespuesta; 	//Paso 3: copia los 4 bytes de la respuesta
				return 0;												//Paso 4: retornar 0 y enviar la respuesta al host
		*/
		case  COMMAND_DSM_STATUS:
		{
			char buffer[3];
			int x = 0;
			
			if(s_pPicCmd->m_abParamsBuffer[0] == '1')
				g_nESB = 1;
			else
				g_nESB = 0;

			if(s_pPicCmd->m_abParamsBuffer[1] == '1')
				g_nES1 = 1;
			else
				g_nES1 = 0;	

			if(s_pPicCmd->m_abParamsBuffer[2] == '1')
				g_nES2 = 1;
			else
				g_nES2 = 0;

			if(s_pPicCmd->m_abParamsBuffer[3] == '1')
				g_nES3 = 1;
			else
				g_nES3 = 0;		

			buffer[0] = s_pPicCmd->m_abParamsBuffer[4];
			buffer[1] = s_pPicCmd->m_abParamsBuffer[5];
			buffer[2] = s_pPicCmd->m_abParamsBuffer[6];
			g_nPSB = atoi(buffer);

			buffer[0] = s_pPicCmd->m_abParamsBuffer[7];
			buffer[1] = s_pPicCmd->m_abParamsBuffer[8];
			buffer[2] = s_pPicCmd->m_abParamsBuffer[9];
			g_nPS1 = atoi(buffer);

			buffer[0] = s_pPicCmd->m_abParamsBuffer[10];
			buffer[1] = s_pPicCmd->m_abParamsBuffer[11];
			buffer[2] = s_pPicCmd->m_abParamsBuffer[12];
			
			g_nPS2 = atoi(buffer);

			buffer[0] = s_pPicCmd->m_abParamsBuffer[13];
			buffer[1] = s_pPicCmd->m_abParamsBuffer[14];
			buffer[2] = s_pPicCmd->m_abParamsBuffer[15];
			g_nPS3 = atoi(buffer);
			
			s_pPicCmd->m_bCommandCode=ERROR_PIC_SUCCESS;
			return 0; 
		}
	}	
	return 1; //Comando no manipulado o no reconocido. @Ware ESS generará la respuesta ERROR_PIC_UNIMPLEMETED_COMMAND
}	

void main(void)
{	
	int nInEncoder = 0;
	int nInServo1_High = 0;
	int nInServo1_Low  = 0;
	int nGrados = 0;
	float nTimer = 0.5;
	float nEquivalencia = 1 / 110;
    int x = 0;
	
	//////////////////////////////////////
	//		Configuración puertos		//
	//////////////////////////////////////

	ADCON1	=	0xff;
	PORTA	=   0x00;
	TRISA	=	0x00;

	//////////////////////////////////
	//		Configuración Timer0	//
	//////////////////////////////////	

	//configuracion del timer0
	T0CONbits.T08BIT = 0;			//contador 16 bits
	T0CONbits.T0CS = 0;				//Fosc/4
	T0CONbits.PSA = 0;				//usa el preescaler
	T0CON = (T0CON & 0xF8) | 0x07;	//preescaler 1:256

	//configuracion del timer1
	T1CONbits.RD16 = 1;				//16 bits
	T1CONbits.TMR1CS = 0;			//Fosc/4
	T1CONbits.T1OSCEN = 0;			//oscilador apagado
	T1CON = (T1CON & 0xCF) | 0x30;	//preescaler 1:8

	//borra la bandera de interrupcion del timer0
	INTCONbits.TMR0IF = 0;
	//borra la bandera de interrupcion del timer1
	PIR1bits.TMR1IF = 0;

	//habilita el permiso de interrupciones globales
	INTCONbits.GIE = 1;
	//habilita el permiso de interrupciones perifericas
	INTCONbits.PEIE = 1;

	//habilita el permiso de interrupcion por timer0
	INTCONbits.TMR0IE = 1;
	//habilita el permiso de interrupcion por timer1
	PIE1bits.TMR1IE = 1;

	//timer0
	TMR0H = 0x00;		//para escribir DEBE ser PRIMERO
	TMR0L = 0x00;		//la parte ALTA del registro

	//timer1
	TMR1H = 0x00;			//para escribir DEBE ser PRIMERO
	TMR1L = 0x00;			//la parte ALTA del registro

	//enciende el timer0
	T0CONbits.TMR0ON = 1;

	//enciende el timer1
	T1CONbits.TMR1ON = 1;		

	//////////////////////////////////
	//			Graficos			//
	//////////////////////////////////
											
	//Se le indica la salida estándar que debe usar la función definida por el usuario para imprimir un caracter
	stdout=_H_USER;

	//Bus de control GLCD (y otros periféricos) como salida 
	PORTB=0xc0;    
	TRISB=GLCD_TRIS_OUTPUT_MASK; 
	
	//Inicializar el Modulo GLCD (Limpieza del estado interno del GLCD tras el reset o power on)
	ATGInit();
	//Activar la polarización del cristal líquido. 
	ATGShow();
	
	//Establecer las tintas de brocha (relleno) y pluma (filete)
	g_GLCDBrush=GLCD_BLACK;
	g_GLCDPen=GLCD_WHITE;	

	//////////////////////////////////////////
	//			Comunicación USB			//
	//////////////////////////////////////////
	
	ESS_USBSetOverlayCommandHandler(USBOverlayCommandHandler);	

	//////////////////////////////
	//			Encoder			//			
	//////////////////////////////

	while(1)
	{	
		//////////////////////////////////////////
		//			Comunicación USB			//
		//////////////////////////////////////////
		
		ESS_USBStatusLedTasks;
		ESS_USBTasks(NULL);	

		Encoder();
		ESS_Delay(500);
	}
}
