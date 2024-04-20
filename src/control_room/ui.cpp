#include "ui.hpp"

#include <ftxui/component/component.hpp>

using namespace ftxui;

// Function Declarations
Element CreateDataLayout();
Element CreateDataTable();
Element CreateGraphLayout();
Element CreateControlsLayout();

Component CreateMainLayout()
{
    return Renderer(
        []
        {
            Element data_layout = CreateDataLayout();
            return vbox({
                       hbox({
                           hbox({
                               text("Rocket Engine Control Room") | center | flex_grow,
                           }) | flex,
                           hbox({text("RPI Spaceflight Society") | center | flex_grow}) | flex,
                       }),
                       separator(),
                       CreateDataLayout() | flex_grow,
                       separator(),
                       CreateControlsLayout(),
                   }) |
                   border;
        });
}
Element CreateDataLayout()
{
    return vbox({
        hbox({
            CreateDataTable() | flex_grow,
            separator(),
            CreateGraphLayout() | flex_grow,
        }) | flex_grow,
    });
}

Element CreateDataTable()
{
    return vbox({
        hbox({
            text("Data Table Placeholder") | center | flex,
        }),
    });
}

Element CreateGraphLayout()
{
    return vbox({
        hbox({
            text("Graph Placeholder") | center | flex,
        }),
    });
}
Element CreateControlsLayout()
{
    return hbox({
        text("10:00") | center | flex,
    });
}