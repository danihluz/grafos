#include <assert.h>
#include <stdio.h>
#include "bellman_ford.h"

#define MAX_VERTICES 10

static void adicionar(GrafoListaPonderado *g, int u, int v, int peso) {
    assert(inserir_aresta_ponderada(g, u, v, peso) == 1);
}

static long long custo_aresta(const GrafoListaPonderado *g, int u, int v) {
    long long menor = LLONG_MAX;
    for (const NoAresta *a = g->adj[u]; a != NULL; a = a->prox) {
        if (a->destino == v && a->peso < menor) {
            menor = a->peso;
        }
    }
    return menor;
}

static void verificar_ciclo(const GrafoListaPonderado *g, const int *pred) {
    int atual = g->vertice_ciclo;
    assert(atual >= 0 && atual < g->n);
    for (int i = 0; i < g->n; i++) {
        atual = pred[atual];
        assert(atual >= 0 && atual < g->n);
    }

    int inicio = atual;
    int tamanho = 0;
    long long custo = 0;
    do {
        int anterior = pred[atual];
        assert(anterior >= 0 && anterior < g->n);
        long long peso = custo_aresta(g, anterior, atual);
        assert(peso != LLONG_MAX);
        custo += peso;
        atual = anterior;
        assert(++tamanho <= g->n);
    } while (atual != inicio);
    assert(custo < 0);
}

static void testar_caminhos(void) {
    GrafoListaPonderado *g = criar_grafo_ponderado(6);
    int dist[6], pred[6];
    const int esperado[] = {0, 2, 7, 4, -2, INFINITO};
    assert(g != NULL);
    adicionar(g, 0, 1, 6);
    adicionar(g, 0, 2, 7);
    adicionar(g, 1, 2, 8);
    adicionar(g, 1, 3, 5);
    adicionar(g, 1, 4, -4);
    adicionar(g, 2, 3, -3);
    adicionar(g, 2, 4, 9);
    adicionar(g, 3, 1, -2);
    adicionar(g, 4, 0, 2);
    adicionar(g, 4, 3, 7);

    assert(bellman_ford(g, 0, dist, pred) == 1);
    for (int v = 0; v < 6; v++) {
        assert(dist[v] == esperado[v]);
    }
    assert(g->vertice_ciclo == -1 && pred[5] == -1);

    assert(bellman_ford(g, 5, dist, pred) == 1);
    for (int v = 0; v < 5; v++) {
        assert(dist[v] == INFINITO && pred[v] == -1);
    }
    assert(dist[5] == 0 && pred[5] == -1);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(5);
    assert(g != NULL);
    for (int v = 4; v > 0; v--) {
        adicionar(g, v, v - 1, -1);
    }
    assert(bellman_ford(g, 4, dist, pred) == 1);
    for (int v = 0; v < 5; v++) {
        assert(dist[v] == v - 4);
    }
    liberar_grafo_ponderado(g);
}

static void testar_ciclos(void) {
    int dist[5], pred[5];
    GrafoListaPonderado *g = criar_grafo_ponderado(5);
    assert(g != NULL);
    adicionar(g, 0, 1, 1);
    adicionar(g, 1, 2, -3);
    adicionar(g, 2, 1, 1);
    adicionar(g, 2, 3, 2);
    adicionar(g, 3, 4, 2);
    assert(bellman_ford(g, 0, dist, pred) == 0);
    verificar_ciclo(g, pred);
    assert(reconstruir_ciclo_negativo(5, g->vertice_ciclo, pred) == 1);

    /* Uma nova origem nao alcanca o ciclo; o estado anterior deve ser limpo. */
    assert(bellman_ford(g, 4, dist, pred) == 1);
    assert(g->vertice_ciclo == -1 && dist[4] == 0);
    for (int v = 0; v < 4; v++) {
        assert(dist[v] == INFINITO && pred[v] == -1);
    }
    liberar_grafo_ponderado(g);

    /* Ciclo direcionado com tres vertices: a impressao nao pode inverte-lo. */
    g = criar_grafo_ponderado(5);
    assert(g != NULL);
    adicionar(g, 0, 1, 0);
    adicionar(g, 1, 2, -3);
    adicionar(g, 2, 3, 1);
    adicionar(g, 3, 1, 1);
    adicionar(g, 3, 4, 0);
    assert(bellman_ford(g, 0, dist, pred) == 0);
    verificar_ciclo(g, pred);
    /* O ponto inicial 4 fica fora do ciclo e exige recuar por pred[]. */
    assert(reconstruir_ciclo_negativo(5, 4, pred) == 1);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(3);
    assert(g != NULL);
    adicionar(g, 0, 1, -2);
    adicionar(g, 1, 2, 2);
    adicionar(g, 2, 0, 0);
    assert(bellman_ford(g, 0, dist, pred) == 1); /* Ciclo de custo zero. */
    assert(dist[0] == 0 && dist[1] == -2 && dist[2] == 0);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(1);
    assert(g != NULL);
    assert(bellman_ford(g, 0, dist, pred) == 1);
    adicionar(g, 0, 0, -1);
    assert(bellman_ford(g, 0, dist, pred) == 0);
    verificar_ciclo(g, pred);
    assert(reconstruir_ciclo_negativo(1, g->vertice_ciclo, pred) == 1);
    liberar_grafo_ponderado(g);
}

