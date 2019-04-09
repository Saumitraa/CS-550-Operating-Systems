#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>

struct CommandArgv
{
	char arguments[50][50];
};

struct process
{
	int pid;
	int item_id;
	int Process_state;
};

struct process P[100];

int New_process( int pid, int Process_state )
{
	int i=1;
	while ( P[i].item_id == 0 )
	{
		P[i].item_id = i;
		P[i].pid = pid;
		P[i].Process_state = Process_state;
	}
		i++;

	return i;
}
void delete( int pid )
{
	int i = 1;
	while ( P[i].pid == pid )
	{
		P[i].item_id = 0;
		P[i].pid = 0;
		P[i].Process_state = 0;
	}
	i++;
}
int return_pid( int item_id )
{
	int i = 1, pid;
	while ( P[i].item_id == item_id )
	{
		pid = P[i].pid;
	}
	i++;
	return pid;
}

int get_itemid( int pid )
{
	int i = 1, item_id;
	int flag = 0;
	while ( P[i].pid != pid )
	{
		{
			flag = 1;
			break;
		}
		i++;
	}
	if ( flag==1 )
	{
		item_id = -1;
	}
	else
	{
		item_id = P[i].item_id;
	}
	return item_id;
}

int get_Process_state( int pid )
{
	int i = 1, Process_state;
	int flag = 0;
	while ( P[i].pid != pid )
	{

		{
			flag = 1;
			break;
		}
		i++;
	}

	if ( flag==1 )
	{
		Process_state = -1;
	}
	else
	{
		Process_state = P[i].Process_state;
	}

	return Process_state;
}

int get_current_pid()
{
	int i=1, cpid;
	int flag = 0;
	while ( P[i].Process_state != 1 )
	{

		{
			flag = 1;
			break;
		}
		i++;
	}

	if ( flag==1 )
	{
		//fprintf(stderr, "No Such item Found\n");
		cpid = -1;
	}
	else
	{
		//printf( "item at location %d\n", i );
		cpid = P[i].pid;
	}
	return cpid;
}

char * trim_whitespaces( char * string )
{
	int first_i = 0;
	int last_i = strlen( string ) - 1;
	while( string[first_i] == ' ' )
	{
		first_i++;
	}

	if( first_i > 0 )
	{
		int i;
		for( i=0; i<=last_i; i++ )
		{
			string[i] = string[i+first_i];
		}
	}

	if( last_i > 0 && string[last_i] == '\n')
	{
		string[last_i] = '\0';
	}

	while( string[last_i] == ' ' )
	{
		string[last_i] = '\0';
		last_i = last_i-1;
	}

	return string;
}

void split_tream_input( char *input_command, struct CommandArgv *com_argv, char * symbol )
{
	int i = 0;
	char *temp_arr;

	temp_arr = strtok( input_command, symbol );

	while( temp_arr != NULL )
	{
		strcpy(com_argv->arguments[i], trim_whitespaces(temp_arr) );
		i++;
		temp_arr = strtok( NULL, symbol );
	}
}

void split_input( char *input_command, struct CommandArgv *com_argv )
{
	int i=0;
	char *temp_arr;
	temp_arr = strtok( input_command, " " );
	while( temp_arr != NULL )
	{
		strcpy(com_argv->arguments[i], temp_arr);
		i++;
		temp_arr = strtok( NULL, " " );
	}
}

void command_exec(char *input_command)
{
	int err, i=0;
	char *argv[50];
	struct CommandArgv com_argv;
	split_input(input_command, &com_argv);
	char str[] = "/bin/";
	strcat( str, com_argv.arguments[0]);
	while( strcmp(com_argv.arguments[i], "" ) != 0)
	{
		argv[i] = com_argv.arguments[i];
		i++;
	}
	argv[i] = NULL;
	err = execvp( com_argv.arguments[0], argv );
	exit(err);
}

int command_pipe_status( char *input_command )
{
	char * option;
	option = strchr( input_command, '|' );
	if(option != NULL)
	{
		return 1;
	}

	return 0;
}

