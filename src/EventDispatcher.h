#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "UIEvent.h"
#include <nlohmann/json.hpp>

#include "spdlog.h"
using json = nlohmann::json;

namespace HeartControl
{
    struct SubscriptionInfo
    {
        int id = 0;
        std::function<void(const UIEvent&)> handler;

        void operator()(const UIEvent& event) const {
            handler(event);
        }
    };

    struct UnsubscribeInfo
    {
        std::string eventType;
        int subscriptionId = -1;
    };

    class EventDispatcher
    {
    public:
        using EventHandler = std::function<void(const UIEvent&)>;

        template <typename Handler>
        int subscribe(const std::string& eventType, Handler&& handler)
        {
            std::lock_guard lock(mutex_);
            int subscriptionId = nextSubscriptionId_++;
            handlers_[eventType].emplace_back(SubscriptionInfo{
                .id = subscriptionId,
                .handler = std::forward<Handler>(handler)
                });
            return subscriptionId;
        }

        bool unsubscribe(const std::string& eventType, const int subscriptionId)
        {
            std::lock_guard lock(mutex_);
            const auto it = handlers_.find(eventType);
            if (it != handlers_.end()) {
                auto& handlerList = it->second;
                for (auto handlerIt = handlerList.begin(); handlerIt != handlerList.end(); ++handlerIt) {
                    if (handlerIt->id == subscriptionId) {
                        handlerList.erase(handlerIt);
                        return true;
                    }
                }
            }
            return false;
        }

        void dispatch(const UIEvent& event)
        {
            spdlog::info("Dispatching event type: '{}'", event.type);

            std::vector<SubscriptionInfo> handlersToCall;
            {
                std::lock_guard lock(mutex_);
                if (const auto it = handlers_.find(event.type); it != handlers_.end()) {
                    handlersToCall = it->second;
                }
            }

            for (const auto& handler : handlersToCall) {
                handler(event);
            }
        }

    private:
        std::mutex mutex_;
        std::unordered_map<std::string, std::vector<SubscriptionInfo>> handlers_;
        int nextSubscriptionId_ = 1;
    };

}