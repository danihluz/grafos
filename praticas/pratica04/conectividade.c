#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"

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

int sao_adjacentes(GrafoLista *g, int u, int v) {
    if (g == NULL ||
        u < 0 || u >= g->n ||
        v < 0 || v >= g->n) {
        return 0;
    }

    No *atual = g->adj[u];

    while (atual != NULL) {
        if (atual->destino == v) {
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

void inserir_aresta(GrafoLista *g, int u, int v) {
    if (g == NULL ||
        u < 0 || u >= g->n ||
        v < 0 || v >= g->n ||
        u == v) {
        return;
    }

    /* Evita arestas duplicadas */
    if (sao_adjacentes(g, u, v)) {
        return;
    }

    No *novo_u = malloc(sizeof(No));
    No *novo_v = malloc(sizeof(No));

    if (novo_u == NULL || novo_v == NULL) {
        free(novo_u);
        free(novo_v);
        return;
    }

    novo_u->destino = v;
    novo_u->prox = g->adj[u];
    g->adj[u] = novo_u;

    novo_v->destino = u;
    novo_v->prox = g->adj[v];
    g->adj[v] = novo_v;
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
   TARJAN - ARTICULACOES
   ========================= */

void dfs_articulacoes(
    GrafoLista *g,
    int u,
    int *visitado,
    int *descoberta,
    int *low,
    int *pai,
    int *articulacao,
    int *tempo
) {
    visitado[u] = 1;

    descoberta[u] = ++(*tempo);
    low[u] = descoberta[u];

    int filhos = 0;

    No *atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {
            filhos++;
            pai[v] = u;

            dfs_articulacoes(
                g,
                v,
                visitado,
                descoberta,
                low,
                pai,
                articulacao,
                tempo
            );

            if (low[v] < low[u]) {
                low[u] = low[v];
            }

            /*
               Caso 1:
               u e raiz da DFS e possui mais de um filho.
            */
            if (pai[u] == -1 && filhos > 1) {
                articulacao[u] = 1;
            }

            /*
               Caso 2:
               u nao e raiz e o filho v nao consegue
               voltar para um ancestral de u.
            */
            if (pai[u] != -1 &&
                low[v] >= descoberta[u]) {
                articulacao[u] = 1;
            }

        } else if (v != pai[u]) {

            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        }

        atual = atual->prox;
    }
}

void detectar_articulacoes(GrafoLista *g) {
    if (g == NULL) {
        return;
    }

    int *visitado = calloc((size_t)g->n, sizeof(int));
    int *descoberta = calloc((size_t)g->n, sizeof(int));
    int *low = calloc((size_t)g->n, sizeof(int));
    int *pai = malloc((size_t)g->n * sizeof(int));
    int *articulacao = calloc((size_t)g->n, sizeof(int));

    if (visitado == NULL ||
        descoberta == NULL ||
        low == NULL ||
        pai == NULL ||
        articulacao == NULL) {

        free(visitado);
        free(descoberta);
        free(low);
        free(pai);
        free(articulacao);
        return;
    }

    for (int i = 0; i < g->n; i++) {
        pai[i] = -1;
    }

    int tempo = 0;

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            dfs_articulacoes(
                g,
                i,
                visitado,
                descoberta,
                low,
                pai,
                articulacao,
                &tempo
            );
        }
    }

    printf("Vertices de corte: ");

    int encontrou = 0;

    for (int i = 0; i < g->n; i++) {
        if (articulacao[i]) {
            printf("%d ", i);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("nenhum");
    }

    printf("\n");

    free(visitado);
    free(descoberta);
    free(low);
    free(pai);
    free(articulacao);
}


/* =========================
   TARJAN - PONTES
   ========================= */

static void dfs_pontes(
    GrafoLista *g,
    int u,
    int *visitado,
    int *descoberta,
    int *low,
    int pai,
    int *tempo
) {
    visitado[u] = 1;

    descoberta[u] = ++(*tempo);
    low[u] = descoberta[u];

    No *atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {

            dfs_pontes(
                g,
                v,
                visitado,
                descoberta,
                low,
                u,
                tempo
            );

            if (low[v] < low[u]) {
                low[u] = low[v];
            }

            /*
               Condicao de ponte:
               low[v] > descoberta[u]
            */
            if (low[v] > descoberta[u]) {
                printf("Ponte: %d - %d\n", u, v);
            }

        } else if (v != pai) {

            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        }

        atual = atual->prox;
    }
}

void detectar_pontes(GrafoLista *g) {
    if (g == NULL) {
        return;
    }

    int *visitado = calloc((size_t)g->n, sizeof(int));
    int *descoberta = calloc((size_t)g->n, sizeof(int));
    int *low = calloc((size_t)g->n, sizeof(int));

    if (visitado == NULL ||
        descoberta == NULL ||
        low == NULL) {

        free(visitado);
        free(descoberta);
        free(low);
        return;
    }

    int tempo = 0;

    printf("Pontes encontradas:\n");

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            dfs_pontes(
                g,
                i,
                visitado,
                descoberta,
                low,
                -1,
                &tempo
            );
        }
    }

    free(visitado);
    free(descoberta);
    free(low);
}