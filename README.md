# Heartbleed PoC - Estudo Acadêmico

## Descrição

Projeto desenvolvido para a disciplina de Segurança da Informação.

O objetivo é estudar os conceitos envolvidos na comunicação cliente-servidor utilizando sockets TCP em linguagem C, bem como o gerenciamento de memória dinâmica utilizando malloc() e free(), tomando como referência a vulnerabilidade Heartbleed para fins educacionais.

O projeto implementa:

- Servidor TCP
- Cliente TCP
- Estrutura de mensagens compartilhada
- Comunicação utilizando Winsock2
- Gerenciamento de memória dinâmica
- Simulação de alocação e desalocação de memória

---

## Estrutura

```
src/
    servidor.c
    atacante.c
    common.c
    common.h

docs/

README.md
```

---

## Requisitos

- Windows
- GCC (MSYS2/MinGW)
- Visual Studio Code

---

## Compilação

Servidor

```bash
gcc src/servidor.c src/common.c -o servidor.exe -lws2_32
```

Cliente

```bash
gcc src/atacante.c src/common.c -o atacante.exe -lws2_32
```

---

## Execução

Terminal 1

```bash
.\servidor.exe
```

Terminal 2

```bash
.\atacante.exe
```

---

## Funcionalidades

- Comunicação TCP
- Estrutura compartilhada de mensagens
- Uso de malloc()
- Uso de free()
- Simulação de gerenciamento de memória
- Troca de mensagens entre cliente e servidor

---

## Tecnologias

- Linguagem C
- GCC
- Winsock2
- Git
- GitHub
- Visual Studio Code

---

## Autor

Douglas Souza

Universidade Federal de Uberlândia – UFU

Disciplina de Segurança da Informação