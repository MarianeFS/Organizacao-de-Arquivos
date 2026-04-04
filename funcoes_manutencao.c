/**
 * @file funcoes_manutencao.c
 * @brief Implementação das funcionalidades de manutenção do banco de dados (DELETE, INSERT, UPDATE).
 *
 * @details Este arquivo concentra todas as operações que alteram o estado e o conteúdo do 
 * arquivo binário de dados. Ele gerencia registros de tamanho fixo (80 bytes) utilizando 
 * a Abordagem Dinâmica de Reuso de Espaço.
 * * As principais funcionalidades englobam:
 * - **Remoção Lógica (DELETE):** Marca o registro como removido e o empilha (Pilha LIFO), 
 * atualizando os RRNs no cabeçalho e no registro para reaproveitamento futuro.
 * - **Inserção (INSERT):** Verifica o cabeçalho para reaproveitar espaços logicamente 
 * removidos (desempilhando). Caso a pilha esteja vazia, insere no final do arquivo.
 * - **Atualização (UPDATE):** Realiza a modificação dos dados *in-place* diretamente no 
 * byte offset correspondente, mantendo o tamanho fixo de 80 bytes e preenchendo o resto com lixo ('$').
 * * Além da parte de manipulação, há também funções de segurança para registrar 
 * logs em `operacoes.log` e capturar sinais de interrupção (SIGINT) a fim de evitar 
 * corrupção do arquivo binário.
 *
 * @author [Seu Nome Aqui] - [Seu NUSP]
 * @author Mariane Ferreira dos Santos - 16840035
 * @date Abril de 2026
 * @note Trabalho Prático 1 - Organização de Arquivos (SCC0215)
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> //Biblioteca necessária para o SIGINT
#include "tipos.h"

//Função auxilidar que calcula a posição do registro no ficheiro (ByteOffset)
long calcular_offset(int rrn) {
    return TAM_CABECALHO + (rrn * TAM_REGISTRO);
}


//========== Função de remoção lógica de um registro (DELETE) ==========
/**
 * Remove logicamente um registo no RRN especificado e atualiza a pilha.
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O Relative Record Number do registo a ser removido.
 * @param cab Ponteiro para o cabeçalho atual (carregado em memória).
 */
 void remover_reg_rrn(FILE *bin, int rrn, Cabecalho *c) {
    long offset = calcular_offset(rrn);

    //Colocar o começo da leitura/escrita no início do registro
    fseek(bin, offset, SEEK_SET);

    //Evitar ler um registro já removido, lendo o status atual do mesmo
    char status_removido;
    fread(&status_removido, sizeof(char), 1, bin);

    if(status_removido == '1') {
        return; //Se o registro já foi logicamente removido, ignora
    }

    //Novos dados de controle do registro
    char status_atualizado = '1';
    int novo_prox = c->topo; //Aponta para o antigo topo da pilha

    //Novamente colocar o começo da leitura/escrita no início do registro, agora para reescrever
    fseek(bin, offset, SEEK_SET);

    //Escrever '1' (1 byte) e o rrn do próximo (4 bytes)
    fwrite(&status_atualizado, sizeof(char), 1, bin);
    fwrite(&novo_prox, sizeof(int), 1, bin);

    //Atualizar cabeçalho em memória, tornando o novo topo da pilha o rrn abaixo
    c->topo = rrn;

    //Atualizar o cabeçalho no disco, fazendo update imediato
    fseek(bin, 0, SEEK_SET); //Voltar ao byte 0 do ficheiro

    //Ir direto para o campo 'topo', pulando o primeiro byte (char status)
    fseek(bin, sizeof(char), SEEK_CUR);

    //Alterar apenas o campo 'topo', reescrevendo no disco com o valor atualizado
    fwrite(&(c->topo), sizeof(int), 1, bin);
 }

//========== Função de inserção lógica de um registro (DELETE) ==========
