/**
 * @file JobControl.h
 * @author Bottini, Franco Nicolas
 * @brief Implementacion de la libreria para el control de trabajos simple.
 * @version 1.2
 * @date Septiembre de 2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../inc/JobControl.h"

const char* PROCESS_STATUS_STRING[] = {
    "running",
    "done",
    "suspended",
    "continued",
    "terminated"
};

job *first_job = NULL;

job* new_job(process *first_process)
{
    job* j = malloc(sizeof(job));

    j->id = 0;
    j->next = NULL;
    j->pgid = -1;
    j->first_process = first_process;
    j->first_process->status = STATUS_READY;

    if(!strcmp(j->first_process->argv[j->first_process->argc - 1], "&"))
    {
        j->mode = BACKGROUND_EXECUTION;
        free(j->first_process->argv[j->first_process->argc]);
        j->first_process->argv[j->first_process->argc - 1] = NULL;
    }
    else
        j->mode = FOREGROUND_EXECUTION;

    return j;
}

process* new_process(char *command)
{
    process *p = malloc(sizeof(process));

    p->next = NULL;
    p->argv = str_to_array(command, &p->argc);
    p->status = STATUS_NEW;
    p->pid = -1;

    return p;
}

int insert_job(job *j) 
{
    job *last_job = get_last_job();

    if(!last_job)
    {
        j->id  = 1;
        first_job = j;
    }
    else
    {
        j->id = last_job->id + 1;
        last_job->next = j;
    }

    return j->id;
}

void insert_process(job *j, process *p)
{
    process *last_p = get_last_process(j);

    if(!last_p)
        j->first_process = p;
    else
        last_p->next = p;
}

void remove_job(job* j) 
{
    if (!first_job)
        return;

    job *parent = get_job_parent(j);

    if(!parent)
    {
        first_job = j->next;
        free_job(j);
        return;
    }

    if(j->next == NULL)
    {
        parent->next = NULL;
        free_job(j);
        return;
    }

    parent->next = j->next;
    free_job(j);
    return;
}

job* get_last_job() 
{
    job* last_j = first_job;

    if(!last_j)
        return NULL;
    
    while (last_j->next)
        last_j = last_j->next;

    return last_j;
}

job* get_job_by_pid(int pid)
{
    int id = get_job_id_by_pid(pid);

    if(id == -1)
        return NULL;
    
    return get_job_by_id(id);
}

job* get_job_by_id(int id)
{
    job* j = first_job;

    if(!j)
        return NULL;

    while (j->id != id)
        j = j->next;

    return j;
}

job* get_job_parent(job *j)
{
    job *parent = first_job;

    if(!parent || j == parent)
        return NULL;

    while (parent->next != j)
        parent = parent->next;
    
    return parent;
}

int get_job_id_by_pid(int pid) 
{    
    for (job* j = first_job; j; j = j->next) 
        for (process* p = j->first_process; p; p = p->next) 
            if (p->pid == pid) 
                return j->id;

    return -1;
}

void set_job_status(job *j, PROCESS_STATUS status)
{
    for (process* p = j->first_process; p; p = p->next)
        if (p->status != STATUS_DONE)
            set_process_status(p, status);
}

int is_job_completed(job *j) 
{
    for (process* p = j->first_process; p != NULL; p = p->next) 
        if (p->status != STATUS_DONE)
            return 0;

    return 1;
}

process* get_last_process(job *j) 
{
    process* last_p = j->first_process;

    if(!last_p)
        return NULL;
    
    while (last_p->next)
        last_p = last_p->next;

    return last_p;
}

process* get_process_by_pid(int pid)
{
    job* j = get_job_by_pid(pid);

    if(!j)
        return NULL;

    process* p = j->first_process;

    if(!p)
        return NULL;

    while (p->pid != pid)
        p = p->next;

    return p;
}

int get_processes_count(job* j, int filter) 
{
    int count = 0;

    for (process* p = j->first_process; p; p = p->next)
        if (filter == PROC_FILTER_ALL ||
           (filter == PROC_FILTER_DONE && p->status == STATUS_DONE) ||
           (filter == PROC_FILTER_REMAINING && p->status != STATUS_DONE))
            count++;

    return count;
}

void set_process_status(process *p, PROCESS_STATUS status) 
{
    p->status = status;
}

void job_control_init()
{
    struct sigaction sigint_action = {
        .sa_handler = sigint_handler,
        .sa_flags = 0
    };
    sigemptyset(&sigint_action.sa_mask);
    sigaction(SIGCHLD, &sigint_action, NULL);

    pid_t pid = getpid();
    setpgid(pid, pid);
    tcsetpgrp(0, pid);
}

void sigint_handler(int signal)
{
    int status, pid;

    while ((pid = waitpid(WAIT_ANY, &status, WNOHANG|WUNTRACED|WCONTINUED)) > 0) 
    {
        job *j = get_job_by_pid(pid);
        process *p = get_process_by_pid(pid);

        if (WIFEXITED(status))
            set_process_status(p, STATUS_DONE);
        else if (WIFSTOPPED(status))
            set_process_status(p, STATUS_SUSPENDED);
        else if (WIFCONTINUED(status))
            set_process_status(p, STATUS_CONTINUED);

        fprintf(stdout, "\n\n");

        print_job_pipe(j);

        if (j->id > 0 && is_job_completed(j)) 
        {
            print_job_status(j);
            remove_job(j);
        }
    }

    fprintf(stdout, "\n");
}

int wait_for_job(job *j)
{
    int proc_count = get_processes_count(j, PROC_FILTER_REMAINING);
    int wait_count = 0;
    int status = 0;

    do 
    {
        int wait_pid = waitpid(-j->pgid, &status, WUNTRACED);
        process *p = get_process_by_pid(wait_pid);

        wait_count++;

        if (WIFEXITED(status))
            set_process_status(p, STATUS_DONE);
        else if (WIFSIGNALED(status))
            set_process_status(p, STATUS_TERMINATED);
        else if (WSTOPSIG(status)) 
        {
            status = -1;
            set_process_status(p, STATUS_SUSPENDED);

            if (wait_count == proc_count)
                print_job_status(j);
        }
    } while (wait_count < proc_count);

    return status;
}

int wait_for_process(process *p)
{
    int status = 0;

    waitpid(p->pid, &status, WUNTRACED);

    if (WIFEXITED(status))
        set_process_status(p, STATUS_DONE);
    else if (WIFSIGNALED(status))
        set_process_status(p, STATUS_TERMINATED);
    else if (WSTOPSIG(status)) 
    {
        status = -1;
        set_process_status(p, STATUS_SUSPENDED);
    }

    return status;
}

int launch_job(job *j) 
{
    int status = 0;
    
    insert_job(j);

    if (pipe(j->io_fd) < 0 || pipe(j->err_fd) < 0)
    {
        perror(KRED"\npipe\n"KDEF);
        exit (EXIT_FAILURE);
    }

    for (process* p = j->first_process; p; p = p->next)
        status = launch_process(j, p);

    if (status >= 0 && j->mode == FOREGROUND_EXECUTION)
        remove_job(j);
    if (j->mode == BACKGROUND_EXECUTION)
        print_job_process(j);

    return status;
}

int launch_process(job *j, process *p) 
{
    p->status = STATUS_RUNNING;

    int status = 0;
    pid_t childpid = fork();

    if (childpid < 0)
        return -1;
    else if (childpid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        p->pid = getpid();
        if (j->pgid <= 0)
            j->pgid = p->pid;

        setpgid(0, j->pgid);

        close(j->io_fd[0]);
        dup2(j->io_fd[1], STDOUT_FILENO);
        close(j->io_fd[1]);

        close(j->err_fd[0]);
        dup2(j->err_fd[1], STDERR_FILENO);
        close(j->err_fd[1]);

        if (execvp(p->argv[0], p->argv) < 0) 
        {
            fprintf(stderr, "Command not found!\n");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    } 
    else 
    {
        p->pid = childpid;

        if (j->pgid <= 0)
            j->pgid = p->pid;

        setpgid(childpid, j->pgid);
        
        if (j->mode == FOREGROUND_EXECUTION) 
        {
            tcsetpgrp(0, j->pgid);
            status = wait_for_job(j);
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(0, getpid());
            signal(SIGTTOU, SIG_DFL);

            print_job_pipe(j);
        }
    }

    return status;
}

void print_job_all_status(void) 
{
    fprintf(stdout, "\n");

    for (job* j = first_job; j; j = j->next)
        print_job_status(j);

    fprintf(stdout, "\n");
}

void print_job_status(job *j) 
{
    fprintf(stdout, KBLU"[%d]"KDEF, j->id);

    for (process* p = j->first_process; p; p = p->next) {
        fprintf(stdout, KBLU" %d %s %s"KDEF, p->pid, PROCESS_STATUS_STRING[p->status], p->argv[0]);

        if (p->next)
            fprintf(stdout, KBLU"|\n"KDEF);
        else
            fprintf(stdout, "\n");
    }
}

void print_job_process(job *j) 
{
    fprintf(stdout, KBLU"\n[%d]"KDEF, j->id);

    for (process* p = j->first_process; p; p = p->next)
        fprintf(stdout, KBLU" %d %s"KDEF, p->pid, p->argv[0]);

    fprintf(stdout, "\n\n");
}

void print_job_pipe(job *j)
{
    close(j->io_fd[1]);
    close(j->err_fd[1]);

    fcntl(j->io_fd[0], F_SETFL, fcntl(j->io_fd[0], F_GETFL) | O_NONBLOCK);
    fcntl(j->err_fd[0], F_SETFL, fcntl(j->err_fd[0], F_GETFL) | O_NONBLOCK);

    char c; int pn = 0;
    while (read(j->err_fd[0], &c, sizeof(c)) > 0)
    {
        fprintf(stdout, KRED"%c"KDEF, c);
        pn = 1;
    }    

    while (read(j->io_fd[0], &c, sizeof(c)) > 0)
    {
        fprintf(stdout, KYEL"%c"KDEF, c);
        pn = 1;
    }

    if(pn)
        fprintf(stdout, "\n");

    close(j->io_fd[0]);
    close(j->err_fd[0]);
}

char** str_to_array(char* str, u_int8_t* n)
{
    char** argv = malloc(sizeof(char*));

    *n = 0;
    
    char* token = strtok(str, " ");
    while (token != NULL)
    {
        argv[*n] = malloc(sizeof(char) * strlen(token));
        strcpy(argv[(*n)++], token);
        argv = realloc(argv, sizeof(char*) * (*n + 1));
        token = strtok(NULL, " ");
    }

    argv[*n] = NULL;

    return argv;
}

void free_job(job *j)
{
    process *tmp;
    process *p = j->first_process;

    while(p)
    {
        free_array(p->argv, p->argc);
        tmp = p;
        p = p->next;
        free(tmp);
    }
    
    free(j);
}

void free_array(char** array, u_int8_t n)
{
    for (u_int8_t i = 0; i < n; i++)
        free(array[i]);
    
    free(array);
}