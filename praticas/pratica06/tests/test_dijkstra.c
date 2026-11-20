#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dijkstra.h"

#define MAX_VERTICES 12

static void verificar_heap(const MinHeap *heap) {
    for (int i = 0; i < heap->tamanho; i++) {
        assert(heap->posicao[heap->dados[i].vertice] == i);
        if (i > 0) {
            assert(heap->dados[(i - 1) / 2].distancia <=
                   heap->dados[i].distancia);
        }
    }
}

static void testar_heap(void) {
    const int chaves[] = {20, 7, 18, 9, 10, 13};
    const int ordem[] = {3, 0, 1, 4, 5, 2};
    const int distancias[] = {1, 5, 7, 10, 13, 18};
    MinHeap *heap = criar_min_heap(6);
    assert(heap != NULL);
    assert(criar_min_heap(0) == NULL);
    assert(extrair_minimo(NULL).vertice == -1);
    assert(extrair_minimo(heap).vertice == -1);

    inserir_heap(heap, -1, 0);
    inserir_heap(heap, 6, 0);
    assert(heap->tamanho == 0);

    for (int i = 0; i < 6; i++) {
        inserir_heap(heap, i, chaves[i]);
        inserir_heap(heap, i, 0); /* Duplicatas nao alteram o mapeamento. */
        assert(heap->tamanho == i + 1);
        verificar_heap(heap);
    }

    diminuir_chave(heap, 0, 5);
    diminuir_chave(heap, 3, 1);
    diminuir_chave(heap, 3, 100); /* Aumentos devem ser ignorados. */
    diminuir_chave(heap, -1, 0);
    diminuir_chave(heap, 6, 0);
    verificar_heap(heap);

    for (int i = 0; i < 6; i++) {
        NoHeap minimo = extrair_minimo(heap);
        assert(minimo.vertice == ordem[i]);
        assert(minimo.distancia == distancias[i]);
        assert(heap->posicao[minimo.vertice] == -1);
        diminuir_chave(heap, minimo.vertice, 0);
        verificar_heap(heap);
    }

    assert(extrair_minimo(heap).vertice == -1);
    inserir_heap(heap, 3, 42);
    assert(extrair_minimo(heap).distancia == 42);
    liberar_min_heap(heap);
    liberar_min_heap(NULL);
}

static void testar_custos_altos(void) {
    GrafoListaPonderado *g = criar_grafo_ponderado(5);
    int dist[5], pred[5];
    assert(g != NULL);

    inserir_aresta_ponderada(g, 0, 1, 1200000000);
    inserir_aresta_ponderada(g, 1, 2, 1200000000);
    inserir_aresta_ponderada(g, 0, 2, 2000000000);
    inserir_aresta_ponderada(g, 0, 3, INT_MAX - 1);
    inserir_aresta_ponderada(g, 3, 4, 2);
    inserir_aresta_ponderada(g, 0, 4, -1);
    inserir_aresta_ponderada(g, -1, 4, 1);
    inserir_aresta_ponderada(g, 0, 5, 1);

    dijkstra(g, 0, dist, pred);
    assert(dist[0] == 0 && pred[0] == -1);
    assert(dist[1] == 1200000000 && pred[1] == 0);
    assert(dist[2] == 2000000000 && pred[2] == 0);
    assert(dist[3] == INT_MAX - 1 && pred[3] == 0);
    assert(dist[4] == INFINITO && pred[4] == -1);
    liberar_grafo_ponderado(g);
}

static unsigned int proximo_aleatorio(void) {
    static unsigned int estado = 12345;
    estado = estado * 1664525u + 1013904223u;
    return estado;
}

static void testar_contra_floyd_warshall(void) {
    for (int caso = 0; caso < 150; caso++) {
        int n = 1 + caso % MAX_VERTICES;
        int custos[MAX_VERTICES][MAX_VERTICES];
        int esperado[MAX_VERTICES][MAX_VERTICES];
        GrafoListaPonderado *g = criar_grafo_ponderado(n);
        assert(g != NULL);

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                custos[i][j] = i == j ? 0 : INFINITO;
            }
        }

        for (int i = 0; i < n; i++) {
            inserir_aresta_ponderada(g, i, i, 0);
            for (int j = i + 1; j < n; j++) {
                if (caso % 10 != 0 && proximo_aleatorio() % 4 == 0) {
                    int peso = (int)(proximo_aleatorio() % 61);
                    custos[i][j] = custos[j][i] = peso;
                    inserir_aresta_ponderada(g, i, j, peso);
                    inserir_aresta_ponderada(g, i, j, peso + 7);
                }
            }
        }

        memcpy(esperado, custos, sizeof(esperado));
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    long long custo = (long long)esperado[i][k] + esperado[k][j];
                    if (custo < esperado[i][j]) {
                        esperado[i][j] = (int)custo;
                    }
                }
            }
        }

        for (int origem = 0; origem < n; origem++) {
            int dist[MAX_VERTICES], pred[MAX_VERTICES];
            dijkstra(g, origem, dist, pred);
            assert(pred[origem] == -1);

            for (int destino = 0; destino < n; destino++) {
                assert(dist[destino] == esperado[origem][destino]);
                if (dist[destino] == INFINITO) {
                    assert(pred[destino] == -1);
                    continue;
                }

                int atual = destino;
                int passos = 0;
                while (atual != origem) {
                    assert(passos++ < n);
                    int anterior = pred[atual];
                    assert(anterior >= 0 && anterior < n);
                    assert(custos[anterior][atual] != INFINITO);
                    assert(dist[atual] == dist[anterior] + custos[anterior][atual]);
                    atual = anterior;
                }
            }
        }

        liberar_grafo_ponderado(g);
    }
}

static void testar_caminho_longo(int imprimir) {
    const int n = 1501;
    GrafoListaPonderado *g = criar_grafo_ponderado(n);
    int *dist = malloc((size_t)n * sizeof(int));
    int *pred = malloc((size_t)n * sizeof(int));
    assert(g != NULL && dist != NULL && pred != NULL);

    for (int i = 1; i < n; i++) {
        inserir_aresta_ponderada(g, i - 1, i, 1);
    }
    dijkstra(g, 0, dist, pred);
    for (int i = 0; i < n; i++) {
        assert(dist[i] == i);
        assert(pred[i] == i - 1);
    }
    if (imprimir) {
        reconstruir_caminho(0, n - 1, pred);
    }

    free(dist);
    free(pred);
    liberar_grafo_ponderado(g);
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--caminho-longo") == 0) {
        testar_caminho_longo(1);
        return 0;
    }

    testar_heap();
    testar_custos_altos();
    testar_contra_floyd_warshall();
    testar_caminho_longo(0);

    int pred[] = {-1, 0, 1, -1};
    reconstruir_caminho(0, 2, pred);
    reconstruir_caminho(0, 0, pred);
    reconstruir_caminho(0, 3, pred);
    puts("Todos os testes passaram.");
    return 0;
}
