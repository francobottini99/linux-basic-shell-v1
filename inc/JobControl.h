/**
 * @file JobControl.h
 * @author Bottini, Franco Nicolas
 * @brief Libreria que permite implementar un control de trabajos simple.
 * @version 1.2
 * @date Septiembre de 2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __JOB_CONTROL_H__
#define __JOB_CONTROL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

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

/** Filtros admitidos para la busqueda de procesos **/
typedef enum PROCESS_FILTERS
{
    PROC_FILTER_ALL,        /** Todos los procesos **/
    PROC_FILTER_DONE,       /** Procesos finalizados **/
    PROC_FILTER_REMAINING   /** Procesos activos **/
} PROCESS_FILTERS;

/** Tipos de ejecuciones admitidas por los trabajos **/
typedef enum PROCESS_EXECUTION_MODES
{
    BACKGROUND_EXECUTION,   /** Ejecucion en segundo plano **/
    FOREGROUND_EXECUTION,   /** Ejecucion en primer plano **/
    PIPELINE_EXECUTION      /** Ejecucion en una pipeline **/
} PROCESS_EXECUTION_MODES;

/** Estados asignables a un proceso **/
typedef enum PROCESS_STATUS
{
    STATUS_RUNNING,     /** En ejecucion **/
    STATUS_DONE,        /** Finalizado **/
    STATUS_SUSPENDED,   /** Suspendido **/
    STATUS_CONTINUED,   /** Reanudado **/
    STATUS_TERMINATED,  /** Anulado **/
    STATUS_NEW,         /** Nuevo proceso **/
    STATUS_READY        /** Proceso agregado a un trabajo listo para correr **/
} PROCESS_STATUS;

/** Estructura de datos que define un proceso **/
typedef struct process 
{
    struct process *next;   /** Siguiente proceso en la lista **/
    u_int8_t argc;          /** Numero de argumentos para el proceso **/
    char **argv;            /** Array de argumentos del proceso **/
    pid_t pid;              /** Process ID **/
    PROCESS_STATUS status;  /** Estado del proceso **/
} process;

/** Estructura de datos que define un trabajo **/
typedef struct job 
{
    struct job *next;               /** Siguiente trabajo en la lista **/
    int id;                         /** ID del trabajo **/
    struct process *first_process;  /** Primer proceso de la lista **/
    pid_t pgid;                     /** Process group ID **/
    PROCESS_EXECUTION_MODES mode;   /** Modo de ejecucion **/
    int io_fd[2], err_fd[2];        /** Pipes de comunicacion **/
} job;

extern const char* PROCESS_STATUS_STRING[]; /** String-array de los estados de un proceso **/

extern job *first_job; /** Primer trabajo de la lista **/

/**
 * @brief Crea un nuevo trabajo.
 * 
 * @param first_process Primer proceso del nuevo trabajo.
 * @param mode Modo de ejecucion del nuevo trabajo.
 * @return job* Trabajo creado.
 */
job* new_job(process *first_process);

/**
 * @brief Crea un nuevo proceso.
 * 
 * @param command Comando de ejecucion para el nuevo proceso.
 * @return process* Proceso creado.
 */
process* new_process(char *command);

/**
 * @brief Agrega un trabajo a la lista de trabajos.
 * 
 * @param j Trabajo a agregar en la lista.
 * @return int ID asignado al trabajo agregado.
 */
int insert_job(job *j);

/**
 * @brief Agrega un proceso a la lista de procesos de un trabajo.
 * 
 * @param j Trabajo al cual agregarle el proceso.
 * @param p Proceso a agregar en la lista.
 */
void insert_process(job *j, process *p);

/**
 * @brief Elimina un trabajo de la lista y libera la memoria utilizada por el trabajo.
 * 
 * @param j Trabajo a eliminar del listado.
 */
void remove_job(job *j);

/**
 * @brief Obtiene el ultimo trabajo agregado a la lista.
 * 
 * @return job* Ultimo trabajo del listado. NULL en caso de que la lista este vacia.
 */
job* get_last_job();

/**
 * @brief Obtiene el trabajo al cual pertenece un proceso a partir de su Process ID.
 * 
 * @param pid Process ID del proceso a buscar dentro de los trabajos.
 * @return job* Trabajo al cual pertenece el proceso dado. NULL en caso de no existir ningun proceso con el PID dado.
 */
job* get_job_by_pid(int pid);

/**
 * @brief Obtiene un trabajo a partir de su ID.
 * 
 * @param id ID del trabajo que se quiere obtener.
 * @return job* Trabajo solicitado. NULL en caso de no existir ningun trabajo con el ID dado.
 */
job* get_job_by_id(int id);

/**
 * @brief Obtiene el trabajo padre de un trabajo.
 * 
 * @param j Trabajo del cual se quiere obtener el padre.
 * @return job* Trabajo padre del trabajo dado.
 */
