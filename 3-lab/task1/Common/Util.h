#pragma once
#include <fstream>
#include <iostream>
#include <string>

class Util
{
private:
public:
    static bool ReadFile(const char* fileName, std::string& outFile)
    {
        std::ifstream file(fileName);

        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << fileName << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            outFile += line + '\n';
        }
        file.close();
        return true;
    }
};
