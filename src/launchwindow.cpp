#include "launchwindow.h"

#include "app.h"
#include "appfinder.h"
#include "appscan.h"
#include "history.h"
#include "launcher.h"
#include "xdgutils.h"

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/enums.h>
#include <gdkmm/rgba.h>
#include <memory>
#include <string>
#include <vector>

#include <iostream>

using Gtk::Box;
using Gtk::Widget;
using std::string;
using std::unique_ptr;
using std::vector;

namespace quicklaunch
{

    // go through range defined by begin and end and look for matches to query
    // just works on iterators returning smart pointers, but wtf, I wanted to try templates
    template<typename iter>
    vector<Launcher*> get_matching_launchers(iter begin, iter end, string query)
    {
        vector<Launcher*> result;
        while (begin != end)
        {
            if ((*begin)->matches(query))
                result.push_back(begin->get());
            ++begin;
        }
        return result;
    }

    Launch_window::Launch_window() :
        container(Gtk::ORIENTATION_VERTICAL)
    {
        frequency_map history;
        // this will be factored out to some other method eventually
        // if this fails...do nothing, actually
        read_history(util::get_history_file(), history);
        apps = get_all_apps();
        sort_by_frequency(apps, history);

//        // create vector of launchers
//        launchers.reserve(apps.size());
//        for (vector<Launcher*>::size_type i = 0; i < apps.size(); ++i)
//            launchers.push_back( unique_ptr<Launcher>(new Launcher(apps[i])) );

        set_default_size(200, 200);
        add(container);
        container.add(query_entry);

        //Gtk::ScrolledWindow* scroll_pane = Gtk::manage(new Gtk::ScrolledWindow());
        //scroll_pane->add(options);
        //container.add(*scroll_pane);
        container.add(options);


        query_entry.signal_changed().connect(sigc::mem_fun(*this, &Launch_window::modified_query));

        query_entry.signal_activate().connect(sigc::mem_fun(*this, &Launch_window::execute_app));
        options.signal_row_activated().connect(sigc::mem_fun(*this, &Launch_window::row_activated));
        
        show_all_children();
        // what the hell why do i need to do this something is screwed up
        //modified_query();
        //modified_query();

        //displayed_launchers.reserve(16);
    }

    Launch_window::~Launch_window()
    {
        //std::cout << "Launch_window destructing" << '\n';
    }
    void state_changed(Gtk::StateType new_state)
    {
        std::cout << new_state << std::endl;
    }

    void Launch_window::modified_query()
    {
        vector<Widget*> children = options.get_children();
        for (vector<Widget*>::size_type i = 0; i < children.size(); ++i)
        {
            Gtk::Bin* row = static_cast<Gtk::Bin*>(children[i]);
            //row->remove();
            options.remove(*row);
            //delete row;
        }
        // clear list of apps
        displayed_launchers.clear();

        typedef vector<App>::size_type vec_sz;

        string query = query_entry.get_text();
//        displayed_launchers = get_matching_launchers(launchers.cbegin(), launchers.cend(), query);
        vector<App> matching_apps = get_matching_apps(apps.cbegin(), apps.cend(), query);
        vec_sz size = matching_apps.size() > 10 ? 10 : matching_apps.size();
        // rebuild based on new query
        //displayed_launchers.reserve(16);
        for (vec_sz i = 0; i < size; ++i)
        {
            displayed_launchers.push_back(matching_apps[i]);
            std::cout << displayed_launchers.size() << std::endl;
            Widget* widget = displayed_launchers[i].contents();
            Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
            row->add(*widget);
            //row->signal_state_changed().connect(sigc::ptr_fun(&state_changed));;
            //row->override_background_color(awkward_grey, Gtk::STATE_FLAG_FOCUSED);

            options.append(*row);
        }

        show_all_children(true);
    }

    void Launch_window::regen_displayed(const string& query)
    {
    }

    void Launch_window::execute_app()
    {
        if (options.get_children().empty())
            return;
        Gtk::ListBoxRow* row = options.get_selected_row();
        int index = row ? options.get_selected_row()->get_index() : 0;
        record_selection(displayed_launchers[index].app(), util::get_history_file());
        displayed_launchers[index].launch();
        close();
    }

    void Launch_window::row_activated(Gtk::ListBoxRow* row)
    {
        execute_app();
    }

}
