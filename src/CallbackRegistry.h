#pragma once

#include <mutex>
#include <functional>
#include <unordered_map>
#include <string>
#include <utility>
#include "UIEvent.h"
#include "EventDispatcher.h"

namespace HeartControl
{
    struct SubscribeParams {
        std::string eventType;
        std::function<void(const UIEvent&)> callback;
        int resultSubscriptionId = -1; // This will be filled in by the message handler
    };

    class CallbackRegistry {
    public:
        using EventCallback = std::function<void(const UIEvent&)>;
       
        int registerEvent(UIEvent&& event) {
            std::lock_guard lock(mutex_);
            int id = nextId_++;
            pendingEvents_[id] = std::move(event);
            spdlog::debug("Event registered with ID: {} (type: {})", id, pendingEvents_[id].type);

            return id;
        }

        UIEvent retrieveEvent(int id) {
            std::lock_guard lock(mutex_);
            spdlog::debug("Retrieving event with ID: {}, pendingEvents_ size: {}", id, pendingEvents_.size());
            std::string keys;
            for (const auto& evt : pendingEvents_ | std::views::keys) {
                keys += std::to_string(evt) + " ";
            }
            spdlog::debug("Pending event IDs: {}", keys);

            if (const auto it = pendingEvents_.find(id); it != pendingEvents_.end()) {
                auto result = std::move(it->second);
                pendingEvents_.erase(it);
                spdlog::debug("Event retrieved: {}", result.type);

                return result;
            }
            spdlog::warn("Failed to find event with ID: {}", id);

            return UIEvent{}; // Empty event
        }

        // Modify your CallbackRegistry to store the full params
        int registerSubscribeParams(SubscribeParams* params) {
            std::lock_guard lock(mutex_);
            int id = nextId_++;
            subscribeParams_[id] = params;
            spdlog::debug("Registered params with ID: {}", id);

            return id;
        }

 

        SubscribeParams* retrieveSubscribeParams(const int id) {
            std::lock_guard lock(mutex_);
            if (const auto it = subscribeParams_.find(id); it != subscribeParams_.end()) {
	            const auto result = it->second;
                subscribeParams_.erase(it);
                return result;
            }
            return nullptr;
        }
        int registerCallback(const std::string& eventType, EventCallback&& callback) {
            std::lock_guard lock(mutex_);
            const int id = nextId_++;
            pendingCallbacks_[id] = { eventType, std::move(callback) };
            return id;
        }

        std::pair<std::string, EventCallback> retrieveCallback(const int id) {
            std::lock_guard lock(mutex_);
            if (const auto it = pendingCallbacks_.find(id); it != pendingCallbacks_.end()) {
                auto result = std::move(it->second);
                pendingCallbacks_.erase(it);
                return result;
            }
            return { "", nullptr };
        }

        int registerUnsubscribe(const std::string& eventType, const int subscription_id) {
            std::lock_guard lock(mutex_);
            const int id = nextId_++;
            pendingUnsubscribes_[id] = { eventType, subscription_id };
            return id;
        }

        std::pair<std::string, int> retrieveUnsubscribe(const int id) {
            std::lock_guard lock(mutex_);
            if (const auto it = pendingUnsubscribes_.find(id); it != pendingUnsubscribes_.end()) {
                auto [eventType, subscriptionId] = it->second;
                pendingUnsubscribes_.erase(it);
                return { eventType, subscriptionId };
            }
            return { "", -1 };
        }

        // Add a way to store subscription results
        void storeSubscriptionResult(const int callbackId, const int subscriptionId) 
        {
            std::lock_guard lock(mutex_);
            subscriptionResults_[callbackId] = subscriptionId;
        }

        // Add a way to retrieve subscription results
        int getSubscriptionResult(const int callbackId) 
        {
            std::lock_guard lock(mutex_);
            const auto it = subscriptionResults_.find(callbackId);
            if (it != subscriptionResults_.end()) 
            {
	            const int result = it->second;
                subscriptionResults_.erase(it);
                return result;
            }
            return -1; // Invalid subscription ID
        }

    private:
        std::mutex mutex_;
        std::unordered_map<int, std::pair<std::string, EventCallback>> pendingCallbacks_;
        std::unordered_map<int, UnsubscribeInfo> pendingUnsubscribes_;
        std::unordered_map<int, int> subscriptionResults_; // callbackId -> subscriptionId
        std::unordered_map<int, SubscribeParams*> subscribeParams_; // callbackId -> SubscribeParams*
        std::unordered_map<int, UIEvent> pendingEvents_;

        int nextId_ = 1;
    };
}