
#include "ContinentProject.h"

#include <fstream>
#include <cstring>

#include "Logger.h"
#include "Constants.h"
#include "StatusCodes.h"

HMDT::Project::ContinentProject::ContinentProject(IMapProject& parent):
    m_parent_project(parent),
    m_continents()
{ }

auto HMDT::Project::ContinentProject::getContinentList() const
    -> const ContinentSet&
{
    return m_continents;
}

/**
 * @brief Writes all continent data to root/$CONTINENTDATA_FILENAME
 *
 * @param root The root where all continent data should go
 * @param ec The error code
 *
 * @return True if continent data was successfully loaded, false otherwise
 */
auto HMDT::Project::ContinentProject::save(const std::filesystem::path& root)
    -> MaybeVoid
{
    auto path = root / CONTINENTDATA_FILENAME;

    // Try to open the continent file for reading.
    if(std::ofstream out(path); out) {
        for(auto&& continent : m_continents) {
            out << continent << '\n';
        }
    } else {
        WRITE_ERROR("Failed to open file ", path, ". Reason: ", std::strerror(errno));
        RETURN_ERROR(std::make_error_code(static_cast<std::errc>(errno)));
    }

    return STATUS_SUCCESS;
}

/**
 * @brief Loads all continent data from a file
 *
 * @param root The root where the continent data file should be found
 * @param ec The error code
 *
 * @return True if data was loaded correctly, false otherwise
 */
auto HMDT::Project::ContinentProject::load(const std::filesystem::path& root)
    -> MaybeVoid
{
    auto path = root / CONTINENTDATA_FILENAME;

    // If the file doesn't exist, then return false (we didn't actually load it
    //  after all), but don't set the error code as it is expected that the
    //  file may not exist
    if(std::error_code ec; !std::filesystem::exists(path, ec)) {
        RETURN_ERROR_IF(ec.value() != 0, ec);

        WRITE_WARN("No data to load! No continents currently exist!");
        return std::make_error_code(std::errc::no_such_file_or_directory);
    }

    if(std::ifstream in(path); in) {
        std::string line;
        while(std::getline(in, line)) {
            if(line.empty()) continue;

            m_continents.insert(line);
        }
    } else {
        WRITE_ERROR("Failed to open file ", path);
        RETURN_ERROR(std::make_error_code(static_cast<std::errc>(errno)));
    }

    return STATUS_SUCCESS;
}

auto HMDT::Project::ContinentProject::getRootParent() -> IProject& {
    return m_parent_project.getRootParent();
}

auto HMDT::Project::ContinentProject::getMapData() -> std::shared_ptr<MapData> {
    return m_parent_project.getMapData();
}

auto HMDT::Project::ContinentProject::getMapData() const 
    -> const std::shared_ptr<MapData>
{
    return m_parent_project.getMapData();
}

void HMDT::Project::ContinentProject::import(const ShapeFinder&, std::shared_ptr<MapData>) { }

bool HMDT::Project::ContinentProject::validateData() {
    // We have nothing to really validate here
    return true;
}

auto HMDT::Project::ContinentProject::getRootMapParent() -> IMapProject& {
    return m_parent_project.getRootMapParent();
}

auto HMDT::Project::ContinentProject::getContinents() -> ContinentSet& {
    return m_continents;
}
