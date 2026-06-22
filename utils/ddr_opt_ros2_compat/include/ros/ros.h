#ifndef DDR_OPT_ROS2_COMPAT_ROS_H_
#define DDR_OPT_ROS2_COMPAT_ROS_H_

#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include <builtin_interfaces/msg/time.hpp>
#include <rclcpp/rclcpp.hpp>

namespace ros
{
class Duration;

class Time
{
public:
  Time() = default;
  explicit Time(double seconds)
  : time_(
      static_cast<int64_t>(seconds * 1000000000.0),
      RCL_ROS_TIME) {}
  explicit Time(const rclcpp::Time &time) : time_(time) {}
  Time(const builtin_interfaces::msg::Time &time) : time_(time) {}

  static Time now();

  double toSec() const { return time_.seconds(); }
  builtin_interfaces::msg::Time toMsg() const { return time_; }
  operator rclcpp::Time() const { return time_; }
  operator builtin_interfaces::msg::Time() const { return time_; }

private:
  rclcpp::Time time_{0, 0, RCL_ROS_TIME};
};

class Duration
{
public:
  Duration() = default;
  explicit Duration(double seconds)
  : duration_(std::chrono::nanoseconds(static_cast<int64_t>(seconds * 1e9))) {}
  explicit Duration(const rclcpp::Duration &duration) : duration_(duration) {}

  double toSec() const { return duration_.seconds(); }
  void sleep() const { std::this_thread::sleep_for(duration_.to_chrono<std::chrono::nanoseconds>()); }
  operator rclcpp::Duration() const { return duration_; }
  operator builtin_interfaces::msg::Duration() const
  {
    const auto nanoseconds = duration_.nanoseconds();
    builtin_interfaces::msg::Duration msg;
    msg.sec = static_cast<int32_t>(nanoseconds / 1000000000LL);
    msg.nanosec = static_cast<uint32_t>(nanoseconds % 1000000000LL);
    return msg;
  }

private:
  rclcpp::Duration duration_{0, 0};
};

inline Duration operator-(const Time &lhs, const Time &rhs)
{
  return Duration(static_cast<rclcpp::Time>(lhs) - static_cast<rclcpp::Time>(rhs));
}

struct TimerEvent {};

namespace detail
{
inline std::string & node_name_storage()
{
  static std::string name{"ddr_opt_node"};
  return name;
}

inline rclcpp::Node::SharedPtr & node_storage()
{
  static rclcpp::Node::SharedPtr node;
  return node;
}

inline std::string normalize_param_name(std::string name)
{
  if (name.rfind("~/", 0) == 0) {
    name.erase(0, 2);
  }
  if (!name.empty() && name.front() == '/') {
    name.erase(0, 1);
  }

  auto node_name = node_storage() ? node_storage()->get_name() : node_name_storage();
  if (name.rfind(node_name + "/", 0) == 0) {
    name.erase(0, node_name.size() + 1);
  }
  std::replace(name.begin(), name.end(), '/', '.');
  return name;
}

inline rclcpp::Node::SharedPtr ensure_node()
{
  auto & node = node_storage();
  if (!node) {
    rclcpp::NodeOptions options;
    options.allow_undeclared_parameters(true);
    options.automatically_declare_parameters_from_overrides(true);
    node = std::make_shared<rclcpp::Node>(node_name_storage(), options);
  }
  return node;
}

template <typename T>
rclcpp::ParameterValue to_parameter_value(const T & value)
{
  return rclcpp::ParameterValue(value);
}

template <typename T>
bool get_parameter_value(const rclcpp::Parameter & parameter, T & value)
{
  if constexpr (std::is_same_v<T, double>) {
    if (parameter.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
      value = static_cast<double>(parameter.as_int());
      return true;
    }
  } else if constexpr (std::is_same_v<T, int>) {
    if (parameter.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
      value = static_cast<int>(parameter.as_int());
      return true;
    }
  } else if constexpr (std::is_same_v<T, std::vector<double>>) {
    if (parameter.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER_ARRAY) {
      const auto integers = parameter.as_integer_array();
      value.clear();
      value.reserve(integers.size());
      for (const auto integer : integers) {
        value.push_back(static_cast<double>(integer));
      }
      return true;
    }
  } else if constexpr (std::is_same_v<T, std::vector<int>>) {
    if (parameter.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER_ARRAY) {
      const auto integers = parameter.as_integer_array();
      value.clear();
      value.reserve(integers.size());
      for (const auto integer : integers) {
        value.push_back(static_cast<int>(integer));
      }
      return true;
    }
  }

  try {
    value = parameter.get_value<T>();
    return true;
  } catch (const rclcpp::ParameterTypeException &) {
    return false;
  }
}
}  // namespace detail

inline void init(int argc, char ** argv, const std::string & name)
{
  if (!rclcpp::ok()) {
    rclcpp::init(argc, argv);
  }
  detail::node_name_storage() = name;
  rclcpp::NodeOptions options;
  options.allow_undeclared_parameters(true);
  options.automatically_declare_parameters_from_overrides(true);
  detail::node_storage() = std::make_shared<rclcpp::Node>(name, options);
}

inline void spin()
{
  rclcpp::spin(detail::ensure_node());
}

inline bool ok()
{
  return rclcpp::ok();
}

inline void shutdown()
{
  rclcpp::shutdown();
}

inline Time Time::now()
{
  return Time(detail::ensure_node()->now());
}

namespace this_node
{
inline std::string getName()
{
  return "/" + std::string(detail::ensure_node()->get_name());
}
}  // namespace this_node

class Publisher
{
public:
  Publisher() = default;

