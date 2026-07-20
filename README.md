# Prova de Conceito Acadêmica — Heartbleed

Projeto desenvolvido para a disciplina de **Segurança da Informação** com o objetivo de demonstrar, de forma controlada, a falha lógica explorada pelo ataque Heartbleed.

A aplicação foi implementada em linguagem C e utiliza dois processos:

- **Servidor vulnerável:** recebe uma solicitação e devolve uma quantidade de bytes baseada no tamanho declarado pelo cliente.
- **Atacante:** envia um pequeno conteúdo, mas informa um tamanho maior, provocando uma leitura além do tamanho real do buffer.

> Este projeto possui finalidade exclusivamente acadêmica e funciona apenas no endereço local `127.0.0.1`.

---

## Objetivo

Implementar uma prova de conceito que reproduza os principais elementos do Heartbleed:

1. Criar um servidor TCP.
2. Permitir a conexão de um processo atacante.
3. Enviar um payload pequeno com um tamanho declarado maior.
4. Fazer o servidor confiar no tamanho declarado.
5. Retornar bytes além do final lógico do payload.
6. Repetir o ataque até que uma string armazenada na memória seja vazada.
7. Utilizar `malloc` e `free` para simular o uso e a fragmentação do heap.

---

## Funcionamento

O atacante envia o seguinte conteúdo:

```text
PING
```

O tamanho real da mensagem é de apenas **4 bytes**, mas o atacante declara que ela possui **256 bytes**.

```text
Tamanho real: 4 bytes
Tamanho declarado: 256 bytes
```

O servidor vulnerável utiliza o tamanho declarado para montar a resposta. Dessa forma, ele devolve 256 bytes, mesmo que apenas os quatro primeiros bytes correspondam ao conteúdo real.

Os demais bytes representam dados existentes na região de memória simulada.

A string sensível utilizada na demonstração é:

```text
CHAVE_SECRETA=TRABALHO4-HEARTBLEED
```

A cada solicitação, essa string pode ficar dentro ou fora da região retornada pelo servidor. O atacante repete o pedido até encontrá-la.

---

## Fluxo da aplicação

```text
Atacante                                   Servidor
   |                                          |
   |-------- Conexão TCP -------------------->|
   |                                          |
   |-- "PING" / real: 4 / declarado: 256 ---->|
   |                                          |
   |<--------- Resposta de 256 bytes ----------|
   |                                          |
   |-- Procura a string secreta               |
   |                                          |
   |-- Repete enquanto não encontrar -------->|
```

---

## Simulação da memória

Antes do ataque, o servidor executa várias operações de alocação e desalocação de memória.

São utilizados:

```c
malloc()
calloc()
free()
```

O programa:

- aloca blocos de diferentes tamanhos;
- preenche os blocos com dados;
- libera alguns blocos;
- realiza novas alocações;
- simula a reutilização de regiões do heap;
- mantém uma string sensível alocada dinamicamente.

Essa etapa busca representar, de forma simplificada, o comportamento da memória de um processo real.

---

## Estrutura do projeto

```text
Trabalho-Heart-Bleeding
│
├── src
│   ├── servidor.c
│   ├── atacante.c
│   ├── common.c
│   └── common.h
│
├── docs
│   └── images
│
├── .gitignore
├── README.md
└── README_EXECUCAO.txt
```

### Arquivos principais

| Arquivo | Descrição |
|---|---|
| `src/servidor.c` | Implementa o servidor TCP e a resposta vulnerável. |
| `src/atacante.c` | Envia solicitações malformadas e procura a string vazada. |
| `src/common.c` | Contém funções compartilhadas de comunicação e simulação de memória. |
| `src/common.h` | Define estruturas, constantes e protótipos utilizados no projeto. |

---

## Tecnologias utilizadas

- Linguagem C
- GCC
- MSYS2
- Winsock2
- TCP sockets
- Visual Studio Code
- PowerShell
- Git e GitHub

---

## Ambiente de execução

O projeto foi desenvolvido e testado no Windows utilizando:

```text
Windows
MSYS2
GCC
PowerShell
Visual Studio Code
```

A biblioteca Winsock2 é vinculada durante a compilação com a opção:

```text
-lws2_32
```

---

## Compilação

