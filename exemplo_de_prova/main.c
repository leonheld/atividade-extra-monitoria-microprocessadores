/*
Faça um programa que aceite interrupção externa 0 (acionada por borda de descida).
A cada solicitação da interrupção externa 0, deve-se ler um dado da tabela apresentada abaixo de forma sequencial 
(cada interrupção corresponde a leitura de um dado). 
O dado lido deve ser convertido em ASCII e o resultado da conversão deve ser escrito no display LCD.
O programa deve então, voltar a aguardar nova solitação de interrupção. 
Ao término da leitura de todos os dados da tabela (realizando sempre a conversão escrita na porta P1),
voltar a ler a mesma do início.

Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15.
*/

//1. Escrever um programa que atenda interrupção externa 0 acionada por borda de descida.
//2. A cada solitação de interrupção, ler um dado da tabela de forma sequencial
//3. O dado lido deve ser convertido em ASCII
//4. O valor lido deve ser escrito no display de LCD.
#define CMD 0 //REGISTERSELECT RS (controlador do LCD) P1.3 = 0
#define SELECIONAENVIODEDADOS 8 //REGISTERSELECT RS (controlador do LCD) P1.3 = 1 ?

RS = 1;

#include <reg51.h>
//Interrupt Service Routine = Tratador de Interrupção
void writeToDisplay(unsigned char *, unsigned char);
void initDisplay(void);
void c51_ISR1(void);
void hardwareInit(void);
void arrToASCII(void);

void delay(unsigned int);

bit ISR0_flag = 0; //flag é usado para verificar ocorrência de interrupções

sbit CS = P0^7;
sbit EN = P1^2;

unsigned char tabela[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
unsigned char init[] = {0x28, 0x0F, 0x06, '$'}; //Tá pronto: disponibilizado pelo professor

unsigned char ISR1_counter = 0;
unsigned char valor_tmp_tabela;

void main(void){	
	
	hardwareInit(); //Inicializa registradores para interrupção
	initDisplay();
	arrToASCII();

	while(1){
		while(ISR0_flag){ //enquanto ISR0_flag não for 1 -> Se torna 1 na interrupção
			ISR0_flag = 0;
			valor_tmp_tabela = tabela[ISR1_counter];
			ISR1_counter = ISR1_counter == 15 ? 0 : ISR1_counter; //Operador ternário: IF-ELSE compacto
			/*
			ISR1_counter recebe 0 se ISR1_counter == 15, senão recebe ele mesmo.
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

//Escreve um byte (4 bits por vez) no barramento que conecta o 8051 ao módulo de LCD
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

/*Converte um array global absoluto pra código ASCII*/

void arrToASCII(void){
	int i = 0;
	unsigned char valor_tmp_arr; //valor temporario pra conversão ASCII
	unsigned char tamanho_tabela = sizeof(tabela) / sizeof(tabela[0]);
	
	for(i; i < (tamanho_tabela + 1); i++){
		valor_tmp_arr = tabela [i];
		valor_tmp_arr = valor_tmp_arr <= 9 ? valor_tmp_arr + 48 : valor_tmp_arr + 55; //Conversão ASCII haxxor
		tabela[i] = valor_tmp_arr;
	}
}

void hardwareInit(void){
	EA = 1; //Permite habilitar todas as interrupções.
	EX0 = 1; //Habilita a interrupção externa 0.
	IT0 = 1; //Habilita interrupção externa 0 por borda de descida.
	
	CS = 0;
}

void c51_ISR1(void) interrupt 0{ //Interrupção Externa 0: P3.2
	ISR0_flag = 1;
	ISR1_counter = ISR1_counter == 0 ? 0 : ISR1_counter++;
}

void delay(unsigned int iterator){
	unsigned int i = 0;
	for(i; i < iterator; i++);
}
