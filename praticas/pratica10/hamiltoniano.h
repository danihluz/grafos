#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

/*
 * adj e uma matriz n x n, em ordem de linhas, com valores 0 ou 1.
 * As buscas respeitam a direcao das arestas; uma matriz simetrica representa
 * um grafo nao direcionado. Retorno: 1 encontrado, 0 inexistente, -1 erro.
 * O caminho tenta todas as origens e usa um vetor de n inteiros.
 * O ciclo fixa a origem e usa n+1 inteiros, repetindo a origem no final.
 * Para n=1, caminho existe; ciclo exige um laco. Para n=2, o ciclo exige
 * as duas direcoes. O chamador fornece os vetores de saida.
 */
int encontrar_caminho_hamiltoniano_bt(int n, const int *adj, int *caminho);
int encontrar_ciclo_hamiltoniano_bt(int n, const int *adj, int origem, int *ciclo);

#endif
