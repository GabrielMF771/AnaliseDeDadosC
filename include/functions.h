#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Definição da estrutura Processo
typedef struct {
    int id;
    char numero[50];
    char data_ajuizamento[50];
    char id_classe[50];
    char id_assunto[50];
    int ano_eleicao;
} Processo;

// Funções
int contarLinhas(const char *nomeArquivo);
int lerDados(const char *nomeArquivo, Processo *processos);
int ocorrenciasClasse(Processo *processos, int chave, int tamanho);
void variosAssuntos(Processo *processos, int tamanho);

void calcularDiasTramitacao(Processo *processos, int chave, int totalProcessos);
int dataParaInt(char *data);

// Ordenação
void quicksort(Processo processos[], int low, int high);
int partition(Processo processos[], int low, int high);
void ordenarPorId(Processo processos[], int n);
void ordenarPorData(Processo *processos, int n);
void salvarDadosOrdenados(const char *nomeArquivo, Processo processos[], int n);

#endif // FUNCTIONS_H