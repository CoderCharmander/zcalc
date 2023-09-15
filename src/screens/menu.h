#pragma once
#include "../screen.h"
#include <optional>

namespace screens::menu {

void enter();
void update(u8g2_t *u8g2);

using menu_action_param = uint8_t;

using menu_action = void (*)(menu_action_param);

struct menu_item {
    const char *title;
    bool submenu;
    menu_action action;
    menu_action_param action_param;
};

struct menu {
    menu_item *items;
    size_t size;
    std::optional<menu*> super_menu;

    explicit menu(std::vector<menu_item> vec) : items(vec.data()), size(vec.size()) {}
    explicit menu(menu_item *items, size_t size) : items(items), size(size) {}
};

extern menu main_menu;

void set_menu(menu &menu);
void set_submenu(menu &menu);

} // namespace screens::menu