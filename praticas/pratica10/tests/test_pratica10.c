#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "hamiltoniano.h"
#include "tps.h"

#define MAX_N 8
#define ESTADOS (1u << MAX_N)

/* Referencias por programacao dinamica em subconjuntos, sem permutacoes. */
static int referencia_hamiltoniano(int n, const int *adj, int origem, int fechar) {
    unsigned char dp[ESTADOS][MAX_N] = {{0}};
    for (int v = 0; v < n; v++) {
        if (origem == -1 || origem == v) {
            dp[1u << v][v] = 1;
        }
    }
    unsigned int total = 1u << n;
    for (unsigned int mascara = 1; mascara < total; mascara++) {
        for (int u = 0; u < n; u++) {
            if (!dp[mascara][u]) {
                continue;
            }
            for (int v = 0; v < n; v++) {
                if (!(mascara & (1u << v)) && adj[u * n + v]) {
                    dp[mascara | (1u << v)][v] = 1;
                }
            }
        }
    }
    for (int v = 0; v < n; v++) {
        if (dp[total - 1][v] && (!fechar || adj[v * n + origem])) {
            return 1;
        }
    }
    return 0;
}

static long long referencia_tsp(int n, const int *pesos, int origem) {
    long long dp[ESTADOS][MAX_N];
    unsigned int total = 1u << n;
    for (unsigned int mascara = 0; mascara < total; mascara++) {
        for (int v = 0; v < n; v++) {
            dp[mascara][v] = LLONG_MAX;
        }
    }
    dp[1u << origem][origem] = 0;
    for (unsigned int mascara = 1; mascara < total; mascara++) {
        for (int u = 0; u < n; u++) {
            if (dp[mascara][u] == LLONG_MAX) {
                continue;
            }
            for (int v = 0; v < n; v++) {
                if (!(mascara & (1u << v))) {
                    unsigned int proxima = mascara | (1u << v);
                    long long custo = dp[mascara][u] + pesos[u * n + v];
                    if (custo < dp[proxima][v]) {
                        dp[proxima][v] = custo;
                    }
                }
            }
        }
    }
    long long melhor = LLONG_MAX;
    for (int v = 0; v < n; v++) {
        if (dp[total - 1][v] != LLONG_MAX) {
            long long custo = dp[total - 1][v] + pesos[v * n + origem];
            if (custo < melhor) {
                melhor = custo;
            }
        }
    }
    return melhor;
}

static void verificar_permutacao(int n, const int *rota, int origem, int fechar) {
    unsigned char visitados[MAX_N] = {0};
    if (origem >= 0) {
        assert(rota[0] == origem);
    }
    if (fechar) {
        assert(rota[n] == rota[0]);
    }
    for (int i = 0; i < n; i++) {
        assert(rota[i] >= 0 && rota[i] < n);
        assert(!visitados[rota[i]]);
        visitados[rota[i]] = 1;
    }
}

static void conferir_hamiltoniano(int n, const int *adj) {
    int rota[MAX_N + 1];
    int encontrado = encontrar_caminho_hamiltoniano_bt(n, adj, rota);
    assert(encontrado == referencia_hamiltoniano(n, adj, -1, 0));
    if (encontrado) {
        verificar_permutacao(n, rota, -1, 0);
        for (int i = 1; i < n; i++) {
            assert(adj[rota[i - 1] * n + rota[i]]);
        }
    } else {
        for (int i = 0; i < n; i++) {
            assert(rota[i] == -1);
        }
    }

    for (int origem = 0; origem < n; origem++) {
        encontrado = encontrar_ciclo_hamiltoniano_bt(n, adj, origem, rota);
        assert(encontrado == referencia_hamiltoniano(n, adj, origem, 1));
        if (encontrado) {
            verificar_permutacao(n, rota, origem, 1);
            for (int i = 0; i < n; i++) {
                assert(adj[rota[i] * n + rota[i + 1]]);
            }
        } else {
            for (int i = 0; i <= n; i++) {
                assert(rota[i] == -1);
            }
        }
    }
}

static void verificar_tsp(int n, const int *pesos, int origem, const int *rota, long long custo) {
    verificar_permutacao(n, rota, origem, 1);
    long long soma = 0;
    for (int i = 0; i < n; i++) {
        soma += pesos[rota[i] * n + rota[i + 1]];
    }
    assert(custo == soma);
}

static long long conferir_tsp(int n, const int *pesos) {
    int copia[MAX_N * MAX_N];
    memcpy(copia, pesos, (size_t)n * (size_t)n * sizeof(*pesos));
    long long primeiro = -1;
    for (int origem = 0; origem < n; origem++) {
        int rota[MAX_N + 1];
        long long esperado = referencia_tsp(n, pesos, origem);
        long long custo = tsp_forca_bruta(n, pesos, origem, rota);
        assert(custo == esperado);
        verificar_tsp(n, pesos, origem, rota, custo);
        if (origem == 0) {
            primeiro = custo;
        }
        custo = tsp_vizinho_mais_proximo(n, pesos, origem, rota);
        assert(custo >= esperado);
        verificar_tsp(n, pesos, origem, rota, custo);
        custo = tsp_insercao(n, pesos, origem, rota);
        assert(custo >= esperado);
        verificar_tsp(n, pesos, origem, rota, custo);
    }
    assert(memcmp(copia, pesos, (size_t)n * (size_t)n * sizeof(*pesos)) == 0);
    return primeiro;
}

