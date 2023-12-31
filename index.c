#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define FILE_PATH "./files/"
#define true 1
#define false 0

// estrutura de coordenadas
typedef struct
{
    int x, y;
} CORD;

// estrutura de ROI que armazena as coordenadas de inicio e fim da palavra
typedef struct
{
    CORD A, B;
} ROI;

// Prototipação das funções
FILE *open_file(char *path);
char **malloc_matrix(int *rows, int *cols);
char **fill_matrix(FILE *file, int *rows, int *cols);
char *create_string(int size);
ROI *create_roi();
ROI *horizontal_forward(char *word, char **matrix, int *rows, int *cols);
ROI *horizontal_backward(char *word, char **matrix, int *rows, int *cols);
ROI *vertical_forward(char *word, char **matrix, int *rows, int *cols);
ROI *vertical_backward(char *word, char **matrix, int *rows, int *cols);
ROI *main_diagonal_forward(char *word, char **matrix, int *rows, int *cols);
ROI *secondary_diagonal_forward(char *word, char **matrix, int *rows, int *cols);
ROI *secondary_diagonal_backward(char *word, char **matrix, int *rows, int *cols);
ROI *main_diagonal_backward(char *word, char **matrix, int *rows, int *cols);
ROI *search_word(char *word, char **matrix, int *rows, int *cols);
void show_matrix(char **matrix, int *rows, int *cols);
void show_roi(ROI *roi);
int has_value(ROI *roi);
void lower_to_upper(char *word);
void destroy_matrix(char **matrix, int *rows);
void create_path(char *path, char *file_name);

int main()
{
    int op = 0;
    int rows, cols;
    char path[100];

    char *file_name = create_string(50);

    printf("Digite o nome do arquivo (deve estar na pasta files): ");
    scanf("%s", file_name);
    scanf("%*c"); // limpar buffer

    create_path(path, file_name); // Cria o caminho para o arquivo (path)

    FILE *file = open_file(path);

    char **matrix = fill_matrix(file, &rows, &cols);
    show_matrix(matrix, &rows, &cols);

    while (op != 1)
    {

        printf("\nSair - 1\n");
        printf("Buscar - 2\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &op);

        if (op == 1)
        {
            printf("Saindo...\n");
            exit(0);
        }
        else if (op == 2)
        {
            char *word = create_string(100);
            printf("Digite a palavra que deseja buscar: ");
            scanf(" %[^\n]", word); // Lê a palavra com espaços
            scanf("%*c");           // limpar buffer

            lower_to_upper(word); // As letras do arquivo são maiúsculas, então é necessário converter o input para maiúsculo

            ROI *roi = search_word(word, matrix, &rows, &cols);

            // Verifica se a ROI tem algum valor e exibe caso positivo
            if (has_value(roi))
            {
                printf("Palavra encontrada.\n");
                show_roi(roi);
            }
            else
            {
                printf("Palavra nao encontrada\n");
            }
            // Liberação de memória
            free(roi);
            free(word);
        }
        else
        {
            printf("Opcao invalida.\n");
        }
    }

    // Liberação de memória
    free(file_name);
    destroy_matrix(matrix, &rows);

    return 0;
}

FILE *open_file(char *path)
{
    FILE *file = fopen(path, "r");

    if (!file)
    {
        printf("Erro ao abrir o arquivo (verifique se está na pasta files).\n");
        exit(1);
    }

    return file;
}

char **malloc_matrix(int *rows, int *cols)
{
    char **matrix;

    matrix = (char **)malloc(*rows * sizeof(char *));

    if (matrix == NULL)
    {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    for (int i = 0; i < *rows; i++)
    {
        matrix[i] = (char *)malloc(*cols * sizeof(char));

        if (matrix[i] == NULL)
        {
            printf("Erro ao alocar memória.\n");
            exit(1);
        }
    }

    return matrix;
}

// Função para preencher a matriz
char **fill_matrix(FILE *file, int *rows, int *cols)
{

    // Detecta qual o tamanho da matriz que esta na primeira linha do arquivo
    fscanf(file, "%d %d", rows, cols);

    printf("Tamanho da matriz: %d x %d\n", *rows, *cols);

    char **matrix = malloc_matrix(rows, cols);

    // Preenche a matriz com os caracteres do arquivo
    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols * 2; j++)
        {
            if (j % 2 == 0)
                fscanf(file, "%*c");
            else
                fscanf(file, "%c", &matrix[i][j / 2]);
        }
    }
    fclose(file);

    return matrix;
}

