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

    if (argc == 1)
    {
        std::cout << "No disk names detected. Aborting.\n";
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

    CPU cpu(&memory, disk_filenames);

    cpu.boot();
    cpu.run();
}
