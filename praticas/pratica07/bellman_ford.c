#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bellman_ford.h"

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoListaPonderado *g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }

    g->adj = calloc((size_t)n, sizeof(*g->adj));
    if (g->adj == NULL) {
        free(g);
        return NULL;
    }

    g->n = n;
    g->vertice_ciclo = -1;
    return g;
}

int inserir_aresta_ponderada(
    GrafoListaPonderado *g, int origem, int destino, int peso
) {
    if (g == NULL || origem < 0 || origem >= g->n ||
        destino < 0 || destino >= g->n) {
        return 0;
    }

    NoAresta *nova = malloc(sizeof(*nova));
    if (nova == NULL) {
        return 0;
    }

    nova->destino = destino;
    nova->peso = peso;
    nova->prox = g->adj[origem];
    g->adj[origem] = nova;
    return 1;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g) {
    if (g == NULL) {
        return;
    }

    for (int u = 0; u < g->n; u++) {
        NoAresta *atual = g->adj[u];
        while (atual != NULL) {
            NoAresta *proxima = atual->prox;
            free(atual);
            atual = proxima;
        }
    }

    free(g->adj);
    free(g);
}

static int relaxar_arestas(
    const GrafoListaPonderado *g, const long long *dist,
    long long *proximas, int *pred
) {
    int ultimo = -1;
    memcpy(proximas, dist, (size_t)g->n * sizeof(*dist));

    for (int u = 0; u < g->n; u++) {
        if (dist[u] == LLONG_MAX) {
            continue;
        }

        for (const NoAresta *a = g->adj[u]; a != NULL; a = a->prox) {
            long long candidato = dist[u] + a->peso;
            if (candidato < proximas[a->destino]) {
                proximas[a->destino] = candidato;
                pred[a->destino] = u;
                ultimo = a->destino;
            }
        }
    }

    return ultimo;
}

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (g == NULL || g->adj == NULL || g->n <= 0 ||
        origem < 0 || origem >= g->n || dist == NULL || pred == NULL) {
        return -1;
    }

    g->vertice_ciclo = -1;
    for (int v = 0; v < g->n; v++) {
        dist[v] = INFINITO;
        pred[v] = -1;
    }

    long long *atuais = malloc((size_t)g->n * sizeof(*atuais));
    long long *proximas = malloc((size_t)g->n * sizeof(*proximas));
    if (atuais == NULL || proximas == NULL) {
        free(atuais);
        free(proximas);
        return -1;
    }

    for (int v = 0; v < g->n; v++) {
        atuais[v] = LLONG_MAX;
    }
    atuais[origem] = 0;

    /*
     * Nas primeiras n-1 passagens, calcula os menores caminhos.
     * Uma melhora na n-esima passagem indica um ciclo negativo alcancavel.
     * Cada passagem usa a anterior: os custos envolvem no maximo n arestas,
     * e a soma em long long evita overflow mesmo com pesos do tipo int.
     */
    for (int passagem = 0; passagem < g->n; passagem++) {
        int ultimo = relaxar_arestas(g, atuais, proximas, pred);
        long long *temporario = atuais;
        atuais = proximas;
        proximas = temporario;

        if (ultimo == -1) {
            break;
        }
        if (passagem == g->n - 1) {
            g->vertice_ciclo = ultimo;
        }
    }

    int resultado = g->vertice_ciclo == -1 ? 1 : 0;
    for (int v = 0; v < g->n; v++) {
        if (atuais[v] == LLONG_MAX) {
            continue;
        }
        if (atuais[v] >= INFINITO || atuais[v] < INT_MIN) {
            if (resultado == 1) {
                resultado = -1;
            }
            /* Conversao limitada; em caso de ciclo, dist nao e definitivo. */
            dist[v] = atuais[v] < INT_MIN ? INT_MIN : INFINITO;
        } else {
            dist[v] = (int)atuais[v];
        }
    }

    free(atuais);
    free(proximas);
    return resultado;
}

int reconstruir_ciclo_negativo(int n, int vertice, const int *pred) {
    if (n <= 0 || vertice < 0 || vertice >= n || pred == NULL) {
        return 0;
    }

    /* O vertice relaxado pode estar depois do ciclo. Recuar n vezes
     * garante que o ponto de partida fique dentro dele.
     */
    int inicio = vertice;
    for (int i = 0; i < n; i++) {
        inicio = pred[inicio];
        if (inicio < 0 || inicio >= n) {
            return 0;
        }
    }

    int *ciclo = malloc(((size_t)n + 1) * sizeof(*ciclo));
    if (ciclo == NULL) {
        return 0;
    }

    int tamanho = 0;
    int atual = inicio;
    do {
        if (atual < 0 || atual >= n || tamanho >= n) {
            free(ciclo);
            return 0;
        }
        ciclo[tamanho++] = atual;
        atual = pred[atual];
    } while (atual != inicio);
    ciclo[tamanho] = inicio;

    printf("Ciclo negativo: ");
    for (int i = tamanho; i >= 0; i--) {
        printf("%d", ciclo[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");

    free(ciclo);
    return 1;
}
