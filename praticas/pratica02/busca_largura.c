#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"

/* =========================
   FUNCOES BASICAS DO GRAFO
   ========================= */

GrafoLista *criar_grafo_lista(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoLista *g = malloc(sizeof(GrafoLista));

    if (g == NULL) {
        return NULL;
    }

    g->n = n;
    g->adj = calloc((size_t)n, sizeof(No *));

    if (g->adj == NULL) {
        free(g);
        return NULL;
    }

    return g;
}

void inserir_aresta(GrafoLista *g, int origem, int destino) {
    if (g == NULL ||
        origem < 0 || origem >= g->n ||
        destino < 0 || destino >= g->n) {
        return;
    }

    No *novo_origem = malloc(sizeof(No));
    No *novo_destino = malloc(sizeof(No));

    if (novo_origem == NULL || novo_destino == NULL) {
        free(novo_origem);
        free(novo_destino);
        return;
    }

    /* origem -> destino */
    novo_origem->destino = destino;
    novo_origem->prox = g->adj[origem];
    g->adj[origem] = novo_origem;

    /* destino -> origem */
    novo_destino->destino = origem;
    novo_destino->prox = g->adj[destino];
    g->adj[destino] = novo_destino;
}

void liberar_grafo(GrafoLista *g) {
    if (g == NULL) {
        return;
    }

    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];

        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }

    free(g->adj);
    free(g);
}


/* =========================
   FILA
   ========================= */

static Fila *criar_fila(int capacidade) {
    Fila *fila = malloc(sizeof(Fila));

    if (fila == NULL) {
        return NULL;
    }

    fila->dados = malloc((size_t)capacidade * sizeof(int));

    if (fila->dados == NULL) {
        free(fila);
        return NULL;
    }

    fila->capacidade = capacidade;
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;

    return fila;
}

static int fila_vazia(Fila *fila) {
    return fila->tamanho == 0;
}

static void enfileirar(Fila *fila, int valor) {
    if (fila == NULL || fila->tamanho == fila->capacidade) {
        return;
    }

    fila->dados[fila->fim] = valor;

    fila->fim =
        (fila->fim + 1) % fila->capacidade;

    fila->tamanho++;
}

static int desenfileirar(Fila *fila) {
    if (fila == NULL || fila_vazia(fila)) {
        return -1;
    }

    int valor = fila->dados[fila->inicio];

    fila->inicio =
        (fila->inicio + 1) % fila->capacidade;

    fila->tamanho--;

    return valor;
}

static void liberar_fila(Fila *fila) {
    if (fila == NULL) {
        return;
    }

    free(fila->dados);
    free(fila);
}


/* =========================
   BFS
   ========================= */

void bfs(GrafoLista *g, int origem, int *dist, int *pred) {
    if (g == NULL ||
        dist == NULL ||
        pred == NULL ||
        origem < 0 ||
        origem >= g->n) {
        return;
    }

    for (int i = 0; i < g->n; i++) {
        dist[i] = -1;
        pred[i] = -1;
    }

    Fila *fila = criar_fila(g->n);

    if (fila == NULL) {
        return;
    }

    dist[origem] = 0;
    enfileirar(fila, origem);

    while (!fila_vazia(fila)) {
        int u = desenfileirar(fila);

        No *atual = g->adj[u];

        while (atual != NULL) {
            int v = atual->destino;

            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                pred[v] = u;

                enfileirar(fila, v);
            }

            atual = atual->prox;
        }
    }

    liberar_fila(fila);
}


/* =========================
   BIPARTICAO
   ========================= */

int eh_bipartido(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int *cor = malloc((size_t)g->n * sizeof(int));

    if (cor == NULL) {
        return 0;
    }

    for (int i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    Fila *fila = criar_fila(g->n);

    if (fila == NULL) {
        free(cor);
        return 0;
    }

    /*
       Precisamos iniciar uma BFS para cada componente,
       pois o grafo pode ser desconexo.
    */
    for (int inicio = 0; inicio < g->n; inicio++) {
        if (cor[inicio] != -1) {
            continue;
        }

        cor[inicio] = 0;
        enfileirar(fila, inicio);

        while (!fila_vazia(fila)) {
            int u = desenfileirar(fila);

            No *atual = g->adj[u];

            while (atual != NULL) {
                int v = atual->destino;

                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    enfileirar(fila, v);
                } else if (cor[v] == cor[u]) {
                    liberar_fila(fila);
                    free(cor);

                    return 0;
                }

                atual = atual->prox;
            }
        }
    }

    liberar_fila(fila);
    free(cor);

    return 1;
}