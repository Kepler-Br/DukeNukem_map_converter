#include "mapreader.h"
#include <vector>

int16_t MapReader::readint16()
{
    int16_t num = 0;
    mapFile.read(reinterpret_cast<char*>(&num), sizeof(int16_t));
    return num;
}

int32_t MapReader::readint32()
{
    int32_t num = 0;
    mapFile.read(reinterpret_cast<char*>(&num), sizeof(int32_t));
    return num;
}

void MapReader::open(const std::string &path)
{
    using namespace std;

    mapFile.open(path, std::ios_base::binary | std::ios_base::in);
    if(!mapFile.is_open())
    {
        cout << "Cannot open file.\n";
        return;
    }

    if(mapFile.bad())
    {
        cout << "Cannot open file: bad.\n";
        return;
    }

    int32_t mapVersion = readint32();
    if(mapVersion != 7)
    {
        cout << "Map version is not 7.\n";
        return;
    }
    auto posx = readint32();
    auto posy = readint32();
    auto posz = readint32();

    mapFile.seekg(0x14, ios_base::beg);
    int16_t numSectors = readint16();
    if(numSectors <= 0)
        throw runtime_error("Negative number of sectors.");

    constexpr int maxSectors = 2<<12;
    if(numSectors > maxSectors)
        throw runtime_error("Maximum sectors count reached(8192).");
    sectors.reserve(numSectors);
    for(int i = 0; i < numSectors; i++)
    {
        sector sec;
        sec.startWall = readint16();
        sec.wallNum = readint16();
        sec.ceilingHeight = readint32();
        sec.floorHeigth = readint32();
        sectors.push_back(sec);
        mapFile.seekg(0x1C, ios_base::cur);
        if(mapFile.eof())
            throw runtime_error("Unexpected eof.");
    }

    int16_t numWalls = readint16();
    constexpr int maxWalls = 2<<16;
    if(numWalls <= 0)
        throw runtime_error("Negative number of walls.");
    if(numWalls > maxWalls)
        throw runtime_error("Max wall count reached(131072).");
    walls.reserve(numWalls);
    for(int i = 0; i < numWalls; i++)
    {
        wall wll;
        wll.x = readint32();
        wll.y = readint32();
        wll.point2 = readint16();
        wll.nextWall = readint16();
        wll.nextSector = readint16();
        walls.push_back(wll);
        mapFile.seekg(0x12, ios_base::cur);
        if(mapFile.eof())
            throw runtime_error("Unexpected eof.");
    }
    mapFile.close();
    isFileOpened = true;
}

void MapReader::convert(const std::string &path)
{
    using namespace std;
    fstream file(path, ios_base::out);
    file << "// secnum stands for sector number. It defines total number of sectors in the map.\n";
    file << "secnum " << sectors.size() << "\n\n";
    file << "// wallnum stands for wall number. It defines total number of wals in the map.\n";
    file << "wallnum " << walls.size() << "\n";
    file << "// pstart stands for player start. It defines where player will be spawn in the level.\n";
    file << "// pstart X Y start_sector\n";
    file << "pstart " << "0.0 0.0 0" << "\n";
    file << "\n";

    file << "// s stands for sector. It defines one sector. Format:\n"
            "//s start_wall_index wall_number floor_height ceiling_height\n";
    constexpr float sectorHeightDivider = 1000.0f;
    for(const auto & sec : sectors)
    {
        file << "s "<< sec.startWall << " " << sec.wallNum << " "
             << static_cast<float>(sec.floorHeigth)/sectorHeightDivider << " "
             << static_cast<float>(sec.ceilingHeight)/sectorHeightDivider << "\n";
    }

    file << "\n// w stands for wall. It defines one wall. Format:\n"
            "//w point_x point_y second_wall_point next_wall_index next_sector(in case if portal > -1)\n";
    constexpr float wallPointDivider = 100.0f;
    for(const auto & wll : walls)
    {
        file << "w "<< static_cast<float>(wll.x)/wallPointDivider << " "
             << static_cast<float>(wll.y)/wallPointDivider
             << " " << wll.point2 << " " << wll.nextWall << " " << wll.nextSector << "\n";
    }
    file.close();
}
