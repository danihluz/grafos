#include <stdlib.h>
#include "coloracao.h"

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

static int sao_adjacentes(
    GrafoLista *g,
    int u,
    int v
) {
    No *atual = g->adj[u];

    while (atual != NULL) {
        if (atual->destino == v) {
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

void inserir_aresta(
    GrafoLista *g,
    int u,
    int v
) {
    if (g == NULL ||
        u < 0 || u >= g->n ||
        v < 0 || v >= g->n ||
        u == v) {
        return;
    }

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

int grau_vertice(
    GrafoLista *g,
    int v
) {
    if (g == NULL ||
        v < 0 ||
        v >= g->n) {
        return -1;
    }

    int grau = 0;

    No *atual = g->adj[v];

    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }

    return grau;
}


/* =========================
   COLORACAO GULOSA
   ========================= */

static int menor_cor_disponivel(
    GrafoLista *g,
    int vertice,
    int *cor
) {
    int *usada =
        calloc((size_t)g->n, sizeof(int));

    if (usada == NULL) {
        return -1;
    }

    No *atual = g->adj[vertice];

    while (atual != NULL) {
        int vizinho = atual->destino;

        if (cor[vizinho] != -1) {
            usada[cor[vizinho]] = 1;
        }

        atual = atual->prox;
    }

    int escolhida = 0;

    while (escolhida < g->n &&
           usada[escolhida]) {
        escolhida++;
    }

    free(usada);

    return escolhida;
}

int *coloracao_gulosa(
    GrafoLista *g,
    int *num_cores
) {
    if (g == NULL || num_cores == NULL) {
        return NULL;
    }

    int *cor =
        malloc((size_t)g->n * sizeof(int));

    if (cor == NULL) {
        return NULL;
    }

    for (int i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    int maior_cor = -1;

    /*
       Ordem arbitraria:
       0, 1, 2, ..., n-1
    */
    for (int v = 0; v < g->n; v++) {
        cor[v] =
            menor_cor_disponivel(g, v, cor);

        if (cor[v] == -1) {
            free(cor);
            return NULL;
        }

        if (cor[v] > maior_cor) {
            maior_cor = cor[v];
        }
    }

    *num_cores = maior_cor + 1;

    return cor;
}


/* =========================
   WELSH-POWELL
   ========================= */

static void ordenar_por_grau(
    GrafoLista *g,
    int *ordem
) {
    for (int i = 0; i < g->n; i++) {
        ordem[i] = i;
    }

    /*
       Ordenacao simples por grau decrescente.
       Em empate, mantemos o menor vertice antes.
    */
    for (int i = 0; i < g->n - 1; i++) {
        for (int j = i + 1; j < g->n; j++) {

            int grau_i =
                grau_vertice(g, ordem[i]);

            int grau_j =
                grau_vertice(g, ordem[j]);

            if (grau_j > grau_i ||
                (grau_j == grau_i &&
                 ordem[j] < ordem[i])) {

                int temp = ordem[i];
                ordem[i] = ordem[j];
                ordem[j] = temp;
            }
        }
    }
}

int *coloracao_welsh_powell(
    GrafoLista *g,
    int *num_cores
) {
    if (g == NULL || num_cores == NULL) {
        return NULL;
    }

    int *cor =
        malloc((size_t)g->n * sizeof(int));

    int *ordem =
        malloc((size_t)g->n * sizeof(int));

    if (cor == NULL || ordem == NULL) {
        free(cor);
        free(ordem);
        return NULL;
    }

    for (int i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    ordenar_por_grau(g, ordem);

    int maior_cor = -1;

    /*
       Aplicamos a coloracao gulosa,
       mas seguindo a ordem de grau
       decrescente.
    */
    for (int i = 0; i < g->n; i++) {
        int v = ordem[i];

        cor[v] =
            menor_cor_disponivel(g, v, cor);

        if (cor[v] == -1) {
            free(cor);
            free(ordem);
            return NULL;
        }

        if (cor[v] > maior_cor) {
            maior_cor = cor[v];
        }
    }

    *num_cores = maior_cor + 1;

    free(ordem);

    return cor;
}


/* =========================
   BIPARTICAO COM BFS
   ========================= */

int eh_bipartido(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int *cor =
        malloc((size_t)g->n * sizeof(int));

    int *fila =
        malloc((size_t)g->n * sizeof(int));

    if (cor == NULL || fila == NULL) {
        free(cor);
        free(fila);
        return 0;
    }

    for (int i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    /*
       Precisamos testar cada componente,
       pois o grafo pode ser desconexo.
    */
    for (int inicio = 0;
         inicio < g->n;
         inicio++) {

        if (cor[inicio] != -1) {
            continue;
        }

        int frente = 0;
        int fim = 0;

        cor[inicio] = 0;
        fila[fim++] = inicio;

        while (frente < fim) {
            int u = fila[frente++];

            No *atual = g->adj[u];

            while (atual != NULL) {
                int v = atual->destino;

                if (cor[v] == -1) {

                    cor[v] = 1 - cor[u];
                    fila[fim++] = v;

                } else if (cor[v] == cor[u]) {

                    free(cor);
                    free(fila);

                    return 0;
                }

                atual = atual->prox;
            }
        }
    }

    free(cor);
    free(fila);

    return 1;
}