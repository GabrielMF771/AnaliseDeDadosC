#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {
    const char *arquivoEntrada = "processo_043_202409032338.csv";
    Processo *processos = NULL;
    int totalProcessos;

    //VARIAVEIS PARA FUNÇÃO DE CONTAGEM POR CLASSE//
    int classeBuscada = 0;
    int quantidadeRepeticaoClasse = 0;

    //Variáveis para a função de cálculo de dias de tramitação
    int idProcesso = 0;

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

    int opcao = -1;

    printf("Analise de Dados em C\n\n");
    printf("Este trabalho pratico tem por finalidade a implementacao, em C, de algoritmo para leitura e manipulacao de dados formatados de um arquivo texto (csv).\n\n");

    while(opcao != 0) {
        printf("Menu de Opcoes:\n");
        printf("(1) Ordenar os processos em ordem crescente a partir do ID\n");
        printf("(2) Ordenar os processos em ordem decrescente por data de ajuizamento\n");
        printf("(3) Contar os processos vinculados a uma classe especifica\n");
        printf("(4) Identificar quantos 'id_assuntos' constam nos processos presentes na base de dados\n");
        printf("(5) Contar os processos com mais de 1 assunto vinculado\n");
        printf("(6) Contar quantos dias um processo esta em tramitacao\n");
        printf("(0) Encerrar o programa\n\n");

        printf("Escolha uma opcao: ");
        // Ler a opção do usuário
        scanf("%d", &opcao);
        
        switch (opcao){
            case (0):
                printf("\nEncerrando o programa...\n");
                break;

            case (1):
                // Ordenar os dados pelo atributo id
                ordenarPorId(processos, totalProcessos);
            
                // Salvar os dados ordenados em um novo arquivo
                salvarDadosOrdenados("ordenado_por_id.csv", processos, totalProcessos);
            
                printf("\nDados ordenados por id e salvos em 'ordenado_por_id.csv'.\n\n");
                break;

            case (2):
                // Ordenar os dados pelo atributo data_ajuizamento
                ordenarPorData(processos, totalProcessos);
            
                // Salvar os dados ordenados em um novo arquivo
                salvarDadosOrdenados("ordenado_por_data.csv", processos, totalProcessos);
            
                printf("\nDados ordenados por data e salvos em 'ordenado_por_data.csv'.\n\n");
                break;

            case (3):
                //CHAMADA DA FUNÇÃO DE CONTAGEM DE REPETIÇÕES
                printf("\nInforme o id_classe a ser buscado: ");
                scanf("%d", &classeBuscada);
                quantidadeRepeticaoClasse = ocorrenciasClasse(processos, classeBuscada, totalProcessos);
                printf("\nProcessos vinculados ao id_classe: \"%d\": %d\n\n",classeBuscada, quantidadeRepeticaoClasse);
                break;

            case (4):
                printf("\nWIP\n\n");
                break;
            
            case (5):
                //CHAMADA DA FUNÇÃO DE LISTAMENTO DOS PROCESSOS COM MAIS DE 1 ASSUNTO
                variosAssuntos(processos,maxProcessos);
                break;

            case (6):
                printf("\nInforme o ID a ser buscado: ");
                scanf("%d", &idProcesso);
                calcularDiasTramitacao(processos, idProcesso, totalProcessos);
                break;

            default: 
                printf("\nOpcao invalida.\n\n");
                free(processos);
                break;
        }
    }

    // Liberar a memória alocada
    free(processos);
    return 0;
}