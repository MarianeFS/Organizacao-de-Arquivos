/**
 * @file logica.h
 * @brief Protótipos das funções de regras de negócio, interface e suporte.
 *
 * @details Este arquivo define a interface para a camada de lógica do sistema.
 * Contém os protótipos das funções que gerenciam o fluxo de dados entre
 * o usuário (entrada via teclado) e as funções de baixo nível no disco.
 *
 * @author [Seu Nome Aqui] - [Seu NUSP]
 * @author Mariane Ferreira dos Santos - 16840035
 * @date Abril de 2026
 */

#ifndef LOGICA_H
#define LOGICA_H

#include <stdio.h>
#include "tipos.h"

//============ Funções auxiliares de suporte ============

/**
 * @brief Lê um registro de 80 bytes do arquivo binário.
 * @param bin Ponteiro para o arquivo binário aberto.
 * @param r Ponteiro para a struct onde os dados serão armazenados.
 * @return Retorna 1 se o registro for válido (não removido), 0 caso contrário.
 */
int ler_registro_binario(FILE *bin, Registro *r);

/**
 * @brief Modifica o valor de um campo específico em uma struct Registro em memória.
 * @param r Ponteiro para o registro a ser modificado.
 * @param campo Nome do campo alvo (ex: "codEstacao", "nomeLinha").
 * @param valor Novo valor em formato de string (convertido internamente).
 */
void atualizar_campo_na_struct(Registro *r, char *campo, char *valor);

/**
 * @brief Compara os dados de um registro com um critério de busca.
 * @param r Ponteiro para o registro lido.
 * @param campo Nome do campo para comparação.
 * @param valor Valor esperado para o campo.
 * @return Retorna 1 se o registro atender ao critério, 0 caso contrário.
 */
int registro_atende_criterio(Registro *r, char *campo, char *valor);

/**
 * @brief Registra uma operação realizada no arquivo de log de manutenção.
 * @param operacao Nome da operação (ex: "INSERT", "DELETE").
 * @param rrn RRN onde a operação ocorreu.
 */
void registrar_log(const char *operacao, int rrn);


//============ Lógicas principais (módulos do menu) ============
/**
 * @brief Coordena a inserção de novos dados, tratando o reuso de espaço (pilha).
 * @param bin Ponteiro para o arquivo binário.
 * @param c Ponteiro para o cabeçalho em memória.
 */
void logica_insercao(FILE *bin, Cabecalho *c);

/**
 * @brief Executa a remoção lógica de registros baseada em critérios de busca.
 * @param bin Ponteiro para o arquivo binário.
 * @param c Ponteiro para o cabeçalho em memória.
 */
void logica_remocao(FILE *bin, Cabecalho *c);

/**
 * @brief Executa a atualização de campos em registros baseada em critérios de busca.
 * @param bin Ponteiro para o arquivo binário.
 * @param c Ponteiro para o cabeçalho em memória.
 */
void logica_atualizacao(FILE *bin, Cabecalho *c);

#endif /* LOGICA_H */