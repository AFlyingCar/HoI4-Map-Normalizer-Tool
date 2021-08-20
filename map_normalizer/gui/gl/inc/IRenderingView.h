#ifndef IRENDERINGVIEW_H
# define IRENDERINGVIEW_H

# include <memory>
# include <vector>

# include "MapData.h"

namespace MapNormalizer::GUI::GL {
    class Program;

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

            virtual void beginRender() = 0;
            virtual void render() = 0;
            virtual void endRender() = 0;

            virtual ProgramList getPrograms() = 0;
    };
}

#endif

