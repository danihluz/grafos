#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agm.h"

#define MAX_VERTICES 6
#define MAX_ARESTAS 10

static void testar_dsu(void) {
    DSU *d = criar_dsu(8);
    assert(d != NULL && criar_dsu(0) == NULL);
    for (int x = 0; x < 8; x++) {
        assert(find(d, x) == x && d->rank[x] == 0);
    }
    assert(unir(d, 0, 1) && unir(d, 2, 3) && unir(d, 0, 2));
    assert(unir(d, 4, 5) && unir(d, 6, 7) && unir(d, 4, 6));
    assert(unir(d, 0, 4));
    assert(d->rank[0] == 3 && d->pai[7] != 0);
    assert(find(d, 7) == 0 && d->pai[7] == 0); /* Compressao de caminho. */
    assert(unir(d, 1, 7) == 0 && d->rank[0] == 3);
    assert(find(d, -1) == -1 && find(d, 8) == -1);
    assert(find(NULL, 0) == -1 && unir(NULL, 0, 1) == 0);
    liberar_dsu(d);
    liberar_dsu(NULL);

    d = criar_dsu(3);
    assert(d != NULL);
    assert(unir(d, 1, 2) && unir(d, 0, 1));
    assert(find(d, 0) == 1 && d->rank[1] == 1); /* Uniao por rank. */
    liberar_dsu(d);
}

/* Referencia independente: rotulos de componentes, sem usar DSU. */
static int juntar_rotulos(int *rotulos, int n, int u, int v) {
    int antigo = rotulos[v];
    int novo = rotulos[u];
    if (antigo == novo) {
        return 0;
    }
    for (int x = 0; x < n; x++) {
        if (rotulos[x] == antigo) {
            rotulos[x] = novo;
        }
    }
    return 1;
}

static int contar_componentes(int n, const Aresta *arestas, int m) {
    int *rotulos = malloc((size_t)n * sizeof(*rotulos));
    assert(rotulos != NULL);
    for (int x = 0; x < n; x++) {
        rotulos[x] = x;
    }
    int componentes = n;
    for (int i = 0; i < m; i++) {
        componentes -= juntar_rotulos(rotulos, n, arestas[i].u, arestas[i].v);
    }
    free(rotulos);
    return componentes;
}

static void verificar_resultado(
    int n, const Aresta *arestas, int m, int status,
    const ResultadoAGM *resultado, long long custo_esperado
) {
    int componentes = contar_componentes(n, arestas, m);
    assert(status == (componentes == 1));
    assert(resultado->componentes == componentes);
    assert(resultado->quantidade == n - componentes);
    assert(resultado->custo_total == custo_esperado);
    int *rotulos = malloc((size_t)n * sizeof(*rotulos));
    assert(rotulos != NULL);
    for (int x = 0; x < n; x++) {
        rotulos[x] = x;
    }
    long long soma = 0;
    for (int i = 0; i < resultado->quantidade; i++) {
        Aresta a = resultado->arestas[i];
        int existe = 0;
        for (int j = 0; j < m; j++) {
            if (a.peso == arestas[j].peso &&
                ((a.u == arestas[j].u && a.v == arestas[j].v) ||
                 (a.u == arestas[j].v && a.v == arestas[j].u))) {
                existe = 1;
            }
        }
        assert(existe);
        assert(juntar_rotulos(rotulos, n, a.u, a.v)); /* Nao pode haver ciclo. */
        soma += a.peso;
    }
    assert(soma == resultado->custo_total);
    for (int i = 0; i < m; i++) {
        assert(rotulos[arestas[i].u] == rotulos[arestas[i].v]);
    }
    free(rotulos);
}

static void conferir_algoritmos(
    int n, const Aresta *arestas, int m, long long custo_esperado
) {
    ResultadoAGM resultado = {0};
    int status = kruskal(n, arestas, m, &resultado);
    verificar_resultado(n, arestas, m, status, &resultado, custo_esperado);
    liberar_agm(&resultado);
    for (int origem = 0; origem < n; origem++) {
        status = prim(n, arestas, m, origem, &resultado);
        verificar_resultado(n, arestas, m, status, &resultado, custo_esperado);
        liberar_agm(&resultado);
    }
}

