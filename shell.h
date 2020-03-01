#ifndef SHELL_H
#define SHELL_H

#define STDIN 0
#define STDOUT 1
#define PIPE_RD 0
#define PIPE_WR 1

struct tree_node;

/*
 * Any value assigned to this will be displayed when asking the user for a
 * command. Do not assign any value to this if its value is NULL, as this
 * indicates the session is not interactive.
 */
extern char *prompt;

/*
 * Called once when the shell starts.
 */
void initialize(void);

/*
 * Called when a command has been read from the user.
 */
void run_command(struct tree_node *n);

void handle_simple_command(struct tree_node *n);

void handle_pipe(struct tree_node *n);

void handle_change_directory(char **argv);

void handle_kill();

void handle_stop();

void handle_sequence(struct tree_node *n);

void handle_redirection(struct tree_node *n);

void handle_set_env_variable(struct tree_node *n);

void handle_unset_env_variable(struct tree_node *n);


#endif
