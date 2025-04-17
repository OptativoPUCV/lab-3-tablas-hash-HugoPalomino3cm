#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long pos = hash(key,map->capacity);

    while(map->buckets[pos] != NULL && map->buckets[pos]->key){
        if (is_equal(map->buckets[pos]->key,key)){
            map->buckets[pos]->value = value;
            return;
        }
        pos = (pos + 1) % map->capacity;
    }

    Pair *nuevoPair = createPair(key,value);

    map->buckets[pos] = nuevoPair;
    map->size++;

    map->current = pos;

    if ((double)map->size / map->capacity > 0.75) {
        enlarge(map);
    }

}

void enlarge(HashMap * map) {
    enlarge_called = 1;

    Pair **antiguo = map->buckets;
    long antiguaCapacidad = map->capacity;

    map->capacity *= 2;
    map->size = 0;
    map->current = -1;

    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));

    for (size_t k = 0; k < antiguaCapacidad; k++) {
        if (antiguo[k] != NULL && antiguo[k]->key != NULL) {
            insertMap(map, antiguo[k]->key, antiguo[k]->value);
            free(antiguo[k]);
        }
    }
    free(antiguo);
}


HashMap * createMap(long capacity) {
    HashMap *mapa = (HashMap *) malloc(sizeof(HashMap));

    mapa->buckets = (Pair**) calloc(capacity,sizeof(Pair*));

    mapa->capacity = capacity;
    mapa->size = 0;
    mapa->current = -1;
    return mapa;
}

void eraseMap(HashMap * map, char * key) {    
    long pos = hash(key, map->capacity);

    while (map->buckets[pos] != NULL) {
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {

            map->buckets[pos] = NULL; 
            map->size--;
            map->current = -1;

            return;
        }
        pos = (pos + 1) % map->capacity;
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    long pos = hash(key,map->capacity);
    while(map->buckets[pos] != NULL){
        if(map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key,key)){
            map->current = pos;
            return map->buckets[pos];
        }
        pos = (pos + 1) % map->capacity;
    }
    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}


Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

