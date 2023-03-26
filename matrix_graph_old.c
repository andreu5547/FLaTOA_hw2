#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Graph {
    uint16_t num_vertices;
    uint16_t** adj_matrix;
} Graph;

Graph* create_graph(uint16_t num_vertices) {
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->num_vertices = num_vertices;

    graph->adj_matrix = (uint16_t**) malloc(num_vertices * sizeof(int*));
    for (uint16_t i = 0; i < num_vertices; i++) {
        graph->adj_matrix[i] = (uint16_t*) malloc(num_vertices * sizeof(int));
        for (uint16_t j = 0; j < num_vertices; j++) {
            graph->adj_matrix[i][j] = 0;
        }
    }

    return graph;
}

void add_edge(Graph* graph, int src, int dest) {
    graph->adj_matrix[src][dest] = 1;
}

Graph* read_graph_from_file(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    uint16_t num_vertices = 0;
    uint16_t src, dest;
    while (fscanf(fp, "%hd %hd", &src, &dest) != EOF) {
        if (src > num_vertices) {
            num_vertices = src;
        }
        if (dest > num_vertices) {
            num_vertices = dest;
        }
    }
    num_vertices++;

    Graph* graph = create_graph(num_vertices);

    rewind(fp);
    while (fscanf(fp, "%hd %hd", &src, &dest) != EOF) {
        add_edge(graph, src, dest);
    }

    fclose(fp);
    return graph;
}

void print_graph(Graph *graph) {
    printf("\nAdjacency Matrix:\n");

    // Print column labels
    printf("  ");
    for (uint16_t i = 0; i < graph->num_vertices; i++) {
        printf("%d ", i);
    }
    printf("\n");

    // Print row labels and adjacency matrix
    for (uint16_t i = 0; i < graph->num_vertices; i++) {
        printf("%d ", i);
        for (uint16_t j = 0; j < graph->num_vertices; j++) {
            printf("%d ", graph->adj_matrix[i][j]);
        }
        printf("\n");
    }
}


void write_graph_to_dot_file(Graph* graph, char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    fputs("graph {", fp);
    for (uint16_t i = 0; i < graph->num_vertices; i++) {
        for (uint16_t j = 0; j < graph->num_vertices; j++) {
            if (graph->adj_matrix[i][j] == 1) {
                fprintf(fp, "  %d -- %d;\n", i, j);
            }
        }
    }
    fputs("}", fp);

    fclose(fp);

    char command[100];
    sprintf(command, "bin\\dot.exe -Tpng %s -o %s.png", filename, filename);
    system(command);
}

void free_graph(Graph* graph) {
    for (uint16_t i = 0; i < graph->num_vertices; i++) {
        free(graph->adj_matrix[i]);
    }
    free(graph->adj_matrix);
    free(graph);
}


int main() {
    Graph* graph = read_graph_from_file("graph.txt");
    if (graph != NULL) {
        print_graph(graph);
        write_graph_to_dot_file(graph, "graph.dot");
        free_graph(graph);
    }

    return 0;
}