static void testar_limites(void) {
    int dist[3], pred[3];
    GrafoListaPonderado *g = criar_grafo_ponderado(3);
    assert(g != NULL);
    adicionar(g, 0, 1, 1200000000);
    adicionar(g, 1, 2, 1200000000);
    adicionar(g, 0, 2, 2000000000);
    assert(bellman_ford(g, 0, dist, pred) == 1);
    assert(dist[1] == 1200000000 && dist[2] == 2000000000);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(3);
    assert(g != NULL);
    adicionar(g, 0, 1, INT_MIN);
    assert(bellman_ford(g, 0, dist, pred) == 1);
    assert(dist[1] == INT_MIN && dist[2] == INFINITO);
    adicionar(g, 1, 2, -1);
    assert(bellman_ford(g, 0, dist, pred) == -1); /* Custo fora de int. */
    adicionar(g, 1, 0, -1);
    assert(bellman_ford(g, 0, dist, pred) == 0); /* Nao pode ocultar o ciclo. */
    verificar_ciclo(g, pred);
    liberar_grafo_ponderado(g);

    g = criar_grafo_ponderado(2);
    assert(g != NULL);
    adicionar(g, 0, 1, INT_MAX);
    assert(bellman_ford(g, 0, dist, pred) == -1); /* INT_MAX e a sentinela. */
    assert(bellman_ford(g, -1, dist, pred) == -1);
    assert(bellman_ford(g, 2, dist, pred) == -1);
    assert(bellman_ford(g, 0, NULL, pred) == -1);
    assert(bellman_ford(g, 0, dist, NULL) == -1);
    assert(!inserir_aresta_ponderada(g, -1, 0, 1));
    assert(!inserir_aresta_ponderada(g, 0, 2, 1));
    liberar_grafo_ponderado(g);

    assert(criar_grafo_ponderado(0) == NULL);
    assert(bellman_ford(NULL, 0, dist, pred) == -1);
    assert(!inserir_aresta_ponderada(NULL, 0, 0, 1));
    liberar_grafo_ponderado(NULL);
    int sem_ciclo[] = {-1, 0, 1};
    int invalido[] = {3, 0, 1};
    assert(!reconstruir_ciclo_negativo(3, 2, sem_ciclo));
    assert(!reconstruir_ciclo_negativo(3, 2, invalido));
    assert(!reconstruir_ciclo_negativo(0, 0, sem_ciclo));
    assert(!reconstruir_ciclo_negativo(3, 3, sem_ciclo));
    assert(!reconstruir_ciclo_negativo(3, -1, sem_ciclo));
    assert(!reconstruir_ciclo_negativo(3, 0, NULL));
}

static unsigned int aleatorio(void) {
    static unsigned int estado = 2026;
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

static void testar_contra_floyd_warshall(void) {
    const long long infinito = LLONG_MAX / 4;
    for (int caso = 0; caso < 250; caso++) {
        int n = 1 + caso % MAX_VERTICES;
        long long esperado[MAX_VERTICES][MAX_VERTICES];
        GrafoListaPonderado *g = criar_grafo_ponderado(n);
        assert(g != NULL);
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                esperado[u][v] = u == v ? 0 : infinito;
            }
        }

        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                if (aleatorio() % 4 == 0) {
                    int peso = (int)(aleatorio() % 31) - 10;
                    adicionar(g, u, v, peso);
                    adicionar(g, u, v, peso + 5); /* Arestas paralelas. */
                    if (peso < esperado[u][v]) {
                        esperado[u][v] = peso;
                    }
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (int u = 0; u < n; u++) {
                for (int v = 0; v < n; v++) {
                    if (esperado[u][k] != infinito && esperado[k][v] != infinito &&
                        esperado[u][k] + esperado[k][v] < esperado[u][v]) {
                        esperado[u][v] = esperado[u][k] + esperado[k][v];
                    }
                }
            }
        }

        for (int origem = 0; origem < n; origem++) {
            int dist[MAX_VERTICES], pred[MAX_VERTICES];
            int tem_ciclo = 0;
            for (int v = 0; v < n; v++) {
                if (esperado[origem][v] != infinito && esperado[v][v] < 0) {
                    tem_ciclo = 1;
                }
            }
            assert(bellman_ford(g, origem, dist, pred) == !tem_ciclo);
            if (tem_ciclo) {
                verificar_ciclo(g, pred);
                continue;
            }

            assert(g->vertice_ciclo == -1 && pred[origem] == -1);
            for (int destino = 0; destino < n; destino++) {
                if (esperado[origem][destino] == infinito) {
                    assert(dist[destino] == INFINITO && pred[destino] == -1);
                    continue;
                }
                assert(dist[destino] == esperado[origem][destino]);
                int atual = destino;
                int passos = 0;
                while (atual != origem) {
                    assert(passos++ < n);
                    int anterior = pred[atual];
                    assert(anterior >= 0 && anterior < n);
                    long long peso = custo_aresta(g, anterior, atual);
                    assert(peso != LLONG_MAX);
                    assert(dist[atual] == dist[anterior] + peso);
                    atual = anterior;
                }
            }
        }
        liberar_grafo_ponderado(g);
    }
}

int main(void) {
    testar_caminhos();
    testar_ciclos();
    testar_limites();
    testar_contra_floyd_warshall();
    puts("Todos os testes de Bellman-Ford passaram.");
    return 0;
}
