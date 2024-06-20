#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PASSWORD "abcd4321" // A senha pré-definida para ser descoberta
#define PASSWORD_LENGTH 8
#define FILE_NAME "dicionario.txt"
#define LINE_LENGTH 256

// Função para adicionar a senha ao arquivo
void adiciona_senha_no_arquivo(const char* password, const char* filename) {
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo");
        return;
    }

    fprintf(file, "%s\n", password);
    fclose(file);
}

// Função para gerar a próxima tentativa de senha
void gera_nova_tentativa(int numero, char* buffer, int length) {
    snprintf(buffer, length + 1, "%0*d", length, numero);
}

// Função para ler parte do arquivo distribuída entre os processos
int le_parte_do_arquivo(const char* filename, int linha_inicial, int linha_final, const char* password, int* numero_linha) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo");
        return 0;
    }

    char linha[LINE_LENGTH];
    int conta_linha = 0;
    int encontrado = 0;
    while (fgets(linha, sizeof(linha), file)) {
        conta_linha++;
        if (conta_linha < linha_inicial) continue;
        if (conta_linha > linha_final) break;
        linha[strcspn(linha, "\n")] = '\0'; // Remover nova linha
        if (strcmp(linha, password) == 0) {
            *numero_linha = conta_linha;
            encontrado = 1;
            break;
        }
    }

    fclose(file);
    return encontrado;
}

int main(int argc, char* argv[]) {
    int rank, tamanho;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanho);

    int encontrado = 0;
    char tentativa[PASSWORD_LENGTH + 1];

    int range = 1;
    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        range *= 10;
    }

    int inicio = (range / tamanho) * rank;
    int final = (range / tamanho) * (rank + 1);

    if (rank == tamanho - 1) {
        final = range;
    }

    double tempo_inicial = MPI_Wtime();

    for (int i = inicio; i < final; i++) {
        gera_nova_tentativa(i, tentativa, PASSWORD_LENGTH);
        if (strcmp(tentativa, PASSWORD) == 0) {
            encontrado = 1;
            break;
        }
    }

    double tempo_final = MPI_Wtime();
    double tempo_total_local = tempo_final - tempo_inicial;
    double tempo_total_maximo;

    MPI_Reduce(&tempo_total_local, &tempo_total_maximo, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    int global_encontrado;
    MPI_Allreduce(&encontrado, &global_encontrado, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    int global_numero_linha = 0;
    int global_rank = 0;

    if (!global_encontrado) {
        // Dividir a leitura do arquivo entre os processos
        FILE* file = fopen(FILE_NAME, "r");
        if (file == NULL) {
            perror("Não foi possível abrir o arquivo");
            MPI_Finalize();
            return 1;
        }

        int total_linhas = 0;
        char linha[LINE_LENGTH];
        while (fgets(linha, sizeof(linha), file)) {
            total_linhas++;
        }
        fclose(file);

        int linhas_per_process = total_linhas / tamanho;
        int extra_linhas = total_linhas % tamanho;

        int linha_inicial = rank * linhas_per_process + (rank < extra_linhas ? rank : extra_linhas) + 1;
        int linha_final = linha_inicial + linhas_per_process - 1 + (rank < extra_linhas ? 1 : 0);

        if (rank == 0) {
            for (int i = 0; i < tamanho; i++) {
                int s = i * linhas_per_process + (i < extra_linhas ? i : extra_linhas) + 1;
                int e = s + linhas_per_process - 1 + (i < extra_linhas ? 1 : 0);
                printf("Processo %d: lendo linhas de %d a %d\n", i, s, e);
            }
        }

        int numero_linha = 0;
        encontrado = le_parte_do_arquivo(FILE_NAME, linha_inicial, linha_final, PASSWORD, &numero_linha);

        if (encontrado) {
            global_numero_linha = numero_linha;
            global_rank = rank;
        }

        MPI_Allreduce(&encontrado, &global_encontrado, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(&numero_linha, &global_numero_linha, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(&rank, &global_rank, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

        if (rank == 0) {
            if (!global_encontrado) {
                // Adiciona a senha ao arquivo se não foi encontrada
                adiciona_senha_no_arquivo(PASSWORD, FILE_NAME);
                printf("Senha não encontrada. Adicionada ao arquivo.\n");
            } else {
                printf("Senha encontrada pelo nó %d na linha %d.\n", global_rank, global_numero_linha);
            }

            printf("Tempo máximo decorrido: %f segundos\n", tempo_total_maximo);
        }
    } else {
        if (rank == 0) {
            printf("Senha encontrada pelo processo de força bruta.\n");
            printf("Tempo máximo decorrido: %f segundos\n", tempo_total_maximo);
        }
    }

    MPI_Finalize();
    return 0;
}
