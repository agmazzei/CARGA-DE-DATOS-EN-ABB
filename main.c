#include "pokedex.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMANDOS 10
#define MAX_NUMERO_ESPECIE 8
#define SALIR 'S'
#define COLOR_ERROR "\x1b[0;31m"
#define COLOR_LINEA_POKEDEX "\x1b[33m"
#define TEXTO_RESET "\x1b[0m"
#define TEXTO_DESTACADO "\x1b[1m"


typedef struct comando
{
	char clave;
	bool (*disponible)(pokedex_t*);
	char descripcion[100];
	void (*accion)(pokedex_t**);

}comando_t;

/*
	Limpia el buffer del teclado moviendo el puntero del archivo hasta un \n o EOF. 
	NO puede usarse si no se sabe que hay algun salto de linea o EOF en el buffer.
*/
void limpiar_stdin();

/*
	Acciones correspondientes a cada uno de los comandos disponibles	
*/

void iniciar_pokedex(pokedex_t** pokedex);
void guardar_pokedex(pokedex_t** pokedex);
void avistar_pokemones(pokedex_t** pokedex);
void evolucionar_pokemones(pokedex_t** pokedex);
void mostrar_ultimas_capturas(pokedex_t** pokedex);
void mostrar_ultimos_vistos(pokedex_t** pokedex);
void salir_del_programa(pokedex_t** pokedex);
void mostrar_comandos_disponibles(pokedex_t** pokedex);
void solicitar_info_especie(pokedex_t** pokedex);
void solicitar_info_pokemon(pokedex_t** pokedex);
void mostrar_comandos_disponibles(pokedex_t** pokedex);

/*
	Funcionas internas a cada comando, comprueban su disponibilidaa en el momento dado en el
	que se las usa
*/

bool avistar_disponible(pokedex_t* pokedex);
bool iniciar_disponible(pokedex_t* pokedex);
bool guardar_disponible(pokedex_t* pokedex);
bool salir_disponible (pokedex_t* pokedex);
bool ayuda_disponible(pokedex_t* pokedex);
bool evolucion_disponible(pokedex_t* pokedex);
bool ultimas_capturas_disponible(pokedex_t* pokedex);
bool ultimos_vistos_disponible(pokedex_t* pokedex);
bool informacion_especie_disponible(pokedex_t* pokedex);
bool informacion_pokemon_disponible(pokedex_t* pokedex);


/*
	leer_ opcion lee un caracter para el menu principal y procesar_comando se fijsa si 
	existe y esta disponible y de estarlo lo ejectura, sino, muestra ayuda.
*/
char leer_opcion();
void procesar_comando(char clave_comando, pokedex_t** pokedex);


/*
	constante global con todos los comandos y su inicializacion correspondiente.
*/
const comando_t comandos[MAX_COMANDOS] = {{'I',&iniciar_disponible,"Iniciar la pokedex",&iniciar_pokedex},
								  {'G',&guardar_disponible,"Guardar pokedex",&guardar_pokedex},
								  {'A',&avistar_disponible,"Avistar nuevos pokemones",&avistar_pokemones},
								  {'S',&salir_disponible,"Salir del programa sin guardar cambios",&salir_del_programa},
								  {'H',&ayuda_disponible,"Ayuda",&mostrar_comandos_disponibles},
								  {'E',&evolucion_disponible,"Evolucionar pokemones",&evolucionar_pokemones},
								  {'C',&ultimas_capturas_disponible,"Muestra los ultimos pokemones capturados",&mostrar_ultimas_capturas},
								  {'V',&ultimos_vistos_disponible,"Muestra los ultimos pokemones vistos",&mostrar_ultimos_vistos},
								  {'M',&informacion_especie_disponible,"Pedir informacion sobre especie",&solicitar_info_especie},
								  {'P',&informacion_pokemon_disponible,"Pedir informacion sobre un pokemon",&solicitar_info_pokemon}};

int main()
{
	int opcion;
	bool terminar = false;
	pokedex_t* pokedex = NULL;

	while(!terminar)
	{	
		printf(COLOR_LINEA_POKEDEX"pokedex >"TEXTO_RESET);
		opcion = (int)leer_opcion();
		opcion = toupper(opcion);
		procesar_comando((char)opcion,&pokedex);
		
		if(opcion == SALIR)
			terminar = true;
	}
}

