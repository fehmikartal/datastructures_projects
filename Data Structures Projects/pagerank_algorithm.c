#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define DAMPING_FACTOR 0.85
#define NUM_STEPS 1000000

typedef struct Edge {
    struct Node *dst;
    int num_link; 
} Edge;

typedef struct Node {
    int webpage_id;
    struct Edge **adjacents;
    int num_adj;
    int capacity;
} Node;

typedef struct Graph {
    Node *nodes; 
    int num_node;
    int capacity;
} Graph;
Graph* create_graph(int num_nodes)
{
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->num_node = num_nodes;
    g->capacity = num_nodes;
    g->nodes = (Node*)malloc(sizeof(Node)*num_nodes);

    for (int i = 0; i < num_nodes; i++)
    {
        g->nodes[i].webpage_id = i; // .'ya dikkat!
        g->nodes[i].num_adj = 0;
        g->nodes[i].capacity = 10;
        g->nodes[i].adjacents = (Edge**)malloc(sizeof(Edge*)*g->nodes[i].capacity);
    }

    return g;
}
void add_directed_edge(Graph *g, int src_id, int dst_id, int weight)
{
    if(weight == 0) return;
    Node* src_node = &g->nodes[src_id];
    Node* dst_node = &g->nodes[dst_id];

    if (src_node->capacity<=src_node->num_adj)
    {
        src_node->capacity *= 2;
        src_node->adjacents = (Edge**)realloc(src_node->adjacents,sizeof(Edge*)*src_node->capacity);
    }
    Edge* new_edge = (Edge*)malloc(sizeof(Edge));
    new_edge->dst = dst_node;
    new_edge->num_link = weight;
    src_node->adjacents[src_node->num_adj++] = new_edge;

}
void add_undirected_edge(Graph *g, int src_id, int dst_id, int weight)
{
    if(weight == 0) return;
    add_directed_edge(g,src_id,dst_id,weight);        
    add_directed_edge(g,dst_id,src_id,weight);
}

void free_graph(Graph *g)
{
    if(g == NULL) return;
    for(int i = 0; i<g->num_node;i++)
    {
        for (int j = 0; i < g->nodes[i].num_adj; j++)
        {
            free(g->nodes[i].adjacents[j]);
        }
        free(g->nodes[i].adjacents);
    }
    free(g->nodes);
    free(g);
}
double* pagerank_random_walk(Graph *graph, double damping_factor, int num_steps)
{
    int N = graph->num_node;
    
    int* counts = (int*)calloc(N,sizeof(int));
    double* rank = (double*)malloc(sizeof(double)*N);
    
    int current_idx = rand() % N;

    for (int i = 0; i < num_steps; i++)
    {
        // şu an olduğumuz yer
        counts[current_idx]++;

        Node* curr = &graph->nodes[current_idx];

        // next mi jump mı?
        double r = (double)rand() / RAND_MAX;
        if (curr->num_adj == 0 || r >= damping_factor)  // jump
        {
            current_idx = rand() % N;
        }else{                                          // next
            // A'dan B'ye 3, C'ye 1 link varsa %75-%25 ihtimal. total_adj: 4, link_choice: rastgele seçim
            int total_adj = 0;
            for (int i = 0; i < curr->num_adj; i++)
            {
                total_adj += curr->adjacents[i]->num_link;
            }

            int link_choice = rand() % total_adj ; 
            int accum = 0;
            for (int  i = 0; i <curr->num_adj; i++)
            {
                accum += curr->adjacents[i]->num_link;  // Döngü 1(B): accum = 3
                if (accum > link_choice)                // link_choice 0,1 veya 2 ise if sağlanır, değilse C'ye gidilir (Döngü 2)
                {
                    current_idx = curr->adjacents[i]->dst->webpage_id;  // bir edge seçiyor, o edge'in dst'sine gönderiyor.
                    break;
                }   
            }
        }   
    }
    // Sonuç
    for (int i = 0; i < N; i++)
    {
        rank[i] = (double) counts[i]/num_steps;
    }
    
    free(counts);
    return rank;

}
int get_shortest_dist(Graph graph, int src_id, int dst_id)
{
    if(src_id == dst_id) return 0;

    int N = graph.num_node;
    int* visited = (int*)calloc(N,sizeof(int));
    int* dist = (int*)malloc(sizeof(int)*N);

    for(int i=0;i<N;i++) dist[i]=-1;    // önce hepsini "ulaşılamaz [=-1] olarak başlatıyor"

    // Queue yapısı (BFS)
    int* queue = (int*)malloc(sizeof(int)*N);
    int front = 0;
    int rear = 0;

    // başlangıç düğümü ziyaret edildi olarak işaretleniyor.
    visited[src_id] = 1;
    dist[src_id] = 0; 
    queue[rear++] = src_id; // sıranın sonuna ekleniyor.

    while (front < rear)
    {
        int u_idx = queue[front++];     // kuyruğun front'undaki node
        Node* u = &graph.nodes[u_idx];

        if (u_idx == dst_id)            // u = gitmek istediğim node mu?
        {
            int result = dist[dst_id];
            free(visited);              // bitirmeden önce queue'yu freele
            free(dist);
            free(queue);
            return result;
        }
        // BFS'nin yayılma kısmı
        for (int  i = 0; i < u->num_adj; i++)
        {
            int v_idx = u->adjacents[i]->dst->webpage_id;   // tek tek komşuları gez
            if(!visited[v_idx]){                // daha önce gezilmemişse
                visited[v_idx] = 1;
                dist[v_idx] = dist[u_idx] + 1;
                queue[rear++] = v_idx;
            }
        }
    }
    // Sonuç bulunamadıysa (2 node arası yol olmayabilir)
    free(visited);
    free(dist);
    free(queue);
    return -1;
}

