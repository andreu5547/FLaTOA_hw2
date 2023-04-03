#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define DIGRAPH false // Если false, то неориентированный, если true, то ориентированный граф
#define SORT_G true // Сортировать ли граф
#define DEL_REPEAT true // Удалять ли повторяющиеся ребра
#define INPUT_FILE "graph.txt" // Файл входных данных (по умолчанию)
#define OUTPUT_FILE "graph.dot" // Файл выходных данных для GraphViz'a
#define COEFF_UP_MEMORY 2 // Коэффициент увеличения выделенной памяти

#define swap_Edge(a, b) Edge temp = a; \
                        a = b; \
                        b = temp;

typedef struct Edge { // Структура данных для ребра
    uint16_t src;
    uint16_t dest;
} Edge;

typedef struct Graph { // Структура данных графа
    uint16_t num_vertices; // Число вершин
    uint16_t num_edges; // Число ребер
    uint16_t capacity; // Вместимость
    Edge *edges; // Ребра
} Graph;

Graph *create_graph(uint16_t capacity) { // Функция создания графа
    Graph *graph = (Graph *) malloc(sizeof(Graph)); // Выделяем память под граф
    if (!graph) { // Проверяем выделилась ли память
        puts("Allocation error!");
        exit(1);
    }
    // Задаем базовые значения и выделяем память для ребер + проверка выделилась ли
    graph->capacity = capacity;
    graph->num_vertices = 0;
    graph->num_edges = 0;
    graph->edges = (Edge *) malloc(capacity * sizeof(Edge)); // Выделяем память под ребра
    if (!graph->edges) { // Проверяем выделилась ли память
        puts("Allocation error!");
        exit(1);
    }
    return graph;
}

void add_memory_graph(Graph *graph) { // Функция расширения вместимости графа
    if (graph->capacity < 10000) // Выбираем коэффициент в зависимости от вместимости графа
        graph->capacity = graph->capacity * COEFF_UP_MEMORY;
    else graph->capacity = (uint16_t) graph->capacity * COEFF_UP_MEMORY / 2; // Устанавливаем новое значение вместимости
    graph->edges = (Edge *) realloc(graph->edges, graph->capacity * sizeof(Edge)); // Выделяем новое количество памяти
    if (!graph->edges) { // Проверяем выделилась ли память
        puts("Allocation error!");
        exit(1);
    }
}

void add_edge(Graph *graph, uint16_t src, uint16_t dest) { // Функция для добавления ребер в граф
    graph->edges[graph->num_edges].src = src;
    graph->edges[graph->num_edges].dest = dest;
}

Graph *read_graph_from_file(char *filename) { // Функция считывания графа из файла в формате "исходная_вершина конечная_вершина"
    FILE *fp = fopen(filename, "r"); // Открываем файл
    if (fp == NULL) { // Проверяем открылся ли файл
        printf("Error opening file %s!", filename);
        return NULL;
    }

    Graph *graph = create_graph(10); // Создание графа
    // создание буферов: для считываемого символа, числа, сохранённого первого числа и флага для проверки корректности колва чисел в строке
    char buff_c;
    uint16_t buff_n = 0, buff_src = 0;
    bool check_num = false;

    while (true) { // Читаем посимвольно файл
        buff_c = getc(fp);
        if (buff_c != '\n' && buff_c != EOF) { // Если не символ не \n или EOF входим в if
            if ('0' <= buff_c && buff_c <= '9') { // Проверяем является ли символ цифрой
                buff_n = buff_n * 10 + buff_c - '0';
            } else if (buff_c == ' ') { // Если символ пробел -> ввод цифры окончен
                if (check_num) { // Проверяем что всего до этой цифры максимум введена 1 цифра в строке
                    puts("Error input file data!");
                    return NULL;
                }
                if (graph->num_vertices < buff_n) // поиск максимального номера вершины
                    graph->num_vertices = buff_n;
                buff_src = buff_n; // Записываем первое число
                buff_n = 0; // Очищаем буфер
                check_num = true; // Ставим флаг, что одно число мы уже получили
            } else { // Если есть посторонние символы - ошибка
                puts("Error input file data!");
                return NULL;
            }
        } else {
            if (graph->capacity == graph->num_edges) // Выделение дополнительной памяти, при её нехватке
                add_memory_graph(graph);
            if (graph->num_vertices < buff_n) // поиск максимального номера вершины
                graph->num_vertices = buff_n;
            if (check_num) { // Проверяем, что введены оба числа
//                printf("%d %d\n", buff_src, buff_n);
                add_edge(graph, buff_src, buff_n); // Добавляем ребро
                graph->num_edges++; // Увеличиваем количество ребер на 1
            }
            // Обнуляем временные переменные
            buff_n = 0;
            buff_src = 0;
            check_num = false;
        }
        if (buff_c == EOF)
            break;
    }

    fclose(fp); // Закрываем файл
    return graph; // Возвращаем граф
}

