#ifndef COMMON_H
#define COMMON_H

#define PORTA 8080

#define TAM_BUFFER 1024

// Estrutura de mensagem
typedef struct
{
    int tamanhoDeclarado;
    int tamanhoReal;

    char dados[TAM_BUFFER];

} Mensagem;


// Função para simular uso da memória
void simularMemoria();


#endif