Abra o PowerShell na pasta raiz do projeto.

### Compilar o servidor

```powershell
gcc src/servidor.c src/common.c -o servidor.exe -lws2_32
```

### Compilar o atacante

```powershell
gcc src/atacante.c src/common.c -o atacante.exe -lws2_32
```

---

## Execução

Abra dois terminais na pasta raiz do projeto.

### Terminal 1 — Servidor

```powershell
.\servidor.exe
```

O servidor ficará aguardando uma conexão em:

```text
127.0.0.1:8080
```

### Terminal 2 — Atacante

```powershell
.\atacante.exe
```

O atacante fará várias solicitações até encontrar a string sensível ou atingir o limite de tentativas.

---

## Resultado obtido

Na execução registrada, o atacante enviou repetidamente um payload de quatro bytes, declarando um tamanho de 256 bytes.

Nas primeiras tentativas, a string sensível não estava dentro da área retornada:

```text
Tentativa 1: declarando 256 bytes e enviando apenas 4.
Segredo ainda nao apareceu; repetindo o ataque.
```

O mesmo processo foi repetido até a sexta tentativa, quando a string foi encontrada:

```text
SEGREDO ENCONTRADO NO OVER-READ!

String vazada: CHAVE_SECRETA=TRABALHO4-HEARTBLEED
Tentativas necessarias: 6
```

No servidor, o resultado correspondente foi:

```text
Rodada 6
Tamanho declarado: 256
Tamanho real: 4

Servidor enviou 256 bytes embora apenas 4 fossem validos.
Resultado interno da rodada: o segredo caiu na area vazada
```

O resultado demonstra que o servidor devolveu mais dados do que o conteúdo realmente recebido.

---

## Vulnerabilidade simulada

O erro está relacionado à confiança no tamanho informado pelo cliente.

A lógica vulnerável pode ser representada da seguinte forma:

```c
enviarTudo(
    cliente,
    resposta,
    pedido->tamanhoDeclarado
);
```

O servidor utiliza `tamanhoDeclarado`, mesmo que a quantidade real de dados seja menor.

A forma correta seria limitar a resposta ao tamanho efetivamente recebido:

```c
enviarTudo(
    cliente,
    resposta,
    pedido->tamanhoReal
);
```

Também é necessário validar os valores antes de realizar qualquer leitura ou envio:

```c
if (pedido->tamanhoDeclarado > pedido->tamanhoReal)
{
    /* Rejeitar a solicitacao ou limitar a resposta. */
}
```

---

## Relação com o Heartbleed

O Heartbleed original ocorreu na implementação da extensão Heartbeat do OpenSSL.

O cliente enviava:

- um pequeno payload;
- um tamanho declarado maior que o payload verdadeiro.

A implementação vulnerável confiava nesse tamanho e devolvia dados além do final do conteúdo recebido, podendo expor informações presentes na memória do servidor.

Este projeto não utiliza OpenSSL ou TLS reais. Ele reproduz apenas a lógica principal da vulnerabilidade em um ambiente local e controlado.

---

## Limitações da demonstração

Esta prova de conceito:

- funciona somente em `127.0.0.1`;
- limita a resposta a uma quantidade pequena de bytes;
- não utiliza OpenSSL;
- não realiza ataques contra sistemas externos;
- utiliza uma região de memória controlada;
- não acessa dados reais de outros programas;
- foi construída exclusivamente para estudo.

---

## Conclusão

O trabalho demonstrou como a confiança em um tamanho fornecido pelo cliente pode provocar o vazamento de informações da memória.

O atacante enviou apenas quatro bytes, mas declarou 256 bytes. Como o servidor utilizou o tamanho declarado na resposta, dados além do conteúdo válido foram enviados.

Após várias tentativas, a string sensível armazenada na memória foi encontrada no conteúdo retornado.

A principal medida de prevenção é validar todos os tamanhos recebidos e nunca permitir que uma leitura ou resposta ultrapasse o tamanho real do buffer.

---

## Autoria

**Aluno:** `Douglas de Souza N Santos`  
**Matrícula:** `12211bsi245`  
**Disciplina:** Segurança da Informação  
**Curso:** Sistemas de Informação  
**Ano:** 2026

---

## Aviso

Este projeto foi desenvolvido somente para fins educacionais.