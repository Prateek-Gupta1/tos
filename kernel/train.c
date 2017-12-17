/*
** Prateek Gupta
** 916601249
** Train application
*/

#include <kernel.h>

#define TRAIN_WINDOW_WIDTH 50
#define TRAIN_WINDOW_HEIGHT 16

#define TICKS_TO_SLEEP 7
#define CLEAR_OUTPUT_BUFFER_COMMAND "R\015"

void setup_train_track(int wnd_id);
int check_configuration(int wnd_id);
void clear_output_buffer(int wnd_id);
int probe_segment(int wnd_id, char* segment, int time);

void flip_switch(int wnd_id, char* ctrl_switch);
void change_train_direction(int wnd_id, char* direction);
void change_train_speedI(int wnd_id, char* speed);
void run_config1(int wnd_id, int zamboni);
void run_config2(int wnd_id, int zamboni);
void run_config3(int wnd_id, int zamboni);
void run_config4(int wnd_id, int zamboni);
void train_process(PROCESS self, PARAM param);
void init_train();


//**************************
//run the train application
//**************************

void setup_train_track(int wnd_id)
{

	char* init_switch[] = {
		"M4G\015", 
		"M1G\015", 
		"M9R\015", 
		"M5G\015", 
		"M8G\015"
	};

	int i = 0;
	int init_switch_size = 5;
	
	for(; i < init_switch_size; i++)
	{
		COM_Message msg;
		
		msg.output_buffer 		= init_switch[i];
		msg.input_buffer 		= NULL;
		msg.len_input_buffer	= 0;

		send(com_port, &msg);
		wm_print(wnd_id, msg.output_buffer);
		wm_print(wnd_id, "\n");
		sleep(TICKS_TO_SLEEP); 
	}

}

int check_configuration(int wnd_id)
{
	wm_print(wnd_id, "checking configuration\n");

	char* train_pos[] = {
		"C8\015"  ,  // config 1
		"C12\015" ,  // config 2
		"C5\015"  ,  // config 3
		"C16\015"    // config 4
	};

	int num_of_configs = 4;
	
	// COM_Message probe;
	// char buffer[6];
	int status = 0;
	
	for(int i = 0; i < num_of_configs; i++){

		//clear_output_buffer(wnd_id);
		
		status = probe_segment(wnd_id, train_pos[i], 2);

		if(status == 1)
			return i+1;
	}

	return -1;

}

void clear_output_buffer(int wnd_id)
{
		COM_Message clr;
		clr.output_buffer 		= CLEAR_OUTPUT_BUFFER_COMMAND;
		clr.input_buffer 		= NULL;
		clr.len_input_buffer 	= 0;

		sleep(TICKS_TO_SLEEP);
		send(com_port, &clr);

		//wm_print(wnd_id, "Cleared output buffer\n");
}

int probe_segment(int wnd_id, char* segment, int time)
{
	//clear_output_buffer(wnd_id);

	wm_print(wnd_id, "Probing segment: ");
	wm_print(wnd_id, segment);
	int i = 0;
	int status = 0;
	
	
	while(i < time){

		clear_output_buffer(wnd_id);

		COM_Message msg;
		char buffer[3];

		msg.output_buffer 		= segment;
		msg.input_buffer 		= buffer;
		msg.len_input_buffer 	= 3;

		sleep(TICKS_TO_SLEEP);
		send(com_port, &msg);

		//wm_print(wnd_id,"got reply\n");

		status = buffer[1] - '0';

		//wm_print(wnd_id, "status %c", buffer[1]);

		if(status == 1)
			break;
		
		i++;

		wm_print(wnd_id, " .");
	}
	wm_print(wnd_id, "\n");

	return status;
}

void set_train_speed(char* speed){}

void change_train_speed(int wnd_id, char* speed)
{
	sleep(TICKS_TO_SLEEP);

	COM_Message msg;

	msg.output_buffer 		= speed;
	msg.input_buffer 		= NULL;
	msg.len_input_buffer 	= 0;

	send(com_port, &msg);

	wm_print(wnd_id, "Changing train speed to: ");
	wm_print(wnd_id, speed);
	wm_print(wnd_id, "\n");
}

void flip_switch(int wnd_id, char* ctrl_switch)
{
	sleep(TICKS_TO_SLEEP);

	COM_Message msg;

	msg.output_buffer 		= ctrl_switch;
	msg.input_buffer 		= NULL;
	msg.len_input_buffer 	= 0;

	send(com_port, &msg);

	wm_print(wnd_id, "Flipping switch: ");
	wm_print(wnd_id, ctrl_switch);
	wm_print(wnd_id, "\n");
}

