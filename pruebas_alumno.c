#include "pa2m.h"
#include "src/hash.h"
#include "src/hash_estructura_privada.h"
#include <string.h>

bool recorrer_todo_sumando_1_al_aux(const char *clave, void *valor, void *_aux)
{
	if (_aux) {
		int *aux = _aux;
		(*aux)++;
	}
	return true;
}

bool iterar_parcialmente_sumando_1_al_aux(const char *clave, void *valor,
					  void *_aux)
{
	int *aux = _aux;
	(*aux)++;

	if (*aux == 5)
		return false;
	return true;
}

void sumar_uno_destructor(void *_elemento)
{
	if (!_elemento)
		return;

	int *valor = _elemento;
	(*valor)++;
	return;
}

void pruebas_hash_creacion()
{
	hash_t *hash = hash_crear(0);
	pa2m_afirmar(!!hash,
		     "Se puede crear un hash con capacidad inicial menor a 3.");
	pa2m_afirmar(
		hash->capacidad == 3,
		"Se crea un hash con capacidad 1, pero se inicializa en 3.\n");
	hash_t *hash2 = hash_crear(10);
	pa2m_afirmar(!!hash2,
		     "Se puede crear un hash con capacidad inicial mayor a 3.");
	pa2m_afirmar(
		hash2->capacidad == 10,
		"Se crea un hash con capacidad 10, y se inicializa en 10.\n");
	pa2m_afirmar(hash_cantidad(hash) == 0 && hash_cantidad(hash2) == 0,
		     "La cantidad inicial se inicializa correctamente.\n");
	hash_destruir(hash);
	hash_destruir(hash2);
}

void pruebas_hash_insercion()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char valores[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	pa2m_afirmar(!hash_insertar(NULL, &claves[0], &valores[0], NULL),
		     "No se puede insertar en un hash nulo.");
	hash_t *hash = hash_crear(3);
	pa2m_afirmar(!hash_insertar(hash, NULL, "A", NULL),
		     "No se puede insertar una clave nula en un hash válido.");
	pa2m_afirmar(
		!!hash_insertar(hash, &claves[0], NULL, NULL),
		"Se puede insertar un elemento nulo con clave válida en un hash válido.");
	pa2m_afirmar(
		!!hash_insertar(hash, &claves[1], &valores[1], NULL),
		"Se puede insertar un elemento válido con clave válida en un hash válido.");

	bool inserciones_realizadas_correctamente = true;
	for (size_t i = 2; i < 10; i++) {
		if (i == 4)
			pa2m_afirmar(
				!!hash_insertar(hash, &claves[i], &valores[i],
						NULL),
				"Se puede insertar un elemento causando rehash.");

		if (i == 6)
			pa2m_afirmar(
				!!hash_insertar(hash, &claves[i], &valores[i],
						NULL),
				"Se puede insertar un elemento causando rehash (OTRA VEZ).");

		if (!hash_insertar(hash, &claves[i], &valores[i], NULL))
			inserciones_realizadas_correctamente = false;
	}
	pa2m_afirmar(
		inserciones_realizadas_correctamente &&
			hash_cantidad(hash) == 10,
		"Se pueden insertar varios elementos válidos con claves válidas en un hash válido.");
	void *anterior = &valores[0];
	pa2m_afirmar(!!hash_insertar(hash, &claves[0], &valores[9],
				     &anterior) &&
			     anterior == NULL,
		     "Se puede insertar un elemento y actualizarlo.\n");
	hash_destruir(hash);
}

void pruebas_hash_eliminacion()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char valores[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	pa2m_afirmar(!hash_quitar(NULL, &claves[0]),
		     "No se puede quitar de un hash nulo.");
	hash_t *hash = hash_crear(15);
	pa2m_afirmar(!hash_quitar(hash, NULL),
		     "No se puede quitar una clave nula de un hash válido.");
	for (size_t i = 1; i < 10; i++) {
		hash_insertar(hash, &claves[i], &valores[i], NULL);
	}
	pa2m_afirmar(
		!hash_quitar(hash, &claves[0]),
		"No se puede quitar una clave inexistente de un hash válido.");
	pa2m_afirmar(
		!!hash_quitar(hash, &claves[9]),
		"Se puede quitar una clave existente (<10,J>) de un hash válido.");
	pa2m_afirmar(
		!hash_quitar(hash, &claves[9]),
		"No se puede quitar una clave eliminada anteriormente (<10,J>).\n");
	hash_destruir(hash);
}

void pruebas_hash_busqueda()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char valores[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	pa2m_afirmar(!hash_obtener(NULL, &claves[0]),
		     "No se puede obtener una clave de un hash nulo.");
	hash_t *hash = hash_crear(15);
	pa2m_afirmar(!hash_obtener(hash, NULL),
		     "No se puede obtener una clave nula.");
	for (size_t i = 0; i < 9; i++) {
		hash_insertar(hash, &claves[i], &valores[i], NULL);
	}
	pa2m_afirmar(!hash_obtener(hash, &claves[9]),
		     "No se puede obtener una clave inexistente.");
	pa2m_afirmar(!!hash_obtener(hash, &claves[8]),
		     "Se puede obtener una clave existente (<9,I>).");
	pa2m_afirmar(!!hash_quitar(hash, &claves[8]),
		     "Se elimina la clave <9,I>.");
	pa2m_afirmar(
		!hash_obtener(hash, &claves[8]),
		"No se puede obtener la clave <9,I> luego de eliminarla.\n");
	hash_destruir(hash);
}

