#include <stdlib.h>
#include <string.h>
#include "cobertura.h"

static int validar_entrada(int n, const Aresta *arestas, int m, const int *cobertura) {
    if (n < 0 || m < 0 || (n > 0 && cobertura == NULL) ||
        (m > 0 && arestas == NULL)) {
        return 0;
    }
    for (int i = 0; i < m; i++) {
        if (arestas[i].u < 0 || arestas[i].u >= n ||
            arestas[i].v < 0 || arestas[i].v >= n) {
            return 0;
        }
    }
    return 1;
}

int eh_cobertura(int n, const Aresta *arestas, int m, const int *cobertura) {
    if (!validar_entrada(n, arestas, m, cobertura)) {
        return 0;
    }
    for (int v = 0; v < n; v++) {
        if (cobertura[v] != 0 && cobertura[v] != 1) {
            return 0;
        }
    }
    for (int i = 0; i < m; i++) {
        if (!cobertura[arestas[i].u] && !cobertura[arestas[i].v]) {
            return 0;
        }
    }
    return 1;
}

int cobertura_gulosa(int n, const Aresta *arestas, int m, int *cobertura) {
    if (!validar_entrada(n, arestas, m, cobertura)) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }

    memset(cobertura, 0, (size_t)n * sizeof(*cobertura));
    if (m == 0) {
        return 0;
    }
    int *grau = calloc((size_t)n, sizeof(*grau));
    if (grau == NULL) {
        return -1;
    }

    int tamanho = 0;
    while (1) {
        memset(grau, 0, (size_t)n * sizeof(*grau));
        for (int i = 0; i < m; i++) {
            int u = arestas[i].u;
            int v = arestas[i].v;
            if (!cobertura[u] && !cobertura[v]) {
                grau[u]++;
                if (u != v) {
                    grau[v]++;
                }
            }
        }

        int escolhido = -1;
        for (int v = 0; v < n; v++) {
            /* Em empate, escolhe o menor indice para ser deterministico. */
            if (grau[v] > 0 && (escolhido == -1 || grau[v] > grau[escolhido])) {
                escolhido = v;
            }
        }
        if (escolhido == -1) {
            break;
        }
        cobertura[escolhido] = 1;
        tamanho++;
    }

    free(grau);
    return tamanho;
}

typedef struct {
    int n, m;
    const Aresta *arestas;
    int *atual;
    int *melhor_cobertura;
    unsigned char *usados;
    int melhor_tamanho;
} Busca;

static int limite_inferior(Busca *busca) {
    /* Cada aresta de um emparelhamento disjunto exige mais um vertice.
     * So considera arestas que ainda nao foram cobertas.
     */
    memset(busca->usados, 0, (size_t)busca->n * sizeof(*busca->usados));
    int limite = 0;
    for (int i = 0; i < busca->m; i++) {
        int u = busca->arestas[i].u;
        int v = busca->arestas[i].v;
        if (!busca->atual[u] && !busca->atual[v] &&
            !busca->usados[u] && !busca->usados[v]) {
            busca->usados[u] = busca->usados[v] = 1;
            limite++;
        }
    }
    return limite;
}

static void buscar(Busca *busca, int tamanho) {
    if (tamanho >= busca->melhor_tamanho) {
        return;
    }

    int descoberta = -1;
    for (int i = 0; i < busca->m; i++) {
        if (!busca->atual[busca->arestas[i].u] &&
            !busca->atual[busca->arestas[i].v]) {
            descoberta = i;
            break;
        }
    }
    if (descoberta == -1) {
        busca->melhor_tamanho = tamanho;
        memcpy(busca->melhor_cobertura, busca->atual,
               (size_t)busca->n * sizeof(*busca->atual));
        return;
    }
    if (limite_inferior(busca) >= busca->melhor_tamanho - tamanho) {
        return;
    }

    /* Toda cobertura deve conter ao menos uma ponta da aresta descoberta. */
    int u = busca->arestas[descoberta].u;
    int v = busca->arestas[descoberta].v;
    busca->atual[u] = 1;
    buscar(busca, tamanho + 1);
    busca->atual[u] = 0;

    if (u != v) {
        busca->atual[v] = 1;
        buscar(busca, tamanho + 1);
        busca->atual[v] = 0;
    }
}

int cobertura_minima_bt(int n, const Aresta *arestas, int m, int *cobertura) {
    /* A heuristica fornece uma cobertura valida como limite superior. */
    int melhor = cobertura_gulosa(n, arestas, m, cobertura);
    if (melhor <= 0) {
        return melhor;
    }

    Busca busca = {
        n, m, arestas, calloc((size_t)n, sizeof(int)), cobertura,
        calloc((size_t)n, sizeof(unsigned char)), melhor
    };
    if (busca.atual == NULL || busca.usados == NULL) {
        free(busca.atual);
        free(busca.usados);
        return -1;
    }

    buscar(&busca, 0);
    free(busca.atual);
    free(busca.usados);
    return busca.melhor_tamanho;
}
