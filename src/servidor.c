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

    Mensagem mensagemRecebida;


    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Erro ao iniciar Winsock\n");
        return 1;
    }


    servidor = socket(AF_INET, SOCK_STREAM, 0);


    if (servidor == INVALID_SOCKET)
    {
        printf("Erro ao criar socket\n");
        WSACleanup();
        return 1;
    }



    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_addr.s_addr = INADDR_ANY;
    enderecoServidor.sin_port = htons(PORTA);



    if(bind(
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



    listen(servidor, 1);


    printf("Servidor esperando conexao na porta %d...\n", PORTA);



    tamanhoEndereco = sizeof(enderecoCliente);


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



    memset(&mensagemRecebida, 0, sizeof(Mensagem));


    receberMensagem(
        cliente,
        &mensagemRecebida
    );



    printf("\nMensagem recebida:\n");


    printf("Tamanho declarado: %d\n",
        mensagemRecebida.tamanhoDeclarado);


    printf("Tamanho real: %d\n",
        mensagemRecebida.tamanhoReal);


    printf("Dados: %s\n",
        mensagemRecebida.dados);



    Mensagem respostaMensagem;


    memset(&respostaMensagem, 0, sizeof(Mensagem));


    strcpy(
        respostaMensagem.dados,
        "Estrutura recebida com sucesso"
    );


    respostaMensagem.tamanhoReal =
        strlen(respostaMensagem.dados);


    respostaMensagem.tamanhoDeclarado =
        respostaMensagem.tamanhoReal;



    enviarMensagem(
        cliente,
        &respostaMensagem
    );



    closesocket(cliente);

    closesocket(servidor);

    WSACleanup();


    return 0;
}