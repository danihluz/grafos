#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

typedef struct {
    int origem;
    int destino;
    int peso;
} ArestaExemplo;

static int executar_exemplo(
    const char *titulo, int n, const ArestaExemplo *arestas, size_t quantidade
) {
    GrafoListaPonderado *g = criar_grafo_ponderado(n);
    int *dist = malloc((size_t)n * sizeof(*dist));
    int *pred = malloc((size_t)n * sizeof(*pred));
    int sucesso = 0;

    if (g == NULL || dist == NULL || pred == NULL) {
        fprintf(stderr, "Erro ao alocar memoria.\n");
        goto finalizar;
    }

    for (size_t i = 0; i < quantidade; i++) {
        if (!inserir_aresta_ponderada(
                g, arestas[i].origem, arestas[i].destino, arestas[i].peso)) {
            fprintf(stderr, "Erro ao inserir aresta.\n");
            goto finalizar;
        }
    }

    printf("=== %s ===\n", titulo);
    int resultado = bellman_ford(g, 0, dist, pred);
    if (resultado < 0) {
        fprintf(stderr, "Erro: entrada, memoria ou custo fora do intervalo de int.\n");
        goto finalizar;
    }

    if (resultado == 0) {
        printf("Existe ciclo negativo alcancavel a partir de 0.\n");
        if (!reconstruir_ciclo_negativo(n, g->vertice_ciclo, pred)) {
            fprintf(stderr, "Erro ao reconstruir o ciclo.\n");
            goto finalizar;
        }
    } else {
        printf("Nao existe ciclo negativo alcancavel a partir de 0.\n");
        for (int v = 0; v < n; v++) {
            if (dist[v] == INFINITO) {
                printf("Vertice %d: distancia = INF, predecessor = -1\n", v);
            } else {
                printf("Vertice %d: distancia = %d, predecessor = %d\n",
                       v, dist[v], pred[v]);
            }
        }
    }
    sucesso = 1;

finalizar:
    free(dist);
    free(pred);
    liberar_grafo_ponderado(g);
    return sucesso;
}

int main(void) {
    /* Pesos negativos, sem ciclo negativo: distancias 0, 2, 7, 4, -2. */
    const ArestaExemplo sem_ciclo[] = {
        {0, 1, 6}, {0, 2, 7}, {1, 2, 8}, {1, 3, 5}, {1, 4, -4},
        {2, 3, -3}, {2, 4, 9}, {3, 1, -2}, {4, 0, 2}, {4, 3, 7}
    };
    /* Ciclo 1 -> 2 -> 1, custo -4; o vertice 3 esta depois do ciclo. */
    const ArestaExemplo com_ciclo[] = {
        {0, 1, 1}, {1, 2, -2}, {2, 1, -2}, {2, 3, 2}
    };

    if (!executar_exemplo(
            "PESOS NEGATIVOS SEM CICLO NEGATIVO", 5, sem_ciclo,
            sizeof(sem_ciclo) / sizeof(sem_ciclo[0]))) {
        return EXIT_FAILURE;
    }
    printf("\n");
    if (!executar_exemplo(
            "CICLO NEGATIVO ALCANCAVEL", 4, com_ciclo,
            sizeof(com_ciclo) / sizeof(com_ciclo[0]))) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
