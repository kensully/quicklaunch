#include <gtkmm.h>
#include <iostream>
#include <fstream>

#include "app.h"
#include "appscan.h"

using namespace quicklaunch;

int main(int argc, char *argv[])
{
    //    Glib::RefPtr<Gtk::Application> app =
    //        Gtk::Application::create(argc, argv,
    //                "org.gtkmm.examples.base");
    //    Gtk::Window window;
    //    window.set_default_size(200, 200);
    //
    //    return app->run(window);
    //    std::ifstream in;
    //    in.open("/usr/share/applications/abiword.desktop");
    //    if (!in)
    //    {
    //        std::cerr << "Error opening file" << std::endl;
    //        std::exit(1);
    //    }
    //    const App& test = createFromFile(in);
    //    in.close();
    //    std::cout << test.description << std::endl;
    std::vector<App> apps = get_all_apps();
    std::cout << apps.size() << std::endl;
}
