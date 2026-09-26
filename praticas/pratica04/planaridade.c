#include <stdio.h>
#include "planaridade.h"

/* =========================
   CONTAGEM DE ARESTAS
   ========================= */

int contar_arestas(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int total = 0;

    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];

        while (atual != NULL) {
            total++;
            atual = atual->prox;
        }
    }

    /*
       Como o grafo e nao orientado,
       cada aresta aparece duas vezes.
    */
    return total / 2;
}


/* =========================
   TESTE DE EULER
   ========================= */

int eh_planar_euler(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int n = g->n;
    int m = contar_arestas(g);

    /*
       Para grafos simples com n >= 3,
       um grafo planar deve satisfazer:
       m <= 3n - 6
    */

    if (n < 3) {
        return 1;
    }

    return m <= 3 * n - 6;
}


/* =========================
   DETECCAO DE K5
   ========================= */

int contem_k5(GrafoLista *g) {
    if (g == NULL || g->n < 5) {
        return 0;
    }

    for (int a = 0; a < g->n - 4; a++) {
        for (int b = a + 1; b < g->n - 3; b++) {
            for (int c = b + 1; c < g->n - 2; c++) {
                for (int d = c + 1; d < g->n - 1; d++) {
                    for (int e = d + 1; e < g->n; e++) {

                        int vertices[5] = {
                            a, b, c, d, e
                        };

                        int completo = 1;

                        for (int i = 0; i < 5; i++) {
                            for (int j = i + 1; j < 5; j++) {

                                if (!sao_adjacentes(
                                        g,
                                        vertices[i],
                                        vertices[j])) {

                                    completo = 0;
                                    break;
                                }
                            }

                            if (!completo) {
                                break;
                            }
                        }

                        if (completo) {
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}


/* =========================
   DETECCAO DE K3,3
   ========================= */

int contem_k33(GrafoLista *g) {
    if (g == NULL || g->n < 6) {
        return 0;
    }

    for (int a = 0; a < g->n - 5; a++) {
        for (int b = a + 1; b < g->n - 4; b++) {
            for (int c = b + 1; c < g->n - 3; c++) {
                for (int d = c + 1; d < g->n - 2; d++) {
                    for (int e = d + 1; e < g->n - 1; e++) {
                        for (int f = e + 1; f < g->n; f++) {

                            int seis[6] = {
                                a, b, c, d, e, f
                            };

                            /*
                               Escolhemos 3 dos 6 para
                               formar o primeiro lado.
                            */
                            for (int i = 0; i < 4; i++) {
                                for (int j = i + 1; j < 5; j++) {
                                    for (int k = j + 1; k < 6; k++) {

                                        int lado1[3] = {
                                            seis[i],
                                            seis[j],
                                            seis[k]
                                        };

                                        int lado2[3];
                                        int pos = 0;

                                        for (int x = 0; x < 6; x++) {
                                            if (x != i &&
                                                x != j &&
                                                x != k) {

                                                lado2[pos++] =
                                                    seis[x];
                                            }
                                        }

                                        int valido = 1;

                                        /*
                                           Em K3,3, cada vertice
                                           de um lado deve estar
                                           conectado aos 3 vertices
                                           do outro lado.
                                        */
                                        for (int x = 0; x < 3; x++) {
                                            for (int y = 0; y < 3; y++) {

                                                if (!sao_adjacentes(
                                                        g,
                                                        lado1[x],
                                                        lado2[y])) {

                                                    valido = 0;
                                                    break;
                                                }
                                            }

                                            if (!valido) {
                                                break;
                                            }
                                        }

                                        if (valido) {
                                            return 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}


/* =========================
   HEURISTICA DE PLANARIDADE
   ========================= */

int eh_planar_heuristica(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    /*
       Primeiro aplicamos a condicao necessaria
       derivada de Euler.
    */
    if (!eh_planar_euler(g)) {
        return 0;
    }

    /*
       Para grafos pequenos, procuramos
       configuracoes K5 e K3,3.
    */
    if (g->n <= 10) {

        if (contem_k5(g)) {
            return 0;
        }

        if (contem_k33(g)) {
            return 0;
        }
    }

    /*
       Como o teste e heuristico, 1 significa
       que nenhum impedimento foi encontrado.
    */
    return 1;
}