#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hamiltoniano.h"
#include "tps.h"

typedef long long (*AlgoritmoTSP)(int, const int *, int, int *);

static void imprimir_rota(const char *nome, const int *rota, int quantidade) {
    printf("%s: ", nome);
    for (int i = 0; i < quantidade; i++) {
        printf("%s%d", i > 0 ? " -> " : "", rota[i]);
    }
    printf("\n");
}

static int demonstrar_reducao(const char *titulo, const int *adj) {
    const int n = 5;
    int caminho[5], ciclo[6], rota_tsp[6], pesos[25];
    int tem_caminho = encontrar_caminho_hamiltoniano_bt(n, adj, caminho);
    int tem_ciclo = encontrar_ciclo_hamiltoniano_bt(n, adj, 0, ciclo);
    if (tem_caminho < 0 || tem_ciclo < 0 || !grafo_para_tsp(n, adj, pesos)) {
        return 0;
    }
    long long custo = tsp_forca_bruta(n, pesos, 0, rota_tsp);
    if (custo < 0 || tem_ciclo != (custo == n)) {
        return 0;
    }

    printf("=== %s ===\n", titulo);
    if (tem_caminho) {
        imprimir_rota("Caminho Hamiltoniano", caminho, n);
    } else {
        printf("Caminho Hamiltoniano: inexistente\n");
    }
    if (tem_ciclo) {
        imprimir_rota("Ciclo Hamiltoniano", ciclo, n + 1);
    } else {
        printf("Ciclo Hamiltoniano: inexistente\n");
    }
    imprimir_rota("TSP da reducao", rota_tsp, n + 1);
    printf("Custo otimo da reducao: %lld | n = %d | custo igual a n: %s\n\n",
           custo, n, custo == n ? "sim" : "nao");
    return 1;
}

static unsigned int aleatorio(unsigned int *estado) {
    *estado ^= *estado << 13;
    *estado ^= *estado >> 17;
    *estado ^= *estado << 5;
    return *estado;
}

static int validar_rota(int n, const int *pesos, const int *rota, long long custo) {
    unsigned char usados[12] = {0};
    if (custo < 0 || rota[0] != 0 || rota[n] != 0) {
        return 0;
    }
    long long soma = 0;
    for (int i = 0; i < n; i++) {
        int u = rota[i];
        int v = rota[i + 1];
        if (u < 0 || u >= n || v < 0 || v >= n || usados[u]) {
            return 0;
        }
        usados[u] = 1;
        soma += pesos[u * n + v];
    }
    return soma == custo;
}

static int medir(
    AlgoritmoTSP algoritmo, int n, const int *pesos, int *rota,
    long long *custo, double *tempo_ms, int *repeticoes
) {
    int quantidade = 1;
    while (1) {
        clock_t inicio = clock();
        if (inicio == (clock_t)-1) {
            return 0;
        }
        for (int i = 0; i < quantidade; i++) {
            *custo = algoritmo(n, pesos, 0, rota);
            if (*custo < 0) {
                return 0;
            }
        }
        clock_t fim = clock();
        if (fim == (clock_t)-1) {
            return 0;
        }
        double segundos = (double)(fim - inicio) / CLOCKS_PER_SEC;
        if (segundos >= 0.02 || quantidade >= 1048576) {
            *tempo_ms = segundos * 1000.0 / quantidade;
            *repeticoes = quantidade;
            return 1;
        }
        quantidade *= 2;
    }
}

int main(void) {
    int adj[25] = {0};
    for (int v = 1; v < 5; v++) {
        adj[(v - 1) * 5 + v] = adj[v * 5 + v - 1] = 1;
    }
    if (!demonstrar_reducao("CAMINHO SEM CICLO HAMILTONIANO", adj)) {
        return EXIT_FAILURE;
    }
    adj[4] = adj[20] = 1;
    if (!demonstrar_reducao("GRAFO COM CICLO HAMILTONIANO", adj)) {
        return EXIT_FAILURE;
    }

    const int tamanhos[] = {5, 8, 10, 12};
    const char *nomes[] = {"Forca bruta", "Vizinho proximo", "Insercao proxima"};
    AlgoritmoTSP algoritmos[] = {
        tsp_forca_bruta, tsp_vizinho_mais_proximo, tsp_insercao
    };
    unsigned int estado = 20261020u;
    printf("=== EXPERIMENTO: TEMPO E CUSTO DO TSP ===\n");
    printf("Matrizes completas simetricas; semente 20261020; pesos de 1 a 99.\n");
    printf("Mesma matriz e origem 0 para todos os algoritmos de cada tamanho.\n");
    printf("Forca bruta enumera (n-1)! permutacoes, sem poda.\n");
    printf("Tempos medios em ms com clock(), CLOCKS_PER_SEC = %ld.\n",
           (long)CLOCKS_PER_SEC);
    printf(" n | Algoritmo       | Custo | Tempo medio (ms) | Repeticoes\n");

    for (size_t caso = 0; caso < sizeof(tamanhos) / sizeof(tamanhos[0]); caso++) {
        int n = tamanhos[caso];
        int pesos[144] = {0};
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                int peso = 1 + (int)(aleatorio(&estado) % 99);
                pesos[u * n + v] = pesos[v * n + u] = peso;
            }
        }

        long long otimo = -1;
        for (int algoritmo = 0; algoritmo < 3; algoritmo++) {
            int rota[13], repeticoes;
            long long custo;
            double tempo_ms;
            if (!medir(algoritmos[algoritmo], n, pesos, rota,
                       &custo, &tempo_ms, &repeticoes) ||
                !validar_rota(n, pesos, rota, custo)) {
                fprintf(stderr, "Erro no experimento para n = %d.\n", n);
                return EXIT_FAILURE;
            }
            if (algoritmo == 0) {
                otimo = custo;
            } else if (custo < otimo) {
                fprintf(stderr, "Erro: heuristica com custo inferior ao otimo.\n");
                return EXIT_FAILURE;
            }
            printf("%2d | %-15s | %5lld | %16.6f | %10d\n",
                   n, nomes[algoritmo], custo, tempo_ms, repeticoes);
        }
    }

    printf("\nA forca bruta garante o otimo e tem crescimento fatorial.\n");
    printf("As heuristicas constroem rotas validas, mas podem ter custo maior.\n");
    printf("Os tempos dependem da maquina e dos pesos utilizados.\n");
    return EXIT_SUCCESS;
}
