#include <stdio.h>
#include <stdlib.h>
#include "common.h"


void simularMemoria()
{
    printf("Simulando uso da memoria...\n");


    // Criando vários blocos dinamicamente
    char *bloco1 = malloc(50);
    char *bloco2 = malloc(100);
    char *bloco3 = malloc(200);


    if(bloco1 && bloco2 && bloco3)
    {
        printf("Blocos de memoria alocados\n");
    }


    // Liberando alguns blocos
    free(bloco2);


    // Nova alocacao
    char *bloco4 = malloc(120);


    if(bloco4)
    {
        printf("Novo bloco alocado\n");
    }


    // Liberando tudo
    free(bloco1);
    free(bloco3);
    free(bloco4);


    printf("Memoria liberada\n");
}