#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DIGRAPH 0 // Если 0, то неориентированный, если 1, то ориентированный граф
#define INPUT_FILE "graph.txt" // Файл входных данных
#define OUTPUT_FILE "graph.dot" // Файл выходных данных для GraphViz'a

typedef struct Edge { // Структура дааных для ребра
    uint16_t src;
    uint16_t dest;
} Edge;

typedef struct Graph { // Структура данных графа
    uint16_t num_vertices; // Число вершин
    uint16_t num_edges; // Число ребер
    Edge *edges; // Ребра
} Graph;

Graph *create_graph(uint16_t num_vertices, uint16_t num_edges) { // Функция создания графа
    Graph *graph = (Graph *) malloc(sizeof(Graph));

    graph->num_vertices = num_vertices;
    graph->num_edges = num_edges;
    graph->edges = (Edge *) malloc(num_edges * sizeof(Edge));
    return graph;
}

void add_edge(Graph *graph, uint16_t index, uint16_t src, uint16_t dest) { // Функция для добавления рбер в граф
    graph->edges[index].src = src;
    graph->edges[index].dest = dest;
}

Graph *read_graph_from_file(char *filename) { // Функция считывания графа из файла в формате исходная_вершина - конечная_вершина
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    uint16_t num_vertices = 0, num_edges = 0; // Подсчёт количества вершин и ребер
    uint16_t src, dest;
    while (fscanf(fp, "%hd %hd", &src, &dest) != EOF) {
        num_edges++;
        if (src > num_vertices) {
            num_vertices = src;
        }
        if (dest > num_vertices) {
            num_vertices = dest;
        }
    }
    num_vertices++;

    Graph *graph = create_graph(num_vertices, num_edges); // Создание графа

    rewind(fp);

    uint16_t i = 0;
    while (fscanf(fp, "%hd %hd", &src, &dest) != EOF) { // Запись ребер в граф
        add_edge(graph, i++, src, dest);
    }

    fclose(fp);
    return graph;
}

void print_graph_info(Graph *graph) { // Функция вывода информации о графе
    printf("Number of vertices: %d\n", graph->num_vertices);
    printf("Number of edges: %d\n", graph->num_edges);
}

void write_graph_to_dot_file(Graph *graph, char *filename, int8_t digraph) { // Функция записи графа в файл, для GraphViz'a
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    if (digraph) { // Выбор режима записи (ориентированный / неориентированный граф)
        fputs("digraph G {", fp);
        for (int16_t i = 0; i < graph->num_edges; i++)
            fprintf(fp, "  %d -> %d;\n", graph->edges[i].src, graph->edges[i].dest);
    } else {
        fputs("graph {", fp);
        for (int16_t i = 0; i < graph->num_edges; i++)
            fprintf(fp, "  %d -- %d;\n", graph->edges[i].src,
                    graph->edges[i].dest);
    }
    fputs("}", fp);

    fclose(fp);

    char command[100]; // Создание и отправка команды для вызова GraphViz'a
    sprintf(command, "dot -Tpng %s -o %s.png", filename, filename);
    system(command);
}

void free_graph(Graph *graph) { // Функция очистки памяти, выделенной для графа
    free(graph->edges);
    free(graph);
}

int main() {
    Graph *graph = read_graph_from_file(INPUT_FILE);
    if (graph != NULL) {
        print_graph_info(graph);
        write_graph_to_dot_file(graph, OUTPUT_FILE, DIGRAPH);
        free_graph(graph);
    }

    return 0;
}
