//    Copyright 2022 Harshavadan Deshpande
//                   Christoph Hellmann Santos
// 
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include <memory>
#include <vector>
#include <rclcpp/executors.hpp>
#include <rclcpp_components/component_manager.hpp>
#include "ros2_canopen_core/master_node.hpp"

#include "device.hpp"

class DeviceManager : public rclcpp_components::ComponentManager {
public:
    DeviceManager(
        std::weak_ptr<rclcpp::Executor> executor =
        std::weak_ptr<rclcpp::executors::MultiThreadedExecutor>(),
        std::string node_name = "device_manager",
        const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions()
            .start_parameter_services(false)
            .start_parameter_event_publisher(false)) :
            rclcpp_components::ComponentManager(executor, node_name, node_options) {
        
        executor_ = executor;
        this->declare_parameter<std::string>("can_interface_name", "");
        this->declare_parameter<std::string>("master_config", "");
        this->declare_parameter<std::string>("bus_config", "");
        this->declare_parameter<std::string>("master_bin", "");
        this->declare_parameter<bool>("enable_lazy_loading", true);   
    }

    bool init();

    RCLCPP_COMPONENTS_PUBLIC
    virtual void
    on_load_node(
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<LoadNode::Request> request,
        std::shared_ptr<LoadNode::Response> response);

    [[deprecated("Use on_load_node() instead")]]
    RCLCPP_COMPONENTS_PUBLIC
    virtual void
    OnLoadNode(
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<LoadNode::Request> request,
        std::shared_ptr<LoadNode::Response> response) override
    {
        on_load_node(request_header, request, response);
    }

    RCLCPP_COMPONENTS_PUBLIC
    virtual void
    on_unload_node(
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<UnloadNode::Request> request,
        std::shared_ptr<UnloadNode::Response> response);

    [[deprecated("Use on_unload_node() instead")]]
    RCLCPP_COMPONENTS_PUBLIC
    virtual void
    OnUnloadNode(
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<UnloadNode::Request> request,
        std::shared_ptr<UnloadNode::Response> response) override
    {
        on_unload_node(request_header, request, response);
    }

    RCLCPP_COMPONENTS_PUBLIC
    virtual void
    on_list_nodes(
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<ListNodes::Request> request,
        std::shared_ptr<ListNodes::Response> response);

    [[deprecated("Use on_list_nodes() instead")]]
    RCLCPP_COMPONENTS_PUBLIC
    virtual void
    OnListNodes(
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<ListNodes::Request> request,
        std::shared_ptr<ListNodes::Response> response) override
    {
        on_list_nodes(request_header, request, response);
    }


private:
    std::map<uint32_t, std::shared_ptr<ros2_canopen::DriverInterface>> drivers_;
    std::map<std::string, std::pair<uint32_t, std::string>> registered_drivers_;
    std::map<std::string, std::pair<uint32_t, std::string>> active_drivers_;
    std::shared_ptr<ros2_canopen::MasterNode> can_master_;
    std::shared_ptr<ev::Executor> exec_;
    std::weak_ptr<rclcpp::Executor> executor_;
    std::map<uint32_t, rclcpp_components::NodeInstanceWrapper> node_wrappers_;

    void set_executor(const std::weak_ptr<rclcpp::Executor> executor);
    void add_node_to_executor(const std::string &plugin_name, const uint32_t node_id, const std::string &node_name);
    void remove_node_from_executor(const std::string &plugin_name, const uint32_t node_id, const std::string &node_name);

    bool load_component(const std::string& pkg_name, const std::string& plugin_name, uint32_t node_id, std::string& node_name);
    std::map<uint32_t, std::string> list_components();

    bool init_devices_from_config(std::string& dcf_txt, std::string& bus_config, std::string &dcf_bin, std::string &can_interface_name);
};
