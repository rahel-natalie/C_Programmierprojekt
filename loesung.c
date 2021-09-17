#define _GNU_SOURCE


#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct
{
	size_t size;
	size_t buffer_size;
	
	int* elements;
	
} dynamic_int_array;


//constructor
dynamic_int_array new_dynamic_int_array()
{
	dynamic_int_array array;
	
	array.size=0;
	array.buffer_size=8;
	array.elements=malloc(array.buffer_size*sizeof(int));	
	
	return array;
}


//destructor
void free_dynamic_int_array(dynamic_int_array array)
{
	free(array.elements);
}


void push_int(dynamic_int_array* array, int element)
{
	if(array->size == array->buffer_size)
	{
		array->buffer_size*=2;
		int* bigger_array=realloc(array->elements, array->buffer_size*sizeof(int));
		
		array->elements=bigger_array;
	}
	
	size_t next_free_position=array->size;
	array->elements[next_free_position]=element;
		
	++array->size;
}







typedef struct
{
	char* id;
	uint32_t marking;
	
	dynamic_int_array neighbor_id_positions;
	
	bool already_parsed;
	
} node;








typedef struct
{
	size_t size;
	size_t buffer_size;
	
	node* elements;
	
} dynamic_node_array;


//constructor
dynamic_node_array new_dynamic_node_array()
{
	dynamic_node_array array;
	
	array.size=0;
	array.buffer_size=8;
	array.elements=malloc(array.buffer_size*sizeof(node));	
	
	return array;
}


//destructor
void free_dynamic_node_array(dynamic_node_array array)
{
	for(size_t i=0; i<array.size; ++i)
	{
		free(array.elements[i].id);
		free_dynamic_int_array(array.elements[i].neighbor_id_positions);
	}
	
	free(array.elements);
}


void push_node(dynamic_node_array* array, node element)
{
	if(array->size == array->buffer_size)
	{
		array->buffer_size*=2;
		node* bigger_array=realloc(array->elements, array->buffer_size*sizeof(node));
		
		array->elements=bigger_array;
	}
	
	size_t next_free_position=array->size;
	array->elements[next_free_position]=element;
		
	++array->size;
}




typedef struct
{
	int current_position_index;
	int previous_position_index;
	
	long steps_till_exhaustion;
	
} ant;



void check_line(char* line)
{
	int i=0;
	
	if(!isalnum(line[i]))
	{
		fprintf(stderr, "line does not start with alphanumeric char\n");
		exit(-42);
	}
	
	while(isalnum(line[i]))
	{
		++i;
	}
	if(line[i]!=':')
	{
		fprintf(stderr, "first non-alphanumeric char isn't a colon\n");
		exit(-42);
	}
	
	++i;
	if(line[i]=='\n' || line[i]=='\0')
	{
		fprintf(stderr, "neighbor and/or marking info missing\n");
		exit(-42);
	}
	else if(line[i]=='-')
	{
		++i;
		while(isdigit(line[i]))
		{
			++i;
		}
		if(line[i]!='\n' && line[i]!='\0')
		{
			fprintf(stderr, "marking should only contain digits\n");
			exit(-42);
		} 
	}
	else
	{
		if(!isalnum(line[i]))
		{
			fprintf(stderr, "neighbor info can't start with non-alphanumeric char\n");
			exit(-42);
		}
		
		do
		{
			++i;
			while(isalnum(line[i]))
			{
				++i;
			}
		}
		while(line[i]==',');

		if(line[i]=='-')
		{
			++i;
			while(isdigit(line[i]))
			{
				++i;
			}
			if(line[i]!='\n' && line[i]!='\0')
			{
				fprintf(stderr, "marking should only contain digits\n");
				exit(-42);
			} 
		}
		
		if(line[i]!='\n' && line[i]!='\0')
		{
			fprintf(stderr, "everything will be okay in the end, if it's not okay, it's not the end. this is not okay btw\n");
			exit(-42);
		}
	}
}


int find_or_create_node(dynamic_node_array* graph, char* wanted_id)
{
	int node_index;
	
	for(size_t i=0; i<graph->size; ++i)
	{
		node current_node=graph->elements[i];
		
		if(strcmp(current_node.id, wanted_id)==0)
		{
			node_index=i;
			return node_index;
		}
	}
	
	
	node new_node;
	new_node.id=strdup(wanted_id);
	new_node.marking=0;
	new_node.neighbor_id_positions=new_dynamic_int_array();
	new_node.already_parsed=false;
	
	push_node(graph, new_node);
	
	node_index=(graph->size)-1;
	return node_index;
}


