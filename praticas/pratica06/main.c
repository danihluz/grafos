#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

int main(void) {

    GrafoListaPonderado *g =
        criar_grafo_ponderado(6);

    if (g == NULL) {
        printf("Erro ao criar grafo.\n");
        return 1;
    }

    /*
        Grafo nao direcionado ilustrado em grafo.dot.
        A partir de 0, as distancias esperadas sao: 0, 3, 2, 8, 10, 13.
        Caminho minimo ate 5: 0 -> 2 -> 1 -> 3 -> 4 -> 5 (custo 13).
    */

    inserir_aresta_ponderada(g, 0, 1, 4);
    inserir_aresta_ponderada(g, 0, 2, 2);
    inserir_aresta_ponderada(g, 1, 2, 1);
    inserir_aresta_ponderada(g, 1, 3, 5);
    inserir_aresta_ponderada(g, 2, 3, 8);
    inserir_aresta_ponderada(g, 2, 4, 10);
    inserir_aresta_ponderada(g, 3, 4, 2);
    inserir_aresta_ponderada(g, 4, 5, 3);

    int *dist =
        malloc((size_t)g->n * sizeof(int));

    int *pred =
        malloc((size_t)g->n * sizeof(int));

    if (dist == NULL || pred == NULL) {
        free(dist);
        free(pred);
        liberar_grafo_ponderado(g);
        return 1;
    }

    int origem = 0;

    dijkstra(
        g,
        origem,
        dist,
        pred
    );

    printf(
        "=== DIJKSTRA A PARTIR DO VERTICE %d ===\n",
        origem
    );

    for (int i = 0; i < g->n; i++) {

        if (dist[i] == INFINITO) {
            printf(
                "Vertice %d: distancia = INF\n",
                i
            );
        } else {
            printf(
                "Vertice %d: distancia = %d, predecessor = %d\n",
                i,
                dist[i],
                pred[i]
            );
        }
    }

    printf("\n");

    reconstruir_caminho(
        origem,
        5,
        pred
    );

    printf(
        "Custo total: %d\n",
        dist[5]
    );

    free(dist);
    free(pred);

    liberar_grafo_ponderado(g);

    return 0;
}
