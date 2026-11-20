#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

/* =========================
   GRAFO PONDERADO
   ========================= */

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoListaPonderado *g =
        malloc(sizeof(GrafoListaPonderado));

    if (g == NULL) {
        return NULL;
    }

    g->n = n;

    g->adj =
        calloc((size_t)n, sizeof(NoAresta *));

    if (g->adj == NULL) {
        free(g);
        return NULL;
    }

    return g;
}

void inserir_aresta_ponderada(
    GrafoListaPonderado *g,
    int origem,
    int destino,
    int peso
) {
    if (g == NULL ||
        origem < 0 || origem >= g->n ||
        destino < 0 || destino >= g->n ||
        peso < 0) {
        return;
    }

    NoAresta *novo1 =
        malloc(sizeof(NoAresta));
    NoAresta *novo2 =
        malloc(sizeof(NoAresta));

    if (novo1 == NULL || novo2 == NULL) {
        free(novo1);
        free(novo2);
        return;
    }

    novo1->destino = destino;
    novo1->peso = peso;
    novo1->prox = g->adj[origem];
    g->adj[origem] = novo1;

    novo2->destino = origem;
    novo2->peso = peso;
    novo2->prox = g->adj[destino];
    g->adj[destino] = novo2;
}

void liberar_grafo_ponderado(
    GrafoListaPonderado *g
) {
    if (g == NULL) {
        return;
    }

    for (int i = 0; i < g->n; i++) {
        NoAresta *atual = g->adj[i];

        while (atual != NULL) {
            NoAresta *prox = atual->prox;
            free(atual);
            atual = prox;
        }
    }

    free(g->adj);
    free(g);
}


/* =========================
   MIN-HEAP
   ========================= */

static void trocar(
    MinHeap *heap,
    int i,
    int j
) {
    NoHeap temp = heap->dados[i];
    heap->dados[i] = heap->dados[j];
    heap->dados[j] = temp;

    heap->posicao[heap->dados[i].vertice] = i;
    heap->posicao[heap->dados[j].vertice] = j;
}

MinHeap *criar_min_heap(int capacidade) {
    if (capacidade <= 0) {
        return NULL;
    }

    MinHeap *heap = malloc(sizeof(MinHeap));

    if (heap == NULL) {
        return NULL;
    }

    heap->dados =
        malloc((size_t)capacidade * sizeof(NoHeap));

    heap->posicao =
        malloc((size_t)capacidade * sizeof(int));

    if (heap->dados == NULL ||
        heap->posicao == NULL) {

        free(heap->dados);
        free(heap->posicao);
        free(heap);

        return NULL;
    }

    heap->capacidade = capacidade;
    heap->tamanho = 0;

    for (int i = 0; i < capacidade; i++) {
        heap->posicao[i] = -1;
    }

    return heap;
}

void inserir_heap(
    MinHeap *heap,
    int vertice,
    int distancia
) {
    if (heap == NULL ||
        vertice < 0 || vertice >= heap->capacidade ||
        heap->tamanho >= heap->capacidade ||
        heap->posicao[vertice] != -1) {
        return;
    }

    int i = heap->tamanho;

    heap->dados[i].vertice = vertice;
    heap->dados[i].distancia = distancia;

    heap->posicao[vertice] = i;

    heap->tamanho++;

    while (i > 0) {
        int pai = (i - 1) / 2;

        if (heap->dados[pai].distancia
            <= heap->dados[i].distancia) {
            break;
        }

        trocar(heap, i, pai);

        i = pai;
    }
}

static void heapify(
    MinHeap *heap,
    int i
) {
    while (1) {
        int menor = i;

        int esquerda = 2 * i + 1;
        int direita = 2 * i + 2;

        if (esquerda < heap->tamanho &&
            heap->dados[esquerda].distancia <
            heap->dados[menor].distancia) {

            menor = esquerda;
        }

        if (direita < heap->tamanho &&
            heap->dados[direita].distancia <
            heap->dados[menor].distancia) {

            menor = direita;
        }

        if (menor == i) {
            break;
        }

        trocar(heap, i, menor);

        i = menor;
    }
}

