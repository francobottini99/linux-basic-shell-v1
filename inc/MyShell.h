/**
 * @file MyShell.h
 * @author Bottini, Franco Nicolas
 * @brief Pequeña aplicacion shell. Hace uso de un mecanismo simple de control de trabajos.
 * Implementa un intérprete de línea de comandos al estilo Bourne shell.
 * @version 1.2
 * @date Septiembre de 2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __MYSHELL_H__
#define __MYSHELL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "JobControl.h"

/** Define los codigos para cambiar el color del texto en la terminal **/
#ifndef TERMINAL_TEXT_COLORS
#define TERMINAL_TEXT_COLORS
    #define KDEF  "\x1B[0m"
    #define KRED  "\x1B[31m"
    #define KGRN  "\x1B[32m"
    #define KYEL  "\x1B[33m"
    #define KBLU  "\x1B[34m"
    #define KMAG  "\x1B[35m"
    #define KCYN  "\x1B[36m"
    #define KWHT  "\x1B[37m"
#endif

/** Define distintos caracteres ASCII para hacer mas legible el codigo **/
#ifndef ASCII_CHARACTERS
#define ASCII_CHARACTERS
    #define ASCII_END_OF_STRING '\0'
    #define ASCII_MONEY_SIGN '$'
    #define ASCII_MIDDLE_DASH '-'
    #define ASCII_SPACE ' '
    #define ASCII_LINE_BREAK '\n'
#endif

/** Longitud maxima de las entradas que admtide el programa **/
#define MAX_LEN_INPUT 256 

/** Macro para calcular el tamaño de un const array **/
#define CONST_STR_ARR_SIZE(arr) sizeof(arr) / sizeof(*arr)

/** Posibles resultados al leer las entradas de comandos **/
typedef enum READ_INPUT_RESULT
{
    INP_TO_LONG = -2,   /** La entrada supera la longitud maxima admitida **/
    INP_END = -1,       /** No hay entradas que leer **/
    INP_EMPTY_LINE = 0, /** Lectura de una linea vacia **/
    INP_READ = 1        /** Lectura exitosa de una entrada **/
} READ_INPUT_RESULT;

/** Flags de los comandos admitidos **/
typedef enum COMMANDS_FLAGS
{
    CMM_EXTERN = -1,    /** Comando externo **/
    CMM_QUIT = 0,       /** Comando quit **/
    CMM_CD = 1,         /** Comando cd **/
    CMM_CLR = 2,        /** Comando clear **/
    CMM_ECHO = 3,       /** Comando echo **/
    CMM_JOBS = 4        /** Comando jobs **/
} COMMANDS_FLAGS;

/** Array de los comandos admitidos **/
const char* CMM_VALIDS[] = {
    "quit",
    "cd",
    "clr",
    "echo",
    "jobs"
};

/**
 * @brief Valida que el numero de parametros introducido al ejecutar el programa sea valido.
 * 
 * @param argc Numero de argumentos de entrada.
 */
void myshell_validate_execution(int argc);

/**
 * @brief Ejecuta el loop principal de la shell de manera indefinida.
 * 
 * @param input_source Archivo desde el cual se van a tomar las entradas de comandos.
 */
void myshell_loop(FILE* input_source);

/**
 * @brief Imprime por consola el prompt.
 * 
 */
void print_prompt(void);

/**
 * @brief Dependiendo los parametros dados en la ejecucion del programa, obtiene el archivo fuente desde donde se van a leer los comandos entrantes.
 * 
 * @param argc Numero de argumentos de entrada.
 * @param argv Array de argumentos de entrada.
 * @return FILE* Puntero al archivo desde el cual se van a tomar los comandos entrantes.
 */
FILE* command_source(int argc, char* argv[]);

/**
 * @brief Interpreta y ejecuta un comando dado.
 * 
 * @param input cadena que se debe interpretar y ejecutar.
 */
void execute_input(char* input);

/**
 * @brief Ejecuta un comando a partir de su identificador y sus argumentos. 
 * 
 * @param cmm Identificador o flag del comando a ejecutar.
 * @param args Cadena con los argumentos necesarios para la ejecucion del comando.
 */
void command_interprete(COMMANDS_FLAGS cmm, char* args);

/**
 * @brief Get the Input object
 * 
 * @param buffer Puntero al buffer donde se quiere almacenar la lectura.
 * @param buffer_size Tamaño del buffer donde se quiere almacenar la lectura.
 * @param fp Puntero al archivo desde donde se debe realizar la lectura.
 * @return READ_INPUT_RESULT Codigo que indica el resultado de la operacion.
 */
READ_INPUT_RESULT get_input(char* buffer, int buffer_size, FILE* fp);

/**
 * @brief Elimina los espacios en blanco al comienzo y final de una cadena.
 * 
 * @param str Cadena sobre la cual operar.
 * @return char* Puntero a la cadena resultante.
 */
char* trim_white_space(char* str);

/**
 * @brief Cambia de directorio de trabajo.
 * 
 * @param dir directorio al que se quiere ir.
 */
void execute_cd(char* dir);

/**
 * @brief Envia un mensaje o el valor de una variable de entorno a la terminal.
 * 
 * @param value mensaje o variable de entorno a enviar.
 */
void execute_echo(char* value);

/**
 * @brief Ejectura un comando externo al programa en un nuevo proceso.
 * 
 * @param command path del comando a ejecutar.
 */
void execute_extern(char* command);

/**
 * @brief Finaliza la ejecucion del programa.
 * 
 */
void execute_quit(void);

/**
 * @brief Limpia la terminal.
 * 
 */
void execute_clr(void);

#endif //__MYSHELL_H__