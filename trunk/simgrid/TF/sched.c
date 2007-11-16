#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg/msg.h"
#include "xbt/sysdep.h"

#include "xbt/log.h"
#include "xbt/asserts.h"
XBT_LOG_NEW_DEFAULT_CATEGORY(msg_test,"Mensagens de log especificas a este codigo");

int master(int argc, char *argv[]);
int slave(int argc, char *argv[]);
int forwarder(int argc, char *argv[]);
void scatter_tasks_rr (int number_of_tasks, m_task_t* todo, int slaves_count, m_host_t* slaves);
MSG_error_t test_all(const char *platform_file, const char *application_file);

typedef enum {
	ROUND_ROBIN = 0,
 DYNAMIC,
 FUJIMOTO
} scheduling;

typedef enum {
	PORT_22 = 0,
 MAX_CHANNEL
} channel_t;

#define FINALIZE ((void*)221297) /* usado para informar os escravos de que devem terminar */

int master(int argc, char *argv[])
{
	scheduling sched;
	int slaves_count = 0;
	m_host_t *slaves = NULL;
	m_task_t *todo = NULL;
	int number_of_tasks = 0;
	double task_comp_size = 0;
	double task_comm_size = 0;


	int i;

  //verifica qual o algoritmo de escalonamento a ser utilizado
	if (!strcmp(argv[1],"RR")) {
		sched = ROUND_ROBIN;
	}
	else if (!strcmp(argv[1],"DYN")) {
		sched = DYNAMIC;
	}
	else if (!strcmp(argv[1],"FUJI")) {
		sched = FUJIMOTO;
	}

	
	xbt_assert1(sscanf(argv[2],"%d", &number_of_tasks),
					"Parametro invalido %s\n",argv[1]);
//   xbt_assert1(sscanf(argv[2],"%lg", &task_comp_size),
// 				  "Invalid argument %s\n",argv[2]);
//   xbt_assert1(sscanf(argv[3],"%lg", &task_comm_size),
// 				  "Invalid argument %s\n",argv[3]);
  

	{                  /*  Task creation */
		char sprintf_buffer[64];

		todo = calloc(number_of_tasks, sizeof(m_task_t));

		srand(1);
		for (i = 0; i < number_of_tasks; i++) {
			task_comp_size = (double) (rand() % 500000);
			task_comm_size = (double) (rand() % 50);
			INFO3("Custo da tarefa %d: CPU (%ld) ; NET (%d)", i, (long) task_comp_size, (int) task_comm_size);
			sprintf(sprintf_buffer, "Tarefa_%d", i);
			todo[i] = MSG_task_create(sprintf_buffer, task_comp_size, task_comm_size, NULL);
		}
	}

	{                  /* Process organisation */
		slaves_count = argc - 3;
		slaves = calloc(slaves_count, sizeof(m_host_t));
    
		for (i = 3; i < argc; i++) {
			slaves[i-3] = MSG_get_host_by_name(argv[i]);
			if(slaves[i-3]==NULL) {
				INFO1("Host nao encontrado %s. Finalizando! ", argv[i]);
				abort();
			}
		}
	}

	INFO1("Existem %d escravo(s) :", slaves_count);
	for (i = 0; i < slaves_count; i++)
		INFO1("%s", slaves[i]->name);

	INFO1("Existem %d tarefas a processar :", number_of_tasks);

	for (i = 0; i < number_of_tasks; i++)
		INFO1("\"%s\"", todo[i]->name);

	if (sched == ROUND_ROBIN)
		scatter_tasks_rr (number_of_tasks, todo, slaves_count, slaves);
	else if (sched == DYNAMIC);
	else if (sched == FUJIMOTO);
  
	INFO0("Todas as tarefas foram despachadas. Mandando os escravos finalizarem.");
	for (i = 0; i < slaves_count; i++) 
		MSG_task_put(MSG_task_create("finalize", 0, 0, FINALIZE),
						 slaves[i], PORT_22);
  
	INFO0("FIM.");
	free(slaves);
	free(todo);
	return 0;
} /* end_of_master */

