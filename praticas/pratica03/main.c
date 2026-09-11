#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

static void imprimir_ordenacao(int *ordem, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d", ordem[i]);

        if (i < tamanho - 1) {
            printf(" -> ");
        }
    }

    printf("\n");
}

int main(void) {

    printf("=== TESTE COM DAG ===\n");

    GrafoLista *g = criar_grafo_lista(6);

    if (g == NULL) {
        printf("Erro ao criar o grafo.\n");
        return 1;
    }

    /*
        Grafo:

        5 -> 2
        5 -> 0
        4 -> 0
        4 -> 1
        2 -> 3
        3 -> 1
    */

    inserir_aresta(g, 5, 2);
    inserir_aresta(g, 5, 0);
    inserir_aresta(g, 4, 0);
    inserir_aresta(g, 4, 1);
    inserir_aresta(g, 2, 3);
    inserir_aresta(g, 3, 1);

    printf("O grafo eh DAG? %s\n",
           eh_dag(g) ? "Sim" : "Nao");

    int tamanho = 0;

    int *ordem_kahn =
        ordenacao_topologica_kahn(g, &tamanho);

    if (ordem_kahn != NULL) {
        printf("Ordenacao topologica - Kahn: ");
        imprimir_ordenacao(ordem_kahn, tamanho);

        free(ordem_kahn);
    } else {
        printf("Kahn: o grafo possui ciclo.\n");
    }

    int *ordem_dfs =
        ordenacao_topologica_dfs(g, &tamanho);

    if (ordem_dfs != NULL) {
        printf("Ordenacao topologica - DFS: ");
        imprimir_ordenacao(ordem_dfs, tamanho);

        free(ordem_dfs);
    } else {
        printf("DFS: o grafo possui ciclo.\n");
    }

    liberar_grafo(g);


    printf("\n=== TESTE COM CICLO ===\n");

    GrafoLista *g_ciclo = criar_grafo_lista(3);

    if (g_ciclo == NULL) {
        printf("Erro ao criar o grafo.\n");
        return 1;
    }

    /*
        Ciclo:

        0 -> 1 -> 2 -> 0
    */

    inserir_aresta(g_ciclo, 0, 1);
    inserir_aresta(g_ciclo, 1, 2);
    inserir_aresta(g_ciclo, 2, 0);

    printf("O grafo eh DAG? %s\n",
           eh_dag(g_ciclo) ? "Sim" : "Nao");

    ordem_kahn =
        ordenacao_topologica_kahn(g_ciclo, &tamanho);

    if (ordem_kahn == NULL) {
        printf("Kahn: ciclo detectado. Ordenacao impossivel.\n");
    } else {
        imprimir_ordenacao(ordem_kahn, tamanho);
        free(ordem_kahn);
    }

    ordem_dfs =
        ordenacao_topologica_dfs(g_ciclo, &tamanho);

    if (ordem_dfs == NULL) {
        printf("DFS: ciclo detectado. Ordenacao impossivel.\n");
    } else {
        imprimir_ordenacao(ordem_dfs, tamanho);
        free(ordem_dfs);
    }

    liberar_grafo(g_ciclo);

    return 0;
}