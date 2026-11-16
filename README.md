# implementa-o-6-deadlocks

# Algoritmo do Banqueiro 

## Sistema Operacional

- Desenvolvido e testado em: Linux (Ubuntu)

## Descrição dos Arquivos

- `banker.c`: Arquivo principal contendo:
  - Leitura e validação de `customer.txt` e `commands.txt` (`RQ`, `RL` e `*`).
  - Verificação de estado seguro
  - Alocação, liberação de recursos, exibição das matrizes e geração de `result.txt`.
- `makefile`: Automação da compilação com `gcc`.
- `evidencias.log`: Registro das sessões de compilação e execução dos testes.

## Como Compilar

Para compilar o programa com o Makefile:

```bash
make
