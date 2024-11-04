#include "uart.h"
#include "file_system.h"
#include "strings.h"

#define COMMAND_HISTORY_SiZE 10	

void notmain(void){
	char c;
	char command[100];
	int index = 0;
	int debug_mode = 0;//0 - debug off, 1 - on
	
	char command_history[COMMAND_HISTORY_SiZE][100];
	int command_history_count = 0;
	int command_history_pos = -1;
	uart_puts("UART test... \n");
	uart_puts("\033[31mThis text should be red.\033[0m\n");
    uart_puts("\033[32mThis text should be green.\033[0m\n");
    uart_puts("\033[1mThis text should be bold.\033[0m\n");
	init_fs();
	while(1){
		c = uart_getc();
		if(c == '\r'){
			command[index] = '\0';
			if(command[0] != '\0'){
				if(command_history_count < COMMAND_HISTORY_SiZE){
					my_strcpy(command_history[command_history_count++], command);
				}else{
					for (int i = 0; i < COMMAND_HISTORY_SiZE; i++){
						my_strcpy(command_history[i-1], command_history[i]);
					}
					my_strcpy(command_history[COMMAND_HISTORY_SiZE - 1], command);
				}
			}
			command_history_pos = command_history_count;
			print_command(command, debug_mode);
			process_command(command, &debug_mode);
			index = 0;
		} else if (c == '\b' || c == 127){
			if(index > 0){
				index--;
				uart_puts("\b \b");
			}
		} else if(c == 27){
			char seq[2];
			seq[0] = uart_getc();
			seq[1] = uart_getc();
			if(seq[0] == '['){
				if(seq[1]=='A'){
					if(command_history_pos > 0){
						command_history_pos--;
						while(index > 0){
							uart_puts("\b \b");
							index--;
						}
						my_strcpy(command, command_history[command_history_pos]);
						index = my_strlen(command);
						uart_puts(command);
					}
				}else if(seq[1] == 'B'){
					if(command_history_pos < command_history_count - 1){
						command_history_pos++;
						while(index > 0){
							uart_puts("\b \b");
							index--;
						}
						my_strcpy(command, command_history[command_history_pos]);
						index = my_strlen(command);
						uart_puts(command);
					}else if(command_history_pos == command_history_count - 1){
						while(index > 0){
							uart_puts("\b \b");
							index--;
						}
						command[0] = '\0';
					}
				}
			}	
		}else{
			if(index < sizeof(command)-1){
				command[index++] = c;
				uart_putc(c);
			}
		}
	}
}