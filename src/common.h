#ifndef COMMON_H
#define COMMON_H

#include <winsock2.h>


#define PORTA 8080

#define TAM_BUFFER 1024


typedef struct
{
    int tamanhoDeclarado;
    int tamanhoReal;

    char dados[TAM_BUFFER];

} Mensagem;



void simularMemoria();


int enviarMensagem(SOCKET socket, Mensagem *mensagem);

int receberMensagem(SOCKET socket, Mensagem *mensagem);


#endif