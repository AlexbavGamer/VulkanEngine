#include "settings.h"
#include "app-window.h"

void Windows::open_settings() {
    auto window = SettingsWindow::create();
    window->show();
}