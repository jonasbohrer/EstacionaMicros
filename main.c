/*
 * 
 *
 * Created: 18/10/2019 11:06:22
 * Author : Jonas Bohrer, Rodolfo Antoniazzi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void send_msg(char msg){

    // Envios do microcontrolador

    switch(msg){
        case 'EO':
            // Envio de sistema operando
            break;
        case 'EN':
            // Envio de novo carro: Mensagem de resposta do microcontrolador
            break;
        case 'EA':
            // Envio de abertura de cancela: Mensagem enviada pelo microcontrolador para pedir abertura da cancela '1' (entrada) ou '2' (saída)
            break;
        case 'EF':
            // Envio de fechamento de cancela: Mensagem enviada pelo microcontrolador para pedir fechamento da cancela '1' (entrada) ou '2' (saída)
            break;
        case 'ES':
            // Envio de carro saindo: Mensagem de resposta do microcontrolador
            break;
        case 'EC':
            // Envio de número do cartão: Mensagem enviada pelo microcontrolador para informar cartão digitado (Ex. “123456”)
            break;
        case 'EP':
            // Envio de pagamento: Mensagem enviada pelo microcontrolador para informar senha (Ex. “123456”) e valor a pagar (Ex 18,00)
            break;
        case 'EI':
            // Envio de impressão de nota: Mensagem enviada pelo microcontrolador para imprimir nota fiscal
            break;
        case 'EM':
            // Envio de pedido de mapa: Mensagem enviada pelo microcontrolador para solicitar mapa de ocupação
            break;
        default:
            break;
    }
    
}

void process_msg(char msg){

    // Envios do servidor

    switch (msg){
        case 'SB':
            // Bloqueio: Mensagem enviada pelo servidor para bloquear sistema
            break;
        case 'SD':
            // Desbloqueio: Mensagem enviada pelo servidor para desbloquear sistema
            break;
        case 'SH':
            // Envio de data e horário: Mensagem enviada pelo servidor para informar data e horário
            break;
        case 'SN':
            // Envio de novo carro: Mensagem do servidor para informar novo carro que chegou na cancela de entrada ('1') de saída ('2')
                //Mensagem enviada pelo servidor para informar novo carro de idoso ou especial (sufixo 'IDE')
            break;
        case 'EN':
            // Envio de abertura de cancela: Mensagem de resposta do servidor 
            break;
        case 'SA':
            // Envio de abertura de cancela: Mensagem de resposta do servidor 
            break;
        case 'SF':
            // Envio de fechamento de cancela: Mensagem de resposta do servidor 
            break;
        case 'SS':
            // Envio de carro saindo: Mensagem do servidor para informar novo carro que saiu da cancela de entrada ('1') de saída ('2') 
            break;
        case 'SC':
            // Envio de número do cartão: Mensagem de resposta do servidor 
                // Caso cartão enviado não exista
                // Informação de nome do titular do cartão
            break;
        case 'SP':
            // Envio de pagamento: Mensagem de resposta do servidor 
                // Tipos de mensagem de resposta do servidor de envio de pagamento
                    //Caso senha informada
                    //Caso saldo do cartão seja insuficiente não confira
                    //Resposta confirmando pagamento
            break;
        case 'SI':
            // Envio de impressão de nota: Mensagem de resposta do servidor 
            break;
        case 'SM':
            // Envio de pedido de mapa: Mensagem de resposta do servidor 
            break;
        default:
            break;
    }

}

int main(void)
{

    // Initializations
    // Heartbeat
    // Message listening
        // Serial decoding
        // Message processing
}