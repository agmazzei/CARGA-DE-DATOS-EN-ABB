# C---ABB-AND-FILES-PROYECT
Trabajo Práctico Algoritmos y Programación II (UBA)

*********** FUNCIONAMIENTO Y ACLARACIONES:

1)
	Cualquier reserva de memoria que no pudo llevarse a cabo la considere un problema grave (dado que si fallo una reserva de memoria muy probablemente fallen todas las siguientes a esta) y por tanto la funcion sea cual sea no continua con su ejecucion normal, hace lo que tenga que  hacer para no generar perdidas de memoria u otros errores y sale. Despues se le informa al usuario del problema pero el programa no termina, puede seguir utilizando sus funcionalidades.
	
	
2) 
	En los archivos que es posible un formato equivocado (avistamientos.txt, evoluciones.txt),y de encontrarse uno, se ignora la linea y se continua con la siguiente. Es decir que no detengo el procesamiento de avistamientos o evoluciones por un problema de formato en una linea, simplemente la ignoro e intento cargar el resto. El usuario no recibe informacion sobre problemas de formato en el archivo.

3)
	De no existir pokedex.txt para levantar en memoria se le informa al usuario y se le consulta si quiere crear una nueva pidiendole el nombre de entrenador para esta.
	
4)
	En general trate de informar sobre problemas durante el programa como en la apertura de archivos o inexistencia de los mismos. Pero al mismo tiempo trate de no mezclar el codigo "funcional" con el informe al usuario de los errores que pudiesen surgir. Por esto algunos errores no son precisos sino mas bien una o varias sugerencias de lo que podria haber pasado.
	
5)
	Las entradas que el usuario emite para interactuar con el programa (para ingresar una opcion del menu o un nombre de pokemon,etc) trate de que estuviesen lo mas limpias posibles limpiando el buffer del teclado para evitar que se pisen unas con otras y limitando lo que se puede leer para evitar overflows. Tambien compruebo el formato de la entrada para distinguir entre numeros y letras.

6) 	La evolucion, consulta sobre pokemon y sobre especie solo pueden hacerse con una pokedex 	 que contenga algo mas que el nombre del entrenador. Los ultimos capturados y vistos solo 	  estan disponibles despues de avistar. El guardado, el salir y la ayuda siempre estan 		    disponibles.

NOTA:
	En algunas funciones como pokedex_apagar y pokedex_prender utilizo un int estado para salir cuando hay un problema de reserva de memoria u otro error. La idea era utilizarlo para tener mas informacion de los errores asignando distintos valores numericos a estado pero no queria crear una variable global ni modificar el .h para agregar un campo de flag de erores asi que quedo como herramiento de debugueo mas que nada (tambien ayuda a controlar el resultado final de la funcion y hacer una cosa u otra dependiendo el estado).
	
	
***************LINEA DE COMPILACION:

	Utilizo el siguiente comando de gcc para compilar el main del programa principal junto con la implementacion de pokedex.c y a la vez compilar el tda lista y abb (todos los .h, los .c de la pokedex y los .c de los tda los tengo en la misma carpeta juntos con los .txt necesarios).
	
	gcc -g *c -Wall -Werror -Wconversion -std=c99 -o pokedex

Para el valgrind uso:

	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./pokedex