void load_matrix_directed(Graph *g, int **matrix, int size)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] > 0) {
                add_directed_edge(g, i, j, matrix[i][j]);
            }
        }
    }
}
void load_matrix_upper_triangle(Graph *g, int **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = i+1; j < size; j++)
        {
            if (matrix[i][j] > 0)
            {
            add_undirected_edge(g,i,j,matrix[i][j]);
            }
        }
    }
}
int main() {
    srand(time(NULL));
    int N = 12;
    int raw_matrix[12][12] = {
        {1,0,0,0,0,0,0,0,3,0,0,0}, 
        {2,1,6,0,0,0,0,0,0,0,0,0}, 
        {1,0,1,0,0,0,0,0,0,0,0,0}, 
        {0,0,0,1,2,0,0,0,0,0,0,0}, 
        {0,6,0,0,0,1,0,0,0,0,0,0}, 
        {0,0,0,1,0,0,1,0,0,0,0,0}, 
        {0,0,0,2,6,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0,1,0,0,1}, 
        {0,0,0,0,0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,2,5,0,2,4,0}, 
        {0,0,0,0,0,1,0,0,6,0,0,7}, 
        {0,0,0,0,0,0,0,1,2,0,0,0}  
    };
    

    int **matrix = (int**)malloc(N * sizeof(int*));
    for(int i=0; i<N; i++) matrix[i] = raw_matrix[i];

    printf("--- PageRank ---\n");
    Graph *g_directed = create_graph(N);
    load_matrix_directed(g_directed, matrix, N);
    
    double *scores = pagerank_random_walk(g_directed, DAMPING_FACTOR, NUM_STEPS);
    
    printf("Node IDs:\t");
    for(int i=0; i<N; i++) printf("%d\t", i);
    printf("\nPageRank:\t");
    for(int i=0; i<N; i++) printf("%.4f\t", scores[i]);
    printf("\n");
    
    free(scores);
    free_graph(g_directed);

    printf("\n--- BFS Shortest Path (Undirected/Upper Triangle) ---\n");
    Graph *g_undirected = create_graph(N);
    load_matrix_upper_triangle(g_undirected, matrix, N);
    
    int src = 0, dst = 6;
    int dist = get_shortest_dist(*g_undirected, src, dst);
    printf("Shortest distance between %d and %d: %d\n", src, dst, dist);

    free_graph(g_undirected);
    free(matrix); 
    
    return 0;
}