int command_send( char *input_command, int inout )
{
	char * option;
	if( inout )
	{
		option = strchr( input_command, '>' );
	}
	else
	{
		option = strchr( input_command, '<' );
	}

	if(option != NULL)
	{
		return 1;
	}

	return 0;
}

void pip_command_exec( char* );
void redi_command_exec( char* input_command, int inout )
{
	int error, i=0, has_pipe, has_inpcomm;
	char *arguments[50];
	char *arguments2[50];
	char *temp_arr;
	char* symb;
	int output_file_fd;
	int input_file_fd;
	if( inout )
	{
		symb = ">";
	}
	else
	{
		symb = "<";
	}
	temp_arr = strtok( input_command, symb );

	while( temp_arr != NULL )
	{
		arguments[i] = trim_whitespaces(temp_arr);
		i++;
		temp_arr = strtok( NULL, symb );
	}
	char * filename = arguments[i-1];
	i=0;

	char *argzero = arguments[0];
	has_pipe = command_pipe_status( arguments[0] );
	has_inpcomm = command_send( arguments[0], 0 );
	if( !has_pipe && !has_inpcomm )
	{
		struct CommandArgv redi_com_argv;
		split_input(arguments[0], &redi_com_argv);
		while( strcmp(redi_com_argv.arguments[i], "" ) != 0)
		{
			arguments2[i] = redi_com_argv.arguments[i];
			i++;
		}

		arguments2[i] = NULL;
	}
	char stri[] = "/usr/";
	if( inout )
	{
		strcat( stri, arguments2[0]);
		output_file_fd = open(filename, O_WRONLY | O_CREAT,0770 );
		dup2( output_file_fd, 1 );
		if( has_pipe )
		{
			pip_command_exec( argzero );
		}
		else if( has_inpcomm )
		{
			redi_command_exec( argzero, 0 );
		}
		else
		{
			error = execvp( arguments2[0], arguments2 );
			exit(error);
		}
	}
	else
	{
		strcat( stri, arguments2[0]);
		input_file_fd = open( filename, O_RDONLY );
		dup2( input_file_fd, 0 );
		close( input_file_fd );
		error = execvp( arguments2[0], arguments2 );
		exit(error);
	}
}

void pip_command_exec_pro( char **current, char * redi_flag, int last )
{
	int process[2], pid, status, process_in = 0, process_no = 1;

    while( *current != NULL )
    {
    	if ( pipe( process ) == -1)
    	{
            perror( "pipe" );
            exit(1);
	    }

	    pid = fork();
		if( pid < 0)
		{
			perror( "fork" );
			exit(1);
	    }
	    if ( pid == 0 ) {
	        dup2( process_in, 0 );
	        if ( *(current + 1) != NULL)
	        {
	        	dup2( process[1], 1 );   /* make stdout same as process[1] */
	        }
	        close( process[0] ); /* we don't need the read end -- process[0] */

	        if( ( process_no == 1 ) && ( ( strcmp( redi_flag, "i" ) == 0 ) || ( strcmp( redi_flag, "b" ) == 0 ) ) )
			{
				redi_command_exec(*current, 0);
			}
			if( ( process_no == last ) && ( ( strcmp( redi_flag, "o" ) == 0 ) || ( strcmp( redi_flag, "b" ) == 0 ) ) )
			{
				redi_command_exec(*current, 1);
			}
	        command_exec(*current);
	    }
	    else
	    {
	    	waitpid( pid, &status, 0 );
	    	close( process[1] );
	    	process_in = process[0];
	    	*current++;
	    	process_no++;
	   	}
    }
}

void pip_command_exec( char* input_command )
{
	int redirection_i = command_send( input_command, 0 ); // in
	int redirection_o = command_send( input_command, 1 ); // out
	char* redi_flag;
	if( redirection_i && redirection_o )
	{
		redi_flag = "b"; //both
	}
	else if( redirection_i )
	{
		redi_flag = "i"; //only input
	}
	else if( redirection_o )
	{
		redi_flag = "o"; //only output
	}
	else
	{
		redi_flag = "n"; //none
	}

	int i=0;
	char *commands[50];
	struct CommandArgv pip_com_argv;
	char* symb = "|";
	split_tream_input( input_command, &pip_com_argv, symb );
	while( strcmp(pip_com_argv.arguments[i], "" ) != 0)
	{
		commands[i] = pip_com_argv.arguments[i];
		i++;
	}
	commands[i] = NULL;
	pip_command_exec_pro( commands, redi_flag, i-1 );
	exit(0);

}

