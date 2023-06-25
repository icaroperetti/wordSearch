#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define FILE_PATH "./files/"
#define true 1
#define false 0

typedef struct
{
  int x, y;
} CORD;

typedef struct
{
  CORD A, B;
} ROI;

FILE *open_file(char *path);
char **malloc_matrix(int *rows, int *cols);
char **fill_matrix(FILE *file, int *rows, int *cols);
char *create_string(int size);
ROI *create_roi();
ROI *horizontal_forward(char *word, char **matrix, int *rows, int *cols);
ROI *horizontal_backward(char *word, char **matrix, int *rows, int *cols);
ROI *vertical_forward(char *word, char **matrix, int *rows, int *cols);
ROI *vertical_backward(char *word, char **matrix, int *rows, int *cols);
ROI *diagonal_SE(char *word, char **matrix, int *rows, int *cols);
ROI *diagonal_SW(char *word, char **matrix, int *rows, int *cols);
ROI *diagonal_NE(char *word, char **matrix, int *rows, int *cols);
ROI *diagonal_NW(char *word, char **matrix, int *rows, int *cols);
ROI *search_word(char *word, char **matrix, int *rows, int *cols);
void show_matrix(char **matrix, int *rows, int *cols);
void show_roi(ROI *roi);
int has_value(ROI *roi);
void lowerToUpper(char *word);

int main()
{
  int flag = 0;
  int rows, cols;
  char path[100];

  char *file_name = create_string(50);
  printf("Digite o nome do arquivo (deve estar na pasta files): ");
  scanf("%s", file_name);
  scanf("%*c"); // limpar buffer

  strcpy(path, FILE_PATH);

  strcat(path, file_name);

  printf("Abrindo arquivo: %s\n", path);

  FILE *file = open_file(path);

  char **matrix = fill_matrix(file, &rows, &cols);
  show_matrix(matrix, &rows, &cols);

  while (flag != 1)
  {

    printf("\nSair - 1\n");
    printf("Buscar - 2\n");
    printf("Escolha a opcao para buscar a palavra: ");
    scanf("%d", &flag);

    if (flag == 1)
    {
      printf("Saindo...\n");
      exit(0);
    }
    else if (flag == 2)
    {
      char *word = create_string(100);
      printf("Digite a palavra que deseja buscar: ");
      scanf(" %[^\n]", word);
      scanf("%*c"); // limpar buffer

      lowerToUpper(word); // As letras do arquivo são maiúsculas, então é necessário converter a palavra para maiúscula

      printf("\nBuscando a palavra: %s\n", word);

      ROI *roi = search_word(word, matrix, &rows, &cols);

      if (has_value(roi))
      {
        printf("Palavra encontrada.\n");
        show_roi(roi);
      }
      else
      {
        printf("Palavra nao encontrada.\n");
      }
      free(word);
      free(roi);
    }
    else
    {
      printf("Opcao invalida.\n");
    }
  }

  for (int i = 0; i < rows; i++)
  {
    free(matrix[i]);
  }

  free(matrix);

  return 0;
}

void lowerToUpper(char *word)
{
  int i = 0;
  while (word[i] != '\0')
  {
    word[i] = toupper(word[i]);
    i++;
  }
}

// Função que unifica as funções de busca
ROI *search_word(char *word, char **matrix, int *rows, int *cols)
{
  ROI *roi = create_roi();

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

  roi = diagonal_SE(word, matrix, rows, cols);
  if (has_value(roi))
  {
    return roi;
  }

  roi = diagonal_SW(word, matrix, rows, cols);
  if (has_value(roi))
  {
    return roi;
  }

  roi = diagonal_NE(word, matrix, rows, cols);
  if (has_value(roi))
  {
    return roi;
  }

  roi = diagonal_NW(word, matrix, rows, cols);
  if (has_value(roi))
  {
    return roi;
  }

  return roi;
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

// Busca na diagonal Sudoeste

// [1]  2  3
// 4  [5]  6
// 7  8  [9]

// 1 -> 5 -> 9
ROI *diagonal_SW(char *word, char **matrix, int *rows, int *cols)
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
          // i + k para evitar que a palavra seja buscada fora da matriz
          // j - k também para evitar que a palavra seja buscada fora da matriz
          if (i + k < *rows && j - k >= 0 && word[k] == matrix[i + k][j - k])
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
          roi->B.y = j - k;
          return roi;
        }
      }
    }
  }

  return roi;
}

//  Busca na diagonal (↗️) Nordeste

// 1  2  [3]
// 4  [5]  6
// [7]  8  9

// 7 -> 5 -> 3
ROI *diagonal_NE(char *word, char **matrix, int *rows, int *cols)
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

//  Busca na Diagonal (↖️) - NW (Noroeste)
// 1  2  [3]
// 4  [5]  6
// [7]  8  9

// 3 -> 5 -> 7

ROI *diagonal_NW(char *word, char **matrix, int *rows, int *cols)
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

// Busca na Diagonal Sudeste
// [1]  2  3
// 4  [5]  6
// 7  8  [9]

// 9 -> 5 -> 1

ROI *diagonal_SE(char *word, char **matrix, int *rows, int *cols)
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

// Busca Horizontal (➡️) - RIGHT
ROI *horizontal_forward(char *word, char **matrix, int *rows, int *cols)
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
          if (j + k < *cols && word[k] == matrix[i][j + k])
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
          roi->B.y = j + k;
          return roi;
        }
      }
    }
  }

  return roi;
}

FILE *open_file(char *path)
{
  FILE *file = fopen(path, "r");

  if (!file)
  {
    perror("Erro ao abrir o arquivo.\n");
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
    perror("Erro ao alocar memória.\n");
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
    perror("Erro ao alocar memória.\n");
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
  printf("x inicial: %d, y inicial: %d\n", roi->A.x, roi->A.y);
  printf("x inicial: %d, y final: %d\n", roi->B.x, roi->B.y);
}
