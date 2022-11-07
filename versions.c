#include <limits.h>
#include <sys/stat.h>
#include "versions.h"


/**
 * @brief Obtiene el hash de un archivo.
 * @param filename Nombre del archivo a obtener el hash
 * @param hash Buffer para almacenar el hash (HASH_SIZE)
 * @return Referencia al buffer, NULL si ocurre error
 */
char *get_file_hash(char * filename, char * hash);

/**
 * @brief Copia un archivo
 *
 * @param source Archivo fuente
 * @param destination Destino
 *
 * @return 1 en caso de exito, 0 si existe error.
 */
int copy(char * source, char * destination);

//DOCUMENTAR
int addVersion(file_version newVersion);
void shortHash(char*hash);
void printVersionStruct(file_version version);
int checkVersion(char*verifyHash);


int copy(char * source, char * destination) {
	//REFERENCIA A ARCHIVOS 
    	FILE*sourceFile;
       	FILE*destFile;

	//VERIFICAR SI SE PUEDEN ABRIR LOS ARCHIVOS
	sourceFile = fopen(source, "r");
      	destFile = fopen(destination, "w");
    
	//SI OCURRIO UN ERROR CON EL ARCHIVO SOURCE
	if(sourceFile == NULL){
		//perror(sourceFile);
		return 0;
	}	


    //Declaración del buffer donde se va a leer
    char buffer[BUFSIZ];
	//Declaracion de la bandera de lineas leidas
	size_t nreads;
    while(!feof(sourceFile))
    {
        nreads = fread(buffer,sizeof(char), BUFSIZ, sourceFile);
        if(nreads == 0)
        {
            break;
        }
        fwrite(buffer, sizeof(char), nreads, destFile);

    }

    	fclose(sourceFile);	
	fclose(destFile);

	return 1;
}



return_code add(char * filename, char * comment) {
	//verificar si el archivo existe
	struct stat s;	
	
	// si stat devuelve -1 hay un error
	if( stat(filename, &s) == -1){		
		perror("stat");
		return 0;
	}
	
	//SI NO ES UN ARCHIVO REGULAR
	if( !S_ISREG(s.st_mode) ){		
		return 0;
	}
	
	//hash del archivo
	char*hash = (char*) malloc(HASH_SIZE);
	get_file_hash(filename, hash);
	
	// TODO VERIFICAR HASH
	checkVersion(hash);
	
	//generar nuevo nombre del archivo
	char*nuevoNombre = (char*) malloc(strlen( VERSIONS_DIR ) +  + HASH_SIZE + 1);
	strcpy(nuevoNombre, VERSIONS_DIR);
	strcat(nuevoNombre, "/");
	strcat(nuevoNombre, filename);
	strcat(nuevoNombre, hash);
	
	if( copy(filename, nuevoNombre) == 1 &&  ){		
		file_version file;
		strcpy(file.filename, filename);
		strcpy(file.hash, hash);
		strcpy(file.comment, comment);
		
		if( addVersion(file) == 1){
			return VERSION_ADDED;
		}
	}
	
	return VERSION_ERROR;
}

//return 1 si la version ya exixst, 3 si ocurrio un error en la carpeta, 0 successfull
int checkVersion(char*hash){
	DIR*versionsDir = opendir("./.versions"); // abrir carpeta de versiones
	struct dirent*ent; //guarda informacion sobre el archivo que se extrae cada momento

	if(versionsDir == NULL) return 3;

	while( (ent = readdir(versionsDir)) != NULL ){
		printf("FILENAME: %s\n", ent->d_name);
	}	

	return 0;
}

//**
// RECIBE una estructura archivo y la guarda en la bd
// retorna 1 si se pudo sobreescribir en la bd 0 en caso de error
//**
int addVersion(file_version newVersion){
	FILE*db = fopen(VERSIONS_DB_PATH, "ab");

	// sino se pudo abrir la bd
	if(db == NULL) return 0;
	
	//escribir en la bd
	fwrite(&newVersion, sizeof(file_version), 1, db);

	return fwrite != 0 ? 1 : 0; // 
}


void list(char * filename) {
	//abrir la base de datos
	FILE*db = fopen(VERSIONS_DB_PATH, "r");

	//verificar si se abrio la base de datos
	if(db == NULL) 	return;

	// se almacena temporarmente la estructura leida en version
	file_version versionStruc;
	
	//SI EL PARAMETRO ARCHIVO ES NULL LISTAR TODOS LOS BACKUP
	if(filename == NULL){
		while( fread(&versionStruc, sizeof(file_version), 1, db) ) printVersionStruct(versionStruc);
		fclose(db);
		return;
	}

	//leer la bd
	while( fread(&versionStruc, sizeof(file_version), 1, db) ){
		// SI EL PARAMETRO DE NOMBRE DE ARCHIVO COINCIDE EN UNA ESTRUCTURA DE LA BASE DE DATOS:
		if(EQUALS(versionStruc.filename, filename)) printVersionStruct(versionStruc);
	}
	fclose(db);
}

void printVersionStruct(file_version version){	
	printf("VERSIONES DE %s\n", version.filename);
	printf("FILENAME: [ %s ] \n", version.filename);
	printf("HASH: [ %s ]\n", version.hash);
	printf("COMMENTS: [ %s ] \n", version.comment);
	printf("\n");
}


void shortHash(char*hash){
	/*int hashLen = strlen(hash);
	char printableHash;
	strcpy(printableHash, hash[0]);
	strcat(printableHash, "...");
	printf("HASH: [ %s ]\n", printableHash);
	*/
}



char *get_file_hash(char * filename, char * hash) {
	char *comando;
	FILE * fp;

	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("stat");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);

	return hash;

}


return_code get(char * filename, int version) {
	FILE*db = fopen(VERSIONS_DB_PATH, "r");
	
	//verificar archivo
	if(db == NULL)	return VERSION_ERROR;
	
	file_version versionStruct;
	//fread(&versionStruct, sizeof(file_version), 1, db);
	int count = 1;
	while( fread(&versionStruct, sizeof(file_version), 1, db) ){
		if( EQUALS(versionStruct.filename, filename)){			
			if(count == version){
				char*path = (char*)malloc(strlen(VERSIONS_DIR) + HASH_SIZE + 2);
				strcpy(path, VERSIONS_DIR);
				strcat(path, "/");
				strcat(path, versionStruct.hash);
								
				char*destFile = (char*)malloc(strlen(versionStruct.filename) + 2);
				strcpy(destFile, "./");
				strcat(destFile, versionStruct.filename);

				copy(path, destFile);
				break;
			}else count++;			

		}
	}
	
	fclose(db);
	
	return VERSION_RECOVERY;
}
