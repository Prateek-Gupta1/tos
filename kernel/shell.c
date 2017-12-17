/*
* This file implements TOS shell api.
* Prateek Gupta
* 916601249
*/

#include <kernel.h>

#define SHELL_WINDOW_WIDTH 60
#define SHELL_WINDOW_HEIGHT 18

#define CHARACTER_SPACE ' '
#define MAX_COMMAND_LENGTH 20

#define COMMAND_HELP 0
#define COMMAND_CLS 1
#define COMMAND_SHELL 2
#define COMMAND_PONG 3
#define COMMAND_PS 4
#define COMMAND_HISTORY 5
#define COMMAND_REPEAT 6
#define COMMAND_ABOUT 7
#define COMMAND_TRAIN 8

#define ERROR_COMMAND_NOT_FOUND -2
#define ERROR_EMPTY_COMMAND -1

static char* shell_commands[] = {
	"help",
	"cls",
	"shell",
	"pong",
	"ps",
	"history",
	"!num",
	"about",
	"train"
};

#define TOTAL_SHELL_COMMANDS 9


typedef struct _SHELL_history
{
	char* cmd;
	int size;

} SHELL_COMMAND_HISTORY;

#define MAX_HISTORY_SIZE 50


void command_help(int wnd_id)
{
	wm_print(wnd_id, "List of available TOS shell commands\n");
	wm_print(wnd_id, "help      - will print all supported TOS command\n");
	wm_print(wnd_id, "cls       - clears the shell screen\n");
	wm_print(wnd_id, "shell     - launches another shell\n");
	wm_print(wnd_id, "pong      - launches pong game\n");
	wm_print(wnd_id, "ps        - prints out process table\n");
	wm_print(wnd_id, "history   - prints all commands that has been typed in shell\n");
	wm_print(wnd_id, "!<number> - repeats the command with given number\n");
	wm_print(wnd_id, "about     - prints the developers name and message\n");
	wm_print(wnd_id, "train     - runs the train application\n");
}

void command_about(int wnd_id)
{
	wm_print(wnd_id, "Hi, this shell was written by Prateek Gupta\n");
	wm_print(wnd_id, "TOS is amazing!\n");
}

void command_clear_window(int wnd_id)
{
	wm_clear(wnd_id);
}

void command_start_pong(){
	start_pong();
}

void command_start_train(){
	init_train();
}

void command_history(int wnd_id, SHELL_COMMAND_HISTORY* head)
{
	SHELL_COMMAND_HISTORY* tmp = head;
	int idx = 0;
	if(head->size == 0)
	{
		wm_print(wnd_id, "You haven't entered any command");
	}	
	else
	{
		int history_number = 1;

		while(tmp < head + MAX_HISTORY_SIZE)
		{
			if(tmp->size == 0)
				break;

			wm_print(wnd_id, "%d - ", history_number);
		
			for(int j = 0; j < tmp->size; j++)
			{
				wm_print(wnd_id, "%c", tmp->cmd[j]);
			}
			wm_print(wnd_id, "\n");
			tmp++;
			history_number++;
		}
	}
}

/*
* The command takes the form !<Number>. The command present at the "Number" in the Shell History is executed.
*/
void command_repeat(int wnd_id, char* command, int cmd_size, SHELL_COMMAND_HISTORY* head)
{
	BOOL valid = TRUE;
	int num = 0;
	int idx = 1;

	if(cmd_size > 1)
	{	
		//Convert characters to number by accumulating in the num variable
		while(idx <= cmd_size-1)
		{
			if(command[idx] >= '0' && command[idx] <= '9')
			{
				num = num*10 + (command[idx]  - '0');
			}
			else
			{
				valid = FALSE;
				break;
			}
			idx++;
		}
	}
	else
	{
		valid = FALSE;
	}

	//If command was valid i.e. it had only numeric characters, then look it up in the shell history. 
	if(valid)
	{
		idx = 0;
		SHELL_COMMAND_HISTORY* temp = head;

		while(temp < head + MAX_HISTORY_SIZE)
		{
			if(idx == num-1)
			{
				break;
			}
			idx++;
			temp++;
		}

		//If the command was not found then temp will take value of last struct in the shell history
		// which has size = 0. If the command was found then print it and run it.
		if(temp != NULL && temp->size != 0)
		{
			wm_print(wnd_id, "> ");
			for(idx = 0 ; idx < temp->size; idx++)
				wm_print(wnd_id, "%c", temp->cmd[idx]);

			int code = find_shell_command(temp->cmd, temp->size);
			run_shell_command(wnd_id, code, head, temp->cmd, temp->size);
		}else{
			wm_print(wnd_id, "Invalid command - ");
			wm_print(wnd_id, command);
		}
	}
	else
	{
		wm_print(wnd_id, "Invalid command - ");
		wm_print(wnd_id, command);
	}

}


