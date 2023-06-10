#ifndef __HASH_ESTRUCTURA_PRIVADA_H_
#define __HASH_ESTRUCTURA_PRIVADA_H_

#include "hash.h"

typedef struct nodo {
	char *clave;
	void *valor;
	struct nodo *siguiente;
} nodo_t;

struct hash {
	nodo_t **vector;
	size_t capacidad;
	size_t cantidad;
};

#endif