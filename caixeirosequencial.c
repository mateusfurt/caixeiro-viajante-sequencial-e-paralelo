#include <stdio.h>
#include <stdlib.h>


int *vet, **m, max;
FILE *fp;

/*verifica se a cidade está na lista de visitados*/
int verifica(int a,int v[], int m){
    int i;
    for (i = 0; i < m; i++)
    {
        if (v[i] == a)
        {
            /* printf("existe %d \n", a);*/
            return 1;
        }
        
    }
    /* printf("não existe %d \n", a);*/
    return 0;

}

/*adiciona a cidade a lista de visitados*/
void adiciona(int a,int v[], int m){
    int i;
    for (i = 0; i < m; i++)
    {
        if (v[i] == -1)
        {
            v[i] = a;
            /* printf("adicionado %d \n", a);*/
            break;
        }
        
    }
}

/* inicializa o vetor de visitado com -1 para representar vazio*/
void visitados(int v[]){
    int i;
    for ( i = 1; i < max; i++)
    {
        v[i] = -1;
        /*printf("ação visitados\n");*/
    }
}


/* thread que lê a matriz do arquico tsp*/
void *t_matriz(void *arg){
    int i, j , b;
    for ( i = 0; i < max; i++)
    {
        for ( j = 0; j < max; j++)
        {
            fscanf(fp, "%d", &b);
            m[i][j] = b;
        }
        /* printf("leitura\n");*/
    }
    return NULL;
}
void *t_caixeiro(int k){

    
    /* cada thread vai rodar com um intervalo de cidades/ valor inicial de k é o numero da primeira cidade*/

        int i, j, prox, menor, total = 0, atual, inicio, *v;
        inicio = k;
        atual = k;
        
        v = malloc(max * sizeof (int*));
        visitados(v);
        v[0] = inicio;
        for (j = 0; j < max-1; j++)
        {
            /* valor alto para que o primeiro seja menor inicialmente*/
            menor = 1000;
            for ( i = 0; i < max; i++)
            {
                /*verifica se a distacia a cidade até o vizinho seja menor que o menor determinado anteriormente e se é uma cidade diferente da atual*/
                if (m[atual][i] < menor && i != atual)
                {
                    /*verifica se ja foi visitado*/
                    if (verifica(i, v, max) == 0)
                    {
                        /*determina qual o menor, que sera o proximo destino*/
                        menor = m[atual][i];
                        prox = i;
                    }
                }
            }
            /*o menor é adicionado na lista de visitador e sera o proximo destino*/
            adiciona(prox, v, max);
            /*adiciona a distancia no total percorrido*/
            total += menor;
            atual = prox;
        }
    
    
    
    /*adiciona a distancia da ultima cidade visitada até a cidade inicial*/
    total+=m[atual][inicio];
    /*exibe total percorrido*/
    printf("inicial(%d)total: %d\n", inicio, total);
    vet[inicio] = total;
    free(v);
    

}




int main(int argc, char const *argv[])
{
    int i, j, total = 0, melhor, melhortotal;
    
    /* arquivo aberto*/
    fp = fopen("teste1.tsp", "r");


    /*le o numero de cidades do arquivo tsp e lê em quantas threads vão ser utilizadas a partir do teclado*/
    fscanf(fp, "%d", &max);
    printf("numero de cidades: %d\n", max);

    /*aloca a memoria da matriz e do vetor de visitados*/
    m = malloc(max * sizeof (int*));
    vet = malloc(max * sizeof (int*));

    for ( i = 0; i < max; i++)
    {
        m[i] = malloc(max * sizeof (int));
    }
 
    /*recolhe as informações da matriz do arquivo tsp*/
    t_matriz(NULL);
    printf("leitura finalizada\n");
    for ( i = 0; i < max; i++)
    {
        t_caixeiro(i);
    }

    melhor = 0;
    melhortotal = vet[0];
    for (i = 0; i < max; i++)
    {
        if (melhortotal > vet[i])
        {
            melhor = i;
            melhortotal = vet[i];
        }
    }
    printf("\n\n%d é a melhor cidade inicial, sendo %d a distancia percorrida para visitar todas as cidades\n\n", melhor, melhortotal);

    /*libera memoria alocada*/
    for ( i = 0; i < max; i++)
    {
        free(m[i]);
    }
    free(m);
    free(vet);

    return 0;
}
