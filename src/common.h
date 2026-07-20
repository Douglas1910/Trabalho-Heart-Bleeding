#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <winsock2.h>

#define PORTA 8080

/* Quantidade máxima de dados realmente enviados pelo atacante. */
#define TAM_DADOS 64

/* Limite da resposta vulnerável para manter a PoC controlada. */
#define TAM_RESPOSTA_MAX 512

/* O atacante pede mais bytes do que realmente envia. */
#define TAMANHO_DECLARADO_ATAQUE 256

/* String que o atacante tentará encontrar no vazamento. */
#define SEGREDO_ALVO "CHAVE_SECRETA=TRABALHO4-HEARTBLEED"

typedef struct
{
    uint32_t tamanhoDeclarado;
    uint32_t tamanhoReal;
    char dados[TAM_DADOS];
} Mensagem;

/* Simula fragmentação com várias chamadas malloc/free. */
void simularMemoria(void);

int enviarTudo(SOCKET socket, const void *buffer, int tamanho);
int receberTudo(SOCKET socket, void *buffer, int tamanho);

int enviarMensagem(SOCKET socket, const Mensagem *mensagem);
int receberMensagem(SOCKET socket, Mensagem *mensagem);

#endif
