#include "abb.h"
#include <stdio.h>
/*
	///////////////////// FUNCIONES AUXILIARES
*/
 
 /*
	Crea un nodo, si puede, y lo inicializa con un dato.
 */
 nodo_abb_t* nuevo_nodo(void* dato)
 {
 	nodo_abb_t* nodo = calloc(1,sizeof(nodo_abb_t));
 	if(nodo)
 	{
 		nodo->elemento = dato;
 	}
 	return nodo;
 }

/*
	Devuelve true si es nodo hoja, false si no lo es.
*/
bool es_hoja(nodo_abb_t* nodo)
{
	return (!nodo->izquierda && !nodo->derecha);
}

/*
 //////////////////////// 
*/


abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor)
{
	if(!comparador) 
		return NULL;

	abb_t* arbol = calloc(1,sizeof(abb_t));
	if(!arbol)
		return NULL;

	arbol->comparador = comparador;
	arbol->destructor = destructor;
	return  arbol;
}

int arbol_insertar(abb_t* arbol, void* elemento)
{
	if(!arbol)
		return -1;
	abb_t arbol_actual = *arbol;
	nodo_abb_t*	nuevo = NULL;

	if(arbol_vacio(arbol))
	{
		nuevo = nuevo_nodo(elemento);
		if(!nuevo) return -1;
		arbol->nodo_raiz = nuevo;
		return 0;
	}
	if(arbol->comparador(arbol->nodo_raiz->elemento,elemento) == -1)
	{
		if(!arbol->nodo_raiz->derecha)
		{
			nuevo = nuevo_nodo(elemento);
			if(!nuevo) return -1;
			arbol_actual.nodo_raiz->derecha = nuevo;
			return 0;
		}
		arbol_actual.nodo_raiz = arbol->nodo_raiz->derecha;
	}
	else
	{
		if(!arbol->nodo_raiz->izquierda)
		{
			nuevo = nuevo_nodo(elemento);
			if(!nuevo) return -1;
			arbol_actual.nodo_raiz->izquierda = nuevo;
			return 0;
		}
		arbol_actual.nodo_raiz = arbol->nodo_raiz->izquierda;
	}

	return arbol_insertar(&arbol_actual,elemento);
}

void destruir_nodos(nodo_abb_t* nodo, abb_liberar_elemento destructor)
{

	if(nodo->izquierda)
	{
		destruir_nodos(nodo->izquierda,destructor);
	}
	if(nodo->derecha)
	{
		destruir_nodos(nodo->derecha,destructor);
	}
	destructor(nodo->elemento);
	free(nodo);
}

void arbol_destruir(abb_t* arbol)
{
	if(!arbol_vacio(arbol))
		destruir_nodos(arbol->nodo_raiz,arbol->destructor);
	
	free(arbol);
}

void* arbol_buscar(abb_t* arbol, void* elemento)
{
	if(arbol_vacio(arbol))
		return NULL;

	abb_t arbol_actual = *arbol;
	if(arbol->comparador(arbol->nodo_raiz->elemento,elemento) == 0)
		return arbol->nodo_raiz->elemento;

	if(arbol->comparador(arbol->nodo_raiz->elemento,elemento) == -1)
	{
		if(!arbol->nodo_raiz->derecha)
			return NULL;
		arbol_actual.nodo_raiz = arbol->nodo_raiz->derecha;
	}
	else
	{
		if(!arbol->nodo_raiz->izquierda)
			return NULL;
		arbol_actual.nodo_raiz = arbol->nodo_raiz->izquierda;	
	}

	return arbol_buscar(&arbol_actual,elemento);
}

void* arbol_raiz(abb_t* arbol)
{
	if(!arbol_vacio(arbol))
		return arbol->nodo_raiz->elemento;
	return NULL;
}

bool arbol_vacio(abb_t* arbol)
{
	return (!arbol || !arbol->nodo_raiz);
}

