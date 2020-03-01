#include "parser/ast.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

void initialize(void){
  if (prompt)
      prompt = "vush$ ";
}

void run_command(node_t *node){
  // print_tree(node);

  signal(SIGINT, handle_kill);
  signal(SIGTSTP, handle_stop);

  switch (node->type) {
    case NODE_COMMAND:
      handle_simple_command(node);
      break;
    case NODE_PIPE:
      handle_pipe(node);
      break;
    case NODE_REDIRECT:
      handle_redirection(node);
      break;
    case NODE_SUBSHELL:
      break;
    case NODE_SEQUENCE:
      handle_sequence(node);
      break;
    case NODE_DETACH:
      printf("Detatch\n");
      break;
    default:
      printf("No Command found\n");
      break;
  }

  if (prompt)
    prompt = "vush$ ";
}

void handle_simple_command(node_t *node){
  if(!strcmp(node->command.program, "exit")){
    if(node->command.argv[1] == NULL){
      exit(1);
    }
    exit(atoi(node->command.argv[1]));
  }
  else if(!strcmp(node->command.program, "cd")){
    handle_change_directory(node->command.argv);
  }
  else if(!strcmp(node->command.program, "set")){
    handle_set_env_variable(node);
  }
  else if(!strcmp(node->command.program, "unset")){
    handle_unset_env_variable(node);
  }
  else{
    pid_t pid;
    int status;

    pid = fork();

    if(pid == 0){
      if(execvp(node->command.program, node->command.argv) != 0){
        perror("child");
        exit(1);
      }
    }
    else{
      wait(&status);
    }
  }
}

void handle_change_directory(char **argv){
  if(argv[1] == NULL){
    chdir(getenv("HOME"));
  }
  else{
    if(chdir(argv[1])){
      perror("cd");
    }
  }
}

void handle_kill(){                                                             // Default is to do nothing

}

void handle_stop(){
  kill(getpid(), 1);
}

void handle_pipe(node_t *node){
  pid_t pid;
  int fd[2];

  pipe(fd);

  pid = fork();

  if(pid == -1){
    perror("fork error");
    exit(1);
  }
  else if(pid == 0){
    close(fd[PIPE_RD]);
    dup2(fd[PIPE_WR], STDOUT);
    handle_simple_command(node->pipe.parts[0]);
    exit(0);
  }
  else{
    pid = fork();

    if(pid == -1){
      perror("fork error");
      exit(1);
    }
    else if(pid == 0){
      close(fd[PIPE_WR]);
      dup2(fd[PIPE_RD], STDIN);
      handle_simple_command(node->pipe.parts[1]);
      exit(0);
    }
    close(fd[PIPE_RD]);
    close(fd[PIPE_WR]);
    waitpid(pid, NULL, 0);
  }
}

void handle_sequence(node_t *node){
  run_command(node->sequence.first);
  run_command(node->sequence.second);
}

void handle_redirection(node_t *node){
  pid_t pid;
  int fd;

  pid = fork();

  if(pid == -1){
    perror("redirection fork error");
    exit(1);
  }
  else if(pid == 0){
      switch(node->redirect.mode){
        case REDIRECT_DUP:
          dup2(node->redirect.fd, node->redirect.fd2);
          break;
        case REDIRECT_INPUT:
          fd = open(node->redirect.target, node->redirect.fd, node->redirect.mode);
          dup2(fd, STDIN);
          break;
        case REDIRECT_OUTPUT:
          fd = creat(node->redirect.target, node->redirect.mode);
          dup2(fd, STDOUT);
          break;
        case REDIRECT_APPEND:
          fd = open(node->redirect.target, node->redirect.fd, node->redirect.mode);
          lseek(fd, 0, SEEK_END);
          dup2(fd, STDOUT);
          break;
      }
      close(fd);
      execvp(node->redirect.child->command.program, node->redirect.child->command.argv);
      exit(0);
  }
  else{
    waitpid(pid, NULL, 0);
  }
}

void handle_set_env_variable(node_t *node){
  char *token = strtok(node->command.argv[1], "=");
  char *variable_name = token;
  token = strtok(NULL, "=");
  char *variable_value = token;

  setenv(variable_name, variable_value, 0);
}

void handle_unset_env_variable(node_t *node){
  unsetenv(node->command.argv[1]);
}
