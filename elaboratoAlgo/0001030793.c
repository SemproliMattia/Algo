/* 
	Nome: Mattia
	Cognome: Semproli
	Numero di matricola: 0001030793
	Gruppo: B
	Email: mattia.semproli2@studio.unibo.it
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*Uso un enum anche per le 4 direzioni in cui posso muovermi*/
enum Direzioni {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

struct Queue { 
    struct QueueItem* item;
    int testa, coda, cap;
};

struct QueueItem {
    int riga;
    int colonna;
    int distanza;
};

struct Queue* queue_create(void)
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    assert(q != NULL);

    q->cap = 2;
    q->item = (struct QueueItem*)malloc(q->cap * sizeof(struct QueueItem));
    assert(q->item != NULL);
    q->testa = q->coda = 0;
    return q;
}

int queue_size(const struct Queue* q)
{
    assert(q != NULL);

    if (q->testa <= q->coda) {
        return (q->coda - q->testa);
    }
    else {
        return (q->cap - (q->testa - q->coda));
    }
}

static int queue_is_full(const struct Queue* q)
{
    assert(q != NULL);

    /* return q->head == ((q->tail + 1) % q->capacity); */
    return (queue_size(q) == (q->cap - 1));
}

int queue_is_empty(const struct Queue* q)
{
    assert(q != NULL);

    return q->coda == q->testa;
}

static void queue_resize(struct Queue* q, int new_capacity)
{
    int cur_size, cur_capacity;
    struct QueueItem* new_data;

    assert(q != NULL);
    cur_size = queue_size(q);
    assert(new_capacity >= cur_size);
    cur_capacity = q->cap;
    new_data = (struct QueueItem*)malloc(new_capacity * sizeof(struct QueueItem));
    assert(new_data != NULL);
    /* copiamo i dati dal vecchio al nuovo buffer */
    if (q->testa <= q->coda) {
        /* Se i dati nella coda sono memorizzati in un segmento
           contiguo dell'array, basta una sola copia */
        memcpy(new_data,
            q->item + q->testa,
            cur_size * sizeof(struct QueueItem));
    }
    else {
        /* Invece, se i dati in coda sono memorizzati in due segmenti
           (cioè, se l'array circolare "torna indietro", ossia
           l'indice finale è minore dell'indice iniziale), sono
           necessarie due operazioni di copia. */
        memcpy(new_data,
            q->item + q->testa,
            (cur_capacity - q->testa) * sizeof(struct QueueItem));
        memcpy(new_data + (cur_capacity - q->testa),
            q->item,
            q->coda * sizeof(struct QueueItem));
    }
    free(q->item);
    q->cap = new_capacity;
    q->item = new_data;
    q->testa = 0;
    q->coda = cur_size;
}

void queue_enqueue(struct Queue* q, struct QueueItem val)
{
    assert(q != NULL);

    if (queue_is_full(q)) {
        queue_resize(q, q->cap * 2);
    }

    q->item[q->coda] = val;
    q->coda = (q->coda + 1) % q->cap;
}

void queue_dequeue(struct Queue* q)
{
    assert(q != NULL);
    assert(!queue_is_empty(q));

    q->testa = (q->testa + 1) % q->cap;

    if (queue_size(q) <= q->cap / 4) {
        queue_resize(q, q->cap / 2);
    }
}

void queue_destroy(struct Queue* q)
{
    assert(q != NULL);

    free(q->item);
    q->item = NULL;
    q->testa = q->coda = -1;
    q->cap = 0;
    free(q);
}

/*Metodo booleano per controllare se ho uno spazio 3x3 per passare*/
int controlloSpazio(enum Direzioni direzione, int riga, int colonna, int** matriceFile, int r, int c) {
    switch (direzione) {
    case UP:
        if ((colonna + 2) < c) {
            if (matriceFile[riga][colonna] == 1 ||
                matriceFile[riga][colonna + 1] == 1 ||
                matriceFile[riga][colonna + 2] == 1)
                return 0;
        }
        else
            return 0;
        break;
    case DOWN:
        if ((riga + 2) < r && (colonna + 2) < c) {
            if (matriceFile[riga + 2][colonna] == 1 ||
                matriceFile[riga + 2][colonna + 1] == 1 ||
                matriceFile[riga + 2][colonna + 2] == 1)
                return 0;
        }
        else
            return 0;
        break;
    case RIGHT:
        if ((riga + 2) < r && (colonna + 2) < c) {
            if (matriceFile[riga][colonna + 2] == 1 ||
                matriceFile[riga + 1][colonna + 2] == 1 ||
                matriceFile[riga + 2][colonna + 2] == 1)
                return 0;
        }
        else
            return 0;
        break;
    case LEFT:
        if ((riga + 2) < r) {
            if (matriceFile[riga][colonna] == 1 ||
                matriceFile[riga + 1][colonna] == 1 ||
                matriceFile[riga + 2][colonna] == 1)
                return 0;
        }
        else
            return 0;
        break;
    }
    return 1;
}

