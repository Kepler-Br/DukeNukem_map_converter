#include "mapconverter.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        puts("Program recieves two arguments: DukeNukemMap; TargetFileName; coordinate divider(default 50.0).\n\0");
        return 0;
    }
    constexpr float defaultDivider = 50.0f;
    float divider = defaultDivider;
    if(argc >= 4)
    {
        divider = stof(argv[3]);
    }
    MapConverter reader(divider);
    reader.read(argv[1]);
    reader.convert(argv[2]);
    puts("Done.\0");
    return 0;
}
