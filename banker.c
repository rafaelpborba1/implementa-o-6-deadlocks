#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int n_clientes = 0;
int n_recursos = 0;
int *disponivel = NULL;
int **maximo = NULL;
int **alocado = NULL;
int **necessidade = NULL;

void alocar_matrizes() {
    disponivel = calloc(n_recursos, sizeof(int));
    maximo = malloc(n_clientes * sizeof(int *));
    alocado = malloc(n_clientes * sizeof(int *));
    necessidade = malloc(n_clientes * sizeof(int *));

    for (int i = 0; i < n_clientes; i++) {
        maximo[i] = calloc(n_recursos, sizeof(int));
        alocado[i] = calloc(n_recursos, sizeof(int));
        necessidade[i] = calloc(n_recursos, sizeof(int));
    }
}

void liberar_matrizes() {
    for (int i = 0; i < n_clientes; i++) {
        free(maximo[i]);
        free(alocado[i]);
        free(necessidade[i]);
    }
    free(maximo);
    free(alocado);
    free(necessidade);
    free(disponivel);
}

int eh_vazia(char *linha) {
    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] != ' ' && linha[i] != '\t' && linha[i] != '\n' && linha[i] != '\r') {
            return 0;
        }
    }
    return 1;
}
int ler_customer() {
    FILE *fp = fopen("customer.txt", "r");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir customer.txt\n");
        return 0;
    }

    char linha[512];
    while (fgets(linha, sizeof(linha), fp)) {
        if (!eh_vazia(linha)) {
            n_clientes++;
        }
    }

    if (n_clientes == 0) {
        fprintf(stderr, "Arquivo customer.txt vazio\n");
        fclose(fp);
        return 0;
    }

    alocar_matrizes();
    rewind(fp);

    int i = 0;
    while (fgets(linha, sizeof(linha), fp)) {
        if (eh_vazia(linha)) continue;

        int j = 0;
        char *token = strtok(linha, ",");
        while (token != NULL) {
            int val;
            char extra;
            if (j >= n_recursos || sscanf(token, " %d %c", &val, &extra) != 1 || val < 0) {
                fprintf(stderr, "Erro de formatacao em customer.txt\n");
                fclose(fp);
                liberar_matrizes();
                return 0;
            }
            maximo[i][j] = val;
            necessidade[i][j] = val;
            j++;
            token = strtok(NULL, ",");
        }

        if (j != n_recursos) {
            fprintf(stderr, "Quantidade de recursos invalida em customer.txt\n");
            fclose(fp);
            liberar_matrizes();
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

    n_recursos = argc - 1;

    if (!ler_customer()) {
        return 1;
    }

    for (int j = 0; j < n_recursos; j++) {
        char extra;
        if (sscanf(argv[j + 1], " %d %c", &disponivel[j], &extra) != 1 || disponivel[j] < 0) {
            fprintf(stderr, "Argumento invalido: %s\n", argv[j + 1]);
            liberar_matrizes();
            return 1;
        }
    }
    printf("OK! %d clientes e %d recursos carregados.\n", n_clientes, n_recursos);
    liberar_matrizes();
    return 0;
}