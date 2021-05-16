#ifndef MAPPROJECT_H
# define MAPPROJECT_H

# include <set>
# include <string>
# include <filesystem>

# include "ShapeFinder2.h"

# include "Types.h"
# include "BitMap.h"

# include "Terrain.h"

# include "IProject.h"

namespace MapNormalizer::Project {
    /**
     * @brief Defines a map project for HoI4
     */
    class MapProject: public IProject {
        public:
            using ProvinceDataPtr = std::shared_ptr<unsigned char[]>;

            MapProject(IProject&);
            virtual ~MapProject();

            virtual bool save(const std::filesystem::path&,
                              std::error_code& = last_error) override;
            virtual bool load(const std::filesystem::path&,
                              std::error_code& = last_error) override;

            void setShapeFinder(ShapeFinder&&);
            void setGraphicsData(unsigned char*);
            void setImage(BitMap*);

            BitMap* getImage();
            const BitMap* getImage() const;

            unsigned char* getGraphicsData();
            const unsigned char* getGraphicsData() const;

            const uint32_t* getLabelMatrix() const;

            void selectProvince(uint32_t);

            OptionalReference<const Province> getSelectedProvince() const;
            OptionalReference<Province> getSelectedProvince();

            const std::set<std::string>& getContinentList() const;

            void addNewContinent(const std::string&);
            void removeContinent(const std::string&);

            const std::vector<Terrain>& getTerrains() const;

            ProvinceDataPtr getPreviewData(ProvinceID);
            ProvinceDataPtr getPreviewData(const Province*);

        protected:
            bool saveShapeLabels(const std::filesystem::path&,
                                 std::error_code&);
            bool saveProvinceData(const std::filesystem::path&,
                                 std::error_code&);
            bool saveContinentData(const std::filesystem::path&,
                                   std::error_code&);

            bool loadShapeLabels(const std::filesystem::path&,
                                 std::error_code&);
            bool loadProvinceData(const std::filesystem::path&,
                                 std::error_code&);
            bool loadContinentData(const std::filesystem::path&,
                                   std::error_code&);

        private:
            void buildProvinceCache(const Province*);

            /**
             * @brief A struct which holds information about shape detection
             */
            struct ShapeDetectionInfo {
                ProvinceList provinces;
                BitMap* image = nullptr;
                uint32_t* label_matrix = nullptr;
                uint32_t label_matrix_size = 0;

                // TODO: This should really be a smart pointer
                unsigned char* graphics_data = nullptr;
            } m_shape_detection_info;

            std::unordered_map<ProvinceID, ProvinceDataPtr> m_data_cache;

            std::set<std::string> m_continents;
            std::vector<Terrain> m_terrains;

            uint32_t m_selected_province;

            //! The parent project that this MapProject belongs to
            IProject& m_parent_project;
    };
}

#endif

