#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NAME_LENGTH 100

typedef struct {


    int id;

    char name[MAX_NAME_LENGTH];


} Alumno;

void menu();

void leerDisco();

void grabarDato();

void agregarRegistro(Alumno **alumnos, int *cantidad);

void guardarEnDisco(Alumno *alumnos, int cantidad);


Alumno* cargarDesdeDisco(int *cantidad);

void imprimirAlumnos(Alumno *alumnos, int cantidad);

int main() {


    int opcion;

    do {


        menu();

        printf("Selecciona una opcion: ");

        scanf("%d", &opcion);

        switch(opcion) {


            case 1:

                leerDisco();

                break;

            case 2:

                grabarDato();

                break;

            case 0:

                printf("Saliendo del programa...\n");

                break;

            default:

                printf("Opcion no valida. Intenta de nuevo.\n");
        }
    }

    while(opcion != 0);

    return 0;
}

void menu() {


    printf("\nMenu:\n");

    printf("1. Leer disco\n");

    printf("2. Grabar dato\n");

    printf("0. Salir\n");
}


void leerDisco() {


    int cantidad;

    Alumno *alumnos = cargarDesdeDisco(&cantidad);

    if (alumnos != NULL) {


        imprimirAlumnos(alumnos, cantidad);

        free(alumnos);


    } else {


        printf("No se pudo leer el archivo o el archivo esta vacio.\n");


    }
}

void grabarDato() {


    Alumno *alumnos = NULL;

    int cantidad = 0;

    char opcion;

    do {


        agregarRegistro(&alumnos, &cantidad);

        printf("Desea ingresar otro registro? (s/n): ");

        scanf(" %c", &opcion);

    }

    while(opcion == 's' || opcion == 'S');
    guardarEnDisco(alumnos, cantidad);
    free(alumnos);
}

void agregarRegistro(Alumno **alumnos, int *cantidad) {
    *alumnos = realloc(*alumnos, (*cantidad + 1) * sizeof(Alumno));
    if (*alumnos == NULL) {
        printf("Error al asignar memoria.\n");
        exit(1);
    }
    printf("Ingrese ID del alumno: ");

    scanf("%d", &(*alumnos)[*cantidad].id);

    printf("Ingrese nombre del alumno: ");

    scanf(" %[^\n]", (*alumnos)[*cantidad].name);


    (*cantidad)++;
}

void guardarEnDisco(Alumno *alumnos, int cantidad)

{
    FILE *file = fopen("alumnos.dat", "wb");


    if (file == NULL)

        {
        printf("Error al abrir el archivo para escribir.\n");

        return;

    }
    fwrite(&cantidad, sizeof(int), 1, file);

    fwrite(alumnos, sizeof(Alumno), cantidad, file);

    fclose(file);

    printf("Datos guardados exitosamente.\n");


}

Alumno* cargarDesdeDisco(int *cantidad)

 {
    FILE *file = fopen("alumnos.dat", "rb");

    if (file == NULL)

        {
        printf("Error al abrir el archivo para leer.\n");

        return NULL;

    }


    fread(cantidad, sizeof(int), 1, file);


    if (*cantidad <= 0)

        {
        fclose(file);

        return NULL;
    }
    Alumno *alumnos = malloc(*cantidad * sizeof(Alumno));


    if (alumnos == NULL)

        {
        printf("Error al asignar memoria.\n");


        exit(1);
    }

    fread(alumnos, sizeof(Alumno), *cantidad, file);

    fclose(file);

    return alumnos;


}

void imprimirAlumnos(Alumno *alumnos, int cantidad)

 {
    printf("\nAlumnos:\n");

    for (int i = 0; i < cantidad; i++)

        {
        printf("ID: %d, Nombre: %s\n", alumnos[i].id, alumnos[i].name);
    }
}
