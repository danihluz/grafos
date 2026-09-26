#include <stdio.h>
#include "conectividade.h"
#include "planaridade.h"

int main(void) {

    printf("=== CONECTIVIDADE ===\n");

    GrafoLista *g = criar_grafo_lista(5);

    if (g == NULL) {
        printf("Erro ao criar grafo.\n");
        return 1;
    }

    /*
        Grafo:

        0 --- 1 --- 3 --- 4
         \   /
           2

        0, 1 e 2 formam um ciclo.
        1-3 e 3-4 sao pontes.
        1 e 3 sao vertices de corte.
    */

    inserir_aresta(g, 0, 1);
    inserir_aresta(g, 1, 2);
    inserir_aresta(g, 2, 0);

    inserir_aresta(g, 1, 3);
    inserir_aresta(g, 3, 4);

    detectar_articulacoes(g);
    detectar_pontes(g);

    printf("\nNumero de arestas: %d\n",
           contar_arestas(g));

    printf(
        "Passa no teste de Euler? %s\n",
        eh_planar_euler(g) ? "Sim" : "Nao"
    );

    printf(
        "Heuristica indica planar? %s\n",
        eh_planar_heuristica(g) ? "Sim" : "Nao"
    );

    liberar_grafo(g);


    /* =========================
       TESTE COM K5
       ========================= */

    printf("\n=== TESTE COM K5 ===\n");

    GrafoLista *k5 = criar_grafo_lista(5);

    if (k5 == NULL) {
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            inserir_aresta(k5, i, j);
        }
    }

    printf(
        "Contem K5? %s\n",
        contem_k5(k5) ? "Sim" : "Nao"
    );

    printf(
        "Passa no teste de Euler? %s\n",
        eh_planar_euler(k5) ? "Sim" : "Nao"
    );

    printf(
        "Heuristica indica planar? %s\n",
        eh_planar_heuristica(k5) ? "Sim" : "Nao"
    );

    liberar_grafo(k5);


    /* =========================
       TESTE COM K3,3
       ========================= */

    printf("\n=== TESTE COM K3,3 ===\n");

    GrafoLista *k33 = criar_grafo_lista(6);

    if (k33 == NULL) {
        return 1;
    }

    /*
       Lado 1: 0, 1, 2
       Lado 2: 3, 4, 5
    */

    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            inserir_aresta(k33, i, j);
        }
    }

    printf(
        "Contem K3,3? %s\n",
        contem_k33(k33) ? "Sim" : "Nao"
    );

    printf(
        "Passa no teste de Euler? %s\n",
        eh_planar_euler(k33) ? "Sim" : "Nao"
    );

    printf(
        "Heuristica indica planar? %s\n",
        eh_planar_heuristica(k33) ? "Sim" : "Nao"
    );

    liberar_grafo(k33);

    return 0;
}