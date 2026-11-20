#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "cobertura.h"

#define MAX_VERTICES 15
#define MAX_ARESTAS 105

static int contar_bits(unsigned int mascara) {
    int quantidade = 0;
    while (mascara != 0) {
        quantidade += (int)(mascara & 1u);
        mascara >>= 1;
    }
    return quantidade;
}

/* Referencia independente: testa todos os subconjuntos de vertices. */
static int minimo_por_enumeracao(int n, const Aresta *arestas, int m) {
    int melhor = n;
    for (unsigned int mascara = 0; mascara < (1u << n); mascara++) {
        int tamanho = contar_bits(mascara);
        if (tamanho >= melhor) {
            continue;
        }
        int valido = 1;
        for (int i = 0; i < m; i++) {
            if ((mascara & (1u << arestas[i].u)) == 0 &&
                (mascara & (1u << arestas[i].v)) == 0) {
                valido = 0;
                break;
            }
        }
        if (valido) {
            melhor = tamanho;
        }
    }
    return melhor;
}

static void verificar_solucao(
    int n, const Aresta *arestas, int m, const int *cobertura, int tamanho
) {
    int contagem = 0;
    for (int v = 0; v < n; v++) {
        assert(cobertura[v] == 0 || cobertura[v] == 1);
        contagem += cobertura[v];
    }
    assert(tamanho == contagem);
    /* Confere a propriedade diretamente, alem da funcao publica. */
    for (int i = 0; i < m; i++) {
        assert(cobertura[arestas[i].u] || cobertura[arestas[i].v]);
    }
    assert(eh_cobertura(n, arestas, m, cobertura));
}

static void conferir_algoritmos(int n, const Aresta *arestas, int m, int esperado) {
    int minima[MAX_VERTICES], gulosa[MAX_VERTICES];
    Aresta copia[MAX_ARESTAS];
    assert(n <= MAX_VERTICES && m <= MAX_ARESTAS);
    if (m > 0) {
        memcpy(copia, arestas, (size_t)m * sizeof(*arestas));
    }
    for (int v = 0; v < n; v++) {
        minima[v] = gulosa[v] = 42; /* A funcao deve inicializar a saida. */
    }

    int tamanho_minimo = cobertura_minima_bt(n, arestas, m, minima);
    int tamanho_guloso = cobertura_gulosa(n, arestas, m, gulosa);
    if (tamanho_minimo != esperado) {
        fprintf(stderr, "n=%d m=%d esperado=%d obtido=%d\n", n, m, esperado, tamanho_minimo);
        for (int i = 0; i < m; i++) {
            fprintf(stderr, "(%d,%d) ", arestas[i].u, arestas[i].v);
        }
        fprintf(stderr, "\n");
    }
    assert(tamanho_minimo == esperado);
    assert(tamanho_guloso >= esperado && tamanho_guloso <= n);
    verificar_solucao(n, arestas, m, minima, tamanho_minimo);
    verificar_solucao(n, arestas, m, gulosa, tamanho_guloso);
    if (m > 0) {
        assert(memcmp(copia, arestas, (size_t)m * sizeof(*arestas)) == 0);
    }
}

static void testar_casos_especiais(void) {
    assert(cobertura_minima_bt(0, NULL, 0, NULL) == 0);
    assert(cobertura_gulosa(0, NULL, 0, NULL) == 0);
    assert(eh_cobertura(0, NULL, 0, NULL) == 1);
    conferir_algoritmos(1, NULL, 0, 0);
    conferir_algoritmos(8, NULL, 0, 0);

    const Aresta estrela[] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}};
    conferir_algoritmos(5, estrela, 4, 1);
    const Aresta caminho[] = {{0, 1}, {0, 3}, {1, 2}, {3, 4}};
    conferir_algoritmos(5, caminho, 4, 2);
    int cobertura[5];
    assert(cobertura_gulosa(5, caminho, 4, cobertura) == 3);
    assert(cobertura[0] && cobertura[1] && cobertura[3]);
    const int incompleta[] = {1, 0, 0, 0, 0};
    assert(eh_cobertura(5, caminho, 4, incompleta) == 0);

    const Aresta ciclo_impar[] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}};
    conferir_algoritmos(5, ciclo_impar, 5, 3);
    const Aresta desconectado[] = {{0, 1}, {1, 2}, {3, 4}, {4, 5}, {3, 5}};
    conferir_algoritmos(8, desconectado,
                        (int)(sizeof(desconectado) / sizeof(desconectado[0])), 3);
    const Aresta lacos[] = {{0, 0}, {0, 1}, {1, 2}, {2, 2}, {2, 1}, {0, 1}};
    conferir_algoritmos(4, lacos, 6, 2);

    /* O limite de 15 pertence ao experimento, nao a implementacao. */
    const Aresta distante = {0, 64};
    int grande[65];
    assert(cobertura_minima_bt(65, &distante, 1, grande) == 1);
    verificar_solucao(65, &distante, 1, grande, 1);
    assert(cobertura_gulosa(65, &distante, 1, grande) == 1);
    verificar_solucao(65, &distante, 1, grande, 1);
}

