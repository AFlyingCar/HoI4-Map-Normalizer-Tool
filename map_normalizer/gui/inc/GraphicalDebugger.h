/**
 * @file GraphicalDebugger.h
 *
 * @brief Defines various functions for the graphical debugger.
 */

#ifndef GRAPHICALDEBUGGER_H
# define GRAPHICALDEBUGGER_H

# include <functional>

# include "BitMap.h"
# include "Types.h"

namespace MapNormalizer {
    void writeDebugColor(uint32_t, uint32_t, Color);

    class GraphicsWorker {
        public:
            using UpdateCallback = std::function<void(const Rectangle&)>;

            static GraphicsWorker& getInstance();

            void init(const BitMap*, unsigned char*);

            void resetDebugData();
            void resetDebugDataAt(const Point2D&);

            void writeDebugColor(uint32_t, uint32_t, Color);

            const unsigned char* getDebugData() const;
            const BitMap* getImage() const;

            void updateCallback(const Rectangle&);

            const UpdateCallback& getWriteCallback() const;
            void setWriteCallback(const UpdateCallback&);
            void resetWriteCallback();

        private:
            GraphicsWorker() = default;

            const BitMap* m_image = nullptr;
            unsigned char* m_debug_data = nullptr;

            UpdateCallback m_write_callback = [](auto) { };
    };

    void checkForPause();
}

#endif