void print_graph_info(Graph *graph) { // Функция вывода информации о графе
    printf("Number of vertices: %d\n", graph->num_vertices + 1);
    printf("Number of edges: %d\n", graph->num_edges);
}

void write_graph_to_dot_file(Graph *graph, char *filename, bool digraph) { // Функция записи графа в файл, для GraphViz'a
    FILE *fp = fopen(filename, "w"); // Открываем файл для записи
    if (fp == NULL) { // Проверяем корректно ли открылся файл
        printf("Error opening file\n");
        return;
    }

    if (digraph) { // Выбор режима записи (ориентированный / неориентированный граф) и его запись
        fputs("digraph G {\n", fp);
        for (uint16_t i = 0; i < graph->num_edges; i++)
            fprintf(fp, "\t%d -> %d;\n", graph->edges[i].src, graph->edges[i].dest);
    } else {
        fputs("graph {\n", fp);
        for (uint16_t i = 0; i < graph->num_edges; i++)
            fprintf(fp, "\t%d -- %d;\n", graph->edges[i].src,
                    graph->edges[i].dest);
    }
    fputs("}", fp);

    fclose(fp); // Закрываем файл

    char command[100]; // Создание и отправка команды для вызова GraphViz'a
    sprintf(command, "dot -Tpng %s -o %s.png", filename, filename);
    system(command);
}

void free_graph(Graph *graph) { // Функция очистки памяти, выделенной для графа
    free(graph->edges);
    free(graph);
}

Graph * sort_edge(Graph *graph) { // Функция сортировки графа
    // сортировка ребер пузырьком по второму элементу
    for (uint16_t i = 0; i < graph->num_edges - 1; i++)
        for (uint16_t j = 0; j < graph->num_edges - i - 1; j++)
            if (graph->edges[j].dest > graph->edges[j + 1].dest) {
                swap_Edge(graph->edges[j], graph->edges[j + 1])
            }
    // сортировка ребер пузырьком по первому элементу
    for (uint16_t i = 0; i < graph->num_edges - 1; i++)
        for (uint16_t j = 0; j < graph->num_edges - i - 1; j++)
            if (graph->edges[j].src > graph->edges[j + 1].src) {
                swap_Edge(graph->edges[j], graph->edges[j + 1])
            }

    return graph; // Возвращаем отсортированный граф
}

Graph * delete_sorting_repeating_edge(Graph *graph) { // Функция удаления повторяющихся ребер из графа
    for (uint16_t i = 0; i < graph->num_edges; ++i) { // перебираем все элементы множества
        bool fl = false;
        for (uint16_t j = i + 1; j < graph->num_edges; ++j) // ищем дубликат текущего элемента
            if (graph->edges[i].src == graph->edges[j].src && graph->edges[i].dest == graph->edges[j].dest)
                fl = true;
        if (fl) { //если элемент нашёлся, то удаляем его и сдвигаем массив на один элемент влево и уменьшаем размер на 1
            for (uint16_t j = i; j < graph->num_edges; ++j)
                graph->edges[j] = graph->edges[j + 1 ];
            i--;
            graph->num_edges--;
        }
    }
    return graph;
}

int main(int argc, char *argv[]) {
    Graph *graph;

    if (*++argv != NULL) // Проверяем передал ли пользователь название файла, если нет ставим по умолчанию
        graph = read_graph_from_file(*argv);
    else graph = read_graph_from_file(INPUT_FILE);

    if (graph != NULL) {
        puts("Input graph info:");
        print_graph_info(graph);
        if (SORT_G || DEL_REPEAT) {
            sort_edge(graph);
            if (DEL_REPEAT) {
                delete_sorting_repeating_edge(graph);
                puts("After removing duplicate edges graph info:");
                print_graph_info(graph);
            }
        }
        write_graph_to_dot_file(graph, OUTPUT_FILE, DIGRAPH);
        free_graph(graph);
    }
    return 0;
}