static void testar_todos_os_grafos_pequenos(void) {
    for (int n = 1; n <= 5; n++) {
        Aresta possiveis[10];
        int quantidade = 0;
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                possiveis[quantidade++] = (Aresta){u, v};
            }
        }
        for (unsigned int mascara = 0; mascara < (1u << quantidade); mascara++) {
            Aresta arestas[10];
            int m = 0;
            for (int i = 0; i < quantidade; i++) {
                if (mascara & (1u << i)) {
                    arestas[m++] = possiveis[i];
                }
            }
            conferir_algoritmos(n, arestas, m, minimo_por_enumeracao(n, arestas, m));
        }
    }
}

static unsigned int aleatorio(void) {
    static unsigned int estado = 18092026u;
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

static void testar_grafos_aleatorios(void) {
    for (int caso = 0; caso < 300; caso++) {
        int n = 1 + caso % 10;
        int m = (int)(aleatorio() % 21);
        Aresta arestas[20];
        for (int i = 0; i < m; i++) {
            arestas[i] = (Aresta){
                (int)(aleatorio() % (unsigned int)n),
                (int)(aleatorio() % (unsigned int)n)
            };
        }
        conferir_algoritmos(n, arestas, m, minimo_por_enumeracao(n, arestas, m));
    }
}

static void testar_quinze_vertices(void) {
    Aresta arestas[MAX_ARESTAS];
    int m = 0;
    for (int u = 0; u < 15; u++) {
        for (int v = u + 1; v < 15; v++) {
            arestas[m++] = (Aresta){u, v};
        }
    }
    conferir_algoritmos(15, arestas, m, 14); /* Grafo completo. */

    m = 0;
    for (int v = 1; v < 15; v++) {
        arestas[m++] = (Aresta){v - 1, v};
    }
    conferir_algoritmos(15, arestas, m, minimo_por_enumeracao(15, arestas, m));
}

static void testar_entradas_invalidas(void) {
    int cobertura[3] = {0};
    const Aresta fora = {0, 3};
    const Aresta negativa = {-1, 0};
    assert(cobertura_minima_bt(-1, NULL, 0, cobertura) == -1);
    assert(cobertura_minima_bt(3, NULL, -1, cobertura) == -1);
    assert(cobertura_minima_bt(3, NULL, 1, cobertura) == -1);
    assert(cobertura_minima_bt(3, &fora, 1, cobertura) == -1);
    assert(cobertura_minima_bt(3, &negativa, 1, cobertura) == -1);
    assert(cobertura_minima_bt(3, NULL, 0, NULL) == -1);
    assert(cobertura_gulosa(3, &fora, 1, cobertura) == -1);
    assert(cobertura_gulosa(3, NULL, 1, cobertura) == -1);
    assert(cobertura_gulosa(3, NULL, 0, NULL) == -1);
    assert(!eh_cobertura(3, &fora, 1, cobertura));
    assert(!eh_cobertura(3, &negativa, 1, cobertura));
    assert(!eh_cobertura(3, NULL, 0, NULL));
    cobertura[0] = 2;
    assert(!eh_cobertura(3, NULL, 0, cobertura));
}

int main(void) {
    testar_casos_especiais();
    testar_todos_os_grafos_pequenos();
    testar_grafos_aleatorios();
    testar_quinze_vertices();
    testar_entradas_invalidas();
    puts("Todos os testes de cobertura de vertices passaram.");
    return 0;
}
