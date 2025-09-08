#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <qt6/QtWidgets/QApplication>

#include "mainwindow.h"

void init_logging()
{
#ifdef NDEBUG
        // Release mode: only info and above
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#else
        // Debug mode: all logs
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
#endif
}

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        init_logging();
        MainWindow w;
        w.show();
        return app.exec();
}
