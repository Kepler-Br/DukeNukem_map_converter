#include "mapreader.h"

using namespace std;

int main(int argc, char* argv[])
{
    MapReader reader;
    reader.open("./newboard.map");
    reader.convert("./map.txt");
    puts("Done.\0");
    return 0;
}
