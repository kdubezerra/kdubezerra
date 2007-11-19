#include <stdio.h>

FILE *arq_plat;
FILE *arq_dep;

int main (int argc, char** argv) {
	char host_name[255];
	int i, j, num_of_slaves = 100;
	int num_of_tasks;
	
	if (argc != 4) {
		printf ("\tUtilização: ./fcpc <no. de escravos> <no. de tarefas> <tipo de escalonamento (RR, DYN ou FUJI)>\n");
		return(1);
	}
		 
	sscanf(argv[1], "%d", &num_of_slaves);
	
	printf ("\n  Numero de escravos = %d\n\n", num_of_slaves);
	
	//INICIO da criação da plataforma
	arq_plat = fopen("new_plat.xml","w");
	
	
	fprintf (arq_plat, "<?xml version='1.0'?>\n");
	fprintf (arq_plat, "<!DOCTYPE platform_description SYSTEM \"surfxml.dtd\">\n");
	fprintf (arq_plat, "<platform_description version=\"1\">\n");
	
	fprintf (arq_plat, "\t<cpu name=\"Nodo_0\" power=\"99999000\"/>\n");	
	for (i = 1 ; i <= num_of_slaves ; i++) {
		fprintf (arq_plat, "\t<cpu name=\"Nodo_%d\" power=\"%d\"/>\n", i, rand() % 99999000);
	}
	
	fprintf (arq_plat, "\t<network_link name=\"Nodo_0_in\" bandwidth=\"%d\" latency=\"%f\"/>\n",  50000000, 0.0001 + (float) (rand() % 10000000) / 1000000000);
	fprintf (arq_plat, "\t<network_link name=\"Nodo_0_out\" bandwidth=\"%d\" latency=\"%f\"/>\n", 10000000, 0.0001 + (float) (rand() % 10000000) / 1000000000);
	
	for (i = 1 ; i <= num_of_slaves ; i++) {
		fprintf (arq_plat, "\t<network_link name=\"Nodo_%d_in\" bandwidth=\"%d\" latency=\"%f\"/>\n",  i, rand() % 50000000, 0.0001 + (float) (rand() % 10000000) / 1000000000);
		fprintf (arq_plat, "\t<network_link name=\"Nodo_%d_out\" bandwidth=\"%d\" latency=\"%f\"/>\n", i, rand() %  5000000, 0.0001 + (float) (rand() % 10000000) / 1000000000);
	}
	
	fprintf (arq_plat, "\t<network_link name=\"loopback_FATPIPE\" bandwidth=\"10000000\" latency=\"0.1\" sharing_policy=\"FATPIPE\"/>\n", i, 50000000);
	fprintf (arq_plat, "\t<network_link name=\"loopback\" bandwidth=\"498000000\" latency=\"0.000015\"/>\n", i, 50000000);
	
	for (i = 0 ; i <= num_of_slaves ; i++) {
		fprintf (arq_plat, "\t<route src=\"Nodo_%d\" dst=\"Nodo_%d\"><route_element name=\"loopback\"/></route>\n",  i, i);
	}

	
	//criando os links de todos-com-todos
	for (i = 0 ; i <= num_of_slaves ; i++) {
		if (!(i % 100))
		printf ("%d origens prontas\n", i);
		for (j = 0 ; j <= num_of_slaves ; j++) {			
			if (i==j) continue;
			
			fprintf (arq_plat, "\t<route src=\"Nodo_%d\" dst=\"Nodo_%d\">\n", i, j);
			fprintf (arq_plat, "\t\t<route_element name=\"Nodo_%d_out\"/><route_element name=\"Nodo_%d_in\"/>\n",  i, j);
			fprintf (arq_plat, "\t</route>\n");
		}
	}
	if (num_of_slaves%100) printf ("%d origens prontas\n", num_of_slaves);

	fprintf (arq_plat, "</platform_description>\n");
	
	fclose (arq_plat);
	
	//FIM da criação da plataforma
	
	
	//INICIO da criação do deployment
	arq_dep = fopen("new_dep.xml","w");
	
	sscanf(argv[2], "%d", &num_of_tasks);
	
	fprintf (arq_dep, "<?xml version='1.0'?>\n");
	fprintf (arq_dep, "<!DOCTYPE platform_description SYSTEM \"surfxml.dtd\">\n");
	fprintf (arq_dep, "<platform_description version=\"1\">\n");
	
	fprintf (arq_dep, "\t<process host=\"Nodo_0\" function=\"master\">\n");
	
	fprintf (arq_dep, "\t\t<argument value=\"%s\"/>     \t<!-- Type of scheduling -->\n", argv[3]);
	fprintf (arq_dep, "\t\t<argument value=\"%d\"/>     \t<!-- Number of tasks to execute -->\n", num_of_tasks);
	
	for (i = 1 ; i <= num_of_slaves ; i++){
		fprintf (arq_dep, "\t\t<argument value=\"Nodo_%d\"/>\n", i);
	}	
	fprintf (arq_dep, "\t\t<argument value=\"Nodo_0\"/>\n");
	
	fprintf (arq_dep, "\t</process>\n");
	
	for (i = 0 ; i <= num_of_slaves ; i++){
		fprintf (arq_dep, "\t<process host=\"Nodo_%d\" function=\"slave\"/>\n", i);
	}
	
	fprintf (arq_dep, "</platform_description>\n");
		
	fclose (arq_dep);
	
	//FIM da criação do deployment
	
	return(0);

}
