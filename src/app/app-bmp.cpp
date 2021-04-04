#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "mandelbrot.h"
#include "color/color-strategy-iteration.h"
#include "color/color-strategy-smooth.h"
#include "color/color-strategy-wavelength.h"
#include "output/output-device-bmp.h"

using namespace mandelbrot;
using namespace std;

struct Argument
{
    std::string shortName;
    std::string longName;
    std::string description;
    std::string defaultValue;
    std::string *value;
};

void printHelp(const std::string &appName, const std::vector<Argument> &argTable)
{
    cout << "Mandelbrot Image Generator" << endl;
    cout << "Usage: " << appName << " [arguments]" << endl << endl;
    
    cout << "Arguments:" << endl;
    
    const int spaceToDescription = 28;
    for (const Argument &arg : argTable)
    {
        int spacesTaken = static_cast<int>(arg.shortName.size()) + 3;
        
        cout << " -" << arg.shortName;
        if (!arg.longName.empty())
        {
            cout << ",  --" << arg.longName << "=VALUE";
            spacesTaken += 11 + static_cast<int>(arg.longName.size());
        }
        
        if (spaceToDescription - spacesTaken > 0)
        {
            std::string spaceBuffer(spaceToDescription - spacesTaken, ' ');
            cout << spaceBuffer;
        }
        cout << " " << arg.description << endl;
        
        std::string spaceBuffer(spaceToDescription, ' ');
        cout << spaceBuffer << "Default: " << arg.defaultValue << endl << endl;
    }
    cout << " -h, --help                 Display this help message." << endl << endl;
}

std::vector<Argument> &parseArgs(int argc, char **argv, std::vector<Argument> &argTable)
{
    if (argc <= 1)
        return argTable;

    const std::string shortHelpFlag = R"(-h)",
                      longHelpFlag = R"(--help)";
    for (int i = 1; i < argc; ++i)
    {
        std::string argN = argv[i];
        
        if (argN.size() <= 1 || argN[0] != '-')
            continue;
        
        // Check if we need to print the help table and exit the program
        if (argN.compare(shortHelpFlag) == 0 || argN.compare(longHelpFlag) == 0)
        {
            std::string appName = argv[0];
            auto appDelimPos = appName.find_last_of('/');
            if (appDelimPos != std::string::npos)
                appName = appName.substr(appDelimPos + 1);
            
            printHelp(appName, argTable);
            argTable.clear();
            return argTable;
        }

        auto it = std::find_if(argTable.begin(), argTable.end(), [&argN](const Argument &arg) {
            return (argN.compare(1, argN.size() - 1, arg.shortName) == 0
                    || argN.compare(2, std::min(argN.size() - 2, arg.longName.size()), arg.longName) == 0);
        });

        if (it == argTable.end())
            continue;

        std::string argValue;
        auto delimPos = argN.find('=');
        if (delimPos != std::string::npos)
        {
            argValue = argN.substr(delimPos + 1);
        }
        else if (i + 1 < argc)
        {
            argValue = argv[i + 1];
            i++;
        }

        std::string *valuePtr = it->value;
        if (valuePtr != nullptr)
            *valuePtr = argValue.empty() ? it->defaultValue : argValue;
    }

    for (Argument &arg : argTable)
    {
        if (arg.value && arg.value->empty())
            *(arg.value) = arg.defaultValue;
    }

    return argTable;
}

int main(int argc, char **argv)
{
    std::string fileName, cXStr, cYStr, scaleStr, widthStr, heightStr, iterStr, colorStr;

    std::vector<Argument> argTable {
        { R"(f)", R"(filename)", R"(Name of the output file)", R"(mandelbrot.bmp)", &fileName },
        { R"(cx)", R"(centerX)", R"(Center x coordinate on the plane)", R"(-0.637011)", &cXStr },
        { R"(cy)", R"(centerY)", R"(Center y coordinate on the plane)", R"(-0.0395159)", &cYStr },
        { R"(s)", R"(scale)", R"(Magnification level of the fractal plane)", R"(0.00403897)", &scaleStr },
        { R"(x)", R"(width)", R"(Width of the BMP file)", R"(1024)", &widthStr },
        { R"(y)", R"(height)", R"(Height of the BMP file)", R"(768)", &heightStr },
        { R"(i)", R"(iterations)", R"(Maximum number of iterations per calculation)", R"(400)", &iterStr },
        { R"(c)", R"(color)", R"(Color strategy. Valid values: smooth, iter, wave)", R"(smooth)", &colorStr}
    };

    parseArgs(argc, argv, argTable);
    
    // Table is cleared if user passes help flag, so we only want to print the help message
    // and abort
    if (argTable.empty())
        return 0;

    double cX = std::stod(cXStr); 
    double cY = std::stod(cYStr);
    double scale = std::stod(scaleStr);

    int maxIter = std::stoi(iterStr);
    int width = std::stoi(widthStr), height = std::stoi(heightStr);
    
    if (fileName.find(R"(.bmp)") == std::string::npos)
        fileName.append(R"(.bmp)");

    std::unique_ptr<OutputDeviceBMP> bmp = std::make_unique<OutputDeviceBMP>();
    bmp->setFileName(fileName);
    bmp->setDimensions(int32_t{width}, int32_t{height});

    std::unique_ptr<ColorStrategy> colorStrategy;

    if (colorStr.compare(R"(smooth)") == 0)
        colorStrategy = std::make_unique<ColorStrategySmooth>();
    else if (colorStr.compare(R"(iter)") == 0)
        colorStrategy = std::make_unique<ColorStrategyIteration>();
    else if (colorStr.compare(R"(wave)") == 0)
        colorStrategy = std::make_unique<ColorStrategyWavelength>();

    MandelbrotSet mbSet; 
    mbSet.setMaxIterations(maxIter);
    mbSet.setOutputDevice(std::move(bmp));
    mbSet.setOutputDimensions(width, height);
    mbSet.setScale(scale);
    mbSet.setCenter(cX, cY);
    mbSet.setColorStrategy(std::move(colorStrategy));
    mbSet.render();

    return 0;
}
