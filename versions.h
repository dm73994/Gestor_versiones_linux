
#ifndef VERSIONS_H
#define VERSIONS_H

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sha256.h"

#define COMMENT_SIZE BUFSIZ /** < Longitud del comentario */
#define HASH_SIZE 65 /**< Longitud del hash incluyendo NULL*/

#define VERSIONS_DB "versions.db" /**< Nombre de la base de datos de versiones. */
#define VERSIONS_DIR ".versions" /**< Directorio del repositorio. */
#define VERSIONS_DB_PATH VERSIONS_DIR "/" VERSIONS_DB /**< Ruta completa de la base de datos.*/


#define EQUALS(s1, s2) (strcmp(s1, s2) == 0) /**< Verdadero si dos cadenas son iguales.*/

/**
 * @brief Version de un archivo.
 * Para cada version de un archivo se almacena el nombre original,
 * el comentario del usuario y el hash de su contenido.
 * El hash es a la vez el nombre del archivo dentro del 
 * repositorio.
 */
typedef struct {
	char filename[NAME_MAX]; /**< Nombre del archivo original. */
	char hash[HASH_SIZE];           /**< Hash del contenido del archivo. */
	char comment[COMMENT_SIZE];	   /**< Comentario del usuario. */
}file_version;


typedef enum {
	VERSION_ERROR,
	VERSION_CREATED,
	VERSION_ADDED,
	VERSION_ALREADY_EXISTS,
	VERSION_DOES_NOT_EXIST,
	VERSION_RETRIEVED,
	VERSION_RECOVERY,
	HASH_NAME_ALREADY_EXIST,
	HASH_NAME_DOESNT_EXIST,
	NAME_DOESNT_EXIST,
	NAME_EXIST,
	HASH_DOESNT_EXIST
}return_code;


/**
 * @brief Adiciona un archivo al repositorio.
 *
 * @param filename Nombre del archivo a adicionar
 * @param comment Comentario de la version actual
 *
 * @return Codigo de la operacion
 */
return_code add(char * filename, char * comment);

/**
 * @brief Lista las versiones de un archivo.
 *
 * @param filename Nombre del archivo, NULL para listar todo el repositorio.
 */
void list(char * filename);

/**
 * @brief Obtiene una version del un archivo.
 * Sobreescribe la version existente.
 *
 * @param filename Nombre de archivo.
 * @param version Numero secuencial de la version.
 *
 * @return Codigo de la operacion (VERSION_ERROR,VERSION_DOES_NOT_EXIST, VERSIO_RETRIEVED)
 */
return_code get(char * filename, int version);

#endif
