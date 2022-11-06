/*
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>
 * Sistema de Control de Versiones
 * Uso: 
 *      versions add ARCHIVO "Comentario" : Adiciona una version del archivo al repositorio
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

	if (argc == 4
			&& EQUALS(argv[1], "add")) {
		if (add(argv[2], argv[3]) == VERSION_ERROR) {
			fprintf(stderr, "No se puede adicionar %s\n", argv[2]);
		}
	}else if (argc == 2
			&& EQUALS(argv[1], "list")) {
		//Listar todos los archivos almacenados en el repositorio
		list(NULL);
	}else if (argc == 3
			&& EQUALS(argv[1], "list")) {
		//Listar el archivo solicitado
		list(argv[2]);
	}else if (argc == 4
			&& EQUALS(argv[1], "get")) {
		int version = atoi(argv[2]);
		if (version <= 0) {
			fprintf(stderr, "Numero de version invalido\n");
			exit(EXIT_FAILURE);
		}
		if (!get(argv[3], version)) {
			fprintf(stderr, "No se puede obtener la version %d de %s\n", version, argv[3]);
			exit(EXIT_FAILURE);
		}
	}else {
		print_help();
	}

	exit(EXIT_SUCCESS);

}

void print_help() {
	printf("Uso: \n");
	printf("versions add ARCHIVO \"Comentario\" : Adiciona una version del archivo al repositorio\n");
	printf("versions list ARCHIVO             : Lista las versiones del archivo existentes\n");
	printf("versions get numver ARCHIVO       : Obtiene una version del archivo del repositorio\n");
}