void create_path(char *path, char *file_name)
{
    strcpy(path, FILE_PATH); // Copia o caminho para a variavel path
    strcat(path, file_name); // Concatena o nome do arquivo com o caminho
}

void destroy_matrix(char **matrix, int *rows)
{
    for (int i = 0; i < *rows; i++)
    {
        free(matrix[i]);
    }

    free(matrix);
}

void lower_to_upper(char *word)
{
    int i = 0;
    while (word[i] != '\0')
    {
        word[i] = toupper(word[i]);
        i++;
    }
}

// Verifica se a ROI tem algum valor
int has_value(ROI *roi)
{
    if (roi->A.x != -1 && roi->A.y != -1 && roi->B.x != -1 && roi->B.y != -1)
    {
        return true;
    }
    return false;
}

// Busca Horizontal (➡️) - RIGHT
ROI *horizontal_forward(char *word, char **matrix, int *rows, int *cols)
{

    ROI *roi = create_roi();        // Cria a ROI (aloca memória e inicializa com -1)
    int length_word = strlen(word); // Tamanho da palavra

    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols; j++)
        {

            if (word[0] == matrix[i][j])
            {
                // printf("Match na posicao [%d][%d]\n\n", i, j);
                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {

                    if (j + k < *cols && word[k] == matrix[i][j + k])
                    {
                        // printf("Match na posicao [%d][%d] = %c \n\n", i, j + k, matrix[i][j + k]);
                        if (++count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i;
                    roi->B.y = j + k;
                    return roi;
                }
            }
        }
    }

    return roi;
}

// 1  2  [3]
// 4  [5]  6
// [7]  8  9

// 3 -> 5 -> 7
ROI *secondary_diagonal_forward(char *word, char **matrix, int *rows, int *cols)
{
    int length_word = strlen(word);
    ROI *roi = create_roi();

    // Percorre a matriz
    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols; j++)
        {
            // Verifica se a primeira letra da palavra é igual a letra da matriz
            if (word[0] == matrix[i][j])
            {
                // printf("Match na posicao [%d][%d]\n\n", i, j);
                int count_equals = 0;
                int k = 0;

                // Percorre a palavra
                for (k = 0; k < length_word; k++)
                {
                    // i + k para evitar que a palavra seja buscada fora da matriz
                    // j - k também para evitar que a palavra seja buscada fora da matriz
                    // printf("Match na posicao [%d][%d]\n\n", i, j);
                    if (i + k < *rows && j - k >= 0 && word[k] == matrix[i + k][j - k])
                    {
                        // printf("Match na posicao [%d][%d]\n\n", i, j);
                        if (++count_equals == length_word)
                            break;
                    }
                }
                // Se o contador de letras iguais for igual ao tamanho da palavra, então a palavra foi encontrada
                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i + k;
                    roi->B.y = j - k;
                    return roi;
                }
            }
        }
    }

    return roi;
}

// 1  2  [3]
// 4  [5]  6
// [7]  8  9

// 7 -> 5 -> 3
ROI *secondary_diagonal_backward(char *word, char **matrix, int *rows, int *cols)
{
    int length_word = strlen(word);
    ROI *roi = create_roi();

    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols; j++)
        {

            if (word[0] == matrix[i][j])
            {
                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {
                    if (i - k >= 0 && j + k < *cols && word[k] == matrix[i - k][j + k])
                    {
                        // printf("Match na posicao [%d][%d] = %c \n\n", i - k, j + k, matrix[i - k][j + k]);
                        count_equals++;
                        if (count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i - k;
                    roi->B.y = j + k;
                    return roi;
                }
            }
        }
    }
    return roi;
}

// [1]  2  3
// 4  [5]  6
// 7  8  [9]

// 9 -> 5 -> 1

