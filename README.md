# MyShell - Interprete de Línea de Comandos

Este proyecto implementa un intérprete de línea de comandos llamado MyShell en el lenguaje de programación C, diseñado para ejecutarse en entornos GNU/Linux. El objetivo principal es proporcionar un shell interactivo con funcionalidades básicas y capacidad para ejecutar comandos internos, programas externos, y scripts en segundo plano.

### Autores:
- **Bottini, Franco Nicolas**

## Características Principales

### 1. Command Line Prompt
MyShell presenta un prompt que indica el nombre de usuario, el nombre del host y el directorio actual. El formato es el siguiente:

```
username@hostname:~$
```

### 2. Comandos Internos
MyShell admite varios comandos internos:

- **cd \<directorio\>**: Cambia el directorio actual a \<directorio\>. Si \<directorio\> no está presente, reporta el directorio actual. Si el directorio no existe, se imprime un mensaje de error. Además, el comando cambia la variable de entorno PWD. También soporta la opción `cd -`, que retorna al último directorio de trabajo (OLDPWD).

- **clr**: Limpia la pantalla.

- **echo \<comentario\|env var\>**: Muestra \<comentario\> en la pantalla seguido por una línea nueva. Múltiples espacios/tabs pueden ser reducidos a un espacio.

- **quit**: Cierra MyShell.

### 3. Invocación de Programas
Las entradas del usuario que no son comandos internos se interpretan como la invocación de un programa. La ejecución se realiza mediante `fork` y `execl`. MyShell admite tanto paths relativos como absolutos.

### 4. Archivo Batch
MyShell puede tomar sus comandos desde un archivo al ser invocado con un argumento. Por ejemplo:

```
./myshell batchfile
```

El batchfile contiene un conjunto de comandos de línea para que MyShell ejecute. Cuando se alcanza el final del archivo (EOF), MyShell se cierra.

### 5. Ejecución en Segundo Plano
Si un comando termina con un ampersand (&), indica que la shell debe retornar al prompt inmediatamente después de lanzar el programa en segundo plano. Se imprime un mensaje indicando el trabajo y el ID de proceso:

```
[<job id>] <process id>
```

Ejemplo:
```
$ echo 'hola' &
[1] 10506
hola
```

## Compilación y Ejecución

Para compilar el proyecto, ejecutar:

```
make
```

Para ejecutar MyShell, utilizar:

```
./myshell [batchfile]
```

- Si se proporciona un batchfile como argumento, MyShell ejecutará los comandos desde el archivo y se cerrará al alcanzar el final del archivo.
- Si no se proporciona ningún argumento, MyShell mostrará el prompt y esperará comandos del usuario vía stdin.