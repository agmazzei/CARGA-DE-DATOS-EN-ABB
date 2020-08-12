#include "pokedex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_RESERVA_MEMORIA -2
#define ERROR_LECTURA_ARCHIVO -1

#define CAMPOS_A_LEER_AVISTADOS 6
#define CAMPOS_A_LEER_EVOLUCIONES 5


bool estado_valido(const int estado)
{
	if(estado == ERROR_RESERVA_MEMORIA || estado == ERROR_LECTURA_ARCHIVO) return false;
	return true;
}


/*	El archivo existe y se abrio correctamente.
	Devuelve false si el archivo tiene contenido, true si esta vacio.
*/ 
bool archivo_vacio(FILE* archivo)
{
	int c = fgetc(archivo);
	if (c == EOF) 
    	return true;
	else 
	{
   		ungetc(c, archivo);
   		return false;
	}
}

/*	
	Funcion para utilizar en el iterador interno de la lista, imprime por pantalla los campos
	de un pokemon.
*/ 

void mostrar_pokemon(void* pokemon, void* no_hace_nada)
{
	printf("  -Nombre: %s \n Nivel: %i \n ¿Capturado?: %c \n",((particular_pokemon_t*)pokemon)->nombre,((particular_pokemon_t*)pokemon)->nivel,((particular_pokemon_t*)pokemon)->capturado ? 'S' : 'N');
}

/*
	Busca el pokemon con el nombre solicitado y de encontrarlo devuelve un puntero a el.
	Si no lo encuentra o hay un error, devuelve NULL
*/

particular_pokemon_t* buscar_pokemon(lista_t* pokemones,char* nombre_pokemon)
{
	if(!pokemones) return NULL;

	lista_iterador_t* it = lista_iterador_crear(pokemones);
	if(!it) return NULL;
	
	particular_pokemon_t* ptr_pokemon = NULL;
	bool pokemon_encontrado = false;

	while(lista_iterador_tiene_siguiente(it) && !pokemon_encontrado)
	{
		ptr_pokemon = lista_iterador_siguiente(it);
		if(strcmp(&ptr_pokemon->nombre[0],nombre_pokemon) == 0)
			pokemon_encontrado = true;
	}
	lista_iterador_destruir(it);

	return pokemon_encontrado ? ptr_pokemon : NULL;
}

/*
	Busca en la lista si el pokemon con el nombre solicitado existe y esta capturado. Si cumple
	ambas condiciones lo retira de la lista y lo devuelve.
	En caso de no existir o no estar capturado, devuelve NULL
	Si la lista no existe, devuelve NULL
*/

particular_pokemon_t* tomar_capturado(lista_t* pokemones, char* nombre_pokemon)
{
	if(!pokemones) return NULL;

	bool puede_borrarse = false;
	size_t posicion = 0;
	particular_pokemon_t* ptr_pokemon = NULL;
	lista_iterador_t* it = lista_iterador_crear(pokemones);

	while(lista_iterador_tiene_siguiente(it) && !puede_borrarse)
	{
		ptr_pokemon = lista_iterador_siguiente(it);
		
		if(strcmp(&ptr_pokemon->nombre[0] ,nombre_pokemon) == 0 && ptr_pokemon->capturado)
		{
			lista_borrar_de_posicion(pokemones,posicion);	
			puede_borrarse = true;
		}
		else
			posicion++;
	}
	lista_iterador_destruir(it);

	return puede_borrarse ? ptr_pokemon : NULL;
}

/*
	Reserva memoria para un pokemon y copia el contenido de la fuente en el.
	De ser NULL el puntero fuente o de no poder reservar memoria para el pokemon devuelve NULL. 
*/

