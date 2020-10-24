/*
Fa�a um programa que aceite interrup��o externa 0 (acionada por borda de descida).
A cada solicita��o da interrup��o externa 0, deve-se ler um dado da tabela apresentada abaixo de forma sequencial 
(cada interrup��o corresponde a leitura de um dado). 
O dado lido deve ser convertido em ASCII e o resultado da convers�o deve ser escrito no display LCD.
O programa deve ent�o, voltar a aguardar nova solita��o de interrup��o. 
Ao t�rmino da leitura de todos os dados da tabela (realizando sempre a convers�o escrita na porta P1),
voltar a ler a mesma do in�cio.

Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15.
*/

//1. Escrever um programa que atenda interrup��o externa 0 acionada por borda de descida.
//2. A cada solita��o de interrup��o, ler um dado da tabela de forma sequencial
//3. O dado lido deve ser convertido em ASCII
//4. O valor lido deve ser escrito no display de LCD.
#define CMD 0 //REGISTERSELECT RS (controlador do LCD) P1.3 = 0
#define DADO 8 //REGISTERSELECT RS (controlador do LCD) P1.3 = 1

#include <reg51.h>
//Interrupt Service Routine = Tratador de Interrup��o
void writeToDisplay(unsigned char *, unsigned char);
void initDisplay(void);
void c51_ISR1(void);
void hardwareInit(void);
void arrToASCII(void);

void delay(unsigned int);

bit ISR0_flag = 0; //flag � usado para verificar ocorr�ncia de interrup��es

sbit CS = P0^7;
sbit EN = P1^2;

unsigned char tabela[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
unsigned char init[] = {0x28, 0x0F, 0x06, '$'}; //T� pronto: disponibilizado pelo professor

unsigned char ISR1_counter = 0;
unsigned char valor_tmp_tabela;

void main(void){	
	
	hardwareInit(); //Inicializa registradores para interrup��o
	initDisplay();
	arrToASCII();

	while(1){
		while(ISR0_flag){ //enquanto ISR0_flag n�o for 1 -> Se torna 1 na interrup��o
			ISR0_flag = 0;
			valor_tmp_tabela = tabela[ISR1_counter];
			ISR1_counter = ISR1_counter == 15 ? 0 : ISR1_counter; //Operador tern�rio: IF-ELSE compacto
			/*
			ISR1_counter recebe 0 se ISR1_counter == 15, sen�o recebe ele mesmo.
			*/
			writeToDisplay(tabela, DADO);
		}
	}
}

void initDisplay(void){
	P1 = 0x20;
	EN = 1;
	EN = 0;
	delay(100);
	writeToDisplay(init, CMD);
}

//Escreve um byte (4 bits por vez) no barramento que conecta o 8051 ao m�dulo de LCD
void writeToDisplay(unsigned char *dados, unsigned char REGISTERSELECT){
	unsigned tmp;
	
	if(REGISTERSELECT){ //DATA = 8
		tmp = tabela[ISR1_counter];
		P1 = (tmp & 0xF0) | REGISTERSELECT;
		EN = 1;
		EN = 0;
		P1 = (tmp << 4) | REGISTERSELECT;
		EN = 1;
		EN = 0;
		delay(100);
	}
	else{
		while(*dados != '$'){
			tmp = *dados++;
			P1 = (tmp & 0xF0) | REGISTERSELECT;
			EN = 1;
			EN = 0;
			P1 = (tmp << 4) | REGISTERSELECT;
			EN = 1;
			EN = 0;
			delay(100);
		}
	}
}

/*Converte um array global absoluto pra c�digo ASCII*/

void arrToASCII(void){
	int i = 0;
	unsigned char valor_tmp_arr; //valor temporario pra convers�o ASCII
	unsigned char tamanho_tabela = sizeof(tabela) / sizeof(tabela[0]);
	
	for(i; i < (tamanho_tabela + 1); i++){
		valor_tmp_arr = tabela [i];
		valor_tmp_arr = valor_tmp_arr <= 9 ? valor_tmp_arr + 48 : valor_tmp_arr + 55; //Convers�o ASCII haxxor
		tabela[i] = valor_tmp_arr;
	}
}

void hardwareInit(void){
	EA = 1; //Permite habilitar todas as interrup��es.
	EX0 = 1; //Habilita a interrup��o externa 0.
	IT0 = 1; //Habilita interrup��o externa 0 por borda de descida.
	
	CS = 0;
}

void c51_ISR1(void) interrupt 0{ //Interrup��o Externa 0: P3.2
	ISR0_flag = 1;
	ISR1_counter = ISR1_counter == 0 ? 0 : ISR1_counter++;
}

void delay(unsigned int iterator){
	unsigned int i = 0;
	for(i; i < iterator; i++);
}