void change_train_direction(int wnd_id, char* direction)
{
	sleep(TICKS_TO_SLEEP);

	COM_Message msg;

	msg.output_buffer 		= direction;
	msg.input_buffer 		= NULL;
	msg.len_input_buffer	= 0;

	send(com_port, &msg);

	wm_print(wnd_id, "Changing train direction: ");
	wm_print(wnd_id, direction);
	wm_print(wnd_id, "\n");
}

void run_config1(int wnd_id, int zamboni)
{
	int status = 0;
	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C10\015", 60);
	}
	
	flip_switch(wnd_id, "M6R\015");
	flip_switch(wnd_id, "M5R\015");
	flip_switch(wnd_id, "M7R\015");
	flip_switch(wnd_id, "M2R\015");

	change_train_speed(wnd_id, "L20S3\015");
	change_train_speed(wnd_id, "L20S5\015");

	status = probe_segment(wnd_id, "C7\015", 30);

	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");
	change_train_direction(wnd_id, "L20D\015");
	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C3\015", 30);
	}
	flip_switch(wnd_id, "M5G\015");
	flip_switch(wnd_id, "M1R\015");

	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S5\015");
	status = probe_segment(wnd_id, "C12\015", 30);
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");

	flip_switch(wnd_id, "M1G\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C3\015", 40);
	}

	flip_switch(wnd_id, "M8R\015");
	
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S5\015");
	status = probe_segment(wnd_id, "C13\015", 30);

	flip_switch(wnd_id, "M8G\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C10\015", 30);
	}

	flip_switch(wnd_id, "M5R\015");
	probe_segment(wnd_id, "C7\015", 30);
	change_train_speed(wnd_id, "L20S4\015");
	probe_segment(wnd_id, "C8\015", 30);
	flip_switch(wnd_id, "M5G\015");
	//sleep(TICKS_TO_SLEEP);
	//sleep(TICKS_TO_SLEEP);
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S1\015");
	change_train_speed(wnd_id, "L20S0\015");
	
}
	
void run_config2(int wnd_id, int zamboni)
{
	int status = 0;

	flip_switch(wnd_id, "M3G\015");
	flip_switch(wnd_id, "M2G\015");
	flip_switch(wnd_id, "M7G\015");
	flip_switch(wnd_id, "M6G\015");
	change_train_direction(wnd_id, "L20D\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C4\015", 30);
	}

	change_train_speed(wnd_id, "L20S3\015");
	change_train_speed(wnd_id, "L20S5\015");
	status = probe_segment(wnd_id, "C7\015", 10);
	flip_switch(wnd_id, "M4R\015");
	//set_train_speed(wnd_id, "L20S4\015");
	status = probe_segment(wnd_id, "C1\015", 20);
	flip_switch(wnd_id, "M4G\015");
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");
	flip_switch(wnd_id, "M7R\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C10\015", 20);
	}

	flip_switch(wnd_id, "M1R\015");
	flip_switch(wnd_id, "M8R\015");

	change_train_speed(wnd_id, "L20S4\015");
	status = probe_segment(wnd_id, "C14\015", 10);
	flip_switch(wnd_id, "M1G\015");
	status = probe_segment(wnd_id, "C11\015", 10);
	flip_switch(wnd_id, "M8G\015");
	change_train_speed(wnd_id, "L20S3\015");
	status = probe_segment(wnd_id, "C12\015", 10);
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");

}

