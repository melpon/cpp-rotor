#pragma once

//
// Copyright (c) 2020 Ivan Baidakou (basiliscos) (the dot dmol at gmail dot com)
//
// Distributed under the MIT Software License
//

#include "plugin.h"
#include <string>

namespace rotor::internal {

struct local_delivery_t {

    /** \brief delivers an message for self of one of child-actors  (non-supervisors)
     *
     * Supervisor iterates on subscriptions (handlers) on the message destination adddress:
     *
     * -# If the handler is local (i.e. it's actor belongs to the same supervisor),
     * -# Otherwise the message is forwarded for delivery for the foreign supervisor,
     * which owns the handler.
     *
     */

    static void delivery(message_ptr_t& message, const subscription_t::joint_handlers_t& local_recipients) noexcept;
};

struct inspected_local_delivery_t {
    static std::string identify(message_base_t* message) noexcept;
    static void delivery(message_ptr_t& message, const subscription_t::joint_handlers_t& local_recipients) noexcept;
};

#ifdef NDEBUG
using default_local_delivery_t = local_delivery_t;
#else
using default_local_delivery_t = inspected_local_delivery_t;
#endif


struct delivery_plugin_base_t : public plugin_t {
    using plugin_t::plugin_t;

    ~delivery_plugin_base_t();
    virtual void process() noexcept = 0;
    void activate(actor_base_t* actor) noexcept override;

    messages_queue_t* queue = nullptr;
    address_t* address  = nullptr;

    /* \brief local and external subscriptions for the addresses generated by the supervisor
     *
     * key: address, value: {@link subscription_t}
     *
     */
    subscription_t* subscription_map;
};

template<typename LocalDelivery = local_delivery_t>
struct delivery_plugin_t: public delivery_plugin_base_t  {
    using delivery_plugin_base_t::delivery_plugin_base_t;

    static const void* class_identity;
    const void* identity() const noexcept override { return class_identity;}
    void process() noexcept override;
};

template<typename LocalDelivery>
const void* delivery_plugin_t<LocalDelivery>::class_identity = static_cast<const void *>(typeid(local_delivery_t).name());





}
