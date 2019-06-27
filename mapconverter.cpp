#include "mapconverter.h"
#include <vector>

int16_t MapConverter::readint16(std::fstream &file)
{
    int16_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(int16_t));
    return num;
}

int32_t MapConverter::readint32(std::fstream &file)
{
    int32_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(int32_t));
    return num;
}

void MapConverter::checkFileFlags(std::fstream &file)
{
//    using std::cout;

    if(!file.is_open())
    {
        puts("Cannot open file.\n\0");
//        cout << "Cannot open file.\n";
        return;
    }

    if(file.bad())
    {
        puts("Cannot open file: bad.\n\0");
//        cout << "Cannot open file: bad.\n";
        return;
    }

    int32_t mapVersion = readint32(file);
    if(mapVersion != 7)
    {
        puts("Map version is not 7.\n\0");
//        cout << "Map version is not 7.\n";
        return;
    }
}

void MapConverter::readPlayerAttributes(std::fstream &file)
{
    pl.posx  = readint32(file);
    pl.posy  = readint32(file);
    pl.posz  = readint32(file);
    pl.angle = readint16(file);
    pl.startSector = readint16(file);
}

void MapConverter::readSectors(std::fstream &file)
{
    using std::runtime_error;
    using std::ios_base;

    file.seekg(0x14, ios_base::beg);

    int16_t numSectors = readint16(file);
    if(numSectors <= 0)
        throw runtime_error("Negative number of sectors.");

    constexpr int maxSectors = 2<<12;
    if(numSectors > maxSectors)
        throw runtime_error("Maximum sectors count reached(8192).");
    sectors.reserve(numSectors);
    for(int i = 0; i < numSectors; i++)
    {
        sector sec;
        sec.startWall = readint16(file);
        sec.wallNum = readint16(file);
        sec.floorHeigth = readint32(file);
        sec.ceilingHeight = readint32(file);
        sectors.push_back(sec);
        file.seekg(0x1C, ios_base::cur);
        if(file.eof())
            throw runtime_error("Unexpected eof.");
    }
}

void MapConverter::readWalls(std::fstream &file)
{
    using std::runtime_error;
    using std::ios_base;

    int16_t numWalls = readint16(file);
    constexpr int maxWalls = 2<<16;
    if(numWalls <= 0)
        throw runtime_error("Negative number of walls.");
    if(numWalls > maxWalls)
        throw runtime_error("Max wall count reached(131072).");
    walls.reserve(numWalls);
    for(int i = 0; i < numWalls; i++)
    {
        wall wll;
        wll.x = readint32(file);
        wll.y = readint32(file);
        wll.point2 = readint16(file);
        wll.nextWall = readint16(file);
        wll.nextSector = readint16(file);
        walls.push_back(wll);
        file.seekg(0x12, ios_base::cur);
        if(file.eof())
            throw runtime_error("Unexpected eof.");
    }
}

void MapConverter::writeHeader(std::fstream &file)
{
    file << "// secnum stands for sector number. It defines total number of sectors in the map.\n";
    file << "secnum " << sectors.size() << "\n\n";
    file << "// wallnum stands for wall number. It defines total number of wals in the map.\n";
    file << "wallnum " << walls.size() << "\n";
    file << "// pstart stands for player start. It defines where player will be spawn in the level.\n";
    file << "// pstart X Y Z angle start_sector\n";
    constexpr float maxIntAngle = 2047.0f;
    file << "pstart " << static_cast<float>(pl.posx)/coordinateDivider << " " << static_cast<float>(pl.posy)/coordinateDivider << " "
         << static_cast<float>(pl.posz)/coordinateDivider << " " << static_cast<float>(pl.angle)/maxIntAngle*360.0f << " " << pl.startSector << "\n";
    file << "\n";
}

void MapConverter::writeSectors(std::fstream &file)
{
    file << "// s stands for sector. It defines one sector. Format:\n"
            "//s start_wall_index wall_number floor_height ceiling_height\n";
    for(const auto & sec : sectors)
    {
        file << "s "<< sec.startWall << " " << sec.wallNum << " "
             << static_cast<float>(sec.floorHeigth)/coordinateDivider << " "
             << static_cast<float>(sec.ceilingHeight)/coordinateDivider << "\n";
    }
}

void MapConverter::writeWalls(std::fstream &file)
{
    file << "\n// w stands for wall. It defines one wall. Format:\n"
            "//w point_x point_y second_wall_point next_sector(in case if portal > -1)\n";

    for(const auto & wll : walls)
    {
        file << "w "<< static_cast<float>(wll.x)/coordinateDivider << " "
             << static_cast<float>(wll.y)/coordinateDivider
             << " " << wll.point2 << " " << wll.nextSector << "\n";
    }
}

void MapConverter::read(const std::string &path)
{
    using std::fstream;
    using std::ios_base;

    std::fstream file;
    file.open(path, std::ios_base::binary | std::ios_base::in);

    checkFileFlags(file);
    readPlayerAttributes(file);
    readSectors(file);
    readWalls(file);
    file.close();
}

void MapConverter::convert(const std::string &path)
{
    using std::fstream;
    using std::ios_base;

    fstream file(path, ios_base::out);

    writeHeader(file);
    writeSectors(file);
    writeWalls(file);

    file.close();
}