void pruebas_hash_contiene()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char valores[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	pa2m_afirmar(!hash_contiene(NULL, &claves[0]),
		     "Devuelve false con un hash nulo.");
	hash_t *hash = hash_crear(15);
	pa2m_afirmar(!hash_contiene(hash, NULL),
		     "Devuelve false con una clave nula.");
	bool hubo_error = false;
	for (size_t i = 0; i < 10; i++) {
		hash_insertar(hash, &claves[i], &valores[i], NULL);
	}
	for (size_t i = 0; i < 10; i++) {
		if (!hash_contiene(hash, &claves[i]))
			hubo_error = true;
	}
	pa2m_afirmar(!hubo_error,
		     "Se encuentran todos los elementos del hash.");
	hash_insertar(hash, &claves[9], NULL, NULL);
	pa2m_afirmar(hash_contiene(hash, &claves[9]),
		     "Devuelve true si el elemento a encontrar es NULL.\n");
	hash_destruir(hash);
}

void pruebas_hash_cantidad()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char valores[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	pa2m_afirmar(!hash_cantidad(NULL),
		     "No se puede obtener la cantidad de un hash nulo.");
	hash_t *hash = hash_crear(15);
	pa2m_afirmar(
		hash_cantidad(hash) == 0,
		"Se inicializa correctamente la cantidad de un hash vacío.");
	for (size_t i = 0; i < 10; i++) {
		hash_insertar(hash, &claves[i], &valores[i], NULL);
	}
	pa2m_afirmar(
		hash_cantidad(hash) == 10,
		"Se actualiza correctamente la cantidad del hash luego de insertar 10 elementos.\n");
	hash_destruir(hash);
}

void pruebas_hash_iterador_interno()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char valores[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	int aux = 0;
	pa2m_afirmar(hash_con_cada_clave(NULL, recorrer_todo_sumando_1_al_aux,
					 &aux) == 0,
		     "No se puede iterar un hash nulo.");
	hash_t *hash = hash_crear(15);
	pa2m_afirmar(hash_con_cada_clave(hash, NULL, &aux) == 0,
		     "No se puede iterar un hash con una función nula.");
	hash_insertar(hash, &claves[0], &valores[0], NULL);
	pa2m_afirmar(hash_con_cada_clave(hash, recorrer_todo_sumando_1_al_aux,
					 &aux) == 1,
		     "Se puede iterar un hash con un auxiliar nulo.");
	for (size_t i = 1; i < 10; i++) {
		hash_insertar(hash, &claves[i], &valores[i], NULL);
	}

	pa2m_afirmar(hash_con_cada_clave(hash, recorrer_todo_sumando_1_al_aux,
					 &aux) == 10,
		     "Se puede iterar el hash en su totalidad.");
	aux = 0;
	pa2m_afirmar(hash_con_cada_clave(hash,
					 iterar_parcialmente_sumando_1_al_aux,
					 &aux) == 5,
		     "Se puede iterar el hash parcialmente.\n");
	hash_destruir(hash);
}

void pruebas_hash_destruir_todo()
{
	const char claves[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int valores[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	hash_t *hash = hash_crear(15);
	for (size_t i = 0; i < 10; i++) {
		hash_insertar(hash, &claves[i], &valores[i], NULL);
	}
	hash_destruir_todo(hash, sumar_uno_destructor);
	pa2m_afirmar(valores[0] == 2 && valores[1] == 3 && valores[2] == 4 &&
			     valores[3] == 5 && valores[4] == 6 &&
			     valores[5] == 7 && valores[6] == 8 &&
			     valores[7] == 9 && valores[8] == 10 &&
			     valores[9] == 11,
		     "Se aplica la función a todos los elementos del hash.\n");
}

void pruebas_hash()
{
	pa2m_nuevo_grupo("---------- Pruebas de Creación ----------\n");
	pruebas_hash_creacion();
	pa2m_nuevo_grupo("---------- Pruebas de Inserción ----------\n");
	pruebas_hash_insercion();
	pa2m_nuevo_grupo("---------- Pruebas de Eliminación ----------\n");
	pruebas_hash_eliminacion();
	pa2m_nuevo_grupo("---------- Pruebas de Búsqueda ----------\n");
	pruebas_hash_busqueda();
	pa2m_nuevo_grupo("---------- Pruebas de Cantidad ----------\n");
	pruebas_hash_cantidad();
	pa2m_nuevo_grupo("---------- Pruebas de Contiene ----------\n");
	pruebas_hash_contiene();
	pa2m_nuevo_grupo("---------- Pruebas de Iterador Interno ----------\n");
	pruebas_hash_iterador_interno();
	pa2m_nuevo_grupo("---------- Pruebas de Destruir Todo ----------\n");
	pruebas_hash_destruir_todo();
}

int main()
{
	pa2m_nuevo_grupo("============== PRUEBAS HASH =============\n");
	pruebas_hash();

	return pa2m_mostrar_reporte();
}
