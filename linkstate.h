int main(int argc, char **argv);

int initializeBuffers(const char *fp);

int getTok(FILE * stream);

void dijkstra(int* edges, int N);

void printStep(int step, int n, int *visited, int *distance, int *predecessors);

void printDashes();

void printTableHeaders(int n);
