#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Função para remover espaços no início e fim da string
void trim(char *str) {
    int len = strlen(str);
    // Remove espaços no fim
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
    // Move o ponteiro para o primeiro caractere não branco
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    // Se houve espaços no início, move o conteúdo da string pra frente
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

// Função para contar o número de linhas no arquivo CSV
int contarLinhas(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    int linhas = 0;
    char linha[512];

    // Contar as linhas
    while (fgets(linha, sizeof(linha), arquivo)) {
        linhas++;
    }

    fclose(arquivo);
    return linhas - 1; // Subtrair 1 para ignorar o cabeçalho
}

// Função para ler os dados do arquivo CSV
int lerDados(const char *nomeArquivo, Processo *processos) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    char linha[512];
    int i = 0;

    // Ignorar o cabeçalho
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        // Remove o '\n' no final da linha, se existir
        linha[strcspn(linha, "\n")] = '\0';

        // Array para armazenar os campos extraídos
        char campos[6][256] = { {0} };
        int campoIndex = 0;
        char *ptr = linha;

        while (*ptr && campoIndex < 6) {
            // Se o campo começa com aspas, precisamos buscar o fechamento, que pode incluir vírgulas
            if (*ptr == '"') {
                ptr++;
                char *start = ptr;
                while (*ptr && *ptr != '"') {
                    ptr++;
                }
                int len = ptr - start;
                strncpy(campos[campoIndex], start, len);
                campos[campoIndex][len] = '\0';
                if (*ptr == '"') {
                    ptr++;
                }
                if (*ptr == ',') {
                    ptr++;
                }
            } else { 
                // Campo sem aspas (formato simples, sem vírgulas internas)
                char *start = ptr;
                while (*ptr && *ptr != ',') {
                    ptr++;
                }
                int len = ptr - start;
                strncpy(campos[campoIndex], start, len);
                campos[campoIndex][len] = '\0';
                if (*ptr == ',') {
                    ptr++;
                }
            }
            campoIndex++;
        }

        // Ppopula a estrutura Processo usando os campos extraídos
        processos[i].id = atoi(campos[0]);

        // Campo numero já foi processado, não incluir aspas adicionais
        strncpy(processos[i].numero, campos[1], sizeof(processos[i].numero));

        // Campo data_ajuizamento
        strncpy(processos[i].data_ajuizamento, campos[2], sizeof(processos[i].data_ajuizamento));

        // Campo id_classe:
        if (campos[3][0] != '{') {
            snprintf(processos[i].id_classe, sizeof(processos[i].id_classe), "{%s}", campos[3]);
        } else {
            strncpy(processos[i].id_classe, campos[3], sizeof(processos[i].id_classe));
        }

        // Campo id_assunto:
        if (campos[4][0] != '{') {
            snprintf(processos[i].id_assunto, sizeof(processos[i].id_assunto), "{%s}", campos[4]);
        } else {
            strncpy(processos[i].id_assunto, campos[4], sizeof(processos[i].id_assunto));
        }

        processos[i].ano_eleicao = atoi(campos[5]);
        i++;
    }

    fclose(arquivo);
    return i;
}

void quicksort(Processo processos[], int low, int high) {
    if (low < high) {
        // Particionar o array e obter o índice do pivô
        int pivotIndex = partition(processos, low, high);

        // Ordenar as duas metades recursivamente
        quicksort(processos, low, pivotIndex - 1);
        quicksort(processos, pivotIndex + 1, high);
    }
}

int partition(Processo processos[], int low, int high) {
    // Escolher o último elemento como pivô
    int pivot = processos[high].id;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (processos[j].id <= pivot) {
            i++;
            // Trocar processos[i] e processos[j]
            Processo temp = processos[i];
            processos[i] = processos[j];
            processos[j] = temp;
        }
    }

    // Colocar o pivô na posição correta
    Processo temp = processos[i + 1];
    processos[i + 1] = processos[high];
    processos[high] = temp;

    return i + 1;
}

// Função para ordenar os processos pelo atributo id
void ordenarPorId(Processo processos[], int n) {
    quicksort(processos, 0, n - 1);
}

// Função para salvar os dados ordenados em um arquivo CSV
void salvarDadosOrdenados(const char *nomeArquivo, Processo processos[], int n) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        perror("Erro ao criar o arquivo");
        return;
    }

    // Escrever o cabeçalho (ajuste os delimitadores conforme necessário)
    fprintf(arquivo, "id;numero;data_ajuizamento;id_classe;id_assunto;ano_eleicao\n");

    for (int i = 0; i < n; i++) {
        // Imprime o id, numero e data_ajuizamento como de costume.
        fprintf(arquivo, "%d,\"%s\",%s,", 
                processos[i].id,
                processos[i].numero,
                processos[i].data_ajuizamento);

        // Campo id_classe:
        if (strchr(processos[i].id_classe, ',') != NULL) {
            fprintf(arquivo, "\"%s\",", processos[i].id_classe);
        } else {
            fprintf(arquivo, "%s,", processos[i].id_classe);
        }

        // Campo id_assunto:
        if (strchr(processos[i].id_assunto, ',') != NULL) {
            fprintf(arquivo, "\"%s\",", processos[i].id_assunto);
        } else {
            fprintf(arquivo, "%s,", processos[i].id_assunto);
        }

        // Imprime o ano da eleição
        fprintf(arquivo, "%d\n", processos[i].ano_eleicao);
    }

    fclose(arquivo);
}

