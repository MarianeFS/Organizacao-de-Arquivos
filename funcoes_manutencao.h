/**
 * @file funcoes_manutencao.h
 * @brief Protótipos das funções de manipulação técnica e manutenção do arquivo binário.
 *
 * @details Este arquivo define a interface para as operações de baixo nível que
 * alteram o estado do disco. Inclui o cálculo de ByteOffset, gestão de 
 * consistência do arquivo e as operações de escrita para inserção, remoção e atualização.
 */

#ifndef FUNCOES_MANUTENCAO_H
#define FUNCOES_MANUTENCAO_H

#include <stdio.h>
#include "tipos.h"

//============ Funções auxiliares e de navegação ============
/**
 * @brief Calcular a posição exata (em bytes) de um RRN no arquivo.
 * @param rrn O "Relative Record Number" (RRN): Número relativo do registro.
 * @return Deslocamento (offset) em bytes a partir do início do arquivo.
 */
long calcular_offset(int rrn);

/**
 * @brief Alterar o primeiro byte do arquivo para indicar que ele está sendo modificado.
 * @param bin Ponteiro para o arquivo binário.
 */
void marcar_incons(FILE *bin);

/**
 * @brief Alterar o primeiro byte do arquivo para indicar que ele está finalizado e íntegro (consistente).
 * @param bin Ponteiro para o arquivo binário.
 */
void marcar_cons(FILE *bin);

/**
 * @brief Mover o ponteiro de leitura/escrita para o início de um registro específico.
 * @param bin Ponteiro para o arquivo binário.
 * @param rrn O "Relative Record Number" (RRN): Número relativo do registro alvo.
 */
void posicionar_rrn(FILE *bin, int rrn);

void escrever_reg(FILE *bin, Registro *r);

//============ Operações de manutenção (disco) ============
/**
 * Remover logicamente um registo no RRN especificado e atualizar a pilha. Se a função DELETE deletar todas as instâncias de uma estação única, é necessário atualizar o número de estações (cabeçalho).
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O "Relative Record Number" (RRN) do registo a ser removido.
 * @param c Ponteiro para o cabeçalho atual (carregado na memória).
 */
void remover_reg_rrn(FILE *bin, int rrn, Cabecalho *c);

/**
 * Inserir logicamente um registo no RRN especificado.
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O "Relative Record Number" (RRN) do registo a ser inserido.
 * @param c Ponteiro para o cabeçalho atual (carregado na memória).
 * @param novo Novo registro que será inserido (struct com os dados).
 */
void inserir_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro novo);

/**
 * Atualizar logicamente um registo no RRN especificado.
 * * @param bin Ficheiro binário aberto em modo "rb+" (leitura e escrita).
 * @param rrn O "Relative Record Number" (RRN) do registo a ser atualizado.
 * @param c Ponteiro para o cabeçalho atual (carregado na memória).
 * @param atualizado Registro que será atualizado.
 */
void atualizar_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro atualizado);

#endif /* FUNCOES_MANUTENCAO_H */