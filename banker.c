#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int n_clientes;
    int n_recursos;
    int *disponivel;
    int **maximo;
    int **alocado;
    int **necessidade;
} Banqueiro;

void alocar_sistema(Banqueiro *b, int clientes, int recursos) {
    b->n_clientes = clientes;
    b->n_recursos = recursos;

    b->disponivel = calloc(recursos, sizeof(int));
    b->maximo = malloc(clientes * sizeof(int *));
    b->alocado = malloc(clientes * sizeof(int *));
    b->necessidade = malloc(clientes * sizeof(int *));

    for (int i = 0; i < clientes; i++) {
        b->maximo[i] = calloc(recursos, sizeof(int));
        b->alocado[i] = calloc(recursos, sizeof(int));
        b->necessidade[i] = calloc(recursos, sizeof(int));
    }
}

void liberar_sistema(Banqueiro *b) {
    for (int i = 0; i < b->n_clientes; i++) {
        free(b->maximo[i]);
        free(b->alocado[i]);
        free(b->necessidade[i]);
    }
    free(b->maximo);
    free(b->alocado);
    free(b->necessidade);
    free(b->disponivel);
}

int ler_clientes(Banqueiro *b, const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir %s\n", arquivo);
        return 0;
    }

    char linha[512];
    int total_linhas = 0;

    while (fgets(linha, sizeof(linha), fp)) {
        if (strspn(linha, " \t\r\n") != strlen(linha)) {
            total_linhas++;
        }
    }

    if (total_linhas == 0) {
        fprintf(stderr, "Arquivo %s vazio.\n", arquivo);
        fclose(fp);
        return 0;
    }

    alocar_sistema(b, total_linhas, b->n_recursos);
    rewind(fp);

    int i = 0;
    while (fgets(linha, sizeof(linha), fp)) {
        if (strspn(linha, " \t\r\n") == strlen(linha)) continue;

        int j = 0;
        char *pedaco = strtok(linha, ",");
        while (pedaco != NULL) {
            int val;
            char lixo;
            if (j >= b->n_recursos || sscanf(pedaco, " %d %c", &val, &lixo) != 1 || val < 0) {
                fprintf(stderr, "Erro de formatacao em %s\n", arquivo);
                fclose(fp);
                return 0;
            }
            b->maximo[i][j] = val;
            b->necessidade[i][j] = val;
            j++;
            pedaco = strtok(NULL, ",");
        }

        if (j != b->n_recursos) {
            fprintf(stderr, "Quantidade incorreta de recursos em %s\n", arquivo);
            fclose(fp);
            return 0;
        }
        i++;
    }

    fclose(fp);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <r0> <r1> ...\n", argv[0]);
        return 1;
    }

    Banqueiro b;
    b.n_recursos = argc - 1;

    if (!ler_clientes(&b, "customer.txt")) {
        return 1;
    }

    for (int j = 0; j < b.n_recursos; j++) {
        char lixo;
        if (sscanf(argv[j + 1], " %d %c", &b.disponivel[j], &lixo) != 1 || b.disponivel[j] < 0) {
            fprintf(stderr, "Argumento invalido: %s\n", argv[j + 1]);
            liberar_sistema(&b);
            return 1;
        }
    }
    printf("OK! %d clientes e %d recursos carregados.\n", b.n_clientes, b.n_recursos);
    liberar_sistema(&b);
    return 0;
}