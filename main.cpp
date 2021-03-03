#include "Tryte.h"
#include "Trint.h"
#include "Memory.h"
#include "CPU.h"
#include "test.h"
#include <fstream>
#include <ciso646>
#include <string>

int main(int argc, char** argv)
{
    Memory<19683> memory;
    std::vector<std::fstream*> disks;
    std::vector<std::string> disk_filenames;
    bool debug_mode_on = false;

    if (argc == 1)
    {
        std::cout << "No disk names detected. Aborting.\n";
    }
    else if (argv[1] == std::string("-debug"))
    {
        debug_mode_on = true;
        if (argc == 2)
        {
            std::cout << "No disk names detected. Aborting.\n";
        }
        else
        {
            size_t no_of_files = argc - 1;
            for (size_t i = 1; i < no_of_files; i++)
            {
                std::string disk_name(argv[i + 1]);
                disk_filenames.push_back(disk_name);
            }
        }
    }
    else
    {
        size_t no_of_files = argc;
        for (size_t i = 1; i < no_of_files; i++)
        {
            std::string disk_name(argv[i]);
            disk_filenames.push_back(disk_name);
        }  
    }

    CPU cpu(memory, disk_filenames);

    cpu.boot();
    if (debug_mode_on)
    {
        std::cout << "Starting program.\n";

        while (cpu.is_on())
        {
            cpu.dump();
            std::cout << "Press n for next instruction or q to quit.\n";
            char c;
            while (std::cin >> c)
            {
                if (c == 'n')
                {
                    cpu.step();
                    break;
                }
                else if (c == 'q')
                {
                    cpu.switch_off();
                    break;
                }
            }         
        }
        

    }
    else
    {
        cpu.run();
    }
    
    
}
