#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "hash_estructura_privada.h"

#define FACTOR_CARGA_MAXIMO 0.7
const int CAPACIDAD_MINIMA = 3;

unsigned long funcion_hash(const char *str)
{
	unsigned long hash = 5381;
	int c;
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) +
		       (unsigned long)c; /* hash * 33 + c */
	}
	return hash;
}

nodo_t *nodo_crear(const char *clave, void *elemento)
{
	char *copia = malloc(strlen(clave) + 1);
	if (!copia)
		return NULL;
	strcpy(copia, clave);
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;
	nodo->clave = copia;
	nodo->valor = elemento;
	nodo->siguiente = NULL;
	return nodo;
}

bool pasar_pares(const char *clave, void *elemento, void *_hash)
{
	hash_t *hash = (hash_t *)_hash;
	if (!hash_insertar(hash, clave, elemento, NULL))
		return false;
	return true;
}

hash_t *rehash(hash_t *hash)
{
	hash_t *nuevo_hash = hash_crear((hash->capacidad) * 2);
	if (!nuevo_hash)
		return NULL;

	size_t contador = hash_con_cada_clave(hash, pasar_pares, nuevo_hash);
	if (contador < hash->cantidad) {
		free(nuevo_hash);
		return NULL;
	}

	hash_t aux = *hash;
	*hash = *nuevo_hash;
	*nuevo_hash = aux;
	hash_destruir(nuevo_hash);
	return hash;
}

hash_t *hash_crear(size_t capacidad)
{
	if (capacidad < CAPACIDAD_MINIMA)
		capacidad = CAPACIDAD_MINIMA;
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;
	hash->capacidad = capacidad;
	hash->vector = calloc(1, sizeof(struct nodo *) * capacidad);
	if (!hash->vector) {
		free(hash);
		return NULL;
	}
	return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	float factor_de_carga = (float)hash->cantidad / (float)hash->capacidad;
	if (factor_de_carga > FACTOR_CARGA_MAXIMO) {
		if (!rehash(hash))
			return NULL;
	}
	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->vector[posicion]) {
		nodo_t *nodo_a_insertar = nodo_crear(clave, elemento);
		if (!nodo_a_insertar)
			return NULL;
		hash->vector[posicion] = nodo_a_insertar;
		if (anterior)
			*anterior = NULL;
		hash->cantidad++;
		return hash;
	}
	nodo_t *nodo_actual = hash->vector[posicion];
	while (nodo_actual) {
		if (strcmp(nodo_actual->clave, clave) == 0) {
			if (anterior)
				*anterior = nodo_actual->valor;

			nodo_actual->valor = elemento;
			return hash;
		}
		nodo_actual = nodo_actual->siguiente;
	}
	nodo_t *nodo_a_insertar = nodo_crear(clave, elemento);
	nodo_a_insertar->siguiente = hash->vector[posicion];
	hash->vector[posicion] = nodo_a_insertar;
	hash->cantidad++;
	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->vector[posicion])
		return NULL;

	nodo_t *nodo_actual = hash->vector[posicion];
	nodo_t *anterior = NULL;
	while (nodo_actual) {
		if (strcmp(nodo_actual->clave, clave) == 0) {
			if (!anterior)
				hash->vector[posicion] = nodo_actual->siguiente;
			else
				anterior->siguiente = nodo_actual->siguiente;
			void *valor = nodo_actual->valor;
			free(nodo_actual->clave);
			free(nodo_actual);
			hash->cantidad--;
			return valor;
		}
		anterior = nodo_actual;
		nodo_actual = nodo_actual->siguiente;
	}
	return NULL;
}

void *obtener_valor_rec(hash_t *hash, const char *clave, nodo_t *nodo)
{
	if (!nodo)
		return NULL;
	if (strcmp(nodo->clave, clave) == 0)
		return nodo->valor;
	return obtener_valor_rec(hash, clave, nodo->siguiente);
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->vector[posicion])
		return NULL;
	return obtener_valor_rec(hash, clave, hash->vector[posicion]);
}

bool buscar_valor_rec(hash_t *hash, const char *clave, nodo_t *nodo)
{
	if (!nodo)
		return false;
	if (strcmp(nodo->clave, clave) == 0)
		return true;
	return buscar_valor_rec(hash, clave, nodo->siguiente);
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->vector[posicion])
		return false;
	return buscar_valor_rec(hash, clave, hash->vector[posicion]);
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void destruir_nodos_rec(nodo_t *nodo, void (*destructor)(void *))
{
	if (!nodo)
		return;
	destruir_nodos_rec(nodo->siguiente, destructor);
	if (destructor)
		destructor(nodo->valor);
	free(nodo->clave);
	free(nodo);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_t *nodo_actual = hash->vector[i];
		destruir_nodos_rec(nodo_actual, destructor);
	}
	free(hash->vector);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	if (!hash || !f)
		return 0;

	size_t contador = 0;
	size_t iterador = 0;
	bool iteracion_finalizada = false;
	while (iterador < hash->capacidad && !iteracion_finalizada) {
		nodo_t *nodo_actual = hash->vector[iterador];
		while (nodo_actual && !iteracion_finalizada) {
			contador++;
			if (!f(nodo_actual->clave, nodo_actual->valor, aux))
				iteracion_finalizada = true;

			nodo_actual = nodo_actual->siguiente;
		}
		iterador++;
	}

	return contador;
}
