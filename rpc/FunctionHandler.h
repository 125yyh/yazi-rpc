#pragma once

#include <string>
#include <map>
#include <functional>
using namespace std;

#include "DataStream.h"
using namespace yazi::serialize;

namespace yazi {
namespace rpc {

class FunctionHandler
{
public:
    FunctionHandler() {}
    ~FunctionHandler() {}

    template <typename F>
    void bind(const string & name, F func);

    void call(const string & name, DataStream & in, DataStream & out);

private:
    template <typename F>
    void wrap(F func, DataStream & in, DataStream & out);

    template <typename R, typename ...Args>
    void wrap_impl(R(*func)(Args...), DataStream & in, DataStream & out);

    template <typename R, typename ...Args>
    void wrap_impl(std::function<R(Args...)> func, DataStream & in, DataStream & out);

    template<typename R, typename F, typename Tuple>
    typename std::enable_if<std::is_same<R, void>::value, int32_t>::type
    call_impl(F func, Tuple args);

    template<typename R, typename F, typename Tuple>
    typename std::enable_if<!std::is_same<R, void>::value, R>::type
    call_impl(F func, Tuple args);

    template<typename R, typename F, typename Tuple>
    auto invoke(F && func, Tuple && t);

    template<typename R, typename F, typename Tuple, std::size_t... Index>
    auto invoke_impl(F && func, Tuple && t, std::index_sequence<Index...>);

    template<typename Tuple, std::size_t... I>
    Tuple get_args(DataStream & ds, std::index_sequence<I...>);

    template<typename Tuple, std::size_t Id>
    void get_arg(DataStream & ds, Tuple & t);

private:
    std::map<string, std::function<void(DataStream &, DataStream &)>> m_handlers;
};

inline void FunctionHandler::call(const string & name, DataStream & in, DataStream & out)
{
    auto func = m_handlers[name];
    func(in, out);
}

template <typename F>
void FunctionHandler::bind(const string & name, F func)
{
    m_handlers[name] = std::bind(&FunctionHandler::wrap<F>, this, func, std::placeholders::_1, std::placeholders::_2);
}

template <typename F>
void FunctionHandler::wrap(F func, DataStream & in, DataStream & out)
{
    wrap_impl(func, in, out);
}

template <typename R, typename ...Args>
void FunctionHandler::wrap_impl(R(*func)(Args...), DataStream & in, DataStream & out)
{
    wrap_impl(std::function<R(Args...)>(func), in, out);
}

template <typename R, typename ...Args>
void FunctionHandler::wrap_impl(std::function<R(Args...)> func, DataStream & in, DataStream & out)
{
    using args_type = std::tuple<typename std::decay<Args>::type...>;
    constexpr auto size = std::tuple_size<typename std::decay<args_type>::type>::value;
    args_type args = get_args<args_type>(in, std::make_index_sequence<size>{});
    auto ret = call_impl<R>(func, args);
    out << ret;
}

template<typename R, typename F, typename Tuple>
typename std::enable_if<std::is_same<R, void>::value, int32_t>::type
FunctionHandler::call_impl(F func, Tuple args)
{
    invoke<R>(func, args);
    return 0;
}

template<typename R, typename F, typename Tuple>
typename std::enable_if<!std::is_same<R, void>::value, R>::type
FunctionHandler::call_impl(F func, Tuple args)
{
    return invoke<R>(func, args);
}

template<typename R, typename F, typename Tuple>
auto FunctionHandler::invoke(F && func, Tuple && t)
{
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return invoke_impl<R>(std::forward<F>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
}

template<typename R, typename F, typename Tuple, std::size_t... Index>
auto FunctionHandler::invoke_impl(F && func, Tuple && t, std::index_sequence<Index...>)
{
    return func(std::get<Index>(std::forward<Tuple>(t))...);
}

template<typename Tuple, std::size_t... I>
Tuple FunctionHandler::get_args(DataStream & ds, std::index_sequence<I...>)
{
    Tuple t;
    initializer_list<int>{((get_arg<Tuple, I>(ds, t)), 0)...};
    return t;
}

template<typename Tuple, std::size_t Id>
void FunctionHandler::get_arg(DataStream & ds, Tuple & t)
{
    ds >> std::get<Id>(t);
}

}
}