/*Metodo per calcolare il percorso ottimo
Uso la BFS sulla matrice per calcolarlo, tenendo anche conto del percorso effettuato nella matrice matriceMosse*/
int calcDistanzaMin(int** matriceMosse, int** matriceFile, int r, int c) {
    struct Queue* q = queue_create();
    /*Sorgente da cui parto, item di appoggio e primo elemento della coda*/
    struct QueueItem src = { 0,0,0 }, itemApp, firstItem;
    queue_enqueue(q, src);
    matriceFile[src.riga][src.colonna] = 1;
    matriceMosse[src.riga][src.colonna] = src.distanza;
    while (!queue_is_empty(q)) {
        firstItem = q->item[q->testa];
        queue_dequeue(q);

        /*Arrivo a destinazione*/
        if (firstItem.riga == (r - 3) && firstItem.colonna == (c - 3)) {
            queue_destroy(q);
            printf("%d\n", firstItem.distanza);
            return firstItem.distanza;
        }

        /*Per quanto riguarda lo spostamento del robot, sposto sempre solo la cella dell'angolo alto sinistro del 3x3.
        Ovviamente però, faccio sempre anche il controllo che riesco a passare negli spazi 3x3*/

        /*Muovi a destra*/
        if ((firstItem.colonna + 1) < c && matriceFile[firstItem.riga][firstItem.colonna + 1] == 0) {
            if (controlloSpazio(RIGHT, firstItem.riga, firstItem.colonna + 1, matriceFile, r, c)) {
                itemApp.riga = firstItem.riga;
                itemApp.colonna = firstItem.colonna + 1;
                itemApp.distanza = firstItem.distanza + 1;
                queue_enqueue(q, itemApp);
                matriceFile[itemApp.riga][itemApp.colonna] = 1;
                matriceMosse[itemApp.riga][itemApp.colonna] = itemApp.distanza;
            }
        }

        /*Muovi in basso*/
        if ((firstItem.riga + 1) < r && matriceFile[firstItem.riga + 1][firstItem.colonna] == 0) {
            if (controlloSpazio(DOWN, firstItem.riga + 1, firstItem.colonna, matriceFile, r, c)) {
                itemApp.riga = firstItem.riga + 1;
                itemApp.colonna = firstItem.colonna;
                itemApp.distanza = firstItem.distanza + 1;
                queue_enqueue(q, itemApp);
                matriceFile[itemApp.riga][itemApp.colonna] = 1;
                matriceMosse[itemApp.riga][itemApp.colonna] = itemApp.distanza;
            }
        }

        /*Muovi in alto*/
        if ((firstItem.riga - 1) >= 0 && matriceFile[firstItem.riga - 1][firstItem.colonna] == 0) {
            if (controlloSpazio(UP, firstItem.riga - 1, firstItem.colonna, matriceFile, r, c)) {
                itemApp.riga = firstItem.riga - 1;
                itemApp.colonna = firstItem.colonna;
                itemApp.distanza = firstItem.distanza + 1;
                queue_enqueue(q, itemApp);
                matriceFile[itemApp.riga][itemApp.colonna] = 1;
                matriceMosse[itemApp.riga][itemApp.colonna] = itemApp.distanza;
            }
        }

        /*Muovi a sinistra*/
        if ((firstItem.colonna - 1) >= 0 && matriceFile[firstItem.riga][firstItem.colonna - 1] == 0) {
            if (controlloSpazio(LEFT, firstItem.riga, firstItem.colonna - 1, matriceFile, r, c)) {
                itemApp.riga = firstItem.riga;
                itemApp.colonna = firstItem.colonna - 1;
                itemApp.distanza = firstItem.distanza + 1;
                queue_enqueue(q, itemApp);
                matriceFile[itemApp.riga][itemApp.colonna] = 1;
                matriceMosse[itemApp.riga][itemApp.colonna] = itemApp.distanza;
            }
        }
    }
    queue_destroy(q);
    printf("-1\n");
    return -1;
}

