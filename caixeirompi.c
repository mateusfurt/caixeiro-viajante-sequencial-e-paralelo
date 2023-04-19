#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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


/* função que lê a matriz do arquico tsp*/
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
/*argoritimo de vizinho mais proximo*/
void *t_caixeiro(int k,int id){
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
    printf("slot(%d)inicial(%d)total: %d\n", id, inicio, total);
    vet[inicio] = total;
    free(v);
    

}


int main(int argc, char **argv) {
    int rank, size;
    int i, j, total = 0, melhor, melhortotal;
    /* Inicializar o MPI*/
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* arquivo aberto*/
    fp = fopen("teste1.tsp", "r");

    /*le o numero de cidades do arquivo tsp*/
    fscanf(fp, "%d", &max);
    if (rank == 0)
    {
        printf("numero de cidades: %d\n", max);
    }
    /*aloca a memoria da matriz e do vetor de visitados*/
    m = malloc(max * sizeof (int*));
    vet = malloc(max * sizeof (int*));
    for ( i = 0; i < max; i++)
    {
        m[i] = malloc(max * sizeof (int));
    }
    
    t_matriz(NULL);
    /*usa o rank como multiplicador pra definir qual intervalo de */
    for ( i = rank*(max/size), j = 0; j < max/size; i++, j++)
    {
        t_caixeiro(i, rank);
    }
    /*a sobra das cidades vão para o ultimo rank*/
    if (rank == size-1){
        if (max%size > 0)
        {
            printf("sobrou %d\n", max%size);
            for (i = max - (max%size), j = 0; j < max%size; i++, j++)
            {
                t_caixeiro(i, rank);
            }
            
        }
        
    }
    /*define a melhor cidade inicial por rank*/
    melhor = 0;
    melhortotal = vet[rank*(max/size)];
    for (i = rank*(max/size), j = 0; j < max/size; i++, j++)
    {
        if (melhortotal > vet[i])
        {
            melhor = i;           
            melhortotal = vet[i];
        }
    }
    /*espera todos os processos para exibir as melhores cidades de cada um*/
    MPI_Barrier( MPI_COMM_WORLD);
    printf("\n\nno rank(%d) %d é a melhor cidade inicial, sendo %d a distancia percorrida para visitar todas as cidades\n\n", rank, melhor, melhortotal);
    
    MPI_Finalize();
    return 0;
}
