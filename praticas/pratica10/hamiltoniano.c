#include <stdint.h>
#include <stdlib.h>
#include "hamiltoniano.h"

static int validar_grafo(int n, const int *adj) {
    if (n <= 0 || adj == NULL || (size_t)n > SIZE_MAX / sizeof(int) / (size_t)n) {
        return 0;
    }
    size_t quantidade = (size_t)n * (size_t)n;
    for (size_t i = 0; i < quantidade; i++) {
        if (adj[i] != 0 && adj[i] != 1) {
            return 0;
        }
    }
    return 1;
}

static int buscar(
    int n, const int *adj, int *rota, unsigned char *visitados, int posicao, int fechar
) {
    if (posicao == n) {
        return !fechar || adj[(size_t)rota[n - 1] * (size_t)n + (size_t)rota[0]];
    }
    int anterior = rota[posicao - 1];
    for (int v = 0; v < n; v++) {
        if (!visitados[v] && adj[(size_t)anterior * (size_t)n + (size_t)v]) {
            rota[posicao] = v;
            visitados[v] = 1;
            if (buscar(n, adj, rota, visitados, posicao + 1, fechar)) {
                return 1;
            }
            visitados[v] = 0;
        }
    }
    return 0;
}

int encontrar_caminho_hamiltoniano_bt(int n, const int *adj, int *caminho) {
    if (n <= 0 || !validar_grafo(n, adj) || caminho == NULL) {
        return -1;
    }
    for (int v = 0; v < n; v++) {
        caminho[v] = -1;
    }
    unsigned char *visitados = calloc((size_t)n, sizeof(*visitados));
    if (visitados == NULL) {
        return -1;
    }

    for (int origem = 0; origem < n; origem++) {
        caminho[0] = origem;
        visitados[origem] = 1;
        if (buscar(n, adj, caminho, visitados, 1, 0)) {
            free(visitados);
            return 1;
        }
        visitados[origem] = 0;
    }

    for (int v = 0; v < n; v++) {
        caminho[v] = -1;
    }
    free(visitados);
    return 0;
}

int encontrar_ciclo_hamiltoniano_bt(int n, const int *adj, int origem, int *ciclo) {
    if (n <= 0 || !validar_grafo(n, adj) || origem < 0 || origem >= n || ciclo == NULL) {
        return -1;
    }
    for (size_t i = 0; i <= (size_t)n; i++) {
        ciclo[i] = -1;
    }
    unsigned char *visitados = calloc((size_t)n, sizeof(*visitados));
    if (visitados == NULL) {
        return -1;
    }
    ciclo[0] = origem;
    visitados[origem] = 1;
    int encontrado = buscar(n, adj, ciclo, visitados, 1, 1);
    if (encontrado) {
        ciclo[n] = origem;
    } else {
        for (size_t i = 0; i <= (size_t)n; i++) {
            ciclo[i] = -1;
        }
    }
    free(visitados);
    return encontrado;
}