ant build_ant_from_last_two_lines(dynamic_node_array* graph, char* line, size_t line_length, size_t buffer_size)
{
	ant freddy_the_ant;
	
	for(size_t i=0; i<line_length; ++i)
	{
		if(line[i]=='\n')
			line[i] = '\0';
	}
	
	bool freddy_is_lost=true;
	for(size_t i=0; i<graph->size; ++i)
	{
		char* start_id=line+2; //strdup to remove \0 ?
		char* current_id=graph->elements[i].id;
		
		if(strcmp(start_id, current_id)==0)
		{
			freddy_is_lost=false;
		}
	}
	if(freddy_is_lost)
	{
		fprintf(stderr, "start position is not part of the graph\n");
		exit(-42);
	}
	
	freddy_the_ant.current_position_index=find_or_create_node(graph, line+2);
			
								
	if(getline(&line, &buffer_size, stdin)!=-1)
	{	
		int i=0;
		
		if(line[i]!='I')
		{
			fprintf(stderr, "steps number line has to start with I\n");
			exit(-42);
		}
		
		++i;
		if(line[i]!=':')
		{
			fprintf(stderr, "colon after I is missing\n");
			exit(-42);
		}
		
		++i;
		while(isdigit(line[i]))
		{
			++i;
		}
		if(line[i]!='\n' && line[i]!='\0')
		{
			fprintf(stderr, "steps number must contain only digits\n");
			exit(-42);
		}
		
		char* steps=line+2;
		freddy_the_ant.steps_till_exhaustion=atol(steps);
	}
	else
	{
		fprintf(stderr, "steps number info is missing\n");
		exit(-42);
	}
	
	return freddy_the_ant;
}





void add_edges(dynamic_node_array* graph, char* next_part, int node_index)
{							
	char* neighbor_id=strdup(next_part);
	int neighbor_index=find_or_create_node(graph, neighbor_id);
	free(neighbor_id);
	
	if(node_index==neighbor_index)
	{
		fprintf(stderr, "node can't be it's own neighbor\n");
		exit(-42);
	}
	
	for(size_t i=0; i<graph->elements[node_index].neighbor_id_positions.size; ++i)
	{
		if(graph->elements[node_index].neighbor_id_positions.elements[i]==neighbor_index)
		{
			fprintf(stderr, "edge is mentioned twice in input\n");
			exit(-42);	
		}
	}

	push_int(&graph->elements[node_index].neighbor_id_positions, neighbor_index);
	push_int(&graph->elements[neighbor_index].neighbor_id_positions, node_index);
}


long parse_checked_marking(char* rest_of_line)
{
	char* read_till_this;
	long result = strtol(rest_of_line, &read_till_this, 10);
     
    if((read_till_this[0]!='\n' && read_till_this[0]!='\0') || result<0 || result>4294967295)
    {
		fprintf(stderr, "marking too big or negativ\n");
		exit(-42);
	}
     
    return result;
}


void handle_neighbor_and_marking_information(dynamic_node_array* graph, char* next_part, int node_index)
{
	bool line_contains_marking_info=false;
				
	while(next_part[0]!='\0' && next_part[0]!='\n') // while there is more input to handle
	{	
		size_t next_part_length=strlen(next_part);

		for(size_t j=0; j<next_part_length; ++j)
		{
			if(next_part[0]=='-') //if there is only marking information but no neighbors
			{
				next_part[j]='\0';

				uint32_t marking=parse_checked_marking(next_part+1);
				graph->elements[node_index].marking=marking;
							
				break;
			}
			
			
			if(next_part[j]==',' || next_part[j]=='-' || (next_part[j]=='\n' && !line_contains_marking_info))
			{
				if(next_part[j]=='-')
					line_contains_marking_info=true;
								
				next_part[j]='\0';
				add_edges(graph, next_part, node_index);
				next_part+=strlen(next_part)+1;
							
				break;
			}
			if(next_part[j]=='\n' && line_contains_marking_info) //handle marking information
			{
				next_part[j]='\0';
				
				uint32_t marking_as_number=parse_checked_marking(next_part);		
				graph->elements[node_index].marking=marking_as_number;
							
				next_part+=strlen(next_part)+1;
				
				break;
			}
		}	
	}
}




