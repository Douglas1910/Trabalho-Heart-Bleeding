#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "common.h"

#pragma comment(lib, "ws2_32.lib")


int main()
{
    simularMemoria();
    
    WSADATA wsa;
    
    SOCKET servidor;
    SOCKET cliente;

    struct sockaddr_in enderecoServidor;
    struct sockaddr_in enderecoCliente;

    int tamanhoEndereco;

    char buffer[TAM_BUFFER];


    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Erro ao iniciar Winsock\n");
        return 1;
    }


    // Criando socket
    servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == INVALID_SOCKET)
    {
        printf("Erro ao criar socket\n");
        WSACleanup();
        return 1;
    }


    // Configurando endereço
    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_addr.s_addr = INADDR_ANY;
    enderecoServidor.sin_port = htons(PORTA);


    // Associando socket à porta
    if (bind(
        servidor,
        (struct sockaddr*)&enderecoServidor,
        sizeof(enderecoServidor)
    ) == SOCKET_ERROR)
    {
        printf("Erro no bind\n");
        closesocket(servidor);
        WSACleanup();
        return 1;
    }


    // Coloca servidor esperando conexões
    listen(servidor, 1);


    printf("Servidor esperando conexao na porta %d...\n", PORTA);



    tamanhoEndereco = sizeof(enderecoCliente);


    // Aceita cliente
    cliente = accept(
        servidor,
        (struct sockaddr*)&enderecoCliente,
        &tamanhoEndereco
    );


    if(cliente == INVALID_SOCKET)
    {
        printf("Erro no accept\n");
        closesocket(servidor);
        WSACleanup();
        return 1;
    }


    printf("Cliente conectado!\n");


    // Recebe mensagem
    memset(buffer, 0, TAM_BUFFER);


    recv(
        cliente,
        buffer,
        TAM_BUFFER,
        0
    );


    printf("Mensagem recebida: %s\n", buffer);



    // Responde
    char resposta[] = "Mensagem recebida pelo servidor";

    send(
        cliente,
        resposta,
        strlen(resposta),
        0
    );



    // Fecha conexões
    closesocket(cliente);
    closesocket(servidor);

    WSACleanup();


    return 0;
}