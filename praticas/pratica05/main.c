#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

static void imprimir_cores(
    int *cores,
    int n
) {
    for (int i = 0; i < n; i++) {
        printf(
            "Vertice %d -> Cor %d\n",
            i,
            cores[i] + 1
        );
    }
}

int main(void) {

    printf("=== TESTE DE COLORACAO ===\n");

    GrafoLista *g =
        criar_grafo_lista(6);

    if (g == NULL) {
        printf("Erro ao criar grafo.\n");
        return 1;
    }

    /*
        Grafo de teste:

        0 ----- 1
        | \     |
        |  \    |
        3 ----- 2

        3 ----- 4 ----- 5
    */

    inserir_aresta(g, 0, 1);
    inserir_aresta(g, 0, 2);
    inserir_aresta(g, 0, 3);

    inserir_aresta(g, 1, 2);

    inserir_aresta(g, 2, 3);

    inserir_aresta(g, 3, 4);
    inserir_aresta(g, 4, 5);


    int num_cores_gulosa = 0;

    int *cores_gulosa =
        coloracao_gulosa(
            g,
            &num_cores_gulosa
        );

    if (cores_gulosa != NULL) {

        printf("\nColoracao gulosa:\n");

        imprimir_cores(
            cores_gulosa,
            g->n
        );

        printf(
            "Numero de cores utilizadas: %d\n",
            num_cores_gulosa
        );

        free(cores_gulosa);
    }


    int num_cores_wp = 0;

    int *cores_wp =
        coloracao_welsh_powell(
            g,
            &num_cores_wp
        );

    if (cores_wp != NULL) {

        printf(
            "\nColoracao Welsh-Powell:\n"
        );

        imprimir_cores(
            cores_wp,
            g->n
        );

        printf(
            "Numero de cores utilizadas: %d\n",
            num_cores_wp
        );

        free(cores_wp);
    }


    printf(
        "\nO grafo eh bipartido? %s\n",
        eh_bipartido(g) ? "Sim" : "Nao"
    );

    liberar_grafo(g);


    /* =====================
       TESTE BIPARTIDO
       ===================== */

    printf(
        "\n=== TESTE COM GRAFO BIPARTIDO ===\n"
    );

    GrafoLista *bipartido =
        criar_grafo_lista(4);

    if (bipartido == NULL) {
        return 1;
    }

    /*
        Ciclo de tamanho 4:

        0 --- 1
        |     |
        3 --- 2
    */

    inserir_aresta(bipartido, 0, 1);
    inserir_aresta(bipartido, 1, 2);
    inserir_aresta(bipartido, 2, 3);
    inserir_aresta(bipartido, 3, 0);

    printf(
        "O grafo eh bipartido? %s\n",
        eh_bipartido(bipartido)
            ? "Sim"
            : "Nao"
    );

    liberar_grafo(bipartido);

    return 0;
}