ROI *main_diagonal_backward(char *word, char **matrix, int *rows, int *cols)
{

    int length_word = strlen(word);
    ROI *roi = create_roi();

    for (int i = *rows - 1; i > -1; i--)
    {
        for (int j = *cols - 1; j > -1; j--)
        {

            if (word[0] == matrix[i][j])
            {
                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {
                    if (i - k >= 0 && j - k >= 0 && word[k] == matrix[i - k][j - k])
                    {
                        if (++count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i - k;
                    roi->B.y = j - k;
                    return roi;
                }
            }
        }
    }
    return roi;
}

// [1]  2  3
// 4  [5]  6
// 7  8  [9]

// 1 -> 5 -> 9

ROI *main_diagonal_forward(char *word, char **matrix, int *rows, int *cols)
{
    int length_word = strlen(word);
    ROI *roi = create_roi();

    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols; j++)
        {

            if (word[0] == matrix[i][j])
            {
                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {
                    // i + k < *rows e j + k < * cols garantir que não vai acessar posições fora da matriz
                    if (i + k < *rows && j + k < *cols && word[k] == matrix[i + k][j + k])
                    {
                        if (++count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i + k; // i + j é onde ele termina
                    roi->B.y = j + k;
                    return roi;
                }
            }
        }
    }

    return roi;
}

ROI *vertical_backward(char *word, char **matrix, int *rows, int *cols)
{
    ROI *roi = create_roi();
    int length_word = strlen(word);

    for (int i = *rows - 1; i > -1; i--)
    {
        for (int j = *cols - 1; j > -1; j--)
        {

            if (word[0] == matrix[i][j])
            {
                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {
                    if (i - k >= 0 && word[k] == matrix[i - k][j])
                    {
                        if (++count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i - k;
                    roi->B.y = j;
                    return roi;
                }
            }
        }
    }

    return roi;
}

// Busca Vertical (⬆️) - UP
ROI *vertical_forward(char *word, char **matrix, int *rows, int *cols)
{

    ROI *roi = create_roi();
    int length_word = strlen(word);

    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols; j++)
        {

            if (word[0] == matrix[i][j])
            {
                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {
                    if (i + k < *rows && word[k] == matrix[i + k][j])
                    {
                        if (++count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i + k;
                    roi->B.y = j;
                    return roi;
                }
            }
        }
    }

    return roi;
}

// Busca Horizontal Reversa (⬅️) - LEFT
ROI *horizontal_backward(char *word, char **matrix, int *rows, int *cols)
{
    ROI *roi = create_roi();
    int length_word = strlen(word);

    for (int i = *rows - 1; i > -1; i--)
    {
        for (int j = *cols - 1; j > -1; j--)
        {

            if (word[0] == matrix[i][j])
            {

                int count_equals = 0;
                int k = 0;

                for (k = 0; k < length_word; k++)
                {
                    // printf("matrix[%d][%d] = %c\n", i, j - k, matrix[i][j - k]);
                    if (j - k >= 0 && word[k] == matrix[i][j - k])
                    {
                        if (++count_equals == length_word)
                            break;
                    }
                }

                if (count_equals == length_word)
                {
                    roi->A.x = i;
                    roi->A.y = j;
                    roi->B.x = i;
                    roi->B.y = j - k;
                    return roi;
                }
            }
        }
    }

    return roi;
}

// Função que unifica as funções de busca
ROI *search_word(char *word, char **matrix, int *rows, int *cols)
{
    ROI *roi = create_roi();

    printf("\nBuscando a palavra: %s\n", word);

    roi = horizontal_forward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = horizontal_backward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = vertical_forward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = vertical_backward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = main_diagonal_forward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = secondary_diagonal_forward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = secondary_diagonal_backward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    roi = main_diagonal_backward(word, matrix, rows, cols);
    if (has_value(roi))
    {
        return roi;
    }

    return roi;
}

// Função para mostrar a matriz
void show_matrix(char **matrix, int *rows, int *cols)
{
    printf("\n");

    for (int i = 0; i < *rows; ++i)
    {
        printf("%d ", i);
        for (int j = 0; j < *cols; j++)
        {
            printf("%c\t", matrix[i][j]);
        }
        printf("\n");
    }
}

// Função para alocar memória para uma string
char *create_string(int size)
{
    char *word = (char *)malloc(size * sizeof(char));

    if (word == NULL)
    {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    return word;
}

// Função para alocar memória e inicializar os valores da ROI
ROI *create_roi()
{
    ROI *roi = malloc(sizeof(ROI));

    if (roi == NULL)
    {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    roi->A.x = -1;
    roi->A.y = -1;
    roi->B.x = -1;
    roi->B.y = -1;

    return roi;
}

// Função para imprimir os vaores das coordenadas da ROI
void show_roi(ROI *roi)
{
    printf("linha inicial: %d, coluna inicial: %d\n", roi->A.x, roi->A.y);
    printf("linha final: %d, coluna final: %d\n", roi->B.x, roi->B.y);
}