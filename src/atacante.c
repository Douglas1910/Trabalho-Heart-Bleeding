#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "common.h"

#pragma comment(lib, "ws2_32.lib")


int main()
{
    WSADATA wsa;

    SOCKET cliente;

    struct sockaddr_in enderecoServidor;


    Mensagem mensagem;

    Mensagem respostaMensagem;



    memset(&mensagem, 0, sizeof(Mensagem));



    strcpy(
        mensagem.dados,
        "Cliente conectado!"
    );


    mensagem.tamanhoDeclarado = 100;


    mensagem.tamanhoReal =
        strlen(mensagem.dados);



    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Erro ao iniciar Winsock\n");
        return 1;
    }



    cliente = socket(AF_INET, SOCK_STREAM, 0);



    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_port = htons(PORTA);
    enderecoServidor.sin_addr.s_addr = inet_addr("127.0.0.1");



    if(connect(
        cliente,
        (struct sockaddr*)&enderecoServidor,
        sizeof(enderecoServidor)
    ) == SOCKET_ERROR)
    {
        printf("Nao foi possivel conectar\n");

        return 1;
    }



    printf("Conectado ao servidor!\n");



    enviarMensagem(
        cliente,
        &mensagem
    );



    printf("Mensagem enviada!\n");


    printf("Declarado: %d\n",
        mensagem.tamanhoDeclarado);


    printf("Real: %d\n",
        mensagem.tamanhoReal);


    printf("Dados: %s\n",
        mensagem.dados);



    memset(&respostaMensagem, 0, sizeof(Mensagem));


    receberMensagem(
        cliente,
        &respostaMensagem
    );


    printf(
        "Resposta do servidor: %s\n",
        respostaMensagem.dados
    );



    closesocket(cliente);

    WSACleanup();


    return 0;
}