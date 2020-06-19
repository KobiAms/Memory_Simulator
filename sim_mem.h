#ifndef _EX4
#define _EX4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <queue>

using namespace std;

#define MEMORY_SIZE 20

typedef struct page_descriptor_t {
    unsigned int V; // valid
    unsigned int D; // dirty
    unsigned int P; // permission
    unsigned int frame; //the number of a frame if in case it is page-mapped
}page_descriptor;


void fatal(const char*);
void error(const char*);

class sim_mem {
private:
    int swapfile_fd;    //swap file fd
    int program_fd;     //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    int program_size;
    page_descriptor *page_table;
    queue<int> frames;
    queue<int> pages;
    int logical_address_maker(int);
    
public:
    sim_mem(char [], char [], int, int, int, int, int, int );
    ~sim_mem();
    char load(int);
    void store(int, char);
    void print_memory();
    void print_swap ();
    void print_page_table();
};

extern char main_memory[MEMORY_SIZE];


#endif
