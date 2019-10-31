/*
 *
 * 
 * Serial ports 0(RX), 1(TX)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void atraso_40us()
{
	// initialize timer
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0B |= (1 << CS01);
	
	// initialize counter
	TCNT0 = 176;		// 256 - 80 = 176
	
	// loop forever
	while(1)
	{
		if (TIFR0 & 1)
		{
			TIFR0 = 1;
			return;
		}
	}
}


void atraso_1650us()
{
	TCCR0A = 0;
	TCCR0B = 0x04;
	
	TCNT0 = 152; // (256 - 104 = 152)
	
	// loop forever
	while(1)
	{
		if (TIFR0 & 1)
		{
			TIFR0 = 1;
			return;
		}
	}
}

void atraso_us(int tempo) //Calcula a quantidade de atrasos em micro segundos em passos de 40
{
	int i;
	int atrasos = (tempo+39)/40;
	for(i=0; i<atrasos; i++){atraso_40us();}
}

void escrita_valor(char valor)
{
	PORTG |= (1 << 5); //RS = 1;
	PORTE |= (1 << 5); //EN = 1;
	PORTD = valor; //porta1 = comando;
	PORTH = valor >> 4;
	PORTJ = valor >> 6;
	PORTE &= ~(1 << 5); //EN = 0;
	atraso_40us();
}

void escrita_comando(char comando)
{
	PORTG &= ~(1 << 5); //RS = 0;
	PORTE |= (1 << 5); //EN = 1;
	PORTD = comando; //porta1 = comando;
	PORTH = comando >> 4;
	PORTJ = comando >> 6;
	PORTE &= ~(1 << 5); //EN = 0;
	atraso_40us();
}

void lcd_config()
{
	escrita_comando(0x38); //'00111000'
	escrita_comando(0x38); //'00111000'
	escrita_comando(0x0C); //'00001100'
	atraso_1650us();
	escrita_comando(0x06); //'00000110'
	escrita_comando(0x01); //'00000001'
	atraso_1650us();
}

void configurar_serial_19200()
{
	UCSR0C = 0x06; // 8-N-1
	//Colocar 51 nos registradores de bit rate
	UBRR0L = 0x33;
	UBRR0H = 0x00;
	UCSR0B = (1<<4) | (1<<3);
}

void transmitir_caractere(char caractere)
{
	while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = caractere;
}

int main(void)
{
	DDRB = (1 << 7);
	DDRG = (1 << 5);
	DDRE = (1 << 5);
	DDRD = 0x0F;
	DDRH = 0x03;
	DDRJ = 0x03;
	
	lcd_config();
	
	escrita_valor('J');
	escrita_valor('O');
	escrita_valor('N');
	escrita_valor('A');
	escrita_valor('S');
	
	escrita_comando(0xc0);
	
	escrita_valor('R'); 
	escrita_valor('O');
	escrita_valor('D');
	escrita_valor('O');
	escrita_valor('L');
	escrita_valor('F');
	escrita_valor('O');
	
	configurar_serial_19200();
	
    while (1)
    {
		PORTB &= ~(1 << 7);
		
		transmitir_caractere('A');
    }
}




