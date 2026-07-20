#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

void simularMemoria(void)
{
    enum { QUANTIDADE_BLOCOS = 12 };

    char *blocos[QUANTIDADE_BLOCOS] = {0};
    const size_t tamanhos[QUANTIDADE_BLOCOS] = {
        48, 96, 160, 72, 240, 128,
        56, 192, 88, 256, 104, 144
    };

    printf("Simulando uso e fragmentacao da memoria...\n");

    for (int i = 0; i < QUANTIDADE_BLOCOS; i++)
    {
        blocos[i] = (char *)malloc(tamanhos[i]);

        if (blocos[i] != NULL)
        {
            memset(blocos[i], 'A' + (i % 26), tamanhos[i]);
        }
    }

    /* Cria lacunas no heap. */
    for (int i = 1; i < QUANTIDADE_BLOCOS; i += 2)
    {
        free(blocos[i]);
        blocos[i] = NULL;
    }

    /* Tenta reutilizar algumas das lacunas recém-liberadas. */
    char *temporario1 = (char *)malloc(90);
    char *temporario2 = (char *)malloc(180);
    char *temporario3 = (char *)malloc(110);

    if (temporario1 != NULL)
    {
        memset(temporario1, 'X', 90);
    }

    if (temporario2 != NULL)
    {
        memset(temporario2, 'Y', 180);
    }

    if (temporario3 != NULL)
    {
        memset(temporario3, 'Z', 110);
    }

    free(temporario2);

    char *reutilizado = (char *)malloc(170);
    if (reutilizado != NULL)
    {
        memset(reutilizado, 'R', 170);
    }

    free(temporario1);
    free(temporario3);
    free(reutilizado);

    for (int i = 0; i < QUANTIDADE_BLOCOS; i++)
    {
        free(blocos[i]);
    }

    printf("Fragmentacao simulada com malloc/free.\n");
}

int enviarTudo(SOCKET socket, const void *buffer, int tamanho)
{
    const char *ponteiro = (const char *)buffer;
    int totalEnviado = 0;

    while (totalEnviado < tamanho)
    {
        int enviados = send(
            socket,
            ponteiro + totalEnviado,
            tamanho - totalEnviado,
            0
        );

        if (enviados == SOCKET_ERROR || enviados == 0)
        {
            return -1;
        }

        totalEnviado += enviados;
    }

    return totalEnviado;
}

int receberTudo(SOCKET socket, void *buffer, int tamanho)
{
    char *ponteiro = (char *)buffer;
    int totalRecebido = 0;

    while (totalRecebido < tamanho)
    {
        int recebidos = recv(
            socket,
            ponteiro + totalRecebido,
            tamanho - totalRecebido,
            0
        );

        if (recebidos == 0)
        {
            return 0;
        }

        if (recebidos == SOCKET_ERROR)
        {
            return -1;
        }

        totalRecebido += recebidos;
    }

    return totalRecebido;
}

int enviarMensagem(SOCKET socket, const Mensagem *mensagem)
{
    Mensagem mensagemRede;

    memset(&mensagemRede, 0, sizeof(mensagemRede));

    mensagemRede.tamanhoDeclarado = htonl(mensagem->tamanhoDeclarado);
    mensagemRede.tamanhoReal = htonl(mensagem->tamanhoReal);
    memcpy(mensagemRede.dados, mensagem->dados, TAM_DADOS);

    return enviarTudo(socket, &mensagemRede, (int)sizeof(mensagemRede));
}

int receberMensagem(SOCKET socket, Mensagem *mensagem)
{
    Mensagem mensagemRede;

    int resultado = receberTudo(
        socket,
        &mensagemRede,
        (int)sizeof(mensagemRede)
    );

    if (resultado <= 0)
    {
        return resultado;
    }

    mensagem->tamanhoDeclarado = ntohl(mensagemRede.tamanhoDeclarado);
    mensagem->tamanhoReal = ntohl(mensagemRede.tamanhoReal);
    memcpy(mensagem->dados, mensagemRede.dados, TAM_DADOS);

    return resultado;
}
