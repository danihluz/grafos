#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"
#include "busca_profundidade.h"

int main(void) {
    printf("=== TESTE BFS E DFS ===\n");

    GrafoLista *g = criar_grafo_lista(7);

    if (g == NULL) {
        printf("Erro ao criar o grafo.\n");
        return 1;
    }

    /*
        Componente 1:

        0 --- 1
        |     |
        2 --- 3

        Componente 2:

        4 --- 5 --- 6
    */

    inserir_aresta(g, 0, 1);
    inserir_aresta(g, 0, 2);
    inserir_aresta(g, 1, 3);
    inserir_aresta(g, 2, 3);

    inserir_aresta(g, 4, 5);
    inserir_aresta(g, 5, 6);


    /* =====================
       BFS
       ===================== */

    int *dist = malloc((size_t)g->n * sizeof(int));
    int *pred = malloc((size_t)g->n * sizeof(int));

    if (dist == NULL || pred == NULL) {
        free(dist);
        free(pred);
        liberar_grafo(g);

        return 1;
    }

    bfs(g, 0, dist, pred);

    printf("\nBFS a partir do vertice 0:\n");

    for (int i = 0; i < g->n; i++) {
        printf(
            "Vertice %d: distancia = %d, predecessor = %d\n",
            i,
            dist[i],
            pred[i]
        );
    }


    /* =====================
       DFS
       ===================== */

    int *visitado =
        calloc((size_t)g->n, sizeof(int));

    int *entrada =
        calloc((size_t)g->n, sizeof(int));

    int *saida =
        calloc((size_t)g->n, sizeof(int));

    if (visitado == NULL ||
        entrada == NULL ||
        saida == NULL) {

        free(dist);
        free(pred);
        free(visitado);
        free(entrada);
        free(saida);
        liberar_grafo(g);

        return 1;
    }

    int tempo = 0;

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            dfs_recursiva(
                g,
                i,
                visitado,
                entrada,
                saida,
                &tempo
            );
        }
    }

    printf("\nDFS - tempos de entrada e saida:\n");

    for (int i = 0; i < g->n; i++) {
        printf(
            "Vertice %d: entrada = %d, saida = %d\n",
            i,
            entrada[i],
            saida[i]
        );
    }


    /* =====================
       OUTRAS ANALISES
       ===================== */

    printf(
        "\nNumero de componentes conexos: %d\n",
        contar_componentes(g)
    );

    printf(
        "O grafo possui ciclo? %s\n",
        tem_ciclo(g) ? "Sim" : "Nao"
    );

    printf(
        "O grafo eh bipartido? %s\n",
        eh_bipartido(g) ? "Sim" : "Nao"
    );


    free(dist);
    free(pred);
    free(visitado);
    free(entrada);
    free(saida);

    liberar_grafo(g);


    /* =====================
       TESTE NAO BIPARTIDO
       ===================== */

    printf("\n=== TESTE COM TRIANGULO ===\n");

    GrafoLista *triangulo =
        criar_grafo_lista(3);

    if (triangulo == NULL) {
        return 1;
    }

    inserir_aresta(triangulo, 0, 1);
    inserir_aresta(triangulo, 1, 2);
    inserir_aresta(triangulo, 2, 0);

    printf(
        "O grafo possui ciclo? %s\n",
        tem_ciclo(triangulo) ? "Sim" : "Nao"
    );

    printf(
        "O grafo eh bipartido? %s\n",
        eh_bipartido(triangulo) ? "Sim" : "Nao"
    );

    liberar_grafo(triangulo);

    return 0;
}