particular_pokemon_t* pokemon_crear_copia(const particular_pokemon_t* fuente)
{
	if(!fuente) return NULL;

	particular_pokemon_t* ptr_pokemon = calloc(1,sizeof(particular_pokemon_t));
	if(!ptr_pokemon) return NULL;

	strcpy(&ptr_pokemon->nombre[0] ,&fuente->nombre[0]);
	ptr_pokemon->nivel = fuente->nivel;
	ptr_pokemon->capturado = fuente->capturado;			

	return ptr_pokemon;
}

/*
	Reserva memoria para una especie y copia el contenido de la fuente en el.
	Se reserva memoria tambien para una lista nueva para la especie copia.
	De ser NULL el puntero fuente o de no poder reservar memoria para la lista o la especie devuelve NULL. 
*/

especie_pokemon_t* especie_crear_copia(const especie_pokemon_t* fuente)
{
	if(!fuente) return NULL;

	especie_pokemon_t* ptr_especie = calloc(1,sizeof(especie_pokemon_t));
	if(!ptr_especie) return NULL;
	
	if(!(ptr_especie->pokemones = lista_crear()))
	{
		free(ptr_especie);
		return NULL;
	}


	ptr_especie->numero = fuente->numero;
	strcpy(&ptr_especie->nombre[0],&fuente->nombre[0]);
	strcpy(&ptr_especie->descripcion[0],&fuente->descripcion[0]);

	return  ptr_especie;
}

/*
	Busca la especie en el arbol de especies y retorna un puntero a ella de existir.
	Si no existe o en caso de error devuelve NULL
*/

especie_pokemon_t* buscar_especie(abb_t* arbol_especies, especie_pokemon_t* especie)
{
	return arbol_buscar(arbol_especies,(void*)especie);
}

/*
	Agrega a la lista de pokemones de la especie el pokemon nuevo. De no existir o en caso de error
	devuelve -1  sino 0.
*/

int especie_agregar_pokemon(especie_pokemon_t* especie, particular_pokemon_t* nuevo)
{
	if(!nuevo) return -1;
	return lista_insertar(especie->pokemones,nuevo);
}


/*
	Comparador para el arbol de especies, devuelve 0 si no existe alguna de las especies,
	1 si la clave de la primera es mayor o -1 si la clave de la segunda es mayor.
*/


int comparador_especies(void* especie1, void* especie2)
{
	if(!especie1 || !especie2)
		return 0;

	if(((especie_pokemon_t*)especie1)->numero > ((especie_pokemon_t*)especie2)->numero)
		return 1;
	
	if(((especie_pokemon_t*)especie1)->numero < ((especie_pokemon_t*)especie2)->numero)
		return -1;
	
	return 0;
}

/*
	Destructor del arbol de especies, libera la memoria de la especie y todos sus pokemones.
*/

void destructor_especies(void* especie)
{
	if(!especie) return;

	particular_pokemon_t* pokemon = NULL;
	lista_t* ptr_lista = ((especie_pokemon_t*)especie)->pokemones;

	while((pokemon = lista_ultimo(ptr_lista)))
	{
		lista_borrar(ptr_lista);
		free(pokemon);
	}

	lista_destruir(ptr_lista);
	free(especie);
}



pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE])
{

	if(!entrenador[0]) return NULL; 

	pokedex_t* ptr = calloc(1,sizeof(pokedex_t));
	
	if(!ptr) return NULL;
	strcpy(&ptr->nombre_entrenador[0],&entrenador[0]);
	ptr->ultimos_capturados = lista_crear();
	ptr->ultimos_vistos = lista_crear();
	ptr->pokemones = arbol_crear(comparador_especies,destructor_especies);

	if(!ptr->ultimos_capturados || !ptr->ultimos_vistos || !ptr->pokemones)
	{
		lista_destruir(ptr->ultimos_capturados);
		lista_destruir(ptr->ultimos_vistos);
		arbol_destruir(ptr->pokemones);
		free(ptr);
		return NULL;	
	}
	return ptr;
}

/*
	Agrega a la lista de ultimos capturados de la pokedex el nuevo pokemon.
	De no existir el pokemon o la lista, devuelve -1, sino  0
*/


