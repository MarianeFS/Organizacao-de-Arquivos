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
#include "funcoes_manutencao.h"

//Função auxiliar que calcula a posição do registro no ficheiro (ByteOffset)
long calcular_offset(int rrn) {
    return TAM_CABECALHO + (rrn * TAM_REGISTRO);
}

//Função auxiliar que marca o arquivo como 'INCONSISTENTE'
void marcar_incons(FILE *bin) {
    fseek(bin, 0, SEEK_SET);
    char status_incons = INCONSISTENTE;
    fwrite(&status_incons, sizeof(char), 1, bin);
}

//Função auxiliar que marca o arquivo como 'CONSISTENTE'
void marcar_cons(FILE *bin) {
    fseek(bin, 0, SEEK_SET);
    char status_cons = CONSISTENTE;
    fwrite(&status_cons, sizeof(char), 1, bin);
}

//Função auxiliar que vai direto para o local e escreve os 80 bytes
void posicionar_rrn(FILE *bin, int rrn) {
    fseek(bin, calcular_offset(rrn), SEEK_SET);
}

//========== Função de remoção lógica de um registro (DELETE) ==========
/**
 * Remove logicamente um registo no RRN especificado e atualiza a pilha. Se a função DELETE deletar todas as instâncias de uma estação única, é necessário atualizar o número de estações (cabeçalho).
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O "Relative Record Number" (RRN) do registo a ser removido.
 * @param c Ponteiro para o cabeçalho atual (carregado na memória).
 */
 void remover_reg_rrn(FILE *bin, int rrn, Cabecalho *c) {
    marcar_incons(bin);
    
    posicionar_rrn(bin, rrn);

    //Evitar ler um registro já removido, lendo o status atual do mesmo
    char ja_removido;
    fread(&ja_removido, sizeof(char), 1, bin);

    if(ja_removido == REMOVIDO) {
        marcar_cons(bin);
        return; //Se o registro já foi logicamente removido, ignorar
    }

    //Retorns ao início do registro, agora para marcar como removido
    posicionar_rrn(bin, rrn);

    //Novos dados de controle do registro
    char atualizado = REMOVIDO;
    int novo_prox = c->topo; //Aponta para o antigo topo da pilha

    fwrite(&atualizado, sizeof(char), 1, bin);
    fwrite(&novo_prox, sizeof(int), 1, bin);

    //Atualizar cabeçalho na memória, tornando o novo topo da pilha o rrn atualizado
    c->topo = rrn;

    //Atualizar o cabeçalho no disco, fazendo update imediato
    fseek(bin, 1, SEEK_SET); //Pular o byte de status para guardar o topo

    //Alterar apenas o campo 'topo', reescrevendo no disco com o valor atualizado
    fwrite(&(c->topo), sizeof(int), 1, bin);

    marcar_cons(bin);
 }

//========== Função de inserção lógica de um registro (INSERT) ==========
/**
 * Insere logicamente um registo no RRN especificado.
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O "Relative Record Number" (RRN) do registo a ser inserido.
 * @param c Ponteiro para o cabeçalho atual (carregado na memória).
 * @param novo Novo registro que será inserido.
 */
void insere_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro novo) {
    marcar_incons(bin);

    //O novo dado vem preenchido da main
    novo.removido = NAO_REMOVIDO;
    novo.proximo = SEM_PROXIMO;
    
    posicionar_rrn(bin, rrn);

    //Escrever os 80 bytes do resgistro
    fwrite(&novo, sizeof(Registro), 1, bin);

    //Atualizar no cabeçalho o proxRRN disponível, caso inserir no fim
    if(rrn >= c->proxRRN) {
        c->proxRRN = rrn + 1;
    }

    //Atualizar o proxRRN, nroEstacoes e nroParesEstacao no disco
    fseek(bin, 5, SEEK_SET); //Pular status (1) e topo (4) para o proxRRN
    fwrite(&(c->proxRRN), sizeof(int), 1, bin);
    fwrite(&(c->nroEstacoes), sizeof(int), 1, bin);
    fwrite(&(c->nroParesEstacao), sizeof(int), 1, bin);
    
    marcar_cons(bin);
}

//========== Função de atualização lógica de um registro (UPDATE) ==========
/**
 * Atualiza logicamente um registo no RRN especificado.
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O "Relative Record Number" (RRN) do registo a ser atualizado.
 * @param c Ponteiro para o cabeçalho atual (carregado na memória).
 * @param atualizado Registro que será atualizado.
 */
void atualizar_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro atualizado) {
    marcar_incons(bin);

    //Garante a coerência dos campos de controle
    atualizado.removido = NAO_REMOVIDO;
    atualizado.proximo = SEM_PROXIMO;

    posicionar_rrn(bin, rrn);

    //Escreve por cima, os 80 bytes 
    fwrite(&atualizado, sizeof(Registro), 1, bin);

    //Atualizar nroEstacoes no cabeçalho, além de necessário se houver mudança de nomes
    fseek(bin, 9, SEEK_SET);
    fwrite(&(c->nroEstacoes), sizeof(int), 1, bin);
    marcar_cons(bin);
}