static void testar_casos_especiais(void) {
    conferir_algoritmos(1, NULL, 0, 0);
    conferir_algoritmos(5, NULL, 0, 0);
    const Aresta paralelo[] = {
        {0, 0, INT_MIN}, {0, 1, 8}, {1, 0, -2}, {0, 1, -2},
        {1, 2, 0}, {0, 2, 7}, {2, 2, -100}
    };
    conferir_algoritmos(3, paralelo, 7, -2);
    const Aresta desconectado[] = {{0, 1, -2}, {1, 2, 0}, {3, 4, 4}};
    conferir_algoritmos(6, desconectado, 3, 2);
    const Aresta empates[] = {{0, 1, 0}, {1, 2, 0}, {0, 2, 0}, {2, 3, 0}};
    conferir_algoritmos(4, empates, 4, 0);
    const Aresta extremos[] = {
        {0, 1, INT_MIN}, {1, 2, INT_MAX}, {0, 2, INT_MAX - 1}
    };
    conferir_algoritmos(3, extremos, 3, -2);
    const Aresta altos[] = {{0, 1, INT_MAX}, {1, 2, INT_MAX}};
    conferir_algoritmos(3, altos, 2, 2LL * INT_MAX);
    const Aresta baixos[] = {{0, 1, INT_MIN}, {1, 2, INT_MIN}};
    conferir_algoritmos(3, baixos, 2, 2LL * INT_MIN);
}

/* Enumera todos os subconjuntos para obter o minimo sem Kruskal nem Prim. */
static long long minimo_por_forca_bruta(int n, const Aresta *arestas, int m) {
    int componentes = contar_componentes(n, arestas, m);
    long long minimo = LLONG_MAX;
    for (unsigned int mascara = 0; mascara < (1u << m); mascara++) {
        int rotulos[MAX_VERTICES];
        for (int x = 0; x < n; x++) {
            rotulos[x] = x;
        }
        int restantes = n;
        int valido = 1;
        long long custo = 0;
        for (int i = 0; i < m; i++) {
            if ((mascara & (1u << i)) == 0) {
                continue;
            }
            if (!juntar_rotulos(rotulos, n, arestas[i].u, arestas[i].v)) {
                valido = 0;
                break;
            }
            restantes--;
            custo += arestas[i].peso;
        }
        if (valido && restantes == componentes && custo < minimo) {
            minimo = custo;
        }
    }
    assert(minimo != LLONG_MAX);
    return minimo;
}

static unsigned int aleatorio(void) {
    static unsigned int estado = 16082026;
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

static void testar_contra_forca_bruta(void) {
    for (int caso = 0; caso < 300; caso++) {
        int n = 1 + caso % MAX_VERTICES;
        int m = (int)(aleatorio() % (MAX_ARESTAS + 1));
        Aresta arestas[MAX_ARESTAS];
        Aresta copia[MAX_ARESTAS];
        for (int i = 0; i < m; i++) {
            arestas[i] = (Aresta){
                (int)(aleatorio() % (unsigned int)n),
                (int)(aleatorio() % (unsigned int)n),
                (int)(aleatorio() % 31) - 15
            };
        }
        memcpy(copia, arestas, (size_t)m * sizeof(*arestas));
        long long esperado = minimo_por_forca_bruta(n, arestas, m);
        conferir_algoritmos(n, arestas, m, esperado);
        assert(memcmp(copia, arestas, (size_t)m * sizeof(*arestas)) == 0);
    }
}

static void testar_grafo_denso(void) {
    const int n = 40;
    const int m = n * (n - 1) / 2;
    Aresta *arestas = malloc((size_t)m * sizeof(*arestas));
    assert(arestas != NULL);
    int quantidade = 0;
    for (int u = 0; u < n; u++) {
        for (int v = u + 1; v < n; v++) {
            arestas[quantidade++] = (Aresta){u, v, v - u};
        }
    }
    /* Toda aresta pesa >= 1 e a cadeia 0--1--...--39 custa exatamente 39. */
    conferir_algoritmos(n, arestas, m, n - 1);
    free(arestas);
}

static void testar_entradas_invalidas(void) {
    ResultadoAGM resultado = {0};
    const Aresta invalida = {0, 2, 1};
    assert(kruskal(0, NULL, 0, &resultado) == -1);
    assert(kruskal(2, NULL, 1, &resultado) == -1);
    assert(kruskal(2, NULL, -1, &resultado) == -1);
    assert(kruskal(2, &invalida, 1, &resultado) == -1);
    assert(kruskal(2, NULL, 0, NULL) == -1);
    assert(prim(0, NULL, 0, 0, &resultado) == -1);
    assert(prim(2, NULL, 1, 0, &resultado) == -1);
    assert(prim(2, &invalida, 1, 0, &resultado) == -1);
    assert(prim(2, NULL, 0, -1, &resultado) == -1);
    assert(prim(2, NULL, 0, 2, &resultado) == -1);
    assert(prim(2, NULL, 0, 0, NULL) == -1);
    liberar_agm(&resultado);
    liberar_agm(&resultado);
    liberar_agm(NULL);
}

int main(void) {
    testar_dsu();
    testar_casos_especiais();
    testar_contra_forca_bruta();
    testar_grafo_denso();
    testar_entradas_invalidas();
    puts("Todos os testes de AGM passaram.");
    return 0;
}
