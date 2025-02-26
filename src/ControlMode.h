#pragma once
#include "EventDispatcher.h"

namespace HeartControl {

    class ControlMode
    {
    public:
        void initialize(EventDispatcher& dispatcher) const
        {
            dispatcher.subscribe("manual-or-auto-mode", [this](const UIEvent& event)
                {
                    processModeSwitch(event.payload);
                });
        }

    private:
        static void processModeSwitch(const nlohmann::json& orderData)
        {
            // Business logic here

        }
    };
}