
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <thread>

#include "BitMap.h" // BitMap
#include "Types.h" // Point, Color, Polygon, Pixel
#include "Logger.h" // writeError
#include "ShapeFinder.h" // findAllShapes
#include "GraphicalDebugger.h" // graphicsWorker
#include "ProvinceMapBuilder.h"
#include "Util.h"

#include "ArgParser.h"

int main(int argc, char** argv) {
    using namespace std::string_literals;

    MapNormalizer::ProgramOptions prog_opts = MapNormalizer::parseArgs(argc, argv);

    switch(prog_opts.status) {
        case 1:
            return 1;
        case 2:
            return 0;
        case 0:
        default:
            break;
    }

    MapNormalizer::setInfoLine("Reading in .BMP file.");

    MapNormalizer::BitMap* image = MapNormalizer::readBMP(prog_opts.infilename);

    if(image == nullptr) {
        MapNormalizer::writeError("Reading bitmap failed.");
        return 1;
    }

    MapNormalizer::writeDebug("BitMap = {", false);
    MapNormalizer::writeDebug("    Header = {", false);
    MapNormalizer::writeDebug("        filetype = "s + std::to_string(image->file_header.filetype), false);
    MapNormalizer::writeDebug("        fileSize = "s + std::to_string(image->file_header.fileSize), false);
    MapNormalizer::writeDebug("        reserved1 = "s + std::to_string(image->file_header.reserved1), false);
    MapNormalizer::writeDebug("        reserved2 = "s + std::to_string(image->file_header.reserved2), false);
    MapNormalizer::writeDebug("        bitmapOffset = "s + std::to_string(image->file_header.bitmapOffset), false);
    MapNormalizer::writeDebug("    }", false);
    MapNormalizer::writeDebug("    headerSize = "s + std::to_string(image->info_header.headerSize), false);
    MapNormalizer::writeDebug("    width = "s + std::to_string(image->info_header.width), false);
    MapNormalizer::writeDebug("    height = "s + std::to_string(image->info_header.height), false);
    MapNormalizer::writeDebug("    bitPlanes = "s + std::to_string(image->info_header.bitPlanes), false);
    MapNormalizer::writeDebug("    bitsPerPixel = "s + std::to_string(image->info_header.bitsPerPixel), false);
    MapNormalizer::writeDebug("    compression = "s + std::to_string(image->info_header.compression), false);
    MapNormalizer::writeDebug("    sizeOfBitmap = "s + std::to_string(image->info_header.sizeOfBitmap), false);
    MapNormalizer::writeDebug("    horzResolution = "s + std::to_string(image->info_header.horzResolution), false);
    MapNormalizer::writeDebug("    vertResolution = "s + std::to_string(image->info_header.vertResolution), false);
    MapNormalizer::writeDebug("    colorsUsed = "s + std::to_string(image->info_header.colorsUsed), false);
    MapNormalizer::writeDebug("    colorImportant = "s + std::to_string(image->info_header.colorImportant), false);
    MapNormalizer::writeDebug("    data = { ... }", false);
    MapNormalizer::writeDebug("}", false);

    unsigned char* graphics_data = nullptr;

    bool done = false;
    graphics_data = new unsigned char[image->info_header.width * image->info_header.height * 3];

#ifdef ENABLE_GRAPHICS
    MapNormalizer::writeDebug("Graphical debugger enabled.");
    std::thread graphics_thread([&image, &done, &graphics_data]() {
            MapNormalizer::graphicsWorker(image, graphics_data, done);
    });
#endif

    MapNormalizer::setInfoLine("Finding all possible shapes.");
    auto shapes = MapNormalizer::findAllShapes(image, graphics_data);

    MapNormalizer::setInfoLine("");
    MapNormalizer::writeStdout("Detected "s + std::to_string(shapes.size()) + " shapes.");

#if 0
    for(size_t i = 0; i < shapes.size(); ++i) {
        auto color = shapes.at(i).color;
        auto c = (color.r << 16) | (color.g << 8) | color.b;

        std::stringstream ss;
        ss << std::dec << i << ": " << shapes.at(i).pixels.size();
        ss << " pixels, color = 0x" << std::hex << c;

        MapNormalizer::writeDebug(ss.str());
    }
#endif

    if(!MapNormalizer::problematic_pixels.empty())
        MapNormalizer::writeWarning("The following "s +
                                    std::to_string(MapNormalizer::problematic_pixels.size()) +
                                    " pixels had problems. This could be a bug "
                                    "with the program, or a problem with y our "
                                    "input file. Please check these pixels in "
                                    "your input in case of any problems.");
    for(auto& problem_pixel : MapNormalizer::problematic_pixels) {
        std::stringstream ss;
        ss << "\t{\n"
           << "\t\t(" << problem_pixel.point.x << ',' << problem_pixel.point.y << ")\n"
           << "\t\t0x" << std::hex << colorToRGB(problem_pixel.color) << "\n"
           << "\t}\n";
        MapNormalizer::writeWarning(ss.str(), false);
    }

    MapNormalizer::setInfoLine("Creating Provinces List.");
    auto provinces = MapNormalizer::createProvinceList(shapes);
    std::filesystem::path output_path(prog_opts.outpath);
    std::ofstream output_csv(output_path / "definition.csv");

    // std::cout << "Provinces CSV:\n"
    //              "=============="
    //           << std::endl;
    for(size_t i = 0; i < provinces.size(); ++i) {
        // std::cout << std::dec << provinces[i] << std::endl;
        output_csv << std::dec << provinces[i] << std::endl;
    }

    MapNormalizer::setInfoLine("Writing province bitmap to file...");
    MapNormalizer::writeBMP(output_path / "provinces.bmp", graphics_data,
                            image->info_header.width, image->info_header.height);

    MapNormalizer::setInfoLine("Press any key to exit.");

    std::getchar();
    done = true;

#ifdef ENABLE_GRAPHICS
    MapNormalizer::setInfoLine("Waiting for graphical debugger thread to join...");
    graphics_thread.join();
#endif

    // One last newline so that the command line isn't horrible at the end of
    //   all this
    std::cout << std::endl;

    // Note: no need to free graphics_data, since we are exiting anyway and the
    //   OS will clean it up for us.

    return 0;
}