int ocorrenciasClasse(Processo *processos, int chave, int tamanho){
    int ocorrencias = 0;
    int i ;
    char aux[50] = "";
    int auxint = 0;
        for(i = 0; i < tamanho; i++){
            int k =0;
            for(int j = 1; j < 50; j++){

                //ATRIBUINDO A STRING AUXILIAR ATE A VIRGULA OU CHAVE FINAL
               if(processos[i].id_classe[j] != '}' && processos[i].id_classe[j] != ','){

                aux[k] = processos[i].id_classe[j];
                k++;
               }
            
               else{   
                    if(processos[i].id_classe[j] == '}'){ //SE TIVER CHEGADO NA CHAVE FINAL
                        auxint = atoi(aux);
                        if(auxint == chave)
                        ocorrencias++;
                        memset(aux, '\0', sizeof(aux));
                        continue;

                    }else{ //SE TIVER CHEGADO NUMA VIRGULA
                        auxint = atoi(aux);
                        if(auxint == chave)
                        ocorrencias++;
                        memset(aux, '\0', sizeof(aux));
                        k = 0;
                    }
               }
            } 
        }
    return ocorrencias;
}

void variosAssuntos(Processo *processos, int tamanho){

    int repeticoes =0;
    int indice = 0;
    printf("\nLista de processos com mais de 1 assunto\n\n");
   
    for(int i = 0; i < tamanho; i++){
      repeticoes =0;
         for(int j = 1; j <50; j++){

            if(processos[i].id_assunto[j] == ','){
                if(repeticoes<1){
                indice++;
                printf("\n\t%d) --> %d",indice,processos[i].id);
                }
                repeticoes++;
                continue;
            }
         }
    }   
    printf("\n\n\tTotal de processos que possuem mais de 1 assunto: '%d'\n\n",indice);
}

void ordenarPorData(Processo *processos, int n) {
    Processo temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (strcmp(processos[j].data_ajuizamento, processos[j + 1].data_ajuizamento) < 0) {
                temp = processos[j];
                processos[j] = processos[j + 1];
                processos[j + 1] = temp;
            }
        }
    }
}

void calcularDiasTramitacao(Processo *processos, int chave, int totalProcessos) {
    time_t t = time(NULL);
    int dias_tramitacao = -1; // Valor padrão caso o ID não seja encontrado

    for (int i = 0; i < totalProcessos; i++){
        if(processos[i].id == chave){
            struct tm data_ajuizamento_tm = {0};
            if (sscanf(processos[i].data_ajuizamento, "%4d-%2d-%2d",
                       &data_ajuizamento_tm.tm_year,
                       &data_ajuizamento_tm.tm_mon,
                       &data_ajuizamento_tm.tm_mday) != 3) {
                printf("Erro ao processar a data de ajuizamento.\n");
                return;
            }

            data_ajuizamento_tm.tm_year -= 1900; // Ajusta o ano
            data_ajuizamento_tm.tm_mon -= 1; // Ajusta o mês

            time_t data_ajuizamento_time = mktime(&data_ajuizamento_tm);
            if (data_ajuizamento_time == -1) {
                printf("Erro ao converter a data de ajuizamento para time_t.\n");
                return;
            }

            double diferenca_segundos = difftime(t, data_ajuizamento_time);
            dias_tramitacao = (int)(diferenca_segundos / (60 * 60 * 24));

            printf("\nDias de tramitacao: %d\n\n", dias_tramitacao);
            return;
        }
    }

    // Se chegou aqui, o ID não foi encontrado
    printf("Processo com ID %d não encontrado.\n", chave);
}

void funcaoGuilherme(const char *nomeArquivo){

char line[MAX_LINE];
char *id_assuntos[MAX_ASSUNTOS];
int count = 0;

FILE *arquivo = fopen(nomeArquivo, "r");
if (!arquivo) {
    perror("Erro ao abrir o arquivo");
    return 0;
}

// Ignorar o cabeçalho
fgets(line, MAX_LINE, arquivo);

while (fgets(line, MAX_LINE, arquivo)) {
    char temp[MAX_LINE];
    strcpy(temp, line);

    char *token = strtok(temp, ","); // coluna 1: id
    for (int i = 1; i < 4 && token != NULL; i++) {
        token = strtok(NULL, ","); // pula até a 4ª coluna
    }

    if (token) {
        char *id_assunto_raw = strtok(NULL, ",");

        if (!id_assunto_raw) continue;

        char *abre = strchr(id_assunto_raw, '{');
        char *fecha = strchr(id_assunto_raw, '}');

        if (!abre || !fecha || fecha <= abre + 1) continue;

        char buffer[MAX_LINE];
        strncpy(buffer, abre + 1, fecha - abre - 1);
        buffer[fecha - abre - 1] = '\0';

        char *subtoken = strtok(buffer, ",");
        while (subtoken) {
            trim(subtoken);

            int exists = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(id_assuntos[i], subtoken) == 0) {
                    exists = 1;
                    break;
                }
            }

            if (!exists && count < MAX_ASSUNTOS) {
                id_assuntos[count++] = strdup(subtoken);
            }

            subtoken = strtok(NULL, ",");
        }
    }
}

fclose(arquivo);

printf("\n\n\nTotal de id_assuntos distintos: %d\n\n", count);

for (int i = 0; i < count; i++) {
    printf("- %s\n", id_assuntos[i]);
    free(id_assuntos[i]);
}

}
