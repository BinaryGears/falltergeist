#pragma once

// Project includes
#include "../State/State.h"
#include "../UI/IResourceManager.h"

// Third-party includes

// stdlib
#include <string>

namespace Falltergeist
{
    namespace UI
    {
        namespace Factory
        {
            class ImageButtonFactory;
        }
    }
    namespace State
    {
        class PlayerEditAlert final : public State
        {
            public:
                PlayerEditAlert(std::shared_ptr<UI::IResourceManager> resourceManager);
                virtual ~PlayerEditAlert() = default;

                void setMessage(const std::string& message);

                void init() override;

                void onDoneButtonClick(Event::Mouse* event);

            protected:
                std::string _message;

            private:
                std::shared_ptr<UI::IResourceManager> resourceManager;
                std::unique_ptr<UI::Factory::ImageButtonFactory> imageButtonFactory;
        };
    }
}
