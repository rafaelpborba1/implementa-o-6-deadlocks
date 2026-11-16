#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int total_clientes = 0;
int total_recursos = 0;
int *disponivel = NULL;
int **maximo = NULL;
int **alocado = NULL;
int **necessidade = NULL;

void limpar_tudo() {
    for (int c = 0; c < total_clientes; c++) {
        free(maximo[c]);
        free(alocado[c]);
        free(necessidade[c]);
    }
    free(maximo);
    free(alocado);
    free(necessidade);
    free(disponivel);
}

int carregar_clientes() {
    FILE *arq = fopen("customer.txt", "r");
    if (!arq) {
        fprintf(stderr, "Erro ao abrir customer.txt\n");
        return 0;
    }

    char buf[512];
    while (fgets(buf, sizeof(buf), arq)) {
        int tem_dado = 0;
        for (int k = 0; buf[k] != '\0'; k++) {
            if (buf[k] != ' ' && buf[k] != '\t' && buf[k] != '\n' && buf[k] != '\r') {
                tem_dado = 1;
                break;
            }
        }
        if (tem_dado) total_clientes++;
    }

    if (total_clientes == 0) {
        fprintf(stderr, "Arquivo customer.txt vazio\n");
        fclose(arq);
        return 0;
    }

    disponivel = calloc(total_recursos, sizeof(int));
    maximo = malloc(total_clientes * sizeof(int *));
    alocado = malloc(total_clientes * sizeof(int *));
    necessidade = malloc(total_clientes * sizeof(int *));

    for (int c = 0; c < total_clientes; c++) {
        maximo[c] = calloc(total_recursos, sizeof(int));
        alocado[c] = calloc(total_recursos, sizeof(int));
        necessidade[c] = calloc(total_recursos, sizeof(int));
    }

    rewind(arq);
    int c = 0;
    while (fgets(buf, sizeof(buf), arq)) {
        int r = 0;
        char *parte = strtok(buf, ",");
        int leu_algo = 0;

        while (parte != NULL) {
            int vazio = 1;
            for (int k = 0; parte[k] != '\0'; k++) {
                if (parte[k] != ' ' && parte[k] != '\t' && parte[k] != '\n' && parte[k] != '\r') {
                    vazio = 0;
                    break;
                }
            }
            if (vazio && r == 0) break;

            leu_algo = 1;
            int valor;
            char sobra;
            if (r >= total_recursos || sscanf(parte, " %d %c", &valor, &sobra) != 1 || valor < 0) {
                fprintf(stderr, "Erro de formatacao em customer.txt\n");
                fclose(arq);
                limpar_tudo();
                return 0;
            }
            maximo[c][r] = valor;
            necessidade[c][r] = valor;
            r++;
            parte = strtok(NULL, ",");
        }

        if (!leu_algo) continue;

        if (r != total_recursos) {
            fprintf(stderr, "Quantidade de recursos invalida em customer.txt\n");
            fclose(arq);
            limpar_tudo();
            return 0;
        }
        c++;
    }

    fclose(arq);
    return 1;
}

int verificar_seguranca() {
    int *trabalho = malloc(total_recursos * sizeof(int));
    int *fim = calloc(total_clientes, sizeof(int));

    for (int r = 0; r < total_recursos; r++) {
        trabalho[r] = disponivel[r];
    }

    for (int c = 0; c < total_clientes; c++) {
        if (fim[c]) continue;

        int atende = 1;
        for (int r = 0; r < total_recursos; r++) {
            if (necessidade[c][r] > trabalho[r]) {
                atende = 0;
                break;
            }
        }

        if (atende) {
            for (int r = 0; r < total_recursos; r++) {
                trabalho[r] += alocado[c][r];
            }
            fim[c] = 1;
            c = -1;
        }
    }

    int seguro = 1;
    for (int c = 0; c < total_clientes; c++) {
        if (!fim[c]) {
            seguro = 0;
            break;
        }
    }

    free(trabalho);
    free(fim);
    return seguro;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <r0> <r1> ...\n", argv[0]);
        return 1;
    }

    total_recursos = argc - 1;

    if (!carregar_clientes()) {
        return 1;
    }

    for (int r = 0; r < total_recursos; r++) {
        char sobra;
        if (sscanf(argv[r + 1], " %d %c", &disponivel[r], &sobra) != 1 || disponivel[r] < 0) {
            fprintf(stderr, "Argumento invalido: %s\n", argv[r + 1]);
            limpar_tudo();
            return 1;
        }
    }

    printf("Estado inicial seguro? %s\n", verificar_seguranca() ? "SIM" : "NAO");

    limpar_tudo();
    return 0;
}