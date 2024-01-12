/**
 * @file MyShell.c
 * @author Bottini, Franco Nicolas
 * @brief Implementacion de MyShell.
 * @version 1.2
 * @date Septiembre de 2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../inc/MyShell.h"

int main(int argc, char* argv[])
{
    myshell_validate_execution(argc);
    job_control_init();
    myshell_loop(command_source(argc, argv));

    return EXIT_SUCCESS;
}

void myshell_validate_execution(int argc)
{
    if(argc > 2)
    {
        fprintf(stderr, KRED"\nOnly one input argument is allowed !\n"KDEF);
        fprintf(stderr, KBLU"Input argument: batchfile.\n\n"KDEF);
        exit(EXIT_FAILURE);
    }
}

void myshell_loop(FILE* input_source)
{
    char input_buffer[MAX_LEN_INPUT];

    while (1)
    {
        if(input_source == stdin)
            print_prompt();
        
        READ_INPUT_RESULT read_result = get_input(input_buffer, MAX_LEN_INPUT, input_source);

        if (read_result == INP_READ)
        {
            if(input_source != stdin)
                fprintf(stdout, "> %s\n", input_buffer);

            execute_input(input_buffer);
        }
        else
        {
            if(input_source != stdin)
            {
                if(read_result != INP_EMPTY_LINE)
                {
                    fclose(input_source);

                    if(read_result == INP_END)
                    {
                        while(first_job);
                        exit(EXIT_SUCCESS);
                    }
                        
                    if(read_result == INP_TO_LONG)
                        exit(EXIT_FAILURE);
                }
            }
        }

    }  
}

void print_prompt(void)
{
    fprintf(stdout, KGRN"%s@%s~$ "KDEF, getenv("USER"), getenv("PWD"));
}

FILE* command_source(int argc, char* argv[])
{
    FILE* fp;

    if (argc == 2)
    {
        fp = fopen(argv[1], "r");
  	    
        if(fp == NULL)
        {
            fprintf(stderr, KRED"\n%s\n\n"KDEF, strerror(errno));  
            exit(EXIT_FAILURE);
        }
    }
    else
        fp = stdin;

    return fp;
}

READ_INPUT_RESULT get_input(char* buffer, int buffer_size, FILE* fp)
{
    if(fgets(buffer, buffer_size + 1, fp) == NULL)
        return INP_END;
    
    char* endOfBuffer = buffer + strlen(buffer) - 1;

    //Verifico que la longitud de la entrada no supere el limite maximo establecido.
    if((*endOfBuffer != ASCII_LINE_BREAK && fp == stdin) || (*endOfBuffer != ASCII_LINE_BREAK && *endOfBuffer != EOF && strlen(buffer) - 1 >= buffer_size && fp != stdin))
    {
        while(getchar() != ASCII_LINE_BREAK); //Limpia el buffer de entrada.
        fprintf(stderr, KRED"\nExceeded max input length !\n\n"KDEF);
        return INP_TO_LONG;
    }

    if(trim_white_space(buffer) == NULL)
        return INP_EMPTY_LINE;

    return INP_READ;
}

char* trim_white_space(char* str)
{
    while(*str == ASCII_SPACE) //Trunca los espacios en blanco al comienzo de la cadena.
        str++;

    if(*str == 0 || *str == ASCII_LINE_BREAK) //Verifica que la entrada no sea una linea vacia.
        return NULL;

    char* endOfStr = str + strlen(str) - 1;

    while(endOfStr > str && (*endOfStr == ASCII_SPACE || *endOfStr == ASCII_LINE_BREAK)) //Trunca los espacios en blanco y saltos de linea al final de la cadena.
        endOfStr--;

    endOfStr[1] = ASCII_END_OF_STRING;

    return str;
}

void execute_input(char* input)
{
    COMMANDS_FLAGS flag;
    char* command;
    char* args;
    char* input_cpy = malloc(sizeof(char) * strlen(input));

    strcpy(input_cpy, input);

    command = strtok_r(input, " ", &args);

    for(flag = CONST_STR_ARR_SIZE(CMM_VALIDS) - 1; flag >= -1; flag--)
        if(flag == CMM_EXTERN || !strcmp(CMM_VALIDS[flag], command)) 
            break;
    
    if (flag != CMM_EXTERN)
        command_interprete(flag, args);
    else
        command_interprete(flag, input_cpy);

    free(input_cpy);
}

void command_interprete(COMMANDS_FLAGS cmm, char* args)
{
    switch (cmm)
    {
        case CMM_JOBS:
            if (strlen(args) > 0)
                fprintf(stderr, KRED"\nThe command does not allow parameters !\n\n"KDEF);
            else
                print_job_all_status();
            
            break;

        case CMM_CD:
            execute_cd(args);
            break;

        case CMM_ECHO:
            execute_echo(args);
            break;

        case CMM_CLR:
            if (strlen(args) > 0)
                fprintf(stderr, KRED"\nThe command does not allow parameters !\n\n"KDEF);
            else
                execute_clr();

            break;

        case CMM_QUIT:
            if (strlen(args) > 0)
                fprintf(stderr, KRED"\nThe command does not allow parameters !\n\n"KDEF);
            else
                execute_quit();
            
            break;
    
        default:
            execute_extern(args);
            break;
    }
}

void execute_cd(char* dir)
{
    if(*dir == ASCII_MIDDLE_DASH)
        dir = getenv("OLDPWD");
    
    if (chdir(dir) != 0)
        fprintf(stderr, KRED"\n%s\n\n"KDEF, strerror(errno));  
    else
    {
        setenv("OLDPWD", getenv("PWD"), 1);
        setenv("PWD", getcwd(NULL, 1024), 1);
        fprintf(stdout, "\n");
    }
}

void execute_echo(char* value)
{
    if(strlen(value) == 0) 
        return;

    char *end_str;
    char *word = strtok_r(value, " ", &end_str);

    fprintf(stdout, "\n");

    while (word != NULL)
    {
        char *end_word;
        char *sub_word = strtok_r(word, "$", &end_word);

        if(*word != ASCII_MONEY_SIGN)
        {
            fprintf(stdout, KBLU"%s"KDEF, sub_word);
            sub_word = strtok_r(NULL, "$", &end_word);
        }
        
        while (sub_word != NULL)
        {
            char* envvar = getenv(sub_word);

            if(envvar)
                fprintf(stdout, KBLU"%s"KDEF, envvar);

            sub_word = strtok_r(NULL, "$", &end_word);
        }

        fprintf(stdout, " ");

        word = strtok_r(NULL, " ", &end_str);
    }

    fprintf(stdout, "\n\n");
}

void execute_extern(char* command)
{
    launch_job(new_job(new_process(command)));
}

void execute_clr(void)
{
    system("clear");
}

void execute_quit(void)
{
    exit(EXIT_SUCCESS);
}