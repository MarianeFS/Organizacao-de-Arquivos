/**
 * @file tipos.h
 * @brief Definição das structs e tipos utilizados no projeto.
 *
 * Contém as definições do registro de cabeçalho e dos registros de dados,
 * seguindo estritamente as especificações do trabalho prático (versão CSV,
 * arquivo único, registros de tamanho fixo de 80 bytes).
 *
 * @note Alunos: [SEU NUSP] - [SEU NOME] / 16840035 - Mariane Ferreira dos Santos
 */

#ifndef TIPOS_H
#define TIPOS_H

/* =========================================================
 * CONSTANTES GERAIS
 * ========================================================= */

/** @brief Tamanho fixo de cada registro de dados em bytes. */
#define TAM_REGISTRO 80

/** @brief Tamanho do registro de cabeçalho em bytes. */
#define TAM_CABECALHO 17

/** @brief Tamanho máximo do campo nomeEstacao (variável dentro do registro). */
#define TAM_MAX_NOME_ESTACAO 32

/** @brief Tamanho máximo do campo nomeLinha (variável dentro do registro). */
#define TAM_MAX_NOME_LINHA 20

/** @brief Caractere usado para identificar lixo nos bytes não utilizados. */
#define LIXO '$'

/** @brief Valor que indica que não há próximo registro removido na lista. */
#define SEM_PROXIMO -1

/** @brief Valor que representa nulo nos campos de tamanho fixo. */
#define NULO_INT -1

/** @brief String exibida quando campo fixo é nulo. */
#define STR_NULO "NULO"

/** @brief Indica que o registro NÃO está logicamente removido. */
#define NAO_REMOVIDO '0'

/** @brief Indica que o registro ESTÁ logicamente removido. */
#define REMOVIDO '1'

/** @brief Arquivo está consistente. */
#define CONSISTENTE '1'

/** @brief Arquivo está inconsistente (aberto para escrita). */
#define INCONSISTENTE '0'


/* =========================================================
 * STRUCT: CABEÇALHO
 * =========================================================
 *
 * Layout em disco (17 bytes):
 *  Byte 0        : status      (char, 1 byte)
 *  Bytes 1-4     : topo        (int,  4 bytes) — RRN do topo da pilha de removidos
 *  Bytes 5-8     : proxRRN     (int,  4 bytes) — próximo RRN disponível (inicia em 0)
 *  Bytes 9-12    : nroEstacoes (int,  4 bytes)
 *  Bytes 13-16   : nroParesEstacao (int, 4 bytes)
 */

/**
 * @brief Registro de cabeçalho do arquivo binário.
 *
 * Armazena metadados do arquivo: consistência, controle de registros
 * removidos e contadores de estações e pares de estações.
 */
typedef struct {
    char status;          /**< Consistência: '1' = consistente, '0' = inconsistente. */
    int  topo;            /**< RRN do topo da pilha de registros removidos (-1 = vazia). */
    int  proxRRN;         /**< Próximo RRN disponível para inserção (inicia em 0). */
    int  nroEstacoes;     /**< Quantidade de estações distintas armazenadas. */
    int  nroParesEstacao; /**< Quantidade de pares (codEstacao, codProxEstacao) distintos. */
} Cabecalho;


/* =========================================================
 * STRUCT: REGISTRO DE DADOS
 * =========================================================
 *
 * Layout em disco (80 bytes FIXOS):
 *  Byte 0        : removido          (char, 1 byte)
 *  Bytes 1-4     : proximo           (int,  4 bytes) — RRN do próx. removido na pilha
 *  Bytes 5-8     : codEstacao        (int,  4 bytes)
 *  Bytes 9-12    : nomeEstacao       (int,  4 bytes) — indicador de tamanho
 *  Bytes 13-16   : codLinha          (int,  4 bytes)
 *  Bytes 17-20   : nomeLinha         (int,  4 bytes) — indicador de tamanho
 *  Bytes 21-24   : codProxEstacao    (int,  4 bytes)
 *  Bytes 25-28   : distProxEstacao   (int,  4 bytes)
 *  Bytes 29-32   : codLinhaIntegra   (int,  4 bytes)
 *  Bytes 33-36   : codEstIntegra     (int,  4 bytes)  ← ocupa até byte 36
 *  Bytes 37-68   : nomeEstacao       (char[32]) — parte variável
 *  Bytes 69-88?  → na verdade layout correto abaixo:
 *
 * Layout real conforme especificação (80 bytes):
 *  [0]     removido         (1 byte)
 *  [1-4]   proximo          (4 bytes)
 *  [5-8]   codEstacao       (4 bytes)
 *  [9-12]  codLinha         (4 bytes)
 *  [13-16] codProxEstacao   (4 bytes)
 *  [17-20] distProxEstacao  (4 bytes)
 *  [21-24] codLinhaIntegra  (4 bytes)
 *  [25-28] codEstIntegra    (4 bytes)
 *  [29]    tamNomeEstacao   (4 bytes) → [29-32]
 *  [33...] nomeEstacao      (variável, até tamNomeEstacao bytes)
 *  [...]   tamNomeLinha     (4 bytes)
 *  [...]   nomeLinha        (variável, até tamNomeLinha bytes)
 *  resto   preenchido com LIXO '$'
 */

/**
 * @brief Registro de dados de uma estação/linha de metrô.
 *
 * Representa um registro lido do CSV ou do arquivo binário.
 * Em disco, o registro ocupa exatamente 80 bytes (campos fixos +
 * strings com indicador de tamanho + lixo '$' no restante).
 */
typedef struct {
    /* --- campos de gerenciamento (não são dados da estação) --- */
    char removido;       /**< '0' = não removido (ativo); '1' = logicamente removido. */
    int  proximo;        /**< RRN do próximo registro removido na pilha (-1 = fim). */

    /* --- campos de tamanho fixo --- */
    int  codEstacao;        /**< Código da estação (-1 se nulo). */
    int  codLinha;          /**< Código da linha. */
    int  codProxEstacao;    /**< Código da próxima estação. */
    int  distProxEstacao;   /**< Distância para a próxima estação (metros). */
    int  codLinhaIntegra;   /**< Código da linha de integração (-1 se não há integração). */
    int  codEstIntegra;     /**< Código da estação de integração (-1 se não há integração). */

    /* --- campos de tamanho variável (com indicador de tamanho) --- */
    int  tamNomeEstacao;    /**< Tamanho em bytes da string nomeEstacao (0 se nulo). */
    char nomeEstacao[TAM_MAX_NOME_ESTACAO]; /**< Nome da estação (sem '\0' no disco). */

    int  tamNomeLinha;      /**< Tamanho em bytes da string nomeLinha (0 se nulo). */
    char nomeLinha[TAM_MAX_NOME_LINHA];     /**< Nome da linha (sem '\0' no disco). */
} Registro;


#endif /* TIPOS_H */