void helper_ps_table_heading(int wnd_id){
	wm_print(wnd_id, "State           Active Prio Name\n");
	wm_print(wnd_id, "-------------------------------------\n");
}

void helper_ps_process_details(int wnd_id, PROCESS p)
{
	 static const char* state[] = 
	{ "READY          ",
	  "SEND_BLOCKED   ",
	  "REPLY_BLOCKED  ",
	  "RECEIVE_BLOCKED",
	  "MESSAGE_BLOCKED",
	  "INTR_BLOCKED   "
	};
    if (!p->used) {
		wm_print(wnd_id, "PCB slot unused!\n");
		return;
    }
    /* State */
    wm_print(wnd_id, state[p->state]);
    /* Check for active_proc */
    if (p == active_proc)
		wm_print(wnd_id, " *      ");
    else
	wm_print(wnd_id, "        ");
    /* Priority */
    wm_print(wnd_id, "      %2d", p->priority);
    /* Name */
    wm_print(wnd_id, "     %s\n", p->name);
}

/*
* Prints out the process table.
*/
void command_ps(int wnd_id)
{
	int i;
	PCB* p = pcb;
	
	helper_ps_table_heading(wnd_id);

	for(i = 0; i < MAX_PROCS; i++, p++)
	{
		if(!p->used) 
			continue;
		helper_ps_process_details(wnd_id, p);
	}
}

/**
* This function looks up for a possible match in the list of shell commands. If a match is found
* then it returns the shell command code which is index in the shell list. If a match is not found then
* it returns ERROR_COMMAND_NOT_FOUND code
**/
int find_shell_command(char* usr_cmd, int cmd_size)
{
	//assert(usr_cmd != null && cm_size > 0);

	int k = 0;
	int shell_cmd_idx = 0;
	//skip whitespaces in the front
	while(usr_cmd[k] == CHARACTER_SPACE && k < cmd_size) k++;

	//only white spaces were passed
	if(k >= cmd_size) return ERROR_EMPTY_COMMAND;

	//Search the command in shell command list
	for(shell_cmd_idx = 0; shell_cmd_idx < TOTAL_SHELL_COMMANDS; shell_cmd_idx++)
	{
		//Proceed if the first character matches
		if(usr_cmd[k] == shell_commands[shell_cmd_idx][0])
		{
			if(usr_cmd[k] == '!') return COMMAND_REPEAT; //No need to process further

			BOOL found = TRUE;  //boolean flag to see if we found a match
			int uc_idx = k; 	//temp index to iterate the characters in usr_cmd string
			int s_idx = 0; 		//index to traverse the characters in the selected shell command string

			//possible shell command selected on the basis of match with first character
			char* temp = shell_commands[shell_cmd_idx]; 
			
			//Compare characterwise the user command and the possible shell command
			while(temp[s_idx] != '\0'){

				//Ignore backspaces
				if(usr_cmd[uc_idx] == '\b')
				{
					uc_idx++;
					continue;
				}
				//First conditinal checks if the length of user command is not less than the actual command size
				//and hence prevents Index out of bound. The seond condition compares the characters
				if(uc_idx >= cmd_size || usr_cmd[uc_idx++] != temp[s_idx++])
				{
					found = FALSE;
					break;
				}
			}

			//This prevents the commands that are substring of the usr_cmd. 
			//Example 'ps' should not be executed if the string passed is 'psychic'
			if(cmd_size > s_idx) continue;

			// Index is mapped to the macros value of commands
			if(found) return shell_cmd_idx;
		}
	}

	//No match found
	return ERROR_COMMAND_NOT_FOUND;
}