int agregar_ultimo_capturado(lista_t* ultimos_capturados, particular_pokemon_t* nuevo)
{
	if(!nuevo || !ultimos_capturados) return -1;

 	return lista_apilar(ultimos_capturados,nuevo);
}

/*
	Agrega a la lista de ultimos vistos de la pokedex el nuevo pokemon.
	De no existir el pokemon o la lista, devuelve -1, sino  0
*/

int agregar_ultimo_visto(lista_t* ultimos_vistos, particular_pokemon_t* nuevo)
{
	if(!nuevo || !ultimos_vistos) return -1;
	
	return lista_encolar(ultimos_vistos,nuevo);
}

/*
	Lee una linea del archivo de avistados y la parsea para guardarla en la especie y pokemon recibidos.
	Asume que el archivo esta abierto y esta en una posicion valida y que la especie y el pokemon existen.
	En caso de error de lectura o EOF devuelve devuelve un valor menor a 0

*/

int parsear_linea_avistado(FILE* inicio_linea,especie_pokemon_t* especie, particular_pokemon_t* pokemon)
{
	char capturado = '\0';
	int lecturas = 0;
	lecturas = fscanf(inicio_linea,"%i;%99[^;];%99[^;];%99[^;];%i;%c\n",&especie->numero,
												  &especie->nombre[0],
												  &especie->descripcion[0],
												  &pokemon->nombre[0],
												  &pokemon->nivel,
												  &capturado);

	pokemon->capturado = capturado == 'S' ? true : false;
	return lecturas;
}

int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA])
{
	FILE* archivo = fopen(ruta_archivo,"r");
	if(!archivo || archivo_vacio(archivo) || !pokedex) return -1;

	int estado = 0;
	int lecturas = 0;
	particular_pokemon_t* ptr_pokemon_ultimos_vistos = NULL;
	particular_pokemon_t* ptr_pokemon_ultimos_capturados = NULL;
	particular_pokemon_t* ptr_pokemon = NULL;

	especie_pokemon_t especie_actual = {-1,"VACIO","VACIO",NULL};
	particular_pokemon_t pokemon_actual = {"VACIO",-1,0};
	especie_pokemon_t* ptr_especie = NULL;


	while((lecturas = parsear_linea_avistado(archivo,&especie_actual,&pokemon_actual)) != EOF && estado_valido(estado))
	{
		if(lecturas != CAMPOS_A_LEER_AVISTADOS)
			fscanf(archivo,"%*[^\n]\n");
		else
		{

			if(!(ptr_especie = buscar_especie(pokedex->pokemones,(void*)&especie_actual)))
			{
				ptr_especie = especie_crear_copia(&especie_actual);
				if(!ptr_especie)
				{
					estado = ERROR_RESERVA_MEMORIA;
					break;
				}
				arbol_insertar(pokedex->pokemones,(void*)ptr_especie);
			}
			if((ptr_pokemon = pokemon_crear_copia(&pokemon_actual)))
				especie_agregar_pokemon(ptr_especie,ptr_pokemon);
		
			if((ptr_pokemon_ultimos_vistos = pokemon_crear_copia(&pokemon_actual)))
				agregar_ultimo_visto(pokedex->ultimos_vistos,ptr_pokemon_ultimos_vistos);
 		
 			if(pokemon_actual.capturado && (ptr_pokemon_ultimos_capturados = pokemon_crear_copia(&pokemon_actual)))
 				agregar_ultimo_capturado(pokedex->ultimos_capturados,ptr_pokemon_ultimos_capturados);

 			if(!ptr_pokemon_ultimos_vistos || !ptr_pokemon || (pokemon_actual.capturado && !ptr_pokemon_ultimos_capturados)) 
 				estado = ERROR_RESERVA_MEMORIA;
		}
	}

	fclose(archivo);
	
	return estado_valido(estado) ? 0 : -1;
}

