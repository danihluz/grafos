#ifndef TPS_H
#define TPS_H

/*
 * Os nomes tps.h/tps.c seguem o enunciado; as funcoes implementam TSP.
 * pesos e uma matriz completa n x n, em ordem de linhas, com diagonal zero
 * e custos int nao negativos. Pode ser simetrica ou assimetrica.
 * rota deve comportar n+1 vertices, com a origem repetida ao final.
 * Retorna o custo em long long, ou -1 em caso de entrada invalida/memoria.
 * As matrizes de entrada nao sao modificadas.
 */
long long tsp_forca_bruta(int n, const int *pesos, int origem, int *rota);
long long tsp_vizinho_mais_proximo(int n, const int *pesos, int origem, int *rota);
long long tsp_insercao(int n, const int *pesos, int origem, int *rota);

/*
 * Reducao: diagonal zero; peso 1 para arestas e 2 para ausencias.
 * adj deve conter 0/1; o chamador fornece pesos com n*n posicoes.
 * Retorna 1 em caso de sucesso ou 0 para entrada invalida.
 * Para n>=3, ha ciclo Hamiltoniano se e somente se o TSP otimo custa n.
 */
int grafo_para_tsp(int n, const int *adj, int *pesos);

#endif