NoHeap extrair_minimo(MinHeap *heap) {
    NoHeap invalido;

    invalido.vertice = -1;
    invalido.distancia = INFINITO;

    if (heap == NULL ||
        heap->tamanho == 0) {

        return invalido;
    }

    NoHeap minimo = heap->dados[0];

    heap->posicao[minimo.vertice] = -1;

    heap->tamanho--;

    if (heap->tamanho > 0) {
        heap->dados[0] =
            heap->dados[heap->tamanho];

        heap->posicao[
            heap->dados[0].vertice
        ] = 0;

        heapify(heap, 0);
    }

    return minimo;
}

void diminuir_chave(
    MinHeap *heap,
    int vertice,
    int nova_distancia
) {
    if (heap == NULL ||
        vertice < 0 ||
        vertice >= heap->capacidade) {
        return;
    }

    int i = heap->posicao[vertice];

    if (i < 0 ||
        i >= heap->tamanho) {
        return;
    }

    if (nova_distancia >
        heap->dados[i].distancia) {
        return;
    }

    heap->dados[i].distancia =
        nova_distancia;

    while (i > 0) {
        int pai = (i - 1) / 2;

        if (heap->dados[pai].distancia
            <= heap->dados[i].distancia) {
            break;
        }

        trocar(heap, i, pai);

        i = pai;
    }
}

void liberar_min_heap(MinHeap *heap) {
    if (heap == NULL) {
        return;
    }

    free(heap->dados);
    free(heap->posicao);
    free(heap);
}


/* =========================
   DIJKSTRA
   ========================= */

void dijkstra(
    GrafoListaPonderado *g,
    int origem,
    int *dist,
    int *pred
) {
    if (g == NULL ||
        dist == NULL ||
        pred == NULL ||
        origem < 0 ||
        origem >= g->n) {
        return;
    }

    for (int i = 0; i < g->n; i++) {
        dist[i] = INFINITO;
        pred[i] = -1;
    }

    MinHeap *heap =
        criar_min_heap(g->n);

    if (heap == NULL) {
        return;
    }

    for (int i = 0; i < g->n; i++) {
        inserir_heap(
            heap,
            i,
            INFINITO
        );
    }

    dist[origem] = 0;

    diminuir_chave(
        heap,
        origem,
        0
    );

    while (heap->tamanho > 0) {
        NoHeap minimo =
            extrair_minimo(heap);

        int u = minimo.vertice;

        if (u == -1) {
            break;
        }

        if (minimo.distancia == INFINITO) {
            break;
        }

        NoAresta *atual = g->adj[u];

        while (atual != NULL) {
            int v = atual->destino;
            int peso = atual->peso;

            /* Evita overflow ao somar custos altos. */
            if (heap->posicao[v] != -1 &&
                peso <= INFINITO - dist[u] &&
                dist[u] + peso < dist[v]) {

                dist[v] =
                    dist[u] + peso;

                pred[v] = u;

                diminuir_chave(
                    heap,
                    v,
                    dist[v]
                );
            }

            atual = atual->prox;
        }
    }

    liberar_min_heap(heap);
}


/* =========================
   RECONSTRUCAO DO CAMINHO
   ========================= */

void reconstruir_caminho(
    int origem,
    int destino,
    int *pred
) {
    if (pred == NULL || origem < 0 || destino < 0) {
        return;
    }

    size_t tamanho = 1;
    int atual = destino;

    while (atual >= 0 && atual != origem) {
        tamanho++;
        atual = pred[atual];
    }

    if (atual < 0) {
        printf(
            "Nao existe caminho entre %d e %d.\n",
            origem,
            destino
        );

        return;
    }

    int *caminho = malloc(tamanho * sizeof(int));

    if (caminho == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para o caminho.\n");
        return;
    }

    atual = destino;
    for (size_t i = 0; i < tamanho; i++) {
        caminho[i] = atual;
        if (atual != origem) {
            atual = pred[atual];
        }
    }

    printf(
        "Caminho minimo de %d ate %d: ",
        origem,
        destino
    );

    for (size_t i = tamanho; i > 0; i--) {
        printf("%d", caminho[i - 1]);

        if (i > 1) {
            printf(" -> ");
        }
    }

    printf("\n");
    free(caminho);
}
