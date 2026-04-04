/**
 * @file funcoes_manutencao.c
 * @brief Implementação das funcionalidades de manutenção do banco de dados (DELETE, INSERT, UPDATE).
 *
 * @details Este arquivo contém todas as operações que alteram o estado e o conteúdo do 
 * arquivo binário de dados. Ele gerencia registros de tamanho fixo (80 bytes) utilizando 
 * a Abordagem Dinâmica de Reuso de Espaço.
 * * As principais funcionalidades englobam:
 * - Remoção Lógica (DELETE): Marca o registro como removido e o empilha (Pilha LIFO), 
 * atualizando os RRNs no cabeçalho e no registro para reaproveitamento futuro.
 * - Inserção (INSERT): Verifica o cabeçalho para reaproveitar espaços logicamente 
 * removidos (desempilhando). Caso a pilha esteja vazia, insere no final do arquivo.
 * - Atualização (UPDATE): Realiza a modificação dos dados 'in-place diretamente no 
 * byte offset correspondente, mantendo o tamanho fixo de 80 bytes e preenchendo o resto com lixo (definido como '$').
 * * Além da parte de manipulação, há também funções de segurança para registrar 
 * logs em `operacoes.log` e capturar sinais de interrupção (SIGINT) a fim de evitar 
 * corrupção do arquivo binário.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> //Biblioteca necessária para o SIGINT
#include "funcoes_manutencao.h"

long calcular_offset(int rrn) {
    return TAM_CABECALHO + (rrn * TAM_REGISTRO);
}

void marcar_incons(FILE *bin) {
    fseek(bin, 0, SEEK_SET);
    char status_incons = INCONSISTENTE;
    fwrite(&status_incons, sizeof(char), 1, bin);
}

void marcar_cons(FILE *bin) {
    fseek(bin, 0, SEEK_SET);
    char status_cons = CONSISTENTE;
    fwrite(&status_cons, sizeof(char), 1, bin);
}

void posicionar_rrn(FILE *bin, int rrn) {
    fseek(bin, calcular_offset(rrn), SEEK_SET);
}

void escrever_reg(FILE *bin, Registro *r) {
    fwrite(&r->removido, sizeof(char), 1, bin);
    fwrite(&r->proximo, sizeof(int), 1, bin);

    fwrite(&r->codEstacao, sizeof(int), 1, bin);
    fwrite(&r->codLinha, sizeof(int), 1, bin);
    fwrite(&r->codProxEstacao, sizeof(int), 1, bin);
    fwrite(&r->distProxEstacao, sizeof(int), 1, bin);
    fwrite(&r->codLinhaIntegra, sizeof(int), 1, bin);
    fwrite(&r->codEstIntegra, sizeof(int), 1, bin);

    fwrite(&r->tamNomeEstacao, sizeof(int), 1, bin);
    fwrite(r->nomeEstacao, sizeof(char), TAM_MAX_NOME_ESTACAO, bin);

    fwrite(&r->tamNomeLinha, sizeof(int), 1, bin);
    fwrite(r->nomeLinha, sizeof(char), TAM_MAX_NOME_LINHA, bin);
}

//========== Função de remoção lógica de um registro (DELETE) ==========
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

    //Retornar ao início do registro, agora para marcar como removido
    posicionar_rrn(bin, rrn);

    //Novos dados de controle do registro
    char atualizado = REMOVIDO;
    int novo_prox = c->topo; //Apontar para o antigo topo da pilha

    fwrite(&atualizado, sizeof(char), 1, bin);
    fwrite(&novo_prox, sizeof(int), 1, bin);

    char lixo = LIXO;
    for (int i = sizeof(char) + sizeof(int); i < TAM_REGISTRO; i++) {
        fwrite(&lixo, sizeof(char), 1, bin);
    }

    //Atualizar cabeçalho na memória, tornando o novo topo da pilha o rrn atualizado
    c->topo = rrn;

    //Atualizar o cabeçalho no disco, fazendo update imediato
    fseek(bin, 0, SEEK_SET); 

    //
    fwrite(c, sizeof(Cabecalho), 1, bin);

    marcar_cons(bin);
 }

//========== Função de inserção lógica de um registro (INSERT) ==========
void inserir_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro novo) {
    marcar_incons(bin);

    //O novo dado vem preenchido da main
    novo.removido = NAO_REMOVIDO;
    novo.proximo = SEM_PROXIMO;
    
    posicionar_rrn(bin, rrn);

    escrever_reg(bin, &novo);

    //Atualizar no cabeçalho o proxRRN disponível, caso inserir no fim
    if(rrn >= c->proxRRN) {
        c->proxRRN = rrn + 1;
    }

    //Atualizar o proxRRN no disco
    fseek(bin, 5, SEEK_SET); //Pular status (1) e topo (4) para o proxRRN **0 ou 5?
    fwrite(c, sizeof(Cabecalho), 1, bin);
    
    marcar_cons(bin);
}

//========== Função de atualização lógica de um registro (UPDATE) ==========
void atualizar_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro atualizado) {
    marcar_incons(bin);

    //Antes de atualizar, verificar se o novo nome já existe no arquivo para decidir se incrementamos o nroEstacoes no cabeçalho
    if (!estacao_ja_existe(bin, c, atualizado.nomeEstacao)) {
        c->nroEstacoes++;
    }

    //Garantir a coerência dos campos de controle
    atualizado.removido = NAO_REMOVIDO;
    atualizado.proximo = SEM_PROXIMO;

    posicionar_rrn(bin, rrn);

    //Escrever por cima, os 80 bytes do registro atualizado
    escrever_reg(bin, &atualizado);

    //Atualizar nroEstacoes no cabeçalho, além de necessário se houver mudança de nomes
    fseek(bin, 9, SEEK_SET);
    fwrite(&(c->nroEstacoes), sizeof(int), 1, bin);

    marcar_cons(bin);
}