static void testar_todos_os_grafos_pequenos(void) {
    for (int n = 1; n <= 5; n++) {
        int pares[10][2];
        int quantidade = 0;
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                pares[quantidade][0] = u;
                pares[quantidade++][1] = v;
            }
        }
        for (unsigned int mascara = 0; mascara < (1u << quantidade); mascara++) {
            int adj[25] = {0};
            int pesos[25];
            for (int i = 0; i < quantidade; i++) {
                if (mascara & (1u << i)) {
                    int u = pares[i][0];
                    int v = pares[i][1];
                    adj[u * n + v] = adj[v * n + u] = 1;
                }
            }
            conferir_hamiltoniano(n, adj);
            assert(grafo_para_tsp(n, adj, pesos));
            for (int u = 0; u < n; u++) {
                for (int v = 0; v < n; v++) {
                    assert(pesos[u * n + v] == (u == v ? 0 : (adj[u * n + v] ? 1 : 2)));
                }
            }
            long long custo = conferir_tsp(n, pesos);
            if (n >= 3) {
                assert((custo == n) == referencia_hamiltoniano(n, adj, 0, 1));
            }
        }
    }
}

static unsigned int aleatorio(void) {
    static unsigned int estado = 20102026u;
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

static void testar_matrizes_aleatorias(void) {
    for (int caso = 0; caso < 80; caso++) {
        int n = 1 + caso % MAX_N;
        int pesos[MAX_N * MAX_N] = {0};
        int adj[MAX_N * MAX_N] = {0};
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                if (u != v) {
                    pesos[u * n + v] = (int)(aleatorio() % 100);
                    adj[u * n + v] = (int)(aleatorio() % 2);
                }
            }
        }
        conferir_hamiltoniano(n, adj);
        conferir_tsp(n, pesos);
    }
}

static void testar_casos_especiais(void) {
    int adj[] = {0, 1, 0, 0, 0, 0, 1, 0, 0}; /* Caminho 2 -> 0 -> 1. */
    int rota[5];
    assert(encontrar_caminho_hamiltoniano_bt(3, adj, rota) == 1);
    assert(rota[0] == 2 && rota[1] == 0 && rota[2] == 1);
    conferir_hamiltoniano(3, adj);
    adj[5] = 1;
    conferir_hamiltoniano(3, adj);
    int laco = 1;
    conferir_hamiltoniano(1, &laco);

    const int classico[] = {0, 10, 15, 20, 10, 0, 35, 25,
                           15, 35, 0, 30, 20, 25, 30, 0};
    assert(conferir_tsp(4, classico) == 80);
    assert(tsp_vizinho_mais_proximo(4, classico, 0, rota) == 80);
    const int vizinho[] = {0, 1, 3, 2, 0};
    assert(memcmp(rota, vizinho, sizeof(vizinho)) == 0);
    assert(tsp_insercao(4, classico, 0, rota) == 80);
    const int insercao[] = {0, 2, 3, 1, 0};
    assert(memcmp(rota, insercao, sizeof(insercao)) == 0);

    const int altos[] = {0, INT_MAX, INT_MAX, INT_MAX, 0, INT_MAX,
                         INT_MAX, INT_MAX, 0};
    assert(conferir_tsp(3, altos) == 3LL * INT_MAX);
    int zeros[16] = {0};
    assert(conferir_tsp(4, zeros) == 0);
}

static void testar_entradas_invalidas(void) {
    int rota[3];
    int matriz[] = {0, 1, 1, 0};
    assert(encontrar_caminho_hamiltoniano_bt(0, matriz, rota) == -1);
    assert(encontrar_caminho_hamiltoniano_bt(2, NULL, rota) == -1);
    assert(encontrar_caminho_hamiltoniano_bt(2, matriz, NULL) == -1);
    assert(encontrar_ciclo_hamiltoniano_bt(2, matriz, -1, rota) == -1);
    assert(encontrar_ciclo_hamiltoniano_bt(2, matriz, 2, rota) == -1);
    assert(tsp_forca_bruta(0, matriz, 0, rota) == -1);
    assert(tsp_forca_bruta(2, NULL, 0, rota) == -1);
    assert(tsp_forca_bruta(2, matriz, 0, NULL) == -1);
    assert(tsp_vizinho_mais_proximo(2, matriz, 2, rota) == -1);
    assert(tsp_insercao(2, matriz, -1, rota) == -1);
    assert(!grafo_para_tsp(0, matriz, matriz));
    assert(!grafo_para_tsp(2, NULL, matriz));
    assert(!grafo_para_tsp(2, matriz, NULL));

    matriz[1] = 2;
    assert(encontrar_caminho_hamiltoniano_bt(2, matriz, rota) == -1);
    assert(!grafo_para_tsp(2, matriz, matriz));
    matriz[1] = -1;
    assert(tsp_forca_bruta(2, matriz, 0, rota) == -1);
    assert(tsp_vizinho_mais_proximo(2, matriz, 0, rota) == -1);
    assert(tsp_insercao(2, matriz, 0, rota) == -1);
    matriz[1] = 1;
    matriz[0] = 1;
    assert(tsp_forca_bruta(2, matriz, 0, rota) == -1);
}

int main(void) {
    testar_casos_especiais();
    testar_todos_os_grafos_pequenos();
    testar_matrizes_aleatorias();
    testar_entradas_invalidas();
    puts("Todos os testes de Hamiltoniano e TSP passaram.");
    return 0;
}
