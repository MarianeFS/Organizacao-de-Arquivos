/**
 * @file funcoes_manutencao.h
 * @brief Protótipos das funções de manipulação técnica e manutenção do arquivo binário.
 *
 * @details Este arquivo define a interface para as operações de baixo nível que
 * alteram o estado do disco. Inclui o cálculo de Byte Offset, gestão de 
 * consistência do arquivo e as operações de escrita para inserção, remoção e atualização.
 *
 * @author [Seu Nome Aqui] - [Seu NUSP]
 * @author Mariane Ferreira dos Santos - 16840035
 * @date Abril de 2026
 */

#ifndef FUNCOES_MANUTENCAO_H
#define FUNCOES_MANUTENCAO_H

#include <stdio.h>
#include "tipos.h"

//============ Funções auxiliares e de navegação ============
/**
 * @brief Calcula a posição exata (em bytes) de um RRN no arquivo.
 * @param rrn Número relativo do registro.
 * @return Deslocamento (offset) em bytes a partir do início do arquivo.
 */
long calcular_offset(int rrn);

/**
 * @brief Altera o primeiro byte do arquivo para indicar que ele está sendo modificado.
 * @param bin Ponteiro para o arquivo binário.
 */
void marcar_incons(FILE *bin);

/**
 * @brief Altera o primeiro byte do arquivo para indicar que ele está finalizado e íntegro.
 * @param bin Ponteiro para o arquivo binário.
 */
void marcar_cons(FILE *bin);

/**
 * @brief Move o ponteiro de leitura/escrita para o início de um registro específico.
 * @param bin Ponteiro para o arquivo binário.
 * @param rrn Número relativo do registro alvo.
 */
void posicionar_rrn(FILE *bin, int rrn);


//============ Operações de manutenção (disco) ============
/**
 * @brief Realiza a remoção lógica, atualizando a pilha de removidos no cabeçalho.
 * @param bin Arquivo binário aberto para leitura e escrita.
 * @param rrn RRN do registro a ser removido.
 * @param c Ponteiro para o cabeçalho em memória (será atualizado e sincronizado no disco).
 */
void remover_reg_rrn(FILE *bin, int rrn, Cabecalho *c);

/**
 * @brief Escreve os 80 bytes de um novo registro no local indicado.
 * @param bin Arquivo binário aberto para leitura e escrita.
 * @param rrn RRN onde o registro será gravado.
 * @param c Ponteiro para o cabeçalho para atualizar proxRRN e contadores.
 * @param novo Struct contendo os dados a serem gravados.
 */
void insere_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro novo);

/**
 * @brief Sobrescreve um registro existente com novos dados.
 * @param bin Arquivo binário aberto para leitura e escrita.
 * @param rrn RRN do registro a ser atualizado.
 * @param c Ponteiro para o cabeçalho para atualizar contadores de estações.
 * @param atualizado Struct contendo os novos dados.
 */
void atualizar_reg_rrn(FILE *bin, int rrn, Cabecalho *c, Registro atualizado);

#endif /* FUNCOES_MANUTENCAO_H */