/*
	Lee una linea del archivo de evoluciones y la parsea para guardarla en la especie orgien y destino.
	Asume que el archivo esta abierto y esta en una posicion valida y que ambas especies existen y son validas.
	En caso de error de lectura o EOF devuelve devuelve un valor menor a 0

*/

int parsear_linea_evolucionar(FILE* inicio_linea,especie_pokemon_t* especie_origen, especie_pokemon_t* especie_destino,char* nombre_pokemon)
{
	int lecturas = 0;
	lecturas = fscanf(inicio_linea,"%i;%99[^;];%i;%99[^;];%99[^\n]",&especie_origen->numero,
												  		nombre_pokemon,
												   		&especie_destino->numero,
												   		&especie_destino->nombre[0],
												   		&especie_destino->descripcion[0]);
	return lecturas;
}



int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA])
{
	FILE* archivo = fopen(ruta_archivo,"r");
	if(!archivo || archivo_vacio(archivo) || !pokedex) return -1;
	
	int lecturas = 0;
	int estado = 0;
	especie_pokemon_t especie_destino = {-1,"VACIO","VACIO",NULL};
	especie_pokemon_t especie_origen = {-1,"VACIO","VACIO",NULL};
	particular_pokemon_t pokemon_a_evol = {"VACIO",-1,0};
	especie_pokemon_t* ptr_especie_origen = NULL;
	especie_pokemon_t* ptr_especie_destino = NULL;
	particular_pokemon_t* ptr_pokemon_a_evol = NULL;

	while((lecturas = parsear_linea_evolucionar(archivo,&especie_origen,&especie_destino,&pokemon_a_evol.nombre[0])) != EOF && estado_valido(estado))
	{
		if(lecturas != CAMPOS_A_LEER_EVOLUCIONES)
		{
			printf("ERROR DE LECTURA CON %s \n",especie_destino.nombre);
			fscanf(archivo,"%*[^\n]\n");
		}
		else
		{
			if((ptr_especie_origen = buscar_especie(pokedex->pokemones,&especie_origen)) && (ptr_pokemon_a_evol = tomar_capturado(ptr_especie_origen->pokemones,&pokemon_a_evol.nombre[0])))  
			{
				ptr_especie_destino = arbol_buscar(pokedex->pokemones,(void*)&especie_destino);

				if(!ptr_especie_destino)
				{ 
					ptr_especie_destino = especie_crear_copia(&especie_destino);
					if(!ptr_especie_destino)
					{
						estado = ERROR_RESERVA_MEMORIA;
						break;
					}
					arbol_insertar(pokedex->pokemones,ptr_especie_destino);
				}
				
				especie_agregar_pokemon(ptr_especie_destino,ptr_pokemon_a_evol);
			}
		}	
		
	}
	fclose(archivo);
	return 0;
}



void pokedex_ultimos_capturados(pokedex_t* pokedex)
{
	if(!pokedex) return;

	particular_pokemon_t* actual = NULL;

	while(!lista_vacia(pokedex->ultimos_capturados))
	{
		actual = lista_tope(pokedex->ultimos_capturados);
		lista_desapilar(pokedex->ultimos_capturados);
		printf("Nombre: %s \n Nivel: %i \n",actual->nombre,actual->nivel);
		free(actual);
	}
}

void pokedex_ultimos_vistos(pokedex_t* pokedex)
{
	if(!pokedex) return;

	particular_pokemon_t* actual = NULL;

	while(!lista_vacia(pokedex->ultimos_vistos))
	{
		actual = lista_primero(pokedex->ultimos_vistos);
		lista_desencolar(pokedex->ultimos_vistos);
		printf("Nombre: %s \n Nivel: %i \n",actual->nombre,actual->nivel);
		free(actual);
	}

}