void kill_command_exec( char* input_command )
{
	int error, pid, item_id;
	struct CommandArgv k_com_argv;
	int i=0;
	char *temp_arr;
	temp_arr = strtok( input_command, " " );
	while( temp_arr != NULL )
	{
		strcpy( k_com_argv.arguments[i], temp_arr );
		i++;
		temp_arr = strtok( NULL, " " );
	}
	char * id = k_com_argv.arguments[1];
	char * option;
	option = strchr( id, '%' );
	if( option != NULL )
	{
		item_id = atoi( &id[1] );
		pid = return_pid( item_id );
	}
	else
	{
		pid = atoi( id );
		item_id = get_itemid( pid );
		if( item_id == -1 )
		{
			pid = -1;
		}
	}
	if( pid != -1 )
	{
		killpg( pid, SIGKILL );
	}
}

int command_kill_status ( char * input_command )
{
	char * option;
	char * str2 = "kill ";
	option = strstr( input_command, str2 );
	if(option != NULL)
	{
		return 1;
	}

	return 0;
}

void childsignal_handler( int signal_number )
{
	int pid, status;
	pid = waitpid( -1, &status, WNOHANG );
	while( pid > 0 )
	{
		delete( pid );
		pid = waitpid( -1, &status, WNOHANG );
	}
}

void intsignal_handler( int signal_number )
{
	int kill_status;
	int current_pid;
	current_pid = get_current_pid();
	if( current_pid == -1 )
	{
		return;
	}
	else
	{
		kill_status = killpg( current_pid, SIGINT );
		if( !kill_status )
		{
			return;
		}

	}
}

void intsignal_SIGQUIT()
{
  printf("Found your exit  \n");
 	//kill;
}

int run_prog()
{
	char input_command[100];
	int background = 0;
	signal( SIGINT, intsignal_handler );
	signal( SIGCHLD, childsignal_handler );
	signal( SIGQUIT, intsignal_SIGQUIT);
	while(1)
	{
		int pid;
		int status;
		printf( "\nminish> " );
		fgets( input_command, 100, stdin );
		int last_i = strlen( input_command ) - 1;

		strcpy(input_command, trim_whitespaces( input_command ));

		if( last_i > 0 && input_command[last_i-1] == '&' )
		{
			input_command[last_i-1]	= '\0';
			input_command[last_i-2]	= '\0';
			background = 1;
		}
		else
		{
			background = 0;
		}

		if( strcmp( input_command, "exit" ) == 0 )
		{
			signal( SIGQUIT, intsignal_SIGQUIT);
			printf("Goodbye\n");
			exit(0);
		}

		int redirection_i = command_send( input_command, 0 ); // in
		int redirection_o = command_send( input_command, 1 ); // out
		int has_pipe = command_pipe_status( input_command ); // out
		int has_kill = command_kill_status( input_command );

		if ( has_kill )
		{
			kill_command_exec( input_command );
		}
		else
		{
			pid = fork();
			int item_id;
			if( pid < 0 )
			{
				printf( "Error in child process fork\n" );
				exit(1);
			}
			else if( pid == 0 )
			{
				setsid();
				if( redirection_o )
				{
					redi_command_exec(input_command, 1);
				}
				else if( has_pipe )
				{
					pip_command_exec(input_command);
				}
				else if( redirection_i )
				{
					redi_command_exec(input_command, 0);
				}
				else
				{
					command_exec(input_command);
				}
			}
			else
			{
				if( background )
				{
					item_id = New_process( pid, 2 );
					printf( "Background Process : %d\n", pid );
				}
				else
				{
					item_id = New_process( pid, 1 );
					waitpid( pid, &status, WUNTRACED );
					int pstatus = get_Process_state( pid );
					if( pstatus == 1 )
					{
						delete( pid );
					}
				}
			}
		}
	}
	return 0;
}

int main()
{
	run_prog();
}
