#pragma once

// Project includes
#include "../State/State.h"

// Third-party includes

// stdlib

namespace Falltergeist
{
    namespace UI
    {
        class ImageList;
    }
    namespace State
    {
        class InventoryDragItem final : public State
        {
            public:
                InventoryDragItem(UI::ImageList* itemUi);
                ~InventoryDragItem() override;

                void init() override;
                void handle(Event::Event* event) override;

            protected:
                UI::ImageList* _itemUi = nullptr;
        };
    }
}
