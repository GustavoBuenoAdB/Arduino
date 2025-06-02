#include <stdio.h>

int main()
{
    FILE *arquivo_entrada, *arquivo_saida;
    arquivo_entrada = fopen("palavras_portugues_sem_acento.txt", "r");
    arquivo_saida = fopen("palavras_sa_5letras.txt", "w");

    int num;
    int id = 0;
    char palavra[50];

    palavra[5] = 'a';

    while (fscanf(arquivo_entrada, "%s", palavra) == 1)
    {
        printf("%d %s\n", id, palavra);
        if (palavra[5] == '\0')
        {
            fprintf(arquivo_saida, "%d %s\n", id, palavra);
            id++;
        }
        palavra[5] = 'a';
    }
    fclose(arquivo_entrada);
    fclose(arquivo_saida);    
}