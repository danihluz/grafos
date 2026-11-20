#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cobertura.h"

typedef int (*Algoritmo)(int, const Aresta *, int, int *);

static void imprimir_cobertura(const char *nome, int n, const int *cobertura, int tamanho) {
    printf("%s: {", nome);
    int separador = 0;
    for (int v = 0; v < n; v++) {
        if (cobertura[v]) {
            printf("%s%d", separador ? ", " : "", v);
            separador = 1;
        }
    }
    printf("} | tamanho = %d\n", tamanho);
}

static int demonstrar_diferenca(void) {
    /* Caminho 2--1--0--3--4: a escolha gulosa de 0 e desnecessaria no otimo. */
    const Aresta arestas[] = {{0, 1}, {0, 3}, {1, 2}, {3, 4}};
    int minima[5], gulosa[5];
    int tamanho_minimo = cobertura_minima_bt(5, arestas, 4, minima);
    int tamanho_guloso = cobertura_gulosa(5, arestas, 4, gulosa);
    if (tamanho_minimo < 0 || tamanho_guloso < 0 ||
        !eh_cobertura(5, arestas, 4, minima) || !eh_cobertura(5, arestas, 4, gulosa)) {
        return 0;
    }

    printf("=== EXEMPLO: A HEURISTICA PODE NAO SER OTIMA ===\n");
    printf("Arestas: (0,1), (0,3), (1,2), (3,4)\n");
    imprimir_cobertura("Backtracking", 5, minima, tamanho_minimo);
    imprimir_cobertura("Gulosa", 5, gulosa, tamanho_guloso);
    return 1;
}

static unsigned int aleatorio(unsigned int *estado) {
    *estado ^= *estado << 13;
    *estado ^= *estado >> 17;
    *estado ^= *estado << 5;
    return *estado;
}

static int medir(
    Algoritmo algoritmo, int n, const Aresta *arestas, int m, int *cobertura,
    int *tamanho, double *tempo_ms, int *repeticoes
) {
    int quantidade = 1;
    while (1) {
        clock_t inicio = clock();
        if (inicio == (clock_t)-1) {
            return 0;
        }
        for (int i = 0; i < quantidade; i++) {
            *tamanho = algoritmo(n, arestas, m, cobertura);
            if (*tamanho < 0) {
                return 0;
            }
        }
        clock_t fim = clock();
        if (fim == (clock_t)-1) {
            return 0;
        }
        double segundos = (double)(fim - inicio) / CLOCKS_PER_SEC;
        if (segundos >= 0.02 || quantidade >= 16384) {
            *tempo_ms = segundos * 1000.0 / quantidade;
            *repeticoes = quantidade;
            return 1;
        }
        quantidade *= 2;
    }
}

int main(void) {
    if (!demonstrar_diferenca()) {
        fprintf(stderr, "Erro ao calcular as coberturas do exemplo.\n");
        return EXIT_FAILURE;
    }

    const int tamanhos[] = {5, 8, 10, 12, 15};
    unsigned int estado = 20260918u;
    printf("\n=== EXPERIMENTO: BACKTRACKING E GULOSA ===\n");
    printf("Grafos simples nao direcionados; semente 20260918; probabilidade de aresta 35%%.\n");
    printf("Mesmos grafos para os dois algoritmos; tempos medios em ms por execucao.\n");
    printf("Medicao com clock(), CLOCKS_PER_SEC = %ld; lotes de ate 16384 repeticoes.\n",
           (long)CLOCKS_PER_SEC);
    printf(" n | Arestas | Minimo | Gulosa | Excesso |    BT (ms) | Gulosa (ms) | Rep. BT | Rep. gulosa\n");

    for (size_t caso = 0; caso < sizeof(tamanhos) / sizeof(tamanhos[0]); caso++) {
        int n = tamanhos[caso];
        Aresta arestas[105]; /* Maximo de arestas para os 15 vertices do experimento. */
        int m = 0;
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                if (aleatorio(&estado) % 100 < 35) {
                    arestas[m++] = (Aresta){u, v};
                }
            }
        }

        int minima[15], gulosa[15];
        int tamanho_minimo, tamanho_guloso, repeticoes_bt, repeticoes_gulosa;
        double tempo_bt, tempo_gulosa;
        if (!medir(cobertura_minima_bt, n, arestas, m, minima,
                   &tamanho_minimo, &tempo_bt, &repeticoes_bt) ||
            !medir(cobertura_gulosa, n, arestas, m, gulosa,
                   &tamanho_guloso, &tempo_gulosa, &repeticoes_gulosa) ||
            !eh_cobertura(n, arestas, m, minima) ||
            !eh_cobertura(n, arestas, m, gulosa) || tamanho_minimo > tamanho_guloso) {
            fprintf(stderr, "Erro ao executar o experimento para n = %d.\n", n);
            return EXIT_FAILURE;
        }

        printf("%2d | %7d | %6d | %6d | %7d | %10.6f | %11.6f | %7d | %11d\n",
               n, m, tamanho_minimo, tamanho_guloso, tamanho_guloso - tamanho_minimo,
               tempo_bt, tempo_gulosa, repeticoes_bt, repeticoes_gulosa);
    }

    printf("\nO backtracking garante o minimo; a gulosa pode escolher vertices a mais.\n");
    printf("Os tempos dependem da maquina e do grafo; o backtracking e exponencial no pior caso.\n");
    return EXIT_SUCCESS;
}
