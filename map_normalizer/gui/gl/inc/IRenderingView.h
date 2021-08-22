#ifndef IRENDERINGVIEW_H
# define IRENDERINGVIEW_H

# include <memory>
# include <vector>
# include <optional>

# include "MapData.h"
# include "IMapDrawingArea.h" // SelectionInfo

namespace MapNormalizer::GUI::GL {
    class Program;
    class MapDrawingArea;

    /**
     * @brief Base interface for implementing a rendering view
     */
    class IRenderingView {
        public:
            using ProgramList = std::vector<std::reference_wrapper<Program>>;

            IRenderingView() = default;
            virtual ~IRenderingView() = default;
            IRenderingView(IRenderingView&&) = default;

            IRenderingView(const IRenderingView&) = delete;
            IRenderingView& operator=(const IRenderingView&) = delete;

            virtual void init() = 0;

            virtual void onMapDataChanged(std::shared_ptr<const MapData>) = 0;
            virtual void onSelectionChanged(std::optional<IMapDrawingAreaBase::SelectionInfo>) { };

            virtual void beginRender() = 0;
            virtual void render() = 0;
            virtual void endRender() = 0;

            virtual ProgramList getPrograms() = 0;

        protected:
            friend class MapDrawingArea;

            const MapDrawingArea* getOwningGLDrawingArea() const {
                return m_owning_gl_drawing_area;
            }

        private:
            const MapDrawingArea* m_owning_gl_drawing_area;
    };
}

#endif