job* get_job_parent(job *j);

/**
 * @brief Obtiene el ID del trabajo al cual pertenece un proceso a partir de su Process ID.
 * 
 * @param pid Process ID del proceso a buscar dentro de los trabajos.
 * @return int ID del trabajo al cual pertenece el proceso dado. -1 en caso de no existir ningun proceso con el PID dado.
 */
int get_job_id_by_pid(int pid);

/**
 * @brief Modifica el estado de todos los procesos de un trabajo.
 * 
 * @param j Trabajo al cual se le quiere modifcar el estado de sus procesos.
 * @param status Estado a establecer en los procesos del trabajo.
 */
void set_job_status(job *j, PROCESS_STATUS status);

/**
 * @brief Determina si un trabajo completo su ejecucion.
 * 
 * @param j Trabajo que se quiere consultar si esta terminado.
 * @return int 1 si el trabajo esta completo. 0 si el trabajo no esta completo. -1 en caso de producirse un error.
 */
int is_job_completed(job *j);

/**
 * @brief Obtiene el ultimo proceso del listado de procesos de un trabajo.
 * 
 * @param j Trabajo del cual se quiere obtener el ultimo proceso de su listado.
 * @return process* Ultimo proceso del listado de procesos del trabajo.
 */
process* get_last_process(job *j);

/**
 * @brief Obtiene un proceso a partir de su Process ID.
 * 
 * @param pid Process ID del proceso que se quiere obtener.
 * @return process* Proceso solicitado. NULL en caso de no existir ningun proceso con el PID dado.
 */
process* get_process_by_pid(int pid);

/**
 * @brief Obtiene el numero de procesos en un trabajo que cumplen con un filtro dado.
 * 
 * @param j Trabajo del cual se quiere obtener el numero de procesos.
 * @param filter Filtro a aplicar sobre el conteo.
 * @return int Numero de procesos en el trabajo que cumplen con el filtro dado.
 */
int get_processes_count(job *j, int filter);

/**
 * @brief Modifica el estado de un proceso.
 * 
 * @param p Proceso al que se le quiere modificar el estado.
 * @param status Estado a establecer en el proceso.
 */
void set_process_status(process *p, PROCESS_STATUS status);

/**
 * @brief Inicializa el control de trabajos.
 * 
 */
void job_control_init(void);

/**
 * @brief Se ejecuta al recibir una señal.
 * 
 * @param signal Señal recibida.
 */
void sigint_handler(int signal);

/**
 * @brief Espera a la finalizacion de todos los procesos de un trabajo.
 * 
 * @param j Trabajo al cual se debe esperar.
 * @return int Estado del trabajo al terminar.
 */
int wait_for_job(job *j);

/**
 * @brief Espera por la finalizacion de un proceso.
 * 
 * @param p Proceso a esperar.
 * @return int Estado del proceso al terminar.
 */
int wait_for_process(process *p);

/**
 * @brief Ejecuta todos los procesos de un trabajo.
 * 
 * @param j Trabajo a ejecutar.
 * @return int Estadado del trbajo ejecutado.
 */
int launch_job(job *j);

/**
 * @brief Ejecuta un nuevo proceso.
 * 
 * @param j Trabajo al cual pertenece el proceso.
 * @param p Proceso a ejecutar.
 * @return int Estado del proceso ejecutado.
 */
int launch_process(job *j, process *p);

/**
 * @brief Imprime por consola el estado de todos los trabajos del listado.
 * 
 */
void print_job_all_status();

/**
 * @brief Imprime por consola el estado de un trabajo dado.
 * 
 * @param j Trabajo del que se quiere obtener el estado.
 */
void print_job_status(job *j);

/**
 * @brief Imprime por consola los procesos en un trabajo dado.
 * 
 * @param j Trabajo del que se obtienen los procesos.
 */
void print_job_process(job *j);

/**
 * @brief Imprime por consola el contenido de la pipeline de un trabajo.
 * 
 * @param j Trabajo del cual queremos imprimir la pipeline.
 */
void print_job_pipe(job *j);

/**
 * @brief Genera un array bidimensional segmentando una cadena en sus espacios. Se agrega NULL como ultimo elemento del array.
 * 
 * @param str Cadena a segmentar en formato de array.
 * @param n Puntero a donde se va a almacenar el numero de elementos del array.
 * @return char** Puntero al array bidimensional resultante.
 */
char** str_to_array(char* str, u_int8_t* n);

/**
 * @brief Libera la memoria alocada por un trabajo.
 * 
 * @param j Trabajo a liberar.
 */
void free_job(job *j);

/**
 * @brief Libera la memoria alocada por un array bidimensional.
 * 
 * @param array Arrray a liberar.
 * @param n numero de lementos del array.
 */
void free_array(char** array, u_int8_t n);

#endif //__JOB_CONTROL_H__