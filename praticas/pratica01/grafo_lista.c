#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"

static int vertice_valido(GrafoLista *grafo, int vertice) {
    return grafo != NULL && vertice >= 0 && vertice < grafo->n;
}

static No *criar_no(int destino) {
    No *novo = malloc(sizeof(No));

    if (novo == NULL) {
        return NULL;
    }

    novo->destino = destino;
    novo->prox = NULL;

    return novo;
}

static int existe_na_lista(No *inicio, int destino) {
    No *atual = inicio;

    while (atual != NULL) {
        if (atual->destino == destino) {
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

static void inserir_na_lista(No **inicio, int destino) {
    if (existe_na_lista(*inicio, destino)) {
        return;
    }

    No *novo = criar_no(destino);

    if (novo == NULL) {
        return;
    }

    novo->prox = *inicio;
    *inicio = novo;
}

static void remover_da_lista(No **inicio, int destino) {
    No *atual = *inicio;
    No *anterior = NULL;

    while (atual != NULL) {
        if (atual->destino == destino) {

            if (anterior == NULL) {
                *inicio = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }

            free(atual);
            return;
        }

        anterior = atual;
        atual = atual->prox;
    }
}

GrafoLista *criar_grafo_lista(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoLista *grafo = malloc(sizeof(GrafoLista));

    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;

    grafo->adj = calloc(n, sizeof(No *));

    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    return grafo;
}

void inserir_aresta_lista(GrafoLista *grafo, int origem, int destino) {
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return;
    }

    inserir_na_lista(&grafo->adj[origem], destino);
    inserir_na_lista(&grafo->adj[destino], origem);
}

void remover_aresta_lista(GrafoLista *grafo, int origem, int destino) {
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return;
    }

    remover_da_lista(&grafo->adj[origem], destino);
    remover_da_lista(&grafo->adj[destino], origem);
}

int grau_lista(GrafoLista *grafo, int vertice) {
    if (!vertice_valido(grafo, vertice)) {
        return -1;
    }

    int grau = 0;

    No *atual = grafo->adj[vertice];

    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }

    return grau;
}

int sao_adjacentes_lista(GrafoLista *grafo, int origem, int destino) {
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return 0;
    }

    return existe_na_lista(grafo->adj[origem], destino);
}

void liberar_grafo_lista(GrafoLista *grafo) {
    if (grafo == NULL) {
        return;
    }

    for (int i = 0; i < grafo->n; i++) {
        No *atual = grafo->adj[i];

        while (atual != NULL) {
            No *proximo = atual->prox;

            free(atual);

            atual = proximo;
        }
    }

    free(grafo->adj);
    free(grafo);
}