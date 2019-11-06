/*
 *
 * 
 * Serial ports 0(RX), 1(TX)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

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

void escrita_texto(char texto[]){
    int i;
    for (i = 0; text[i] != 0x00; i++){
        escrita_valor(text[i]);
    }
}

void lcd_config(){
	escrita_comando(0x38); //'00111000'
	escrita_comando(0x38); //'00111000'
	escrita_comando(0x0C); //'00001100'
	atraso_1650us();
	escrita_comando(0x06); //'00000110'
	escrita_comando(0x01); //'00000001'
	atraso_1650us();
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

    char comando[2];
    comando[0] = msg[0];
    comando[1] = msg[1];
    comando[2] = 0x00;

    if (strcmp(comando, "EB") == 0) {
        // Mensagem de resposta do microcontrolador após efetivado pedido de bloqueio pelo servidor
        transmitir_string("EB");
    }
    else if (strcmp(comando, "ED") == 0) {
        // Mensagem de resposta do microcontrolador após efetivado pedido de desbloqueio pelo servidor
    }
    else if (strcmp(comando, "EH") == 0) {
        // Mensagem de resposta do microcontrolador após efetivado evio de data/hora pelo servidor
    }
    else if (strcmp(comando, "EO") == 0) {
        // Envio de sistema operando
    }
    else if (strcmp(comando, "EN") == 0) {
        // Envio de novo carro: Mensagem de resposta do microcontrolador
    }
    else if (strcmp(comando, "EA") == 0) {
        // Envio de abertura de cancela: Mensagem enviada pelo microcontrolador para pedir abertura da cancela "1" (entrada) ou "2" (saída) [EAn]
        transmitir_string("EA1");
    }
    else if (strcmp(comando, "EF") == 0) {
        // Envio de fechamento de cancela: Mensagem enviada pelo microcontrolador para pedir fechamento da cancela "1" (entrada) ou "2" (saída) [EFn]
        transmitir_string("EF1");
    }
    else if (strcmp(comando, "ES") == 0) {
        // Envio de carro saindo: Mensagem de resposta do microcontrolador
    }
    else if (strcmp(comando, "EC") == 0) {
        // Envio de número do cartão: Mensagem enviada pelo microcontrolador para informar cartão digitado (Ex. “123456”)
    }
    else if (strcmp(comando, "EP") == 0) {
        // Envio de pagamento: Mensagem enviada pelo microcontrolador para informar senha (Ex. “123456”) e valor a pagar (Ex 18,00)
    }
    else if (strcmp(comando, "EI") == 0) {
        // Envio de impressão de nota: Mensagem enviada pelo microcontrolador para imprimir nota fiscal
    }
    else if (strcmp(comando, "EM") == 0) {
        // Envio de pedido de mapa: Mensagem enviada pelo microcontrolador para solicitar mapa de ocupação
    }
    
}

void processar_msg(char msg){ // Chama subrotinas de acordo com os envios do servidor
    char comando[2];
    comando[0] = msg[0];
    comando[1] = msg[1];
    comando[2] = 0x00;

    if (strcmp(comando, "SB") == 0){
        // Bloqueio: Mensagem enviada pelo servidor para bloquear sistema
        // Neste caso deve-se exibir a mensagem “DESLIGADO!”
        // Só volta a funcionar se o aplicativo de servidor externo liberar o sistema.
        // Microcontrolador deve responder "EB"
        
            escrita_texto("DESLIGADO!")
            break;
    }
    else if (strcmp(comando, "SD") == 0){
        // Desbloqueio: Mensagem enviada pelo servidor para desbloquear sistema
    }
    else if (strcmp(comando, "SH") == 0){
        // Envio de data e horário: Mensagem enviada pelo servidor para informar data e horário
    }
    else if (strcmp(comando, "SN") == 0){
        // Envio de novo carro: Mensagem do servidor para informar novo carro que chegou na cancela de entrada ("1") de saída ("2")
            //Mensagem enviada pelo servidor para informar novo carro de idoso ou especial (sufixo "IDE")
    }
    else if (strcmp(comando, "SA") == 0){
        // Envio de abertura de cancela: Mensagem de resposta do servidor
        enviar_msg("EF1");
    }
    else if (strcmp(comando, "SF") == 0){
        // Envio de fechamento de cancela: Mensagem de resposta do servidor
        enviar_msg("EA1");
    }
    else if (strcmp(comando, "SS") == 0){
        // Envio de carro saindo: Mensagem do servidor para informar novo carro que saiu da cancela de entrada ("1") de saída ("2") 
    }
    else if (strcmp(comando, "SC") == 0){
        // Envio de número do cartão: Mensagem de resposta do servidor 
            // Caso cartão enviado não exista
            // Informação de nome do titular do cartão
    }
    else if (strcmp(comando, "SP") == 0){
        // Envio de pagamento: Mensagem de resposta do servidor 
            // Tipos de mensagem de resposta do servidor de envio de pagamento
                //Caso senha informada
                //Caso saldo do cartão seja insuficiente não confira
                //Resposta confirmando pagamento
    }
    else if (strcmp(comando, "SI") == 0){
        // Envio de impressão de nota: Mensagem de resposta do servidor 
    }
    else if (strcmp(comando, "SM") == 0){
        // Envio de pedido de mapa: Mensagem de resposta do servidor 
    }

}

void espera_servidor(){ // Loop de recepção de comandos do servidor
    
    char char_recebido;
    char msg[1];

    while(1) {
        // Leitura serial. Comandos vindos do servidor devem começar com "S"
        char_recebido = receber_caractere();
        if (char_recebido == 'S'){
            msg[0] = char_recebido
            msg[1] = receber_caractere();

            // Continua o processamento da mensagem de acordo com o código de 2 bytes recebido
            processar_msg(msg);
        }
    }
}

int main(void){

    /*
    * Para poder entrar no estacionamento o motorista (cliente) irá parar seu carro a frente da
    cancela, quando será lida sua placa. Um sensor indica a presença de veículo na entrada.
    * Seu sistema então pode autorizar a entrada
    * Para tanto irá abrir a cancela e permitir sua entrada e para que possa estacionar no interior
    do recinto.
    * O tempo de permanência do veiculo no estacionamento definirá
    o valor a ser pago na sua saída
    
    * O tempo máximo de permanência na frente da cancela sem entrar não pode exceder 1 minuto.
    * Se isto acontecer a cancela fecha e o cliente deve se identificar de novo.
    * Nos últimos 20 segundos antes de fechar a cancela deve-se gerar uma informação visual
    em LED (piscando 2 vezes por segundo)
    */

    // Inicializações
	DDRB = (1 << 7);
	DDRG = (1 << 5);
	DDRE = (1 << 5);
	DDRD = 0x0F;
	DDRH = 0x03;
	DDRJ = 0x03;
	
	lcd_config();
	lcd_test();
	configurar_serial_19200();

    /*while (1)
    {
		PORTB &= ~(1 << 7);
		transmitir_string("EA1");
    }*/

    // Loop principal de escuta ao servidor
    espera_servidor();
}