  template <typename MessageT>
  static Publisher make(typename rclcpp::Publisher<MessageT>::SharedPtr publisher)
  {
    Publisher wrapper;
    wrapper.publisher_ = std::move(publisher);
    wrapper.publish_fn_ = [](const std::shared_ptr<void> & pub, const void * msg) {
      std::static_pointer_cast<rclcpp::Publisher<MessageT>>(pub)->publish(
        *static_cast<const MessageT *>(msg));
    };
    return wrapper;
  }

  template <typename MessageT>
  void publish(const MessageT & msg) const
  {
    if (publish_fn_) {
      publish_fn_(publisher_, &msg);
    }
  }

  explicit operator bool() const { return static_cast<bool>(publisher_); }

private:
  std::shared_ptr<void> publisher_;
  std::function<void(const std::shared_ptr<void> &, const void *)> publish_fn_;
};

class Subscriber
{
public:
  Subscriber() = default;

  explicit Subscriber(std::shared_ptr<void> subscription)
  : subscription_(std::move(subscription)) {}
  Subscriber(std::shared_ptr<void> subscription, std::string topic)
  : subscription_(std::move(subscription)), topic_(std::move(topic)) {}

  void shutdown() { subscription_.reset(); }
  std::string getTopic() const { return topic_; }

private:
  std::shared_ptr<void> subscription_;
  std::string topic_;
};

class Timer
{
public:
  Timer() = default;
  explicit Timer(rclcpp::TimerBase::SharedPtr timer) : timer_(std::move(timer)) {}

private:
  rclcpp::TimerBase::SharedPtr timer_;
};

struct TransportHints
{
  TransportHints & unreliable() { return *this; }
};

class NodeHandle
{
public:
  NodeHandle() : node_(detail::ensure_node()) {}
  explicit NodeHandle(const std::string & /*ns*/) : node_(detail::ensure_node()) {}

  template <typename MessageT>
  Publisher advertise(const std::string & topic, size_t queue_size)
  {
    return Publisher::make<MessageT>(node_->create_publisher<MessageT>(topic, rclcpp::QoS(queue_size)));
  }

  template <typename MessageT, typename ObjT>
  Subscriber subscribe(
    const std::string & topic,
    size_t queue_size,
    void (ObjT::* callback)(typename MessageT::ConstSharedPtr),
    ObjT * object)
  {
    auto sub = node_->create_subscription<MessageT>(
      topic, rclcpp::QoS(queue_size),
      [object, callback](typename MessageT::ConstSharedPtr msg) {
        (object->*callback)(msg);
      });
    return Subscriber(sub, topic);
  }

  template <typename MessageT, typename ObjT>
  Subscriber subscribe(
    const std::string & topic,
    size_t queue_size,
    void (ObjT::* callback)(const typename MessageT::ConstSharedPtr &),
    ObjT * object)
  {
    auto sub = node_->create_subscription<MessageT>(
      topic, rclcpp::QoS(queue_size),
      [object, callback](typename MessageT::ConstSharedPtr msg) {
        (object->*callback)(msg);
      });
    return Subscriber(sub, topic);
  }

  template <typename MessageT, typename ObjT>
  Subscriber subscribe(
    const std::string & topic,
    size_t queue_size,
    void (ObjT::* callback)(typename MessageT::ConstSharedPtr),
    ObjT * object,
    const TransportHints &)
  {
    return subscribe<MessageT>(topic, queue_size, callback, object);
  }

  template <typename MessageT, typename ObjT>
  Subscriber subscribe(
    const std::string & topic,
    size_t queue_size,
    void (ObjT::* callback)(const typename MessageT::ConstSharedPtr &),
    ObjT * object,
    const TransportHints &)
  {
    return subscribe<MessageT>(topic, queue_size, callback, object);
  }