/*Metodo per stampare le direzioni del percorso ottimo.
Per stampare le mosse fatte, ripercorro al contrario la matrice matriceMosse, salvandomi in un array le varie mosse.
Ovviamente, leggendo al contario, anche le mosse le dovrò scrivere al contrario (se vedo che c'è uno spostamento
verso Nord leggendo al contrario, nell'array metterò Sud)*/
void stampaMosse(int** matriceMosse, int r, int c, int nMosse) {
    int i = r - 3, j = c - 3, contaMosse = 0;
    char* mosse = (char*)malloc(nMosse * sizeof(char));
    assert(mosse != NULL);
    while (nMosse > 0) {
        if (matriceMosse[i][j] == nMosse) {
            if ((i - 1) >= 0 && matriceMosse[i - 1][j] == nMosse - 1) {
                mosse[contaMosse] = 'S';
                contaMosse++;
                i--;
                nMosse--;
            }
            else if ((i + 1) < r && matriceMosse[i + 1][j] == nMosse - 1) {
                mosse[contaMosse] = 'N';
                contaMosse++;
                i++;
                nMosse--;
            }
            else if ((j - 1) >= 0 && matriceMosse[i][j - 1] == nMosse - 1) {
                mosse[contaMosse] = 'E';
                contaMosse++;
                j--;
                nMosse--;
            }
            else if ((j + 1) < c && matriceMosse[i][j + 1] == nMosse - 1) {
                mosse[contaMosse] = 'O';
                contaMosse++;
                j++;
                nMosse--;
            }
        }
    }
    for (i = contaMosse - 1; i >= 0; i--)
        printf("%c", mosse[i]);
    free(mosse);
}

/*Funzione breve che utilizzo durante l'inizializzazione delle matrici per evitare che le mie variabili di scorrimento escano dai limiti*/
int checkBound(int i, int j, int r, int c) {
    if ((i >= 0 && i < r) && (j >= 0 && i < c))
        return 1;
    else
        return 0;
}

int main(int argc, char* argv[]) {
    FILE* fileIn = stdin;

    /*Utilizzo r e c per salvare il numero di righe e colonne, i e j per ciclare sulle matrici, nMosse per salvare le mosse*/
    /*matriceMosse mi permette di salvare gli spostamenti e le mosse eseguite dalla mia matrice*/
    /*matriceFile contiene la matrice con i valori della matrice presi dal file, quindi quella su cui faccio gli spostamenti e calcolo il percorso*/
    int r, c, i, j, nMosse, ** matriceFile, ** matriceMosse;

    if ((argc < 2) || (argc > 4)) {
        fprintf(stderr, "Usage: %s filename [src [dst]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-") != 0) {
        fileIn = fopen(argv[1], "r");
        if (fileIn == NULL) {
            fprintf(stderr, "Can not open %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }

    /*Allocazione delle matrici*/
    fscanf(fileIn, "%d %d \t", &r, &c);

    matriceMosse = (int**)malloc(r * sizeof(int*));
    assert(matriceMosse != NULL);
    for (i = 0; i < r; i++)
        matriceMosse[i] = (int*)malloc(c * sizeof(int));

    matriceFile = (int**)malloc(r * sizeof(int*));
    assert(matriceFile != NULL);
    for (i = 0; i < r; i++)
        matriceFile[i] = (int*)malloc(c * sizeof(int));

    char s;
    while ((s = fgetc(fileIn)) != EOF) {
        if (s == '.' && checkBound(i, j, r, c)) {
            matriceFile[i][j] = 0;
            j++;
        }
        else if (s == '*' && checkBound(i, j, r, c)) {
            matriceFile[i][j] = 1;
            j++;
        }
        else if (s == '\n') {
            i++;
            j = 0;
        }
    }

    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            matriceMosse[i][j] = 0;
        }
    }

    fclose(fileIn);
    nMosse = calcDistanzaMin(matriceMosse, matriceFile, r, c);
    /*Se ho trovato un percorso ottimale, lo stampo*/
    if (nMosse != -1)
        stampaMosse(matriceMosse, r, c, nMosse);
    
    /*utilizzo la funzione free per liberare la memoria*/
    for (i = 0; i < r; i++) {
        free(matriceMosse[i]);
        free(matriceFile[i]);
    }
    free(matriceMosse);
    free(matriceFile);

    return EXIT_SUCCESS;
}