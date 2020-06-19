#include "sim_mem.h"

#define R_ONLY 0
#define R_AND_W 1
#define ON 1
#define OFF 0


char main_memory[MEMORY_SIZE];
         
sim_mem :: sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size, int bss_size, int heap_stack_size, int num_of_pages, int page_size) {
    

    //  --------------------------------------------------------------------//
    //  --------------------------- init section ---------------------------//
    //  --------------------------------------------------------------------//

    if((this->program_fd = open(exe_file_name, R_ONLY)) == -1)
        fatal("exec file open fail");
    if((this->swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT , 0666)) == -1)
        fatal("swap file open fail");

    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;
    this->page_table = (page_descriptor*)malloc(sizeof(page_descriptor)*num_of_pages);
    this->program_size = text_size+data_size+bss_size+heap_stack_size;
    
    if(this->page_table== NULL)
        fatal("in page table malloc creation");
    
    // init evey page state in page table 
    for(int i = 0; i < this->num_of_pages; i++){
        this->page_table[i].V = OFF;
        this->page_table[i].D = OFF;
        int text_erea = text_size/page_size;
        if(i<text_erea)
            this->page_table[i].P = R_ONLY;
        else
            this->page_table[i].P = R_AND_W;
    }
    
    int memory_frames = MEMORY_SIZE/page_size;
    for (int i = 0; i < memory_frames; i++)                 //create queue for empty frames
        frames.push(i);
    
    for (int i = 0; i < MEMORY_SIZE; i++)                   //init main memory
        main_memory[i] = '0';
    


    int execSize = num_of_pages*page_size;                  // init swap file to be full of zero's
    char str[execSize];
    for(int i = 0; i < execSize; i++)
        str[i] = '0';
    
    lseek(swapfile_fd, 0, SEEK_SET);
    if(write(swapfile_fd, str, execSize) < execSize)
        error("in swapFile init");
    
    
}

sim_mem ::~sim_mem()
{
    close(this->swapfile_fd);
    close(this->program_fd);
    free(page_table);
}

// func to get fatal errors to print the error specific & quit program  
void fatal(const char* msg){
    char err[100];
    strcpy(err, "[!Fatal Error] ");
    strncat(err, msg, 85);
    perror(err);
    exit(-1);
}

// func to get an errors and print to error fd
void error(const char* msg){
    char err[100];
    strcpy(err, "[!Error] ");
    strncat(err, msg, 85);
    perror(err);
    
}

// the main function on the program! the function gets an address, check where the page is and import page to memory if needed,
// in case memory is full the function save dirty pages to swap_file 
int sim_mem :: logical_address_maker(int virtual_address){
    int page, offset, logical_address;
    page = virtual_address/page_size;
    offset = virtual_address%page_size;
    if(page_table[page].V == ON){                                   // is in main memory.
        logical_address = page_table[page].frame*page_size+offset;
        return logical_address;
    }
                                                                    
    int fd, frame;
    char str_readed[page_size];
    if(page_table[page].D == ON || virtual_address>=text_size+bss_size+data_size)    // in swapfile_fd
        fd = this->swapfile_fd;
    else                                                            // in program exec file
        fd = this->program_fd;
    
    lseek(fd, page*page_size, SEEK_SET);
    if(read(fd, str_readed, page_size) != page_size){
        error("read from file");
        logical_address = -1;
        return logical_address;
    }
    
    if(frames.empty()){                                         // no empty space
        int page_to_clear = pages.front();
        pages.pop();
        frame = page_table[page_to_clear].frame;
        if(page_table[page_to_clear].D == ON){            // page to clear is dirty -> copy to swap file
            lseek(swapfile_fd, page_to_clear*page_size, SEEK_SET);
            if(write(swapfile_fd, &main_memory[frame*page_size], page_size) != page_size){
                error("write to swap file");
                logical_address = -1;
                return logical_address;
            }
        }
        page_table[page_to_clear].V = OFF;
    } else {                                                  // empty space avilable -> get free frame
        frame = frames.front();
        frames.pop();
    }
    logical_address = frame*page_size;
    strncpy(&main_memory[logical_address], str_readed, page_size);
    page_table[page].frame = frame;
    page_table[page].V = ON;
    pages.push(page);
    logical_address = frame*page_size+offset;
    return logical_address;
}

// func to load char ONLY, the func checks if the load request is legal
char sim_mem :: load(int address){
    if(address > program_size){
        error("requested page out of size");
        return '\0';
    }
    int page = address/page_size;
    if(address >= (program_size-heap_stack_size)){
        if(page_table[page].D == OFF){
            error("segmentation fault");
            return '\0';
        }
    }
    return main_memory[logical_address_maker(address)];
    
}

// func to store a char in memory, the func chacks if the store request is legal and in the memory size 
void sim_mem :: store(int address, char data){
    if(address > program_size){
        error("requested page out of size");
        return;
    }
    int page = address/page_size;
    if(page_table[page].P == R_ONLY){
        error("not permission to write on secion");
        return ;
    }
    int logical_address = logical_address_maker(address);
    main_memory[logical_address_maker(address)] = data;
    page_table[page].D = ON;
    
}




// func to print the main memory in currnet position
void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
    for(i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", main_memory[i]);
    }
}

// print swap file current position
void sim_mem::print_swap() {
    char* str = (char*)malloc(this->page_size *sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while(read(swapfile_fd, str, this->page_size) == this->page_size) {
        for(i = 0; i < page_size; i++) {
             printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
}

// print page table current position and the page states
void sim_mem::print_page_table() {
    int i;
    printf("\n page table \n");
    printf("Valid\t\tDirty\t\tPermission\tFrame\n\n");
    for(i = 0; i < num_of_pages; i++) {
        printf("[%d]\t\t[%d]\t\t[%d]\t\t[%d]\n", page_table[i].V, page_table[i].D, page_table[i].P, page_table[i].frame);
    }
}
