#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

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

    g->adj = calloc(n, sizeof(No *));

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

    No *novo = malloc(sizeof(No));

    if (novo == NULL) {
        return;
    }

    novo->destino = destino;
    novo->prox = g->adj[origem];

    g->adj[origem] = novo;
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
   ALGORITMO DE KAHN
   ========================= */

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho) {
    if (g == NULL || tamanho == NULL) {
        return NULL;
    }

    *tamanho = 0;

    int *grau_entrada = calloc(g->n, sizeof(int));
    int *fila = malloc(g->n * sizeof(int));
    int *ordem = malloc(g->n * sizeof(int));

    if (grau_entrada == NULL || fila == NULL || ordem == NULL) {
        free(grau_entrada);
        free(fila);
        free(ordem);
        return NULL;
    }

    /* Calcula o grau de entrada de cada vertice */
    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];

        while (atual != NULL) {
            grau_entrada[atual->destino]++;
            atual = atual->prox;
        }
    }

    int inicio = 0;
    int fim = 0;

    /* Coloca na fila os vertices com grau de entrada zero */
    for (int i = 0; i < g->n; i++) {
        if (grau_entrada[i] == 0) {
            fila[fim++] = i;
        }
    }

    int quantidade = 0;

    while (inicio < fim) {
        int vertice = fila[inicio++];

        ordem[quantidade++] = vertice;

        No *atual = g->adj[vertice];

        while (atual != NULL) {
            int vizinho = atual->destino;

            grau_entrada[vizinho]--;

            if (grau_entrada[vizinho] == 0) {
                fila[fim++] = vizinho;
            }

            atual = atual->prox;
        }
    }

    free(grau_entrada);
    free(fila);

    /*
       Se nem todos os vertices foram processados,
       existe ciclo no grafo.
    */
    if (quantidade != g->n) {
        free(ordem);
        return NULL;
    }

    *tamanho = quantidade;

    return ordem;
}


/* =========================
   ORDENACAO TOPOLOGICA DFS
   ========================= */

/*
    Estados:
    0 = nao visitado
    1 = sendo visitado
    2 = finalizado
*/

static int dfs_topologica(
    GrafoLista *g,
    int vertice,
    int *estado,
    int *ordem,
    int *posicao
) {
    estado[vertice] = 1;

    No *atual = g->adj[vertice];

    while (atual != NULL) {
        int vizinho = atual->destino;

        /*
           Encontrou um vertice que ainda esta
           sendo visitado: existe ciclo.
        */
        if (estado[vizinho] == 1) {
            return 0;
        }

        if (estado[vizinho] == 0) {
            if (!dfs_topologica(
                    g,
                    vizinho,
                    estado,
                    ordem,
                    posicao)) {
                return 0;
            }
        }

        atual = atual->prox;
    }

    estado[vertice] = 2;

    /*
       O vertice e colocado da direita para
       a esquerda depois que sua DFS termina.
    */
    ordem[*posicao] = vertice;
    (*posicao)--;

    return 1;
}

int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho) {
    if (g == NULL || tamanho == NULL) {
        return NULL;
    }

    *tamanho = 0;

    int *estado = calloc(g->n, sizeof(int));
    int *ordem = malloc(g->n * sizeof(int));

    if (estado == NULL || ordem == NULL) {
        free(estado);
        free(ordem);
        return NULL;
    }

    int posicao = g->n - 1;

    for (int i = 0; i < g->n; i++) {
        if (estado[i] == 0) {
            if (!dfs_topologica(
                    g,
                    i,
                    estado,
                    ordem,
                    &posicao)) {

                free(estado);
                free(ordem);

                return NULL;
            }
        }
    }

    free(estado);

    *tamanho = g->n;

    return ordem;
}


/* =========================
   VERIFICAR SE E DAG
   ========================= */

int eh_dag(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int tamanho;

    int *ordem = ordenacao_topologica_kahn(g, &tamanho);

    if (ordem == NULL) {
        return 0;
    }

    free(ordem);

    return 1;
}