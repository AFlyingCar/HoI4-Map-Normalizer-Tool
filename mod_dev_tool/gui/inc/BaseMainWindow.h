#ifndef BASEMAINWINDOW_H
# define BASEMAINWINDOW_H

# include <any>

# include "gtkmm/box.h"
# include "gtkmm/frame.h"
# include "gtkmm/paned.h"
# include "gtkmm/scrolledwindow.h"
# include "gtkmm/notebook.h"
# include "gtkmm/eventbox.h"

# include "BaseActiveWidgetContainer.h"
# include "Window.h"

namespace HMDT::GUI {
    class BaseMainWindow: public Window,
                          public virtual BaseActiveWidgetContainer<Gtk::Box,
                                                                   Gtk::Frame,
                                                                   Gtk::ScrolledWindow,
                                                                   Gtk::Notebook,
                                                                   Gtk::EventBox>
    {
        public:
            using Window::Window;

            /**
             * @brief Enum describing which part of the main window is described
             */
            enum class PartType {
                MAIN,
                DRAWING_AREA,
                PROPERTIES_PANE,
                FILE_TREE
            };

            // NOTE: This is left undefined on purpose. Constructing a
            //   BaseMainWindow _must_ be done through the base Window
            //   non-default constructor. Do not attempt to define this.
            BaseMainWindow();
            virtual ~BaseMainWindow() = default;

        protected:
            virtual void updatePart(const PartType&, const std::any&) noexcept = 0;
    };
}

#endif

