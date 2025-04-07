#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {
    const char *arquivoEntrada = "processo_043_202409032338.csv";
    const char *arquivoSaida = "ordenado_por_id.csv";
    Processo *processos = NULL;
    int totalProcessos;

    // Contar o número de linhas no arquivo
    int maxProcessos = contarLinhas(arquivoEntrada);
    if (maxProcessos <= 0) {
        printf("Erro ao contar as linhas do arquivo ou arquivo vazio.\n");
        return 1;
    }

    // Alocar memória para o vetor de processos
    processos = (Processo *)malloc(maxProcessos * sizeof(Processo));
    if (!processos) {
        perror("Erro ao alocar memória");
        return 1;
    }

    // Ler os dados do arquivo
    totalProcessos = lerDados(arquivoEntrada, processos);
    if (totalProcessos == 0) {
        printf("Nenhum dado foi carregado.\n");
        free(processos);
        return 1;
    }

    // Ordenar os dados pelo atributo id
    ordenarPorId(processos, totalProcessos);

    // Salvar os dados ordenados em um novo arquivo
    salvarDadosOrdenados(arquivoSaida, processos, totalProcessos);

    printf("Dados ordenados por id e salvos em '%s'.\n", arquivoSaida);

    // Liberar a memória alocada
    free(processos);
    return 0;
}