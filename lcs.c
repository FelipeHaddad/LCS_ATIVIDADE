// Felipe Bertacco Haddad
// RA: 10437372

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 1000

/* Lê todo o conteúdo do arquivo `filename` até MAXLEN bytes.
   Retorna uma string alocada dinamicamente (nula-terminada) e coloca o tamanho em *out_len.
   Em erro, imprime mensagem e termina o programa. */
char *read_string_file(const char *filename, int *out_len) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        fprintf(stderr, "Erro ao ler o arquivo '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    long sz = ftell(f);
    if (sz < 0) sz = 0;
    rewind(f);

    if (sz > MAXLEN) sz = MAXLEN; /* respeita o limite do enunciado */

    char *buf = (char*) malloc((size_t)sz + 1);
    if (!buf) {
        fclose(f);
        fprintf(stderr, "Erro de memoria\n");
        exit(EXIT_FAILURE);
    }

    size_t r = fread(buf, 1, (size_t)sz, f);
    buf[r] = '\0';
    fclose(f);

    /* Remove finais de linha/carriage return se houver */
    while (r > 0 && (buf[r-1] == '\n' || buf[r-1] == '\r')) {
        buf[--r] = '\0';
    }

    *out_len = (int) r;
    return buf;
}

int main(void) {
    int m, n;
    char *s1 = read_string_file("string1", &m);
    char *s2 = read_string_file("string2", &n);

    /* tabela dp: (m+1) x (n+1) inteiros, alocada em bloco contíguo */
    int rows = m + 1;
    int cols = n + 1;
    long total = (long)rows * cols;
    int *block = (int*) calloc((size_t) total, sizeof(int));
    if (!block) {
        fprintf(stderr, "Erro de memoria ao alocar DP\n");
        free(s1); free(s2);
        exit(EXIT_FAILURE);
    }
    int **dp = (int**) malloc((size_t) rows * sizeof(int*));
    if (!dp) {
        fprintf(stderr, "Erro de memoria ao alocar DP ponteiros\n");
        free(block); free(s1); free(s2);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; ++i) dp[i] = block + i * cols;

    /* Preenchimento da tabela DP */
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = (dp[i-1][j] >= dp[i][j-1]) ? dp[i-1][j] : dp[i][j-1];
            }
        }
    }

    int lcs_len = dp[m][n];
    char *lcs = (char*) malloc((size_t) lcs_len + 1);
    if (!lcs && lcs_len > 0) {
        fprintf(stderr, "Erro de memoria ao alocar LCS\n");
        free(block); free(dp); free(s1); free(s2);
        exit(EXIT_FAILURE);
    }

    /* Reconstrução de uma LCS (backtracking) */
    int i = m, j = n, pos = lcs_len - 1;
    while (i > 0 && j > 0) {
        if (s1[i-1] == s2[j-1]) {
            lcs[pos--] = s1[i-1];
            i--; j--;
        } else if (dp[i-1][j] >= dp[i][j-1]) {
            i--;
        } else {
            j--;
        }
    }
    if (lcs_len > 0) lcs[lcs_len] = '\0';
    else {
        /* string vazia para lcs */
        lcs[0] = '\0';
    }

    /* Saída */
    printf("Tamanho da LCS: %d\n", lcs_len);
    if (lcs_len > 0) {
        printf("Uma LCS: %s\n", lcs);
    } else {
        printf("Nao ha subsequencia comum (LCS vazia).\n");
    }

    /* limpeza */
    free(lcs);
    free(block);
    free(dp);
    free(s1);
    free(s2);
    return 0;
}
