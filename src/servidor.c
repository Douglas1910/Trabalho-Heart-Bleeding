#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "common.h"

#pragma comment(lib, "ws2_32.lib")

static unsigned char *montarRespostaVulneravel(
    const Mensagem *pedido,
    const char *segredoHeap,
    int *segredoVazou
)
{
    unsigned char *arenaHeap =
        (unsigned char *)malloc(TAM_RESPOSTA_MAX);

    if (arenaHeap == NULL)
    {
        return NULL;
    }

    /* Conteúdo residual fictício para representar outros dados do heap. */
    for (size_t i = 0; i < TAM_RESPOSTA_MAX; i++)
    {
        arenaHeap[i] = (unsigned char)('a' + (rand() % 26));
    }

    memcpy(arenaHeap, pedido->dados, pedido->tamanhoReal);

    size_t tamanhoSegredo = strlen(segredoHeap);
    size_t inicioOverRead = pedido->tamanhoReal;
    size_t limiteEnviado = pedido->tamanhoDeclarado;

    *segredoVazou = 0;

    /* Aproximadamente uma em quatro tentativas posiciona o segredo na parte
       que será indevidamente enviada. Isso simula o "por acaso" do enunciado. */
    int colocarDentroDaResposta = (rand() % 4 == 0);

    if (
        colocarDentroDaResposta &&
        limiteEnviado > inicioOverRead + tamanhoSegredo + 1
    )
    {
        size_t espaco =
            limiteEnviado - inicioOverRead - tamanhoSegredo;

        size_t deslocamento =
            inicioOverRead + 1 + (size_t)(rand() % (int)espaco);

        memcpy(
            arenaHeap + deslocamento,
            segredoHeap,
            tamanhoSegredo
        );

        *segredoVazou = 1;
    }
    else
    {
        /* O segredo continua existindo no heap simulado, mas fora dos bytes
           enviados nesta rodada. */
        size_t primeiroOffsetSeguro = limiteEnviado + 1;

        if (
            primeiroOffsetSeguro + tamanhoSegredo <
            TAM_RESPOSTA_MAX
        )
        {
            size_t espaco =
                TAM_RESPOSTA_MAX -
                primeiroOffsetSeguro -
                tamanhoSegredo;

            size_t deslocamento = primeiroOffsetSeguro;

            if (espaco > 0)
            {
                deslocamento += (size_t)(rand() % (int)espaco);
            }

            memcpy(
                arenaHeap + deslocamento,
                segredoHeap,
                tamanhoSegredo
            );
        }
    }

    return arenaHeap;
}

int main(void)
{
    WSADATA wsa;
    SOCKET servidor = INVALID_SOCKET;
    SOCKET cliente = INVALID_SOCKET;

    struct sockaddr_in enderecoServidor;
    struct sockaddr_in enderecoCliente;
    int tamanhoEndereco = sizeof(enderecoCliente);

    srand((unsigned int)time(NULL));

    simularMemoria();

    /* O dado sensível é criado explicitamente no heap. */
    size_t tamanhoSegredo = strlen(SEGREDO_ALVO) + 1;
    char *segredoHeap = (char *)malloc(tamanhoSegredo);

    if (segredoHeap == NULL)
    {
        printf("Falha ao alocar o segredo no heap.\n");
        return 1;
    }

    memcpy(segredoHeap, SEGREDO_ALVO, tamanhoSegredo);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Erro ao iniciar Winsock.\n");
        free(segredoHeap);
        return 1;
    }

    servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == INVALID_SOCKET)
    {
        printf("Erro ao criar socket.\n");
        free(segredoHeap);
        WSACleanup();
        return 1;
    }

    memset(&enderecoServidor, 0, sizeof(enderecoServidor));
    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    enderecoServidor.sin_port = htons(PORTA);

    if (
        bind(
            servidor,
            (struct sockaddr *)&enderecoServidor,
            sizeof(enderecoServidor)
        ) == SOCKET_ERROR
    )
    {
        printf("Erro no bind: %d\n", WSAGetLastError());
        closesocket(servidor);
        free(segredoHeap);
        WSACleanup();
        return 1;
    }

    if (listen(servidor, 1) == SOCKET_ERROR)
    {
        printf("Erro no listen.\n");
        closesocket(servidor);
        free(segredoHeap);
        WSACleanup();
        return 1;
    }

    printf("Servidor academico ouvindo somente em 127.0.0.1:%d...\n", PORTA);

    cliente = accept(
        servidor,
        (struct sockaddr *)&enderecoCliente,
        &tamanhoEndereco
    );

    if (cliente == INVALID_SOCKET)
    {
        printf("Erro no accept.\n");
        closesocket(servidor);
        free(segredoHeap);
        WSACleanup();
        return 1;
    }

    printf("Atacante conectado.\n");

    unsigned int rodada = 0;

    for (;;)
    {
        Mensagem *pedido = (Mensagem *)calloc(1, sizeof(Mensagem));

        if (pedido == NULL)
        {
            printf("Falha ao alocar o pedido no heap.\n");
            break;
        }

        int recebido = receberMensagem(cliente, pedido);

        if (recebido <= 0)
        {
            free(pedido);
            printf("Conexao encerrada pelo atacante.\n");
            break;
        }

        rodada++;

        printf("\nRodada %u\n", rodada);
        printf("Tamanho declarado: %u\n", pedido->tamanhoDeclarado);
        printf("Tamanho real: %u\n", pedido->tamanhoReal);

        if (
            pedido->tamanhoReal == 0 ||
            pedido->tamanhoReal > TAM_DADOS ||
            pedido->tamanhoDeclarado < pedido->tamanhoReal ||
            pedido->tamanhoDeclarado > TAM_RESPOSTA_MAX
        )
        {
            printf("Pedido invalido ou fora dos limites da PoC.\n");
            free(pedido);
            break;
        }

        int segredoVazou = 0;

        unsigned char *resposta = montarRespostaVulneravel(
            pedido,
            segredoHeap,
            &segredoVazou
        );

        if (resposta == NULL)
        {
            printf("Falha ao alocar a resposta vulneravel.\n");
            free(pedido);
            break;
        }

        uint32_t tamanhoRespostaRede =
            htonl(pedido->tamanhoDeclarado);

        if (
            enviarTudo(
                cliente,
                &tamanhoRespostaRede,
                (int)sizeof(tamanhoRespostaRede)
            ) < 0 ||
            enviarTudo(
                cliente,
                resposta,
                (int)pedido->tamanhoDeclarado
            ) < 0
        )
        {
            printf("Erro ao enviar a resposta.\n");
            free(resposta);
            free(pedido);
            break;
        }

        printf(
            "Servidor enviou %u bytes embora apenas %u fossem validos.\n",
            pedido->tamanhoDeclarado,
            pedido->tamanhoReal
        );

        printf(
            "Resultado interno da rodada: %s\n",
            segredoVazou ? "o segredo caiu na area vazada" :
                           "o segredo ficou fora da area enviada"
        );

        free(resposta);
        free(pedido);
    }

    closesocket(cliente);
    closesocket(servidor);
    free(segredoHeap);
    WSACleanup();

    return 0;
}