void run_config3(int wnd_id, int zamboni)
{
	int status = 0;

	flip_switch(wnd_id, "M3R\015");
	flip_switch(wnd_id, "M6G\015");
	flip_switch(wnd_id, "M7G\015");
	flip_switch(wnd_id, "M2R\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C10\015", 30);
	}

	flip_switch(wnd_id, "M4R\015");  // need to flip it back later
	flip_switch(wnd_id, "M5R\015");  // need to flip it back later

	change_train_speed(wnd_id, "L20S4\015");
	change_train_speed(wnd_id, "L20S5\015");

	status = probe_segment(wnd_id, "C7\015", 10);
	flip_switch(wnd_id, "M4G\015");

	status = probe_segment(wnd_id, "C9\015", 10);
	flip_switch(wnd_id, "M5G\015");
	change_train_speed(wnd_id, "L20S3\015");

	status = probe_segment(wnd_id, "C12\015", 10);

	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");
	flip_switch(wnd_id, "M7R\015");
	change_train_direction(wnd_id, "L20D\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C14\015", 30);
	}

	flip_switch(wnd_id, "M8R\015");
	change_train_speed(wnd_id, "L20S3\015");
	change_train_speed(wnd_id, "L20S5\015");

	status = probe_segment(wnd_id, "C14\015", 10);
	flip_switch(wnd_id, "M8G\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C7\015", 20);
	}

	status = probe_segment(wnd_id, "C6\015", 20);
	change_train_speed(wnd_id, "L20S3\015");
	change_train_speed(wnd_id, "L20S0\015");
	change_train_direction(wnd_id, "L20D\015");
	flip_switch(wnd_id, "M4R\015");

	change_train_speed(wnd_id, "L20S4\015");
	sleep(10);
	status = probe_segment(wnd_id, "C5\015", 4);
	flip_switch(wnd_id, "M4G\015");
	sleep(TICKS_TO_SLEEP);
	change_train_speed(wnd_id, "L20S2\015");
	sleep(TICKS_TO_SLEEP);
	change_train_speed(wnd_id, "L20S0\015");

}


void run_config4(int wnd_id, int zamboni)
{
	int status = 0;

	flip_switch(wnd_id, "M6G\015");
	flip_switch(wnd_id, "M7G\015");
	flip_switch(wnd_id, "M2R\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C13\015", 20);
	}
	
	flip_switch(wnd_id, "M9G\015"); // need to flip it back
	change_train_speed(wnd_id, "L20S02\015");
	change_train_speed(wnd_id, "L20S5\015");
	status = probe_segment(wnd_id, "C14\015", 5);
	flip_switch(wnd_id, "M9R\015"); // done

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C6\015", 10);
	}

	status = probe_segment(wnd_id, "C7\015", 10);
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");
	
	flip_switch(wnd_id, "M5R\015"); // need to flip it back
	change_train_direction(wnd_id, "L20D\015");
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S4\015");
	change_train_speed(wnd_id, "L20S5\015");
	status = probe_segment(wnd_id, "C12\015", 15);
	flip_switch(wnd_id, "M5G\015"); // done M5
	
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");

	if(zamboni == 1)
	{
		status = probe_segment(wnd_id, "C14\015", 30);
	}

	flip_switch(wnd_id, "M1R\015"); // need to flip it back
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S4\015");
	status = probe_segment(wnd_id,"C14\015", 10);
	flip_switch(wnd_id, "M1G\015"); // done M1
	
	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");
	change_train_direction(wnd_id, "L20D\015");
	
	flip_switch(wnd_id, "M9G\015"); // need to flip it back
	change_train_speed(wnd_id, "L20S3\015");
	change_train_speed(wnd_id, "L20S4\015");
	status = probe_segment(wnd_id, "C16\015", 6);
	flip_switch(wnd_id, "M9R\015"); // done M9

	change_train_speed(wnd_id, "L20S2\015");
	change_train_speed(wnd_id, "L20S0\015");
}


void train_process(PROCESS self, PARAM param)
{
	int wnd_id = wm_create(6, 4, TRAIN_WINDOW_WIDTH, TRAIN_WINDOW_HEIGHT);
	wm_print(wnd_id, "Setting up outer switches\n");
	setup_train_track(wnd_id);
	
	int conf = check_configuration(wnd_id);
	wm_print(wnd_id, "Detected configuration %d \n", conf);

	wm_print(wnd_id, "Looking for Zamboni\n");
	int zamboni = probe_segment(wnd_id, "C4\015", 20);
	if(zamboni == 1)
	{
		wm_print(wnd_id, "Oh No! Zamboni is there\n");
	}else{
		wm_print(wnd_id, "Phew! No zamboni.\n");
	}

	switch(conf){

		case 1:
				run_config1(wnd_id, zamboni);
				wm_print(wnd_id, "Success! Phew...That was close.");
		break;

		case 2:
				run_config2(wnd_id, zamboni);
				wm_print(wnd_id, "Success! Wagon is safe.\n");
		break;

		case 3:
				run_config3(wnd_id, zamboni);
				wm_print(wnd_id, "Success! The train lives to see another day\n");
		break;

		case 4:
				run_config4(wnd_id, zamboni);
				wm_print(wnd_id, "Success! This is Thug life dB-)\n");
		break;

		default: wm_print(wnd_id, "Unknown configuration. Please check the model train\n");
	};

	while(1){
		resign();
	}
}


void init_train()
{
	PORT pr = create_process(train_process, 5, 0, "train process");
	resign();
}
		
		
