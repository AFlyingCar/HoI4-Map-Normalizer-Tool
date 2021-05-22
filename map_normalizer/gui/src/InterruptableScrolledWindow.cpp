
#include "InterruptableScrolledWindow.h"

auto MapNormalizer::GUI::InterruptableScrolledWindow::signalOnScroll()
    -> SignalOnScroll
{
    return m_signal_on_scroll;
}

bool MapNormalizer::GUI::InterruptableScrolledWindow::on_scroll_event(GdkEventScroll* event)
{
    if(m_signal_on_scroll.emit(event)) {
        return true;
    }

    // Call base on_scroll
    Gtk::ScrolledWindow::on_scroll_event(event);

    return false;
}

