#include "mapconverter.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        puts("Program recieves two arguments: DukeNukemMap TargetFileName coordinate divider(100 default).\n\0");
        return 0;
    }
    float coordinateDivider = 100.0f;
    if(argc > 3)
    {
        coordinateDivider = stof(argv[3]);
    }
    const string dukeMap = argv[1];
    const string targetFile = argv[2];
    MapConverter reader;
    reader.read(dukeMap);
    reader.convert(targetFile, coordinateDivider);
    puts("Done.\0");
    return 0;
}
