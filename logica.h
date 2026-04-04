/**
 * @file logica.h
 * @brief Protótipos das funções de regras, interface e suporte.
 *
 * @details Este arquivo define a interface para a lógica do sistema.
 * Contém os protótipos das funções que gerenciam o fluxo de dados entre
 * o usuário (entrada via teclado) e as funções de baixo nível no disco.
 */

#ifndef LOGICA_H
#define LOGICA_H

#include <stdio.h>
#include "tipos.h"

//============ Funções auxiliares de suporte ============
/**
 * @brief Gerar o checksum do ficheiro binário para comparação no run.codes.
 * @details Ler todos os bytes do ficheiro, somá-los e imprimir o resultado/100.
 * @attention O ficheiro deve estar fechado (fclose) antes da chamada.
 * @param arquivo Nome da string do ficheiro (ex: "estacoes.bin").
 */
void BinarioNaTela(char *arquivo);

/**
 * @brief Lê uma string e trata o valor "NULO".
 * @details Caso a entrada seja "NULO", define a string como vazia.
 * @param destino Ponteiro para o buffer de destino.
 * @param tam_max Limite de caracteres a serem copiados.
 */
void ler_string_fixa(char *destino, int tam_max);

/**
 * @brief Lê os dados de uma estação do stdin e preenche a struct.
 * @details Trata campos numéricos e usa ScanQuoteString para strings, 
 * lidando com aspas e valores "NULO".
 * @param r Ponteiro para o registro de destino.
 */
void ler_dados_teclado(Registro *r);

/**
 * @brief Ler um registro de 80 bytes do arquivo binário.
 * @param bin Ponteiro para o arquivo binário aberto.
 * @param r Ponteiro para a struct onde os dados serão armazenados.
 * @return Retornar 1 se o registro for válido (não removido) e 0 caso contrário.
 */
int ler_reg_binario(FILE *bin, Registro *r);

/**
 * @brief Modificar o valor de um campo específico em uma struct Registro na memória.
 * @param r Ponteiro para o registro a ser modificado.
 * @param campo Nome do campo procurado.
 * @param valor Novo valor em formato de string (convertido internamente).
 */
void atualizar_campo_struct(Registro *r, char *campo, char *valor);

/**
 * @brief Comparar os dados de um registro com um critério de busca.
 * @param r Ponteiro para o registro lido.
 * @param campo Nome do campo para comparação.
 * @param valor Valor esperado para o campo.
 * @return Retornar 1 se o registro atender ao critério e 0 caso contrário.
 */
int reg_atende_criterio(Registro *r, char *campo, char *valor);

/**
 * @brief Registrar uma operação realizada no arquivo de log de manutenção.
 * @param operacao Nome da operação.
 * @param rrn RRN onde a operação ocorreu.
 */
void reg_log(const char *operacao, int rrn);

/**
 * @brief Verifica se um nome de estação já existe no arquivo.
 * @return 1 se existe, 0 caso contrário.
 */
int estacao_ja_existe(FILE *bin, Cabecalho *c, char *nome);

/**
 * @brief Lê uma string do stdin tratando aspas e a palavra "NULO".
 * @details Remove aspas delimitadoras e retorna string vazia se for "NULO".
 * @param str Ponteiro para o buffer de destino.
 */
void ScanQuoteString(char *str);

//============ Lógicas principais (módulos do menu) ============
/**
 * @brief Executar a remoção lógica de registros baseada em critérios de busca.
 * @param bin Ponteiro para o arquivo binário.
 * @param c Ponteiro para o cabeçalho na memória.
 */
void logica_remocao(FILE *bin, Cabecalho *c);

/**
 * @brief Gerenciar a inserção de novos dados, tratando o reuso de espaço (pilha).
 * @param bin Ponteiro para o arquivo binário.
 * @param c Ponteiro para o cabeçalho na memória.
 */
void logica_insercao(FILE *bin, Cabecalho *c);

/**
 * @brief Executar a atualização de campos em registros baseada em critérios de busca.
 * @param bin Ponteiro para o arquivo binário.
 * @param c Ponteiro para o cabeçalho na memória.
 */
void logica_atualizacao(FILE *bin, Cabecalho *c);

#endif /* LOGICA_H */