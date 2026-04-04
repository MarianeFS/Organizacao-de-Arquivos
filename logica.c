/**
 * @file logica.c
 * @brief Implementação de regras de negócio e interface com o usuário.
 *
 * @details Este ficheiro coordena as operações de INSERT, DELETE e UPDATE,
 * realizando a ponte entre a entrada de dados e a manipulação binária.
 *
 * @author [Seu Nome] - [Seu NUSP]
 * @author Mariane Ferreira dos Santos - 16840035
 * @date Abril de 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "funcoes_manutencao.c" // 

//========== Funções auxiliares de suporte ==========
/**
 * @brief Ler um registro completo de 80 bytes do disco para a RAM.
 * @return '1' se o registro não está removido, '0' caso contrário.
 */
int ler_registro_binario(FILE *bin, Registro *r) {
    // Ler o registro inteiro (80 bytes)
    if (fread(r, sizeof(Registro), 1, bin) < 1) return 0;
    
    //Retorna 1 apenas se o registro estiver ativo
    return (r->removido == NAO_REMOVIDO);
}

/**
 * @brief Atualiza um campo específico na struct em memória.
 */
void atualizar_campo_na_struct(Registro *r, char *campo, char *valor) {
    if (strcmp(campo, "codEstacao") == 0) {
        r->codEstacao = atoi(valor);
    } else if (strcmp(campo, "nomeEstacao") == 0) {
        strncpy(r->nomeEstacao, valor, TAM_MAX_NOME_ESTACAO);
        r->tamNomeEstacao = strlen(r->nomeEstacao);
    } else if (strcmp(campo, "codLinha") == 0) {
        r->codLinha = atoi(valor);
    } else if (strcmp(campo, "nomeLinha") == 0) {
        strncpy(r->nomeLinha, valor, TAM_MAX_NOME_LINHA);
        r->tamNomeLinha = strlen(r->nomeLinha);
    } else if (strcmp(campo, "distProxEstacao") == 0) {
        r->distProxEstacao = atoi(valor);
    }
    // Adicionar os demais campos conforme os tipos.h
}

/**
 * @brief Verifica se um registro atende ao critério de busca.
 */
int registro_atende_criterio(Registro *r, char *campo, char *valor) {
    if (strcmp(campo, "codEstacao") == 0) return r->codEstacao == atoi(valor);
    if (strcmp(campo, "nomeEstacao") == 0) return strcmp(r->nomeEstacao, valor) == 0;
    if (strcmp(campo, "codLinha") == 0) return r->codLinha == atoi(valor);
    if (strcmp(campo, "nomeLinha") == 0) return strcmp(r->nomeLinha, valor) == 0;
    return 0;
}

//============== Lógicas principais ===============

//========== Lógica de inserção (INSERT) ==========
void logica_insercao(FILE *bin, Cabecalho *c) {
    Registro novo;
    memset(&novo, LIXO, sizeof(Registro)); // Limpa com '$'

    //Chamar a função que lê do teclado, ex:
    // ler_dados_teclado(&novo); 

    int rrn_destino;

    if (c->topo == SEM_PROXIMO) {
        rrn_destino = c->proxRRN;
    } else {
        rrn_destino = c->topo;
        posicionar_rrn(bin, rrn_destino);
        
        char removido;
        int proximo_na_pilha;
        
        fread(&removido, sizeof(char), 1, bin);
        fread(&proximo_na_pilha, sizeof(int), 1, bin);

        c->topo = proximo_na_pilha;

        //Atualizar o topo no cabeçalho (disco)
        fseek(bin, 1, SEEK_SET); 
        fwrite(&(c->topo), sizeof(int), 1, bin);
    }

    insere_reg_rrn(bin, rrn_destino, c, novo);
    registrar_log("INSERT", rrn_destino);
}

//========== Lógica de remoção (DELETE) ==========
void logica_remocao(FILE *bin, Cabecalho *c) {
    char campo[50], valor[50];
    scanf("%s %s", campo, valor); //Ler critério de busca

    int encontrou = 0;
    for (int i = 0; i < c->proxRRN; i++) {
        posicionar_rrn(bin, i);
        Registro reg;
        
        if (ler_registro_binario(bin, &reg)) {
            if (registro_atende_criterio(&reg, campo, valor)) {
                remover_reg_rrn(bin, i, c);
                registrar_log("DELETE", i);
                encontrou = 1;
            }
        }
    }
    if (!encontrou) printf("Registro inexistente.\n");
}

//========== Lógica de atualização (UPDATE) ==========
void logica_atualizacao(FILE *bin, Cabecalho *c) {
    char campo_busca[50], valor_busca[50];
    char campo_atualizar[50], novo_valor[50];

    scanf("%s %s %s %s", campo_busca, valor_busca, campo_atualizar, novo_valor);

    int encontrou = 0;
    for (int i = 0; i < c->proxRRN; i++) {
        posicionar_rrn(bin, i);
        Registro reg;
        
        if (ler_registro_binario(bin, &reg)) { 
            if (registro_atende_criterio(&reg, campo_busca, valor_busca)) {
                atualizar_campo_na_struct(&reg, campo_atualizar, novo_valor);
                atualizar_reg_rrn(bin, i, c, reg);
                registrar_log("UPDATE", i);
                encontrou = 1;
            }
        }
    }
    if (!encontrou) printf("Registro inexistente.\n");
}