int slave(int argc, char *argv[])
{
	while(1) {
		m_task_t task = NULL;
		int a;
		a = MSG_task_get(&(task), PORT_22);
		if (a == MSG_OK) {
			INFO1("Recebido \"%s\"", MSG_task_get_name(task));
			if(MSG_task_get_data(task)==FINALIZE) {
				MSG_task_destroy(task);
				break;
			}
			INFO1("Processando \"%s\"", MSG_task_get_name(task));
			MSG_task_execute(task);
			INFO1("\"%s\" feita", MSG_task_get_name(task));
			MSG_task_destroy(task);
		} else {
			INFO0("Ocorreu um erro ");
			xbt_assert0(0,"Unexpected behavior");
		}
	}
	INFO0("Processo terminado");
	return 0;
} /* end_of_slave */

int forwarder(int argc, char *argv[])
{
	int i;
	int slaves_count;
	m_host_t *slaves;

	{                  /* Process organisation */
		slaves_count = argc - 1;
		slaves = calloc(slaves_count, sizeof(m_host_t));
    
		for (i = 1; i < argc; i++) {
			slaves[i-1] = MSG_get_host_by_name(argv[i]);
			if(slaves[i-1]==NULL) {
				INFO1("Unknown host %s. Stopping Now! ", argv[i]);
				abort();
			}
		}
	}

	i=0;
	while(1) {
		m_task_t task = NULL;
		int a;
		a = MSG_task_get(&(task), PORT_22);
		if (a == MSG_OK) {
			INFO1("Received \"%s\"", MSG_task_get_name(task));
			if(MSG_task_get_data(task)==FINALIZE) {
				INFO0("All tasks have been dispatched. Let's tell everybody the computation is over.");
				for (i = 0; i < slaves_count; i++) 
					MSG_task_put(MSG_task_create("finalize", 0, 0, FINALIZE),
									 slaves[i], PORT_22);
				MSG_task_destroy(task);
				break;
			}
			INFO2("Sending \"%s\" to \"%s\"",
					MSG_task_get_name(task),
											slaves[i% slaves_count]->name);
			MSG_task_put(task, slaves[i % slaves_count],
							 PORT_22);
			i++;
		} else {
			INFO0("Hey ?! What's up ? ");
			xbt_assert0(0,"Unexpected behavior");
		}
	}

	INFO0("I'm done. See you!");
	return 0;
} /* end_of_forwarder */

MSG_error_t test_all(const char *platform_file,
							const char *application_file)
{
	MSG_error_t res = MSG_OK;

	/* MSG_config("surf_workstation_model","KCCFLN05"); */
	{                             /*  Simulation setting */
		MSG_set_channel_number(MAX_CHANNEL);
		MSG_paje_output("msg_test.trace");
		MSG_create_environment(platform_file);
	}
	{                            /*   Application deployment */
		MSG_function_register("master", master);
		MSG_function_register("slave", slave);
		MSG_function_register("forwarder", forwarder);
		MSG_launch_application(application_file);
	}
	res = MSG_main();
  
	INFO1("Tempo de simulacao: %g",MSG_get_clock());
	return res;
} /* end_of_test_all */

int main(int argc, char *argv[])
{
	MSG_error_t res = MSG_OK;

	MSG_global_init(&argc,argv);
	if (argc < 3) {
		printf ("Uso: %s plataforma deployment\n",argv[0]);
		printf ("exemplo: %s msg_platform.xml msg_deployment.xml\n",argv[0]);
		exit(1);
	}
	res = test_all(argv[1],argv[2]);
	MSG_clean();

	if(res==MSG_OK)
		return 0;
	else
		return 1;
} /* end_of_main */

void scatter_tasks_rr (int number_of_tasks, m_task_t* todo, int slaves_count, m_host_t* slaves) {
	int i;	
	
	for (i = 0; i < number_of_tasks; i++) {
		INFO2("Enviando \"%s\" para \"%s\"",
				todo[i]->name,
	 slaves[i % slaves_count]->name);
		if(MSG_host_self() == slaves[i % slaves_count]) {
			//... O mestre também executa
		}

		MSG_task_put(todo[i], slaves[i % slaves_count],
						 PORT_22);
		INFO0("Envio completado");
	}
	
}
