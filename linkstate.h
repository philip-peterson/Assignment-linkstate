int main(int argc, char **argv);

int initializeBuffers(const char *fp);

int getTok(FILE * stream);

void dijkstra(int* edges, int N);

void printVisited(int n, int* visited);