/**
* Runs the selected shell command
**/
void run_shell_command(int wnd_id, int cmd_code, SHELL_COMMAND_HISTORY* head, char* usr_cmd, int usr_cmd_size)
{
	wm_print(wnd_id, "\n");

	switch(cmd_code)
	{
		case COMMAND_ABOUT:
			command_about(wnd_id);
		break;

		case COMMAND_REPEAT:
			command_repeat(wnd_id, usr_cmd, usr_cmd_size, head);
		break;

		case COMMAND_HELP:
			command_help(wnd_id);
		break;

		case COMMAND_PS:
			command_ps(wnd_id);
		break;

		case COMMAND_HISTORY:
			command_history(wnd_id, head);
		break;

		case COMMAND_PONG:
			command_start_pong();
		break;

		case COMMAND_CLS:
			command_clear_window(wnd_id);
		break;

		case COMMAND_SHELL:
			start_shell();
		break;

		case COMMAND_TRAIN:
			command_start_train();
		break;

		case ERROR_COMMAND_NOT_FOUND:
			wm_print(wnd_id, "Command not found\n");
		break;

	};
}

void shell_process(PROCESS self, PARAM param)
{
	const char* SHELL_CHARACTER = "> ";
	int wnd_id = wm_create(5, 2, SHELL_WINDOW_WIDTH, SHELL_WINDOW_HEIGHT);
	//char* buffer = wm_get_buffer(wnd_id);
	wm_print(wnd_id, SHELL_CHARACTER);
	
	int key_count = 0;
	SHELL_COMMAND_HISTORY history[MAX_HISTORY_SIZE];
	int hidx = 0;

	for(; hidx < MAX_HISTORY_SIZE; hidx++)
	{
		history[hidx].cmd = "";
		history[hidx].size = 0;
	}
	hidx = 0;
	
	SHELL_COMMAND_HISTORY* history_head = NULL;
	char* command = malloc(MAX_COMMAND_LENGTH);

	while(1){
		
		unsigned char key = keyb_get_keystroke(wnd_id, TRUE);
		
		if(key == '\r' || key == '\n' || key == 0x0D)
		{
			if(key_count > 0)
			{
				SHELL_COMMAND_HISTORY* h = malloc(sizeof(SHELL_COMMAND_HISTORY));
				h->cmd =  &command[0];
				h->size = key_count;
				
				if(hidx >= MAX_HISTORY_SIZE)
				{
					for(hidx = 0; hidx < MAX_HISTORY_SIZE; hidx++)
					{
						history[hidx].cmd = "";
						history[hidx].size = 0;
					}
					hidx = 0;
				}
				
				history[hidx++] = *h;

				//parse the command entered by user and find the matching shell command
				int cmd_code = find_shell_command(command, key_count);

				//run the shell command
				run_shell_command(wnd_id, cmd_code, history, command, key_count);

				command = malloc(MAX_COMMAND_LENGTH);
			}	

			if(key_count == 0)
				wm_print(wnd_id, "\n");
			
			key_count = 0;
			
			wm_print(wnd_id, SHELL_CHARACTER);
		}
		else if(key == CHARACTER_SPACE)
		{
			wm_print(wnd_id, " ");
		}
		else
		{
			
			wm_print(wnd_id, "%c", key);
			command[key_count] = key;
			key_count++;
			
			//Command length cannot exceed the maximum permissible length
			if(key_count > MAX_COMMAND_LENGTH)
			{
				wm_print(wnd_id, "\nLength of command string is too long (max %d). Please try again\n", MAX_COMMAND_LENGTH);
				wm_print(wnd_id, SHELL_CHARACTER);

				//Clear the command array
				free(command);
				command = malloc(MAX_COMMAND_LENGTH);
				key_count = 0;
			}
		}
	}
}

void start_shell()
{
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
