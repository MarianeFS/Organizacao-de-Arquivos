#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "funcoes_manutencao.c"

//Funções que ajudam as lógicas principais
int buscar_rrn_por_campo(FILE *bin, Cabecalho *c, char *nomeCampo, char *valor);
void ler_dados_teclado(Registro *r);

//========== Lógica de inserção (INSERT) ==========
void logica_insercao(FILE *bin, Cabecalho *c) {
    Registro novo;
    memset(&novo, LIXO, sizeof(Registro)); //Importante: Limpa a struct com '$'

    // 1. Ler dados
    // Ex: ler_dados_teclado(&novo); 

    int rrn_destino;

    // 2. Estratégia de Reuso (Pilha)
    if (c->topo == SEM_PROXIMO) {
        rrn_destino = c->proxRRN;
    } else {
        rrn_destino = c->topo;

        // Desempilhar (Pop): Lê quem é o próximo na pilha antes de sobrescrever
        posicionar_rrn(bin, rrn_destino);
        char removido;
        int proximo_na_pilha;
        
        fread(&removido, sizeof(char), 1, bin);
        fread(&proximo_na_pilha, sizeof(int), 1, bin);

        c->topo = proximo_na_pilha; // Atualiza o topo na memória

        // Atualiza o novo topo no cabeçalho do disco
        fseek(bin, 1, SEEK_SET); 
        fwrite(&(c->topo), sizeof(int), 1, bin);
    }

    // 3. Gravar efetivamente
    insere_reg_rrn(bin, rrn_destino, c, novo);
}

//========== Lógica de remoção (DELETE) ==========
void logica_remocao(FILE *bin, Cabecalho *c) {
    // 1. Ler qual campo o usuário quer buscar (ex: codEstacao)
    // 2. rrn = buscar_rrn_por_campo(bin, c, "codEstacao", "10");
    
    // 3. Se achou, chama a função de manutenção
    // remover_reg_rrn(bin, rrn, c);
}

//========== Lógica de atualização (UPDATE) ==========
void logica_atualizacao(FILE *bin, Cabecalho *c) {
    char campo_busca[50], valor_busca[50];
    char campo_atualizar[50], novo_valor[50];

    // 1. O usuário diz QUEM quer atualizar
    // Ex: "codEstacao 10"
    scanf("%s %s", campo_busca, valor_busca);

    // 2. O usuário diz O QUE quer mudar
    // Ex: "nomeLinha Azul"
    scanf("%s %s", campo_atualizar, novo_valor);

    // 3. Percorrer o arquivo procurando o(s) registro(s)
    for (int i = 0; i < c->proxRRN; i++) {
        posicionar_rrn(bin, i);
        
        Registro reg;
        // Lemos o registro completo para verificar se ele bate com a busca
        if (ler_registro_binario(bin, &reg)) { 
            
            // 4. Verificação: "Este registro atende ao critério de busca?"
            if (registro_atende_criterio(&reg, campo_busca, valor_busca)) {
                
                // 5. Modificar apenas o campo necessário na struct 'reg' em RAM
                atualizar_campo_na_struct(&reg, campo_atualizar, novo_valor);
                
                // 6. Chamar a função técnica que já fizemos!
                atualizar_reg_rrn(bin, i, c, reg);
                
                registrar_log("UPDATE", i);
            }
        }
    }
}