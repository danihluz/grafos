#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tps.h"

static int validar_pesos(int n, const int *pesos, int origem, const int *rota) {
    if (n <= 0 || pesos == NULL || rota == NULL || origem < 0 || origem >= n ||
        (size_t)n > SIZE_MAX / sizeof(int) / (size_t)n) {
        return 0;
    }
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            int peso = pesos[(size_t)u * (size_t)n + (size_t)v];
            if (peso < 0 || (u == v && peso != 0)) {
                return 0;
            }
        }
    }
    return 1;
}

typedef struct {
    int n, origem;
    const int *pesos;
    int *atual, *melhor_rota;
    long long melhor_custo;
} BuscaTSP;

static void trocar(int *a, int *b) {
    int temporario = *a;
    *a = *b;
    *b = temporario;
}

static void permutar(BuscaTSP *busca, int posicao, long long custo) {
    int anterior = busca->atual[posicao - 1];
    if (posicao == busca->n) {
        long long total = custo + busca->pesos[
            (size_t)anterior * (size_t)busca->n + (size_t)busca->origem];
        if (total < busca->melhor_custo) {
            busca->melhor_custo = total;
            memcpy(busca->melhor_rota, busca->atual,
                   (size_t)busca->n * sizeof(*busca->atual));
            busca->melhor_rota[busca->n] = busca->origem;
        }
        return;
    }

    /* Sem poda: enumera todas as (n-1)! permutacoes com a origem fixa. */
    for (int i = posicao; i < busca->n; i++) {
        trocar(&busca->atual[posicao], &busca->atual[i]);
        int proximo = busca->atual[posicao];
        long long peso = busca->pesos[(size_t)anterior * (size_t)busca->n +
                                     (size_t)proximo];
        permutar(busca, posicao + 1, custo + peso);
        trocar(&busca->atual[posicao], &busca->atual[i]);
    }
}

long long tsp_forca_bruta(int n, const int *pesos, int origem, int *rota) {
    if (n <= 0 || !validar_pesos(n, pesos, origem, rota)) {
        return -1;
    }
    int *atual = malloc((size_t)n * sizeof(*atual));
    if (atual == NULL) {
        return -1;
    }
    atual[0] = origem;
    int posicao = 1;
    for (int v = 0; v < n; v++) {
        if (v != origem) {
            atual[posicao++] = v;
        }
    }
    BuscaTSP busca = {n, origem, pesos, atual, rota, LLONG_MAX};
    permutar(&busca, 1, 0);
    free(atual);
    return busca.melhor_custo;
}

long long tsp_vizinho_mais_proximo(int n, const int *pesos, int origem, int *rota) {
    if (n <= 0 || !validar_pesos(n, pesos, origem, rota)) {
        return -1;
    }
    unsigned char *visitados = calloc((size_t)n, sizeof(*visitados));
    if (visitados == NULL) {
        return -1;
    }
    rota[0] = origem;
    visitados[origem] = 1;
    long long custo = 0;
    for (int posicao = 1; posicao < n; posicao++) {
        int anterior = rota[posicao - 1];
        int escolhido = -1;
        for (int v = 0; v < n; v++) {
            if (!visitados[v] && (escolhido == -1 ||
                pesos[(size_t)anterior * (size_t)n + (size_t)v] <
                pesos[(size_t)anterior * (size_t)n + (size_t)escolhido])) {
                escolhido = v;
            }
        }
        rota[posicao] = escolhido;
        visitados[escolhido] = 1;
        custo += pesos[(size_t)anterior * (size_t)n + (size_t)escolhido];
    }
    rota[n] = origem;
    custo += pesos[(size_t)rota[n - 1] * (size_t)n + (size_t)origem];
    free(visitados);
    return custo;
}

long long tsp_insercao(int n, const int *pesos, int origem, int *rota) {
    if (n <= 0 || !validar_pesos(n, pesos, origem, rota)) {
        return -1;
    }
    unsigned char *visitados = calloc((size_t)n, sizeof(*visitados));
    if (visitados == NULL) {
        return -1;
    }
    rota[0] = rota[1] = origem;
    visitados[origem] = 1;
    long long custo = 0;

    for (int tamanho = 1; tamanho < n; tamanho++) {
        int escolhido = -1;
        long long proximidade = LLONG_MAX;
        /* Escolhe o vertice externo mais proximo de algum vertice do ciclo. */
        for (int v = 0; v < n; v++) {
            if (visitados[v]) {
                continue;
            }
            for (int i = 0; i < tamanho; i++) {
                int distancia = pesos[(size_t)rota[i] * (size_t)n + (size_t)v];
                if (distancia < proximidade) {
                    proximidade = distancia;
                    escolhido = v;
                }
            }
        }

        int posicao = 0;
        long long menor_aumento = LLONG_MAX;
        /* Insere esse vertice onde houver o menor aumento do custo. */
        for (int i = 0; i < tamanho; i++) {
            int u = rota[i];
            int v = rota[i + 1];
            long long aumento = (long long)pesos[(size_t)u * (size_t)n + (size_t)escolhido] +
                pesos[(size_t)escolhido * (size_t)n + (size_t)v] -
                pesos[(size_t)u * (size_t)n + (size_t)v];
            if (aumento < menor_aumento) {
                menor_aumento = aumento;
                posicao = i;
            }
        }
        memmove(rota + posicao + 2, rota + posicao + 1,
                (size_t)(tamanho - posicao) * sizeof(*rota));
        rota[posicao + 1] = escolhido;
        visitados[escolhido] = 1;
        custo += menor_aumento;
    }
    free(visitados);
    return custo;
}

int grafo_para_tsp(int n, const int *adj, int *pesos) {
    if (n <= 0 || adj == NULL || pesos == NULL ||
        (size_t)n > SIZE_MAX / sizeof(int) / (size_t)n) {
        return 0;
    }
    size_t quantidade = (size_t)n * (size_t)n;
    for (size_t i = 0; i < quantidade; i++) {
        if (adj[i] != 0 && adj[i] != 1) {
            return 0;
        }
    }
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            size_t indice = (size_t)u * (size_t)n + (size_t)v;
            pesos[indice] = u == v ? 0 : (adj[indice] ? 1 : 2);
        }
    }
    return 1;
}