  template <typename MessageT>
  Subscriber subscribe(
    const std::string & topic,
    size_t queue_size,
    void (* callback)(const typename MessageT::ConstSharedPtr &))
  {
    auto sub = node_->create_subscription<MessageT>(
      topic, rclcpp::QoS(queue_size),
      [callback](typename MessageT::ConstSharedPtr msg) { callback(msg); });
    return Subscriber(sub, topic);
  }

  template <typename MessageT>
  Subscriber subscribe(
    const std::string & topic,
    size_t queue_size,
    void (* callback)(typename MessageT::ConstSharedPtr))
  {
    auto sub = node_->create_subscription<MessageT>(
      topic, rclcpp::QoS(queue_size),
      [callback](typename MessageT::ConstSharedPtr msg) { callback(msg); });
    return Subscriber(sub, topic);
  }

  template <typename ObjT>
  Timer createTimer(const Duration & duration, void (ObjT::* callback)(const TimerEvent &), ObjT * object)
  {
    auto timer = node_->create_wall_timer(
      duration.operator rclcpp::Duration().to_chrono<std::chrono::nanoseconds>(),
      [object, callback]() {
        TimerEvent event;
        (object->*callback)(event);
      });
    return Timer(timer);
  }

  Timer createTimer(const Duration & duration, void (* callback)(const TimerEvent &))
  {
    auto timer = node_->create_wall_timer(
      duration.operator rclcpp::Duration().to_chrono<std::chrono::nanoseconds>(),
      [callback]() {
        TimerEvent event;
        callback(event);
      });
    return Timer(timer);
  }

  template <typename CallbackT>
  Timer createTimer(const Duration & duration, CallbackT callback)
  {
    std::function<void()> timer_callback = [callback]() { callback(); };
    auto timer = node_->create_wall_timer(
      duration.operator rclcpp::Duration().to_chrono<std::chrono::nanoseconds>(),
      timer_callback);
    return Timer(timer);
  }

  template <typename T>
  void param(const std::string & name, T & value, const T & default_value) const
  {
    const auto normalized = detail::normalize_param_name(name);
    if (!node_->has_parameter(normalized)) {
      node_->declare_parameter(normalized, default_value);
    }
    if (!getParam(normalized, value)) {
      value = default_value;
    }
  }

  template <typename T>
  bool getParam(const std::string & name, T & value) const
  {
    const auto normalized = detail::normalize_param_name(name);
    rclcpp::Parameter parameter;
    if (!node_->get_parameter(normalized, parameter)) {
      return false;
    }
    return detail::get_parameter_value(parameter, value);
  }

  bool hasParam(const std::string & name) const
  {
    return node_->has_parameter(detail::normalize_param_name(name));
  }

  rclcpp::Node::SharedPtr node() const { return node_; }

private:
  rclcpp::Node::SharedPtr node_;
};

class Rate
{
public:
  explicit Rate(double hz) : rate_(hz) {}
  void sleep() { rate_.sleep(); }

private:
  rclcpp::Rate rate_;
};

inline rclcpp::Logger logger()
{
  return detail::ensure_node()->get_logger();
}

inline rclcpp::Clock::SharedPtr clock()
{
  return detail::ensure_node()->get_clock();
}
}  // namespace ros

#define ROS_INFO(...) RCLCPP_INFO(ros::logger(), __VA_ARGS__)
#define ROS_WARN(...) RCLCPP_WARN(ros::logger(), __VA_ARGS__)
#define ROS_ERROR(...) RCLCPP_ERROR(ros::logger(), __VA_ARGS__)
#define ROS_INFO_THROTTLE(period, ...) \
  RCLCPP_INFO_THROTTLE(ros::logger(), *ros::clock(), static_cast<int>((period) * 1000.0), __VA_ARGS__)
#define ROS_WARN_THROTTLE(period, ...) \
  RCLCPP_WARN_THROTTLE(ros::logger(), *ros::clock(), static_cast<int>((period) * 1000.0), __VA_ARGS__)
#define ROS_INFO_STREAM(expr) \
  do { std::ostringstream _ros_stream; _ros_stream << expr; RCLCPP_INFO(ros::logger(), "%s", _ros_stream.str().c_str()); } while (0)
#define ROS_WARN_STREAM(expr) \
  do { std::ostringstream _ros_stream; _ros_stream << expr; RCLCPP_WARN(ros::logger(), "%s", _ros_stream.str().c_str()); } while (0)
#define ROS_ERROR_STREAM(expr) \
  do { std::ostringstream _ros_stream; _ros_stream << expr; RCLCPP_ERROR(ros::logger(), "%s", _ros_stream.str().c_str()); } while (0)

#endif  // DDR_OPT_ROS2_COMPAT_ROS_H_