char leer_opcion()
{
	char buffer[2];
	char opcion = '\0';
	fgets(&buffer[0],sizeof(buffer),stdin);
	if(strcspn(buffer,"\n") > 0) limpiar_stdin();

	sscanf(buffer,"%c\n",&opcion);
	return opcion;
}

void procesar_comando(char clave_comando, pokedex_t** pokedex)
{
	bool existe = false;
	for(size_t i = 0; i<MAX_COMANDOS && !existe; i++)
	{
		if(comandos[i].clave == clave_comando && comandos[i].disponible(*pokedex))
		{
			comandos[i].accion(pokedex);
			existe = true;
		}

	}
	if(!existe) mostrar_comandos_disponibles(pokedex);
}

void salir_del_programa(pokedex_t** pokedex)
{
	pokedex_destruir(*pokedex);
}


bool avistar_disponible(pokedex_t* pokedex)
{
	 return pokedex ? true : false;
}

bool iniciar_disponible(pokedex_t* pokedex)
{
	return pokedex ? false : true;
}

bool guardar_disponible(pokedex_t* pokedex)
{
	return pokedex ? true : false;
}

bool salir_disponible (pokedex_t* pokedex) 
{
	return true;
}

bool ayuda_disponible(pokedex_t* pokedex)
{
	return true;
}

bool evolucion_disponible(pokedex_t* pokedex)
{
	return (pokedex && !arbol_vacio(pokedex->pokemones))? true : false;
}

bool ultimas_capturas_disponible(pokedex_t* pokedex)
{
	if(!pokedex || lista_vacia(pokedex->ultimos_capturados))
		return false;
	return true;
}

bool ultimos_vistos_disponible(pokedex_t* pokedex)
{
	if(!pokedex || lista_vacia(pokedex->ultimos_vistos))
		return false;
	return true;
}

bool informacion_especie_disponible(pokedex_t* pokedex)
{
	return (pokedex && !arbol_vacio(pokedex->pokemones))? true : false;
}

bool informacion_pokemon_disponible(pokedex_t* pokedex)
{
	return (pokedex && !arbol_vacio(pokedex->pokemones))? true : false;
}

void limpiar_stdin()
{	
	int c;
	while((c = getchar()) != '\n' && c != EOF);
}

void crear_archivo_pokedex(pokedex_t** pokedex)
{
	char buffer[MAX_NOMBRE];
	FILE* arch_pokedex = fopen("pokedex.txt","w");
	
	if(!arch_pokedex)
	{
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET COLOR_ERROR"No pudo crearse un nuevo archivo pokedex.txt \n"TEXTO_RESET);
		return;
	} 
	printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"Ingrese su nombre \n"TEXTO_RESET);
	

	fgets(buffer,sizeof(buffer),stdin);
	if(strcspn(buffer,"\n") > MAX_NOMBRE-2) limpiar_stdin();

	while(buffer[0] == '\n' && buffer[1] == '\0')
	{
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"El nombre del entrenador no puede estar vacio \n"TEXTO_RESET);
		fgets(buffer,sizeof(buffer),stdin);
	    if(strcspn(buffer,"\n") > MAX_NOMBRE-2) limpiar_stdin();
	}

	*pokedex = pokedex_crear(buffer);
	if(!*pokedex)
	{
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET COLOR_ERROR"Error al crearse la pokedex :( \n"TEXTO_RESET);
		fclose(arch_pokedex);
		return;
	} 

	pokedex_apagar(*pokedex);
	fclose(arch_pokedex);
}

void iniciar_pokedex(pokedex_t** pokedex)
{
	*pokedex = pokedex_prender();
	
	if(!*pokedex && !fopen("pokedex.txt","r"))
	{
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"El archivo pokedex.txt no existe, ¿desea crear uno nuevo? S/N\n"TEXTO_RESET);
	 	int c = (int)leer_opcion();
	 	c = toupper(c);
	 	if(c == 'S') crear_archivo_pokedex(pokedex);
	}
	else if(!*pokedex)
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET COLOR_ERROR"Hubo un problema al prender la pokedex :( \n"TEXTO_RESET);
	
	else 
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"Pokedex iniciada \n"TEXTO_RESET);
}

void guardar_pokedex(pokedex_t** pokedex)
{
	if(pokedex_apagar(*pokedex))
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET COLOR_ERROR"Hubo un problema al guardar la pokedex :("TEXTO_RESET);
	else
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"Cambios guardados correctamente! \n"TEXTO_RESET);
}

