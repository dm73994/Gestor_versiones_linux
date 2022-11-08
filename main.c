/*
 * @file
 * @author Carlos David Mesa Martinez <cadmesa@unicauca.edu.co>
 * @author Anderson Camilo Bonilla <>
 * Sistema de Control de Versiones
 * Uso: 
 *      versions add ARCHIVO "Comentario" : Adiciona una version del archivo al repositorio
 *      versions list 		              : Lista todas las versiones de todos los archivos existentes
 *      versions list ARCHIVO             : Lista las versiones del archivo existentes
 *      versions get numver ARCHIVO       : Obtiene una version del archivo del repositorio
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "versions.h"

/**
* @brief Imprime la ayuda
*/
void print_help();

int main(int argc, char *argv[]) {
	struct stat s;

	mkdir(VERSIONS_DIR, 0755);		

	if (stat(VERSIONS_DB_PATH, &s) != 0) {
		creat(VERSIONS_DB_PATH, 0755);
	}

	if (argc == 4 && EQUALS(argv[1], "add")) {
		return_code proccess = add(argv[2], argv[3]);
		if ( proccess == HASH_NAME_ALREADY_EXIST) {
			fprintf(stderr, "[ERROR] No se puede adicionar %s, el nombre de archivo y hash ya existen.\n", argv[2]);
		}else if( proccess == VERSION_ADDED ){
			printf("[SUCCESS] Se ha agregado %s con exito!\n", argv[2]);
			exit(EXIT_SUCCESS);
		}
	}else if (argc == 2 && EQUALS(argv[1], "list")) {
		//Listar todos los archivos almacenados en el repositorio
		list(NULL);
	}else if (argc == 3
			&& EQUALS(argv[1], "list")) {
		//Listar el archivo solicitado
		list(argv[2]);
	}
	else if (argc == 4 && EQUALS(argv[1], "get")) {
		int version = atoi(argv[2]);
		if (version <= 0) {
			fprintf(stderr, "[ERROR] Numero de version invalido\n");
			exit(EXIT_FAILURE);
		}
		return_code getCode = get(argv[3], version);
		if ( getCode == OPEN_FILE_ERROR ) {
			fprintf(stderr, "[ERROR] Error al ingresar al abrir la base de datos, asegurese de que esta exista.\n");
			exit(EXIT_FAILURE);
		}
		else if( getCode == FILENAME_DOESNT_EXIST){
			fprintf(stderr, "[ERROR] El nombre de archivo (%s) que desea recuperar no existe.\n", argv[3]);
			exit(EXIT_FAILURE);
		}
		else if( getCode == FILE_VERSION_DOESNT_EXIST){
			fprintf(stderr, "[ERROR] No se puede obtener la version %d de %s, asegurese de ingresar una version correcta.\n", version, argv[3]);
			exit(EXIT_FAILURE);
		}
		else if( getCode == VERSION_RECOVERY){
			printf("[SUCCESS] Se ha recuperado la version (%d) del archivo %s con exito!\n",version, argv[3] );
			exit(EXIT_SUCCESS);
		}
	}else {
		print_help();
	}

	exit(EXIT_SUCCESS);

}

void print_help() {
	printf("Uso: \n");
	printf("versions add ARCHIVO \"Comentario\" : Adiciona una version del archivo al repositorio\n");
	printf("versions list                       : Lista todas las versiones de todos los archivos existentes\n");
	printf("versions list ARCHIVO               : Lista las versiones del archivo existentes\n");
	printf("versions get numver ARCHIVO         : Obtiene una version del archivo del repositorio\n");
}