dynamic_node_array* graph_for_sorting;

int compare_ids(const void* lhs, const void* rhs)
{
	int lhs_as_int=*(int*)lhs;
	int rhs_as_int=*(int*)rhs;
	
	return strcmp(graph_for_sorting->elements[lhs_as_int].id, graph_for_sorting->elements[rhs_as_int].id);
}

void sort_neighbors_for_every_node(dynamic_node_array* graph)
{
	graph_for_sorting=graph;
	
	for(size_t i=0; i<graph->size; ++i)
	{
		node* current_node=&graph->elements[i];
		int* array_beginning=current_node->neighbor_id_positions.elements;
		
		size_t number_of_elements=current_node->neighbor_id_positions.size;
		size_t element_size=sizeof(int);
		
		qsort(array_beginning, number_of_elements, element_size, &compare_ids);
	}
}




int move_ant(dynamic_node_array* graph, ant* freddy_the_ant)
{	
	for(long i=freddy_the_ant->steps_till_exhaustion; i>0; --i)
	{	
		if(graph->elements[freddy_the_ant->current_position_index].neighbor_id_positions.size==0)
		{
			++graph->elements[freddy_the_ant->current_position_index].marking;
		}
		else
		{
			int position_of_next_node=graph->elements[freddy_the_ant->current_position_index].marking%graph->elements[freddy_the_ant->current_position_index].neighbor_id_positions.size;
				
			freddy_the_ant->previous_position_index=freddy_the_ant->current_position_index;
			freddy_the_ant->current_position_index=graph->elements[freddy_the_ant->current_position_index].neighbor_id_positions.elements[position_of_next_node];
			
			++graph->elements[freddy_the_ant->previous_position_index].marking;
		}
	}
	
	return freddy_the_ant->current_position_index;
}




void print_result(dynamic_node_array* graph, int end_position_index)
{
	for(size_t i=0; i<graph->size; ++i)
	{
		node* current_node=&graph->elements[i];
	
		printf("%s:", current_node->id);
		printf("%i\n", current_node->marking);
	}
	
	printf("E:%s\n", graph->elements[end_position_index].id);
}



dynamic_node_array graph;
char* line=NULL;

void free_everything()
{
	free(line);
	free_dynamic_node_array(graph);
}

int main()
{
	atexit(free_everything);
	
	graph=new_dynamic_node_array();
	ant freddy_the_ant;
	freddy_the_ant.current_position_index=0;
	freddy_the_ant.previous_position_index=0;
	freddy_the_ant.steps_till_exhaustion=0;
	
	
	size_t buffer_size;

	bool whole_input_handled=false;
	bool line_with_A_missing=true;
	while(getline(&line, &buffer_size, stdin)!=-1) //getline returns -1 in case of error
	{
		check_line(line);
		size_t line_length=strlen(line);
		
		for(size_t i=0; i<line_length; ++i)
		{
			if(line[i]==':')
			{
				line[i]='\0';
				
				if(strcmp(line, "A")==0)
				{
					freddy_the_ant=build_ant_from_last_two_lines(&graph, line, line_length, buffer_size);
					whole_input_handled=true;
					line_with_A_missing=false;
					break;
				}
				
				char* node_id=line;
				int node_index=find_or_create_node(&graph, node_id);
				
				if(graph.elements[node_index].already_parsed)
				{
					fprintf(stderr, "node is mentioned twice in input\n");
					exit(-42);
				}
				else
					graph.elements[node_index].already_parsed=true;
				
				char* next_part=line+strlen(node_id)+1;
				handle_neighbor_and_marking_information(&graph, next_part, node_index);
			}
		}
		
		if(whole_input_handled)
			break;
	}
	
	if(getchar()!=EOF)
	{
		fprintf(stderr, "too much input\n");
		exit(-42);
	}
	if(line_with_A_missing)
	{
		fprintf(stderr, "no start id info\n");
		exit(-42);
	}
	
	
	sort_neighbors_for_every_node(&graph);

	int end_position_index=move_ant(&graph, &freddy_the_ant);
	print_result(&graph, end_position_index);

	
	return 0;
}