void avistar_pokemones(pokedex_t** pokedex)
{
	if(pokedex_avistar(*pokedex,"avistamientos.txt"))
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET COLOR_ERROR"Hubo un problema al avistar los nuevos pokemones. Puede que el archivo este vacio o no exista.\n" TEXTO_RESET);
		
	else
		printf(COLOR_LINEA_POKEDEX"pokedex >"TEXTO_RESET TEXTO_DESTACADO"Pokemones avistados! \n"TEXTO_RESET);
}

void evolucionar_pokemones(pokedex_t** pokedex)
{
	if(pokedex_evolucionar(*pokedex,"evoluciones.txt"))
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET COLOR_ERROR"Hubo un problema al evolucionar los pokemones. Puede que el archivo este vacio o no exista. \n"TEXTO_RESET);
	else
		printf(COLOR_LINEA_POKEDEX"pokedex >"TEXTO_RESET TEXTO_DESTACADO"Pokemones evolucionados! \n"TEXTO_RESET);
}

void mostrar_ultimas_capturas(pokedex_t** pokedex)
{
	pokedex_ultimos_capturados(*pokedex);
}

void mostrar_ultimos_vistos(pokedex_t** pokedex)
{
	pokedex_ultimos_vistos(*pokedex);
}

void salir_del_programa(pokedex_t** pokedex);


void mostrar_comandos_disponibles(pokedex_t** pokedex)
{
	printf(TEXTO_DESTACADO" OPCIONES DISPONIBLES :  \n"TEXTO_RESET);
	for(size_t i=0; i<MAX_COMANDOS; i++)
	{
		if(comandos[i].disponible(*pokedex))
			printf("  %c - %s \n",comandos[i].clave,comandos[i].descripcion);
	}
}

void solicitar_info_especie(pokedex_t** pokedex)
{
	printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"Ingrese el numero de la especie: \n"TEXTO_RESET);
	printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET);
	char buffer[MAX_NUMERO_ESPECIE+2];
	int numero_especie;

	fgets(&buffer[0],sizeof(buffer),stdin);
	
	if(strcspn(buffer,"\n") > MAX_NUMERO_ESPECIE) /*Evita que quede basura en el stdin despues del tam de buffer*/
		limpiar_stdin();		  				  /* e interfiera con lecturas posteriores*/

	while(sscanf(buffer,"%i\n",&numero_especie) != 1)
	{
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"Lo ingresado no es numero, intenta denuevo \n"TEXTO_RESET);
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET);
		fgets(&buffer[0],sizeof(buffer),stdin);
		if(strcspn(buffer,"\n") > MAX_NUMERO_ESPECIE)
			limpiar_stdin();
	}

	pokedex_informacion(*pokedex,numero_especie,"");
}

void solicitar_info_pokemon(pokedex_t** pokedex)
{
	char buffer_nombre[MAX_NOMBRE];
	char buffer_numero[MAX_NUMERO_ESPECIE+2];
	int numero_especie;
	size_t pos_enter = 0;
	
	printf(COLOR_LINEA_POKEDEX"pokedex >"TEXTO_RESET TEXTO_DESTACADO"Ingresa el nombre del pokemon, y seguido de enter, el numero de especie \n"TEXTO_RESET);
	printf(COLOR_LINEA_POKEDEX"pokedex >"TEXTO_RESET);
	
	fgets(&buffer_nombre[0],sizeof(buffer_nombre),stdin);
	
	if((pos_enter = strcspn(buffer_nombre,"\n")) > MAX_NOMBRE-2) limpiar_stdin();
	else buffer_nombre[pos_enter] = '\0';
	
	printf(COLOR_LINEA_POKEDEX"pokedex >"TEXTO_RESET);
	fgets(&buffer_numero[0],sizeof(buffer_numero),stdin);
	
	if(strcspn(buffer_numero,"\n") > MAX_NUMERO_ESPECIE) limpiar_stdin();
	else buffer_nombre[pos_enter] = '\0';

	while(sscanf(buffer_numero,"%i\n",&numero_especie) != 1)
	{
		printf(COLOR_LINEA_POKEDEX"pokedex > "TEXTO_RESET TEXTO_DESTACADO"No podes ingresar un valor no numerico o vacio, intenta denuevo \n"TEXTO_RESET);
		fgets(&buffer_numero[0],sizeof(buffer_numero),stdin);
		if(strcspn(buffer_nombre,"\n") > MAX_NUMERO_ESPECIE) limpiar_stdin();
	}

	
	pokedex_informacion(*pokedex,numero_especie,buffer_nombre);
}
