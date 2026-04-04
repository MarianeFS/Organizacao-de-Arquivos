/**
 * @file main.c
 * @brief Ponto de entrada do programa e gerenciador do menu principal.
 *
 * @details Este arquivo contém a função principal que inicializa o sistema,
 * carrega o cabeçalho do arquivo binário para a memória e gerencia o loop
 * de opções baseado na entrada do usuário. Também implementa o tratamento
 * de sinais (SIGINT) para garantir a integridade do arquivo em caso de 
 * fechamento inesperado.
 * * Funcionalidades principais:
 * - Abertura e fechamento seguro do arquivo binário.
 * - Gerenciamento do status de consistência ('0' ou '1').
 * - Direcionamento das opções do menu para as funções de lógica.
 *
 * @author [Seu Nome Aqui] - [Seu NUSP]
 * @author Mariane Ferreira dos Santos - 16840035
 * @note Trabalho Prático 1 - (SCC0215) Organização de Arquivos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "tipos.h"
#include "funcoes_manutencao.c"

/** 
 * @brief Ponteiro global para o arquivo binário.
 * Utilizado pelo tratador de sinais (SIGINT) para fechar o arquivo corretamente.
 */
FILE *bin_global = NULL;

/**
 * @brief Captura o sinal de interrupção (Ctrl+C) e fecha o arquivo com segurança.
 * @param sig Número do sinal recebido.
 */
void trata_sigint(int sig) {
    if (bin_global != NULL) {
        printf("\nAVISO: Interrupção detectada! Salvando status do arquivo...\n");
        fseek(bin_global, 0, SEEK_SET);
        char status = CONSISTENTE; 
        fwrite(&status, sizeof(char), 1, bin_global);
        fclose(bin_global);
    }
    exit(0);
}

/**
 * @brief Função principal (Main).
 * Responsável por carregar o cabeçalho, validar o status e executar o menu.
 * @return int Código de execução (0 para sucesso).
 */
int main() {
    signal(SIGINT, trata_sigint);

    FILE *bin = fopen("estacoes.bin", "rb+");
    if (bin == NULL) {
        printf("Erro ao abrir o ficheiro.\n");
        return 0;
    }
    bin_global = bin; 

    //Ler o cabeçalho para a memória
    Cabecalho c;
    fseek(bin, 0, SEEK_SET);
    fread(&c.status, sizeof(char), 1, bin);
    fread(&c.topo, sizeof(int), 1, bin);
    fread(&c.proxRRN, sizeof(int), 1, bin);
    fread(&c.nroEstacoes, sizeof(int), 1, bin);
    fread(&c.nroParesEstacao, sizeof(int), 1, bin);

    //Verificar consistência
    if (c.status == INCONSISTENTE) {
        printf("Ficheiro corrompido.\n");
        fclose(bin);
        return 0;
    }

    //Loop do menu principal
    int opcao;
    //Se a entrada acabar, EOF permite que o programa pare
    while (scanf("%d", &opcao) != EOF) {
        switch (opcao) {
            case 1:// CREATE TABLE
                //
                break;
            case 2: // SELECT
                //
                break;
            case 3: // SELECT WHERE
                //
                break;
            case 4: // INSERT
                logica_insercao(bin, &c);
                registrar_log("INSERT", c.proxRRN - 1); // Exemplo de log
                break;
            case 5: // DELETE (Apenas rascunho da lógica)
                // 1. ler campo de busca (ex: nomeEstacao)
                // 2. rrn_encontrado = busca_registro(bin, &c, valor);
                // 3. remover_reg_rrn(bin, rrn_encontrado, &c);
                break;
            case 6: // UPDATE
                //
                break;
            default:
                break;
        }
    }

    marcar_cons(bin);
    fclose(bin);
    return 0;
}