int arbol_recorrido_inorden_interno(nodo_abb_t* raiz_actual, void**array, int tamanio_array, int contador)
{

	if(raiz_actual->izquierda)
	{
		contador = arbol_recorrido_inorden_interno(raiz_actual->izquierda,array,tamanio_array,contador);
	}
	
	if(contador < tamanio_array)
	{
		array[contador] = raiz_actual->elemento;
		contador++;		
	}
	else 
		return contador;

	if(raiz_actual->derecha)
	{
		contador = arbol_recorrido_inorden_interno(raiz_actual->derecha,array,tamanio_array,contador);
	}

	return contador;
}

int arbol_recorrido_preorden_interno(nodo_abb_t* raiz_actual, void**array, int tamanio_array, int contador)
{

	if(contador < tamanio_array)
	{
		array[contador] = raiz_actual->elemento;
		contador++;		
	}
	else 
		return contador;

	if(raiz_actual->izquierda)
	{
		contador = arbol_recorrido_preorden_interno(raiz_actual->izquierda,array,tamanio_array,contador);
	}

	if(raiz_actual->derecha)
	{
		contador = arbol_recorrido_preorden_interno(raiz_actual->derecha,array,tamanio_array,contador);
	}

	return contador;
}

int arbol_recorrido_postorden_interno(nodo_abb_t* raiz_actual, void**array, int tamanio_array, int contador)
{
	if(contador >= tamanio_array)
		return contador;

	if(raiz_actual->izquierda)
	{
		contador = arbol_recorrido_postorden_interno(raiz_actual->izquierda,array,tamanio_array,contador);
	}
	
	if(raiz_actual->derecha)
	{
		contador = arbol_recorrido_postorden_interno(raiz_actual->derecha,array,tamanio_array,contador);
	}

	if(contador < tamanio_array)
	{
		array[contador] = raiz_actual->elemento;
		contador++;		
	}
	return contador;
}


int arbol_recorrido_inorden(abb_t* arbol, void** array, int tamanio_array)
{
	if(arbol_vacio(arbol) || tamanio_array == 0 || array == NULL)
		return 0;
	int contador = 0;
	
	return arbol_recorrido_inorden_interno(arbol->nodo_raiz,array,tamanio_array,contador);
}

int arbol_recorrido_preorden(abb_t* arbol, void** array, int tamanio_array)
{
	if(arbol_vacio(arbol) || tamanio_array == 0 || array == NULL)
		return 0;
	int contador = 0;
	
	return arbol_recorrido_preorden_interno(arbol->nodo_raiz,array,tamanio_array,contador);
}

int arbol_recorrido_postorden(abb_t* arbol, void** array, int tamanio_array)
{
	if(arbol_vacio(arbol) || tamanio_array == 0 || array == NULL)
		return 0;
	int contador = 0;
	
	return arbol_recorrido_postorden_interno(arbol->nodo_raiz,array,tamanio_array,contador);
}


void* tomar_mayor_menores(nodo_abb_t* padre, nodo_abb_t* hijo)
{
	void* dato = NULL;

	while(hijo->derecha)
	{
		padre = hijo;
		hijo = padre->derecha;
	}
	
	dato = hijo->elemento;
	nodo_abb_t* izq_hijo = hijo->izquierda;
	free(hijo);
	
	if(padre->izquierda == hijo)
		padre->izquierda = izq_hijo;

	else 
		padre->derecha = izq_hijo;

	return dato;
}

