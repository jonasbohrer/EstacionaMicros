/*
 *
 * 
 * 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define tamanhoEstacionamento 30

typedef struct Carro
{
	char letras[3];
	char numeros[4];
} Carro;

int ativo;
int temporizador;
Carro *carros[tamanhoEstacionamento];

ISR (TIMER1_OVF_vect) // Interrupção timer1
{
	temporizador++;
	TCNT1 = 63974; // 1 seg em 16MHz
}

void init_timer1(){
    temporizador = 0; // Zera temporizador
    TCNT1 = 63974; // 1 seg em 16MHz

	TCCR1A = 0x00;
	TCCR1B = (1<<CS10) | (1<<CS12); // prescaler de 1024
	TIMSK = (1 << TOIE1) ; // Liga interrupção por overflow
	sei(); // Permite interrupções globais
}

void atraso_40us(){
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

void atraso_1650us(){
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

void atraso_us(int tempo){ //Calcula a quantidade de atrasos em micro segundos em passos de 40
	int i;
	int atrasos = (tempo+39)/40;
	for(i=0; i<atrasos; i++){atraso_40us();}
}

void escrita_valor(char valor){
	PORTG |= (1 << 5); //RS = 1;
	PORTE |= (1 << 5); //EN = 1;
	PORTD = valor; //porta1 = comando;
	PORTH = valor >> 4;
	PORTJ = valor >> 6;
	PORTE &= ~(1 << 5); //EN = 0;
	atraso_40us();
	atraso_40us();
}

void escrita_comando(char comando){
	PORTG &= ~(1 << 5); //RS = 0;
	PORTE |= (1 << 5); //EN = 1;
	PORTD = comando; //porta1 = comando;
	PORTH = comando >> 4;
	PORTJ = comando >> 6;
	PORTE &= ~(1 << 5); //EN = 0;
	atraso_40us();
}


void port_config(){
	DDRG = (1 << 5);
	DDRE = (1 << 5);
	DDRD = 0x0F;
	DDRH = 0x03;
	DDRJ = 0x03;
}

void limpar_display()
{
	escrita_comando(0x01); //'00000001'
	atraso_1650us();
}

void lcd_config(){
	escrita_comando(0x38); //'00111000'
	escrita_comando(0x38); //'00111000'
	escrita_comando(0x0C); //'00001100'
	atraso_1650us();
	escrita_comando(0x06); //'00000110'
	limpar_display();
}

void escrita_texto(char texto[]){
	//limpar_display();
	int i;
	for (i = 0; texto[i] != 0x00; i++){
		escrita_valor(texto[i]);
		atraso_40us();
	}
}

void configurar_contraste_lcd(){
	DDRB = (1 << 5); //contraste
	TCCR1A  = ((1 << COM1A1) | (1 << COM1A0));
	TCCR1B  = ((1 << CS10) | (1 << WGM13));
	OCR1A=0Xa;
}

void lcd_test(){
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
}

void configurar_serial_19200(){
	UCSR0C = 0x06; // 8-N-1
	//Colocar 51 nos registradores de bit rate
	UBRR0L = 0x33;
	UBRR0H = 0x00;
	UCSR0B = (1<<4) | (1<<3);
}

char receber_caractere(void) {
    while(!(UCSR0A & (1<<RXC0)));
	//escrita_valor(UDR0);
    return UDR0;
}

void transmitir_caractere(char caractere){
	while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = caractere;
}

void transmitir_string(char text[]){
    int i;
    for (i = 0; text[i] != 0x00; i++){
        transmitir_caractere(text[i]);
    }
}

void enviar_msg(char msg[]){ // Chama subrotinas para envios do microcontrolador

    char comando[3];
    comando[0] = msg[0];
    comando[1] = msg[1];
    comando[2] = 0x00;

    if (strcmp(comando, "EB") == 0) {
        // Mensagem de resposta do microcontrolador ap�s efetivado pedido de bloqueio pelo servidor
        transmitir_string("EB");
    }
    else if (strcmp(comando, "ED") == 0) {
        // Mensagem de resposta do microcontrolador ap�s efetivado pedido de desbloqueio pelo servidor
		transmitir_string("ED");
    }
    else if (strcmp(comando, "EH") == 0) {
        // Mensagem de resposta do microcontrolador ap�s efetivado evio de data/hora pelo servidor
    }
    else if (strcmp(comando, "EO") == 0) {
        // Envio de sistema operando
    }
    else if (strcmp(comando, "EN") == 0) {
        // Envio de novo carro: Mensagem de resposta do microcontrolador
    }
    else if (strcmp(comando, "EA") == 0) {
        // Envio de abertura de cancela: Mensagem enviada pelo microcontrolador para pedir abertura da cancela "1" (entrada) ou "2" (sa�da) [EAn]
		if (msg[2]=='1')
		{
			transmitir_string("EA1");
		}
        else if (msg[2]=='2')
		{
			transmitir_string("EA2");
		}
    }
    else if (strcmp(comando, "EF") == 0) {
        // Envio de fechamento de cancela: Mensagem enviada pelo microcontrolador para pedir fechamento da cancela "1" (entrada) ou "2" (sa�da) [EFn]
        transmitir_string("EF1");
    }
    else if (strcmp(comando, "ES") == 0) {
        // Envio de carro saindo: Mensagem de resposta do microcontrolador
    }
    else if (strcmp(comando, "EC") == 0) {
        // Envio de n�mero do cart�o: Mensagem enviada pelo microcontrolador para informar cart�o digitado (Ex. �123456�)
    }
    else if (strcmp(comando, "EP") == 0) {
        // Envio de pagamento: Mensagem enviada pelo microcontrolador para informar senha (Ex. �123456�) e valor a pagar (Ex 18,00)
    }
    else if (strcmp(comando, "EI") == 0) {
        // Envio de impress�o de nota: Mensagem enviada pelo microcontrolador para imprimir nota fiscal
    }
    else if (strcmp(comando, "EM") == 0) {
        // Envio de pedido de mapa: Mensagem enviada pelo microcontrolador para solicitar mapa de ocupa��o
    }
    
}

void processar_msg(char msg[]){ // Chama subrotinas de acordo com os envios do servidor
    char comando[3];
    comando[0] = msg[0];
    comando[1] = msg[1];
    comando[2] = 0x00;

    if (strcmp(comando, "SB") == 0){
        // Bloqueio: Mensagem enviada pelo servidor para bloquear sistema
        // Neste caso deve-se exibir a mensagem �DESLIGADO!�
        // S� volta a funcionar se o aplicativo de servidor externo liberar o sistema.
        // Microcontrolador deve responder "EB"
        
        escrita_texto("DESLIGADO!");
		ativo=0;
		enviar_msg("EB");
        //break;
    }
    else if (strcmp(comando, "SD") == 0){
        // Desbloqueio: Mensagem enviada pelo servidor para desbloquear sistema
		ativo=1;
		limpar_display();
		enviar_msg("ED");
    }
    else if (strcmp(comando, "SH") == 0){
        // Envio de data e hor�rio: Mensagem enviada pelo servidor para informar data e hor�rio
    }
    else if (strcmp(comando, "SN") == 0){
        // Envio de novo carro: Mensagem do servidor para informar novo carro que chegou na cancela de entrada ("1") de sa�da ("2")
        // Mensagem enviada pelo servidor para informar novo carro de idoso ou especial (sufixo "IDE")
		char estado = receber_caractere();
        // Se carro na cancela de entrada
		if (estado == '1') 
		{
            // Se o sistema mandou a placa quer dizer que existe lugar para o carro, ent�o deve-se anotar a placa e liberar a cancela
            char placa[7];
            placa[0]=receber_caractere();
            placa[1]=receber_caractere();
            placa[2]=receber_caractere();
            placa[3]=receber_caractere();
            placa[4]=receber_caractere();
            placa[5]=receber_caractere();
            placa[6]=receber_caractere();
            placa[7]=receber_caractere(); //Limpa o /0

            //Assume que a placa é válida
            //Envia msg para abrir a cancela
			enviar_msg("EA1"); 
            //Espera pela msg do servidor "SA" indicando abertura da cancela 
            espera_msg_servidor("SA", 0);

            //Espera por no max 60s msg do servidor avisando que o carro saiu da cancela 1
            if (espera_msg_servidor("SS", 60) != NULL) {
                //Se ele saiu, armazena a placa e a hora.
                
                //Coleta a placa novamente (aqui pode ser feita uma verificção de consistência entre as duas placas. Devem ser iguais, mas não é especificado no projeto esse caso de erro)
                char placa[7];
                placa[0]=receber_caractere();
                placa[1]=receber_caractere();
                placa[2]=receber_caractere();
                placa[3]=receber_caractere();
                placa[4]=receber_caractere();
                placa[5]=receber_caractere();
                placa[6]=receber_caractere();
                placa[7]=receber_caractere(); //Limpa o /0
                
                //Salva a placa
                for (int i=0; i<tamanhoEstacionamento; i++) {
                    if(carros[i] == NULL){
                        carros[i] = malloc(sizeof(Carro));
                        carros[i]->letras[0]=placa[0];
                        carros[i]->letras[1]=placa[1];
                        carros[i]->letras[2]=placa[2];
                        carros[i]->numeros[0]=placa[3];
                        carros[i]->numeros[1]=placa[4];
                        carros[i]->numeros[2]=placa[5];
                        carros[i]->numeros[3]=placa[6];
                        break;
                    }
                }

                //Testa se salvou a placa
                limpar_display();
                for (int i=0; i<tamanhoEstacionamento; i++) {
                    if(!(carros[i] == NULL)) {
                        escrita_valor(carros[i]->numeros[3]);
                    }
                }
            } 
            //Envia mensagem para fechar a cancela.
            enviar_msg("EF1");
            //Espera pela msg do servidor "SA" indicando fechamento da cancela 
            espera_msg_servidor("SF", 0);
		}
        // Se carro na cancela de saída
        else if (estado == '2') {
            // Gerencia a saída do carro
        }
    }
    else if (strcmp(comando, "SA") == 0){
        // Envio de abertura de cancela: Mensagem de resposta do servidor
        //enviar_msg("EF1");
    }
    else if (strcmp(comando, "SF") == 0){
        // Envio de fechamento de cancela: Mensagem de resposta do servidor
        //enviar_msg("EA1");
    }
    else if (strcmp(comando, "SS") == 0){
        // Envio de carro saindo: Mensagem do servidor para informar novo carro que saiu da cancela de entrada ("1") de sa�da ("2") 
    }
    else if (strcmp(comando, "SC") == 0){
        // Envio de n�mero do cart�o: Mensagem de resposta do servidor 
            // Caso cart�o enviado n�o exista
            // Informa��o de nome do titular do cart�o
    }
    else if (strcmp(comando, "SP") == 0){
        // Envio de pagamento: Mensagem de resposta do servidor 
            // Tipos de mensagem de resposta do servidor de envio de pagamento
                //Caso senha informada
                //Caso saldo do cart�o seja insuficiente n�o confira
                //Resposta confirmando pagamento
    }
    else if (strcmp(comando, "SI") == 0){
        // Envio de impress�o de nota: Mensagem de resposta do servidor 
    }
    else if (strcmp(comando, "SM") == 0){
        // Envio de pedido de mapa: Mensagem de resposta do servidor 
    }

}

void espera_servidor(){ // Loop de recep��o de comandos do servidor
    
    char char_recebido;
    char msg[1];

    while(1) {
		// Leitura serial. Comandos vindos do servidor devem come�ar com "S"
		char_recebido = receber_caractere();
        if (char_recebido == 'S'){
            msg[0] = char_recebido;
            msg[1] = receber_caractere();
			// Continua o processamento da mensagem de acordo com o c�digo de 2 bytes recebido
			if((ativo==1) || (msg[1]=='D')){
				processar_msg(msg);
			}
        }
    }
}

int espera_msg_servidor(char msg[], int tempo){ // Loop de recep��o de comandos do servidor
    
    char char_recebido;
    char msg[1];

    // Se o tempo for válido, inicia o timer
    if (tempo <= 0) {
        init_timer1();
    }
    // Espera pelo comando enquanto o temporizador não estourar
    while(!(temporizador >= tempo)) {
        if (receber_caractere() == msg[0]){
            if (receber_caractere() == msg[1]){
                //processar_msg(msg);
                return 1;
            }
        }
    }
    return NULL;
}

int main(void){

    /*
    * Para poder entrar no estacionamento o motorista (cliente) ir� parar seu carro a frente da
    cancela, quando ser� lida sua placa. Um sensor indica a presen�a de ve�culo na entrada.
    * Seu sistema ent�o pode autorizar a entrada
    * Para tanto ir� abrir a cancela e permitir sua entrada e para que possa estacionar no interior
    do recinto.
    * O tempo de perman�ncia do veiculo no estacionamento definir�
    o valor a ser pago na sua sa�da
    
    * O tempo m�ximo de perman�ncia na frente da cancela sem entrar n�o pode exceder 1 minuto.
    * Se isto acontecer a cancela fecha e o cliente deve se identificar de novo.
    * Nos �ltimos 20 segundos antes de fechar a cancela deve-se gerar uma informa��o visual
    em LED (piscando 2 vezes por segundo)
    */

    // Inicializa��es
	ativo=1;
    port_config();
	configurar_contraste_lcd();
	lcd_config();
	configurar_serial_19200();

    // Loop principal de escuta ao servidor
    espera_servidor();
}