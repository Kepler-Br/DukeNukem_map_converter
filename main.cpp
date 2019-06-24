#include "mapreader.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        puts("Program recieves two arguments: DukeNukemMap TargetFileName.\n\0");
        return 0;
    }
    MapReader reader;
    reader.open(argv[1]);
    reader.convert(argv[2]);
    puts("Done.\0");
    return 0;
}