nodo_abb_t* borrar_elemento(nodo_abb_t* raiz_actual, void* dato, int* estado, abb_liberar_elemento destructor)
{
		*estado = 0;
		nodo_abb_t* aux = NULL;

		if(es_hoja(raiz_actual))
		{
			destructor(dato);
			free(raiz_actual);
			raiz_actual = NULL;
			return raiz_actual;
		}

		if(raiz_actual->derecha && raiz_actual->izquierda)
		{
			void* elemento = tomar_mayor_menores(raiz_actual,raiz_actual->izquierda);
			destructor(dato);
			raiz_actual->elemento = elemento;
			return raiz_actual;
		}

		if(!raiz_actual->derecha)
		{
			 aux = raiz_actual->izquierda;
		}

		if(!raiz_actual->izquierda) 
		{

			aux = raiz_actual->derecha;
		}

		destructor(dato);
		free(raiz_actual);
		return aux;
}

nodo_abb_t* arbol_borrar_interno(nodo_abb_t* raiz_actual, void* dato,int* estado, abb_comparador comparador, abb_liberar_elemento destructor)
{
	if(comparador(raiz_actual->elemento,dato) == 0)
	{
		return borrar_elemento(raiz_actual,dato,estado,destructor);
	}
	if(comparador(raiz_actual->elemento,dato) == 1 && raiz_actual->izquierda)
	{
		raiz_actual->izquierda = arbol_borrar_interno(raiz_actual->izquierda,dato,estado,comparador,destructor);
	}
	else if (raiz_actual->derecha)
		raiz_actual->derecha = arbol_borrar_interno(raiz_actual->derecha,dato,estado,comparador,destructor);

	return  raiz_actual;
}

int arbol_borrar(abb_t* arbol, void* elemento)
{
	if(arbol_vacio(arbol)) return -1;

	int estado = -1;
	arbol->nodo_raiz = arbol_borrar_interno(arbol->nodo_raiz,elemento,&estado,arbol->comparador,arbol->destructor);

	return estado;
}

void abb_con_cada_elemento_inorden(nodo_abb_t* raiz,bool (*funcion)(void*, void*), void* extra, bool* estado)
{
	if(raiz->izquierda && !(*estado))
		abb_con_cada_elemento_inorden(raiz->izquierda,funcion,extra,estado);
	if(!(*estado))
		*estado = funcion(raiz->elemento,extra);
	else
		return;
	if(raiz->derecha && !(*estado))
		abb_con_cada_elemento_inorden(raiz->derecha,funcion,extra,estado);

}

void abb_con_cada_elemento_preorden(nodo_abb_t* raiz,bool (*funcion)(void*, void*), void* extra, bool* estado)
{
	if((*estado = funcion(raiz->elemento,extra)))
		return; 

	if(raiz->izquierda && !(*estado))
		abb_con_cada_elemento_preorden(raiz->izquierda,funcion,extra,estado);

	if(raiz->derecha && !(*estado))
		abb_con_cada_elemento_preorden(raiz->derecha,funcion,extra,estado);

}

void abb_con_cada_elemento_postorden(nodo_abb_t* raiz,bool (*funcion)(void*, void*), void* extra, bool* estado)
{
	if(raiz->izquierda && !(*estado))
		abb_con_cada_elemento_postorden(raiz->izquierda,funcion,extra,estado);

	if(raiz->derecha && !(*estado))
		abb_con_cada_elemento_postorden(raiz->derecha,funcion,extra,estado);

	if(!(*estado))
		*estado = funcion(raiz->elemento,extra);

}

void abb_con_cada_elemento(abb_t* arbol, int recorrido, bool (*funcion)(void*, void*), void* extra)
{
	if(arbol_vacio(arbol) || !funcion) return;

	bool estado = false;
	switch(recorrido)
	{
		case ABB_RECORRER_INORDEN:
			abb_con_cada_elemento_inorden(arbol->nodo_raiz,funcion,extra,&estado);
		break;

		case ABB_RECORRER_PREORDEN:
			abb_con_cada_elemento_preorden(arbol->nodo_raiz,funcion,extra,&estado);
		break;

		case ABB_RECORRER_POSTORDEN:
			abb_con_cada_elemento_postorden(arbol->nodo_raiz,funcion,extra,&estado);
		break;	

		default:
			printf("Recorrido no valido \n");
	}
}