void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE])
{
	if(!pokedex) return;
	especie_pokemon_t especie_aux = {-1,"VACIO","VACIO",NULL};
	especie_aux.numero = numero_pokemon;
	particular_pokemon_t* ptr_pokemon = NULL;

	especie_pokemon_t* ptr_especie = arbol_buscar(pokedex->pokemones,(void*)&especie_aux);
	if(ptr_especie)
	{
		
		if(!nombre_pokemon[0])
		{
			printf("Especie: %s \n",ptr_especie->nombre);
			lista_con_cada_elemento(ptr_especie->pokemones,mostrar_pokemon,NULL);
		}
		else
		{
			ptr_pokemon = buscar_pokemon(ptr_especie->pokemones,&nombre_pokemon[0]);
			if(ptr_pokemon)
			{
				printf("Especie: %s \n",ptr_especie->nombre);
				mostrar_pokemon(ptr_pokemon,NULL);
			}
			else
				printf("El pokemon %s de la especie %s no se posee \n",&nombre_pokemon[0],ptr_especie->nombre);	
		}
	} 
	else
		printf("La especie %i no existe o no se conoce \n",numero_pokemon);
}
/*
	Dado un archivo abierto imprime en el con un formato la informacion de la estructura
	pokemon_t recibida por referencia.
	Asume que el archivo existe y esta abierto y que no existen punteros a pokemon NULL en la lista.
*/

void guardar_pokemon(void* pokemon, void* archivo)
{
	particular_pokemon_t* aux = (particular_pokemon_t*)pokemon;
	fprintf((FILE*)archivo,"P;%s;%i;%c\n",aux->nombre,aux->nivel,aux->capturado ? 'S' : 'N');
}

/*
	Dado un archivo abierto imprime en el una linea con formato que corresponde a la informacion 
	de la estructura especie_pokemon_t recibida por referencia.
	Se asume que no punteros a especies NULL y el archivo esta abierto y es valido.
	Si ocurre un error de escritura se detiene el recorrido.

*/

bool guardar_especie(void* especie,void* archivo)
{
	especie_pokemon_t* aux = (especie_pokemon_t*)especie;

	if(fprintf((FILE*)archivo,"E;%s;%i;%s\n",aux->nombre,aux->numero,aux->descripcion) < 0)
		return true;

	lista_con_cada_elemento(aux->pokemones,guardar_pokemon,(void*)archivo);
	return false;
}

int pokedex_apagar(pokedex_t* pokedex)
{
	FILE* destino = fopen("arch_aux.txt","w");
	if(!destino) return -1;
	
	fprintf(destino,"%s\n",pokedex->nombre_entrenador);
	abb_con_cada_elemento(pokedex->pokemones,ABB_RECORRER_PREORDEN,guardar_especie,(void*) destino);
	if(ferror(destino))
	{
		fclose(destino);
		remove("arch_aux.txt");
		return -1;
	}
	fclose(destino);
	remove("pokedex.txt");
	rename("arch_aux.txt","pokedex.txt");
	return 0;
}

/*
	Si puede reserva memoria para una especie y su correspondiente lista y devuelve
	el puntero, sino, devuelve NULL.
*/

especie_pokemon_t* especie_crear()
{
	especie_pokemon_t* especie = calloc(1,sizeof(especie_pokemon_t));
	if(!especie) return NULL;
	especie->pokemones = lista_crear();

	if(!especie->pokemones)
	{
		free(especie);
		return NULL;
	}
	return especie;
}

/*
	Supone el archivo abierto y al comienzo de la linea correspondiente con el pokemon a leer y el 
	formato adecuado (Lo cual tiene sentido porque yo mismo creo la pokedex.txt). 

*/

int leer_pokemon(FILE* archivo, particular_pokemon_t* ptr_pokemon)
{
	char capturado = '\0';

	fscanf(archivo,";%[^;];%i;%c\n",&ptr_pokemon->nombre[0],&ptr_pokemon->nivel,&capturado);
	ptr_pokemon->capturado = capturado == 'S' ? true : false;

	return 0;
}

