/* 
-------------------------------------------------------------
	(c) 2009 @Ware Systems 
	Author: M. Sc. Pedro Arturo Cornejo Torres
	mailto:pcornejo@gdl.cinvestav.mx

	@Ware ESS SDK File
----------------------------------------------------------------*/

#ifndef __COMMAND_INCLUDED
#define __COMMAND_INCLUDED

#ifndef ROM
	#define ROM rom
#endif
/*
	struct PICCOMMAND
	Tipo estructural con la informaci�n de un comando para el PIC
	
	Esta estructura tiene como objetivos:
	1) Conformar un comando durante el proceso de recepci�n
	2) Permitir decodificar y ejecutar un comando
	3) Permitir enviar un comando/respuesta hacia el host (PC o PDA)

*/
struct PICCOMMAND
{
	//C�digo de comando. (Tambi�n es �til como c�digo de error)
	unsigned char m_bCommandCode; 
	//Tama�o en bytes de los par�metros del comando
	unsigned char m_bParamsBufferSize; 
	//Buffer con los valores de los par�metros del comando 
	unsigned char m_abParamsBuffer[16]; 
};

//@Ware Default ESS commands, DO NOT REIMPLEMENT 
#define COMMAND_WRITE_TO_PORT 			0x01
#define COMMAND_READ_FROM_PORT 			0x02
#define COMMAND_WRITE_TO_TRIS			0X05
#define COMMAND_READ_FROM_TRIS			0x06
#define COMMAND_READ_FROM_ADC			0x09
#define COMMAND_RUN_PROGRAM				0x0c
#define COMMAND_ERASE_PROGRAM			0x0d
#define COMMAND_WRITE_PROGRAM			0x0e
#define COMMAND_READ_PROGRAM			0x0f
#define COMMAND_FIRMWARE_INFO			0x10
#define COMMAND_ESS_SYSTEM_CALL			0x11
#define COMMAND_TO_SLAVE				0x80
//@Ware Default Command Execution Return Values
#define ERROR_PIC_INVALID_PARAMS		0xfd
#define ERROR_PIC_UNIMPLEMENTED_COMMAND 0xfe
#define ERROR_PIC_SUCCESS				0x00

//Aqu� puedes definir m�s c�digos de comando (Estos son ejemplos)
//V�ase ProcessCommand para ver c�mo utilizarlos

#define COMMAND_DSM_STATUS				0x21

//Aqu� puedes definir m�s c�digos de error de los que ya estan.

#endif
