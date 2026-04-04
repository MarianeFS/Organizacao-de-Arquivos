/**
 * @file logica.c
 * @brief Implementação de regras e interface com o usuário.
 *
 * @details Este ficheiro gerencia as operações de DELETE, INSERT e UPDATE,
 * realizando a ponte entre a entrada de dados e a manipulação binária.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funcoes_manutencao.h" 
#include "logica.h"

//========== Funções auxiliares de suporte ==========
//Função interna para ler strings do teclado tratando o valor "NULO"
void ler_string_fixa(char *destino, int tam_max) {
    char buffer[100];
    scanf("%s", buffer);
    if (strcmp(buffer, STR_NULO) == 0) {
        destino[0] = '\0';
    } else {
        strncpy(destino, buffer, tam_max);
    }
}

//Função necessária para o INSERT funcionar
void ler_dados_teclado(Registro *r) {
    char buffer[TAM_MAX_NOME_ESTACAO];

    // Campos inteiros simples
    if (scanf("%d", &r->codEstacao) != 1) r->codEstacao = NULO_INT;
    if (scanf("%d", &r->codLinha) != 1) r->codLinha = NULO_INT;
    if (scanf("%d", &r->codProxEstacao) != 1) r->codProxEstacao = NULO_INT;
    if (scanf("%d", &r->distProxEstacao) != 1) r->distProxEstacao = NULO_INT;
    if (scanf("%d", &r->codLinhaIntegra) != 1) r->codLinhaIntegra = NULO_INT;
    if (scanf("%d", &r->codEstIntegra) != 1) r->codEstIntegra = NULO_INT;

    // Campos de string usando a função fornecida
    ScanQuoteString(buffer);
    strcpy(r->nomeEstacao, buffer);
    r->tamNomeEstacao = strlen(buffer);

    ScanQuoteString(buffer);
    strcpy(r->nomeLinha, buffer);
    r->tamNomeLinha = strlen(buffer);
}

int ler_reg_binario(FILE *bin, Registro *r) {
    fread(&r->removido, sizeof(char), 1, bin);
    fread(&r->proximo, sizeof(int), 1, bin);

    fread(&r->codEstacao, sizeof(int), 1, bin);
    fread(&r->codLinha, sizeof(int), 1, bin);
    fread(&r->codProxEstacao, sizeof(int), 1, bin);
    fread(&r->distProxEstacao, sizeof(int), 1, bin);
    fread(&r->codLinhaIntegra, sizeof(int), 1, bin);
    fread(&r->codEstIntegra, sizeof(int), 1, bin);

    fread(&r->tamNomeEstacao, sizeof(int), 1, bin);
    fread(r->nomeEstacao, sizeof(char), TAM_MAX_NOME_ESTACAO, bin);

    fread(&r->tamNomeLinha, sizeof(int), 1, bin);
    fread(r->nomeLinha, sizeof(char), TAM_MAX_NOME_LINHA, bin);

    return (r->removido == NAO_REMOVIDO);
}

void atualizar_campo_struct(Registro *r, char *campo, char *valor) {
    if (strcmp(campo, "codEstacao") == 0) r->codEstacao = atoi(valor);
    else if (strcmp(campo, "codLinha") == 0) r->codLinha = atoi(valor);
    else if (strcmp(campo, "codProxEstacao") == 0) r->codProxEstacao = atoi(valor);
    else if (strcmp(campo, "distProxEstacao") == 0) r->distProxEstacao = atoi(valor);
    else if (strcmp(campo, "codLinhaIntegra") == 0) r->codLinhaIntegra = atoi(valor);
    else if (strcmp(campo, "codEstIntegra") == 0) r->codEstIntegra = atoi(valor);
    else if (strcmp(campo, "nomeEstacao") == 0) {
        strcpy(r->nomeEstacao, valor);
        r->tamNomeEstacao = strlen(valor);
    } else if (strcmp(campo, "nomeLinha") == 0) {
        strcpy(r->nomeLinha, valor);
        r->tamNomeLinha = strlen(valor);
    }
}

int reg_atende_criterio(Registro *r, char *campo, char *valor) {
    if (strcmp(campo, "codEstacao") == 0) return r->codEstacao == atoi(valor);
    if (strcmp(campo, "codLinha") == 0) return r->codLinha == atoi(valor);
    if (strcmp(campo, "codProxEstacao") == 0) return r->codProxEstacao == atoi(valor);
    if (strcmp(campo, "distProxEstacao") == 0) return r->distProxEstacao == atoi(valor);
    if (strcmp(campo, "codLinhaIntegra") == 0) return r->codLinhaIntegra == atoi(valor);
    if (strcmp(campo, "codEstIntegra") == 0) return r->codEstIntegra == atoi(valor);
    if (strcmp(campo, "nomeEstacao") == 0) return strcmp(r->nomeEstacao, valor) == 0;
    if (strcmp(campo, "nomeLinha") == 0) return strcmp(r->nomeLinha, valor) == 0;
    return 0;
}

void reg_log(const char *operacao, int rrn) {
    FILE *f = fopen("operacoes.log", "a");
    if (f) {
        fprintf(f, "[%s] Registro processado no RRN: %d\n", operacao, rrn);
        fclose(f);
    }
}

int estacao_ja_existe(FILE *bin, Cabecalho *c, char *nome) {
    if (nome[0] == '\0') return 0;
    
    Registro aux;
    for (int i = 0; i < c->proxRRN; i++) {
        posicionar_rrn(bin, i);
        if (ler_reg_binario(bin, &aux)) {
            if (strcmp(aux.nomeEstacao, nome) == 0) {
                return 1; // Nome já existe
            }
        }
    }
    return 0;
}

void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; 

    if (R == 'N' || R == 'n') {
        getchar();
        getchar();
        getchar();     
        strcpy(str, ""); 
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { 
            strcpy(str, "");
        }
        getchar();         
    } else if (R != EOF) { 
                           
                           
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}

//============== Lógicas principais ===============

//========== Lógica de remoção (DELETE) ==========
void logica_remocao(FILE *bin, Cabecalho *c) {
    int n_buscas;
    scanf("%d", &n_buscas); 

    for(int i = 0; i < n_buscas; i++) {
        char campo[50], valor[50];
        scanf("%s %s", campo, valor);

        int encontrou = 0;
        for (int j = 0; j < c->proxRRN; j++) {
            posicionar_rrn(bin, i);
            Registro reg;
            if (ler_reg_binario(bin, &reg)) {
                if (reg_atende_criterio(&reg, campo, valor)) {
                    remover_reg_rrn(bin, i, c);
                    encontrou = 1;
                }
            }
        }
        if (!encontrou) {
        printf("Registro inexistente.\n");
        }
    }
}

//========== Lógica de inserção (INSERT) ==========
void logica_insercao(FILE *bin, Cabecalho *c) {
    int n_insercoes;
    if(scanf("%d", &n_insercoes) != 1) return;

    for (int i = 0; i < n_insercoes; i++) {
        Registro novo;
        memset(&novo, LIXO, sizeof(Registro)); // Limpa com '$'
        ler_dados_teclado(&novo); 

        //Se o nome da estação é novo, incrementar o contador no cabeçalho
        if (!estacao_ja_existe(bin, c, novo.nomeEstacao)) {
            c->nroEstacoes++;
        }

        int rrn_procurado;

        if (c->topo == SEM_PROXIMO) {
            rrn_procurado = c->proxRRN;
        } else {
            rrn_procurado = c->topo;
            posicionar_rrn(bin, rrn_procurado);
            
            char removido;
            int prox_na_pilha;
            
            fread(&removido, sizeof(char), 1, bin);
            fread(&prox_na_pilha, sizeof(int), 1, bin);

            c->topo = prox_na_pilha;
        }
        inserir_reg_rrn(bin, rrn_procurado, c, novo);
    }
}

//========== Lógica de atualização (UPDATE) ==========
void logica_atualizacao(FILE *bin, Cabecalho *c) {
    int n_atualizacoes;
    scanf("%d", &n_atualizacoes);

    for(int i = 0; i < n_atualizacoes; i++){
        char campo_busca[50], valor_busca[50];
        char campo_atualizar[50], novo_valor[50];
        scanf("%s %s %s %s", campo_busca, valor_busca, campo_atualizar, novo_valor);

        for (int j = 0; j < c->proxRRN; j++) {
            posicionar_rrn(bin, i);
            Registro reg;
            if (ler_reg_binario(bin, &reg)) { 
                if (reg_atende_criterio(&reg, campo_busca, valor_busca)) {
                    atualizar_campo_struct(&reg, campo_atualizar, novo_valor);
                    atualizar_reg_rrn(bin, i, c, reg);
                }
            }
        }
    }
}