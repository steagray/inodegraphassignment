#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

// probably make the file handler for the dot graph global
FILE* graph;

int
dotgraph(char* f_name, uintmax_t f_size, intmax_t a_blocks, uintmax_t i_num, uintmax_t links, char* p_name){
	// Add all data from this node into dot graph previously opened via main
	fprintf(graph, "\"%s\" [label=\"\\n", f_name);
	fprintf(graph, "Name: %s\\n", f_name);
	fprintf(graph, "File Size: %lu\\n", f_size);
	fprintf(graph, "Allocated Blocks: %lu\\n", a_blocks);
	fprintf(graph, "I-node Number: %lu\\n", i_num);
	fprintf(graph, "# Of Links: %lu\\n", links);
	fprintf(graph, "\"]\n");
	fprintf(graph, "\"%s\" -> \"%s\"\n\n", p_name, f_name);
	return 0;
}

int
recur_graph(char* loc){
	// First, start at the location provided
	struct stat sb;
	stat(loc, &sb);
	char loc_copy[1024];
	strcpy(loc_copy, loc);

	// get data for directory
	// Data Needed:
	// - File/Directory Name
	char* tok = strtok(loc_copy, "/");
	char p_name[1024];
	char f_name[1024];
	while(tok != NULL){
		strcpy(p_name, f_name);
		strcpy(f_name, tok);
		tok = strtok(NULL, "/");
	}

	// - File Size
	// - Number of Allocated Blocks
	// - i-node number
	// - Number of links
	// put data into dot graph
	dotgraph(f_name, sb.st_size, sb.st_blocks, sb.st_ino, sb.st_nlink, p_name);	


	// continue with next directory
	struct dirent *de;
	DIR *dr = opendir(loc);
	if(dr == NULL){
		return 0;
	}
	de = readdir(dr);
	while(de != NULL){
		char path[1024];
		if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
			de = readdir(dr);
			continue;
		}
		//printf("%s\n", de->d_name);
		
		strcpy(path, loc);
		strcat(path, "/");
		strcat(path, de->d_name);

		recur_graph(path);
		de = readdir(dr);
	}
	closedir(dr);
	return 0;
}

int
main(int argc, char* argv[]){
	if(argc < 2){
		printf("Usage: %s [location to start generating graph]\n", argv[0]);
		return 0;
	}
	//start dotgraph file here
	graph = fopen("./graph", "w");
	fprintf(graph, "digraph{\nrankdir=LR\n");
	recur_graph(argv[1]);
	//end dotgraph file after recur
	fprintf(graph, "\n}");
	fclose(graph);
	return 0;
}
