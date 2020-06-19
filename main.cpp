#include "sim_mem.h"
#include "stdio.h"

int main(){
    char exec_file_name[] = "exec_file.txt";
    char swap_file_name[] = "swap_file.txt";
    sim_mem simulator(exec_file_name, swap_file_name, 5, 5, 10, 20, 8, 5);
    
    simulator.print_memory();
    
    simulator.load(41);                 // out of program size error
    simulator.load(20);                 // stack&heap not init error
    simulator.store(2, '4');            // permission error
    simulator.store(41, '4');           // out of program size error
    
    char c1;
    for (int i = 0; i < 20; i+=5) {     //fill execfile to main memory
        c1 = simulator.load(i);
    }
    simulator.print_memory();
    
    simulator.store(7, '$');
    simulator.store(12, '$');
    simulator.store(17, '$');
    simulator.print_memory();
    
    for (int i = 20; i<40; i++) {       //fill some heap&stack to main memory
        if(i%5 == 0)
            c1++;
        simulator.store(i, c1);
    }
    simulator.print_memory();
    simulator.print_swap();
    simulator.print_page_table();
    
    for (int i = 0; i < 20; i+=5) {     //fill execfile to main memory
        c1 = simulator.load(i);
    }
    simulator.store(2, '$');
    simulator.print_swap();
    simulator.print_page_table();
    simulator.print_memory();
    
//
    return 0;
}