/*
	Supone el archivo abierto y al comienzo de la linea correspondiente con la especie a leer y el 
	formato adecuado (Lo cual tiene sentido porque yo mismo creo la pokedex.txt). 

*/

int leer_especie(FILE* archivo, especie_pokemon_t* ptr_especie)
{
return fscanf(archivo,";%[^;];%i;%[^\n]\n",&ptr_especie->nombre[0],&ptr_especie->numero,&ptr_especie->descripcion[0]);
}


/*
	Siempre devuelve el proximo caracter en el archivo abierto. Si el caracter consultado coincide
	con el caracter a retirar, lo lee avanzando el puntero del archivo y consumiendolo.
	No posee un valor de control para retirar dado que cualquier caracter (imprimible o no) puede
	estar en un archivo.
*/

int mirar_caracter(FILE* archivo,const char retirar)
{
	int c = fgetc(archivo);
	if((char)c != retirar)
		ungetc(c,archivo);
	return c;
}

pokedex_t* pokedex_prender()
{
	int inicio = 0;
	int estado = 0;
	pokedex_t* ptr_pokedex = NULL;
	particular_pokemon_t* ptr_pokemon = NULL;
	especie_pokemon_t* ptr_especie = NULL;

	FILE* arch_pokedex = fopen("pokedex.txt","r");
	if(!arch_pokedex) return NULL;

	if(archivo_vacio(arch_pokedex) || !(ptr_pokedex = pokedex_crear("VACIO")))
	{
		fclose(arch_pokedex);
		return NULL;
	} 

	/*Leo la primer linea del archivo y la guardo como nombre de entrenador
	  NOTA: no podria ser vacio el nombre del entrenador porque yo genere el .txt*/
	fscanf(arch_pokedex,"%[^\n]\n",&ptr_pokedex->nombre_entrenador[0]);

	while(((inicio = mirar_caracter(arch_pokedex,'E')) != EOF) && estado_valido(estado))
	{ 
		ptr_especie = especie_crear();
		if(!ptr_especie)
		{
			estado = ERROR_RESERVA_MEMORIA;
			break;
		}
		leer_especie(arch_pokedex,ptr_especie);

		while(( inicio = mirar_caracter(arch_pokedex,'P')) != EOF && (char)inicio == 'P')
		{
			ptr_pokemon = calloc(1,sizeof(particular_pokemon_t));
			if(!ptr_pokemon)
			{	
				estado = ERROR_RESERVA_MEMORIA;
				break;
			}
			leer_pokemon(arch_pokedex,ptr_pokemon);
			lista_insertar(ptr_especie->pokemones,(void*)ptr_pokemon);
		}
		arbol_insertar(ptr_pokedex->pokemones,(void*)ptr_especie);

	}
	
	if(ferror(arch_pokedex)) estado = ERROR_LECTURA_ARCHIVO;

	if(!estado_valido(estado))
	{
		pokedex_destruir(ptr_pokedex);
		fclose(arch_pokedex);
		return NULL;
	}

	fclose(arch_pokedex);
	return ptr_pokedex;
}

void pokedex_destruir(pokedex_t* pokedex)
{
	if(!pokedex)
		return;

	particular_pokemon_t* pokemon;

	while((pokemon = lista_tope(pokedex->ultimos_capturados)) != NULL)
	{
		lista_desapilar(pokedex->ultimos_capturados);
		free(pokemon);
	}

	while((pokemon = lista_primero(pokedex->ultimos_vistos)) != NULL)
	{
		lista_desencolar(pokedex->ultimos_vistos);
		free(pokemon);
	}

	lista_destruir(pokedex->ultimos_capturados);
	lista_destruir(pokedex->ultimos_vistos);
	arbol_destruir(pokedex->pokemones);
	free(pokedex);
}

