#include <stdio.h>
#include "graph.h"


int main() {
    Node* n1 = new_node(NULL, 0);
    printf("node %p\n", (void*)n1);
    print_node(n1);
    Node* n2 = new_node((Node**)(Node*[]){n1}, 1);
    Node* n3 = new_node((Node**)(Node*[]){n1}, 1);
    Node* n4 = new_node((Node**)(Node*[]){n2,n3}, 2);
    printf("node %p\n", (void*)n1);
    print_node(n1);
    printf("node %p\n", (void*)n2);
    print_node(n2);
    printf("node %p\n", (void*)n3);
    print_node(n3);
    printf("node %p\n", (void*)n4);
    print_node(n4);
    return 0;
}
