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

    char mensagem[] = "Cliente conectado!";
    char resposta[TAM_BUFFER];


    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Erro ao iniciar Winsock\n");
        return 1;
    }


    // Criando socket
    cliente = socket(AF_INET, SOCK_STREAM, 0);


    if(cliente == INVALID_SOCKET)
    {
        printf("Erro ao criar socket\n");
        WSACleanup();
        return 1;
    }


    // Configuração do servidor
    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_port = htons(PORTA);


    // Endereço local
    enderecoServidor.sin_addr.s_addr = inet_addr("127.0.0.1");


    // Conectar no servidor
    if(connect(
        cliente,
        (struct sockaddr*)&enderecoServidor,
        sizeof(enderecoServidor)
    ) == SOCKET_ERROR)
    {
        printf("Nao foi possivel conectar ao servidor\n");

        closesocket(cliente);
        WSACleanup();

        return 1;
    }


    printf("Conectado ao servidor!\n");


    // Envia mensagem
    send(
        cliente,
        mensagem,
        strlen(mensagem),
        0
    );


    printf("Mensagem enviada: %s\n", mensagem);



    // Recebe resposta

    memset(resposta, 0, TAM_BUFFER);


    recv(
        cliente,
        resposta,
        TAM_BUFFER,
        0
    );


    printf("Resposta do servidor: %s\n", resposta);



    closesocket(cliente);

    WSACleanup();


    return 0;
}