
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>

class SVGToHexConverter {
private:
    std::string generateVariableName(const std::string& filename) {
        std::string varName = filename;

        // Remove extension
        size_t dotPos = varName.find_last_of('.');
        if (dotPos != std::string::npos) {
            varName = varName.substr(0, dotPos);
        }

        // Replace non-alphanumeric characters with underscore
        for (char& c : varName) {
            if (!std::isalnum(c)) {
                c = '_';
            }
        }

        // Ensure it starts with a letter or underscore
        if (!varName.empty() && std::isdigit(varName[0])) {
            varName = "_" + varName;
        }

        return varName;
    }

public:
    bool convertSVGToHeader(const std::string& svgFilePath, const std::string& outputHeaderPath, const std::string& prefixName) {
        // Read SVG file
        std::ifstream svgFile(svgFilePath, std::ios::binary);
        if (!svgFile) {
            std::cerr << "Error: Cannot open SVG file: " << svgFilePath << std::endl;
            return false;
        }

        // Read entire file into vector
        std::vector<unsigned char> svgData;
        svgFile.seekg(0, std::ios::end);
        size_t fileSize = svgFile.tellg();
        svgFile.seekg(0, std::ios::beg);

        svgData.resize(fileSize);
        svgFile.read(reinterpret_cast<char*>(svgData.data()), fileSize);
        svgFile.close();

        if (svgData.empty()) {
            //std::cerr << "Error: SVG file is empty" << std::endl;
            return false;
        }

        // Use provided prefix name
        std::string varName = generateVariableName(prefixName);

        // Create header file
        std::ofstream headerFile(outputHeaderPath);
        if (!headerFile) {
            //std::cerr << "Error: Cannot create header file: " << outputHeaderPath << std::endl;
            return false;
        }

        // Write header guard
        headerFile << "#pragma once" << std::endl << std::endl;

        // Write size constant (excluding null terminator)
        std::filesystem::path path(svgFilePath);
        std::string filename = path.filename().string();
        headerFile << "// Size of " << filename << " (excluding null terminator)" << std::endl;
        headerFile << "static const unsigned int " << varName << "_size = " << fileSize << ";" << std::endl << std::endl;

        // Write hex array
        headerFile << "// " << filename << " as hex array (null terminated)" << std::endl;
        headerFile << "static const char " << varName << "_data[] = {" << std::endl;

        // Write hex data
        const int bytesPerLine = 16;
        for (size_t i = 0; i < svgData.size(); ++i) {
            if (i % bytesPerLine == 0) {
                headerFile << "    ";
            }

            headerFile << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                << static_cast<unsigned int>(svgData[i]);

            if (i < svgData.size() - 1) {
                headerFile << ", ";
            }

            if ((i + 1) % bytesPerLine == 0 || i == svgData.size() - 1) {
                headerFile << std::endl;
            }
        }

        // Add null terminator (not counted in size)
        headerFile << "    , 0x00  // Null terminator" << std::endl;
        headerFile << "};" << std::endl << std::endl;

        headerFile.close();

        return true;
    }
};

