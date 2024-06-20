#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PASSWORD "abcd4321" // A senha pré-definida para ser descoberta
#define PASSWORD_LENGTH 8
#define FILE_NAME "dicionario.txt"
#define linha_LENGTH 256

// Função para adicionar a senha ao arquivo
void adiciona_senha_ao_arquivo(const char *password, const char *filename)
{
    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        perror("Não foi possível abrir o arquivo");
        return;
    }

    fprintf(file, "%s\n", password);
    fclose(file);
}

// Função para gerar a próxima tentativa de senha
void gera_nova_tentativa(int numero, char *buffer, int length)
{
    snprintf(buffer, length + 1, "%0*d", length, numero);
}

// Função para ler o arquivo sequencialmente
int le_arquivo(const char *filename, const char *password, int *numero_linha)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Não foi possível abrir o arquivo");
        return 0;
    }

    char linha[linha_LENGTH];
    int contador_linha = 0;
    int encontrado = 0;
    while (fgets(linha, sizeof(linha), file))
    {
        contador_linha++;
        linha[strcspn(linha, "\n")] = '\0'; // Remover nova linha
        if (strcmp(linha, password) == 0)
        {
            *numero_linha = contador_linha;
            encontrado = 1;
            break;
        }
    }

    fclose(file);
    return encontrado;
}

int main()
{
    int encontrado = 0;
    char tentativa[PASSWORD_LENGTH + 1];

    int range = 1;
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        range *= 10;
    }

    double tempo_inicial = clock();

    // Força bruta para encontrar a senha
    for (int i = 0; i < range; i++)
    {
        gera_nova_tentativa(i, tentativa, PASSWORD_LENGTH);
        if (strcmp(tentativa, PASSWORD) == 0)
        {
            encontrado = 1;
            break;
        }
    }

    double tempo_final = clock();
    double tempo_total = (tempo_final - tempo_inicial) / CLOCKS_PER_SEC;

    if (!encontrado)
    {
        int numero_linha = 0;
        encontrado = le_arquivo(FILE_NAME, PASSWORD, &numero_linha);

        if (encontrado)
        {
            printf("Senha encontrada na linha %d.\n", numero_linha);
        }
        else
        {
            // Adiciona a senha ao arquivo se não foi encontrada
            adiciona_senha_ao_arquivo(PASSWORD, FILE_NAME);
            printf("Senha não encontrada. Adicionada ao arquivo.\n");
        }

        printf("Tempo decorrido: %f segundos\n", tempo_total);
    }
    else
    {
        printf("Senha encontrada pelo processo de força bruta.\n");
        printf("Tempo decorrido: %f segundos\n", tempo_total);
    }

    return 0;
}