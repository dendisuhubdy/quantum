/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef BLOOMBERG_QUANTUM_STL_IMPL_H
#define BLOOMBERG_QUANTUM_STL_IMPL_H

#include <tuple>
#include <utility>
#include <type_traits>

namespace std {
#if (__cplusplus == 201103L)
    // From <utility>
    template< bool B, class T = void >
    using enable_if_t = typename std::enable_if<B,T>::type;
    
    template<class T>
    using decay_t = typename std::decay<T>::type;
    
    template<class T>
    using is_same_v = typename std::decay<T>::type;
    
    template<typename T>
    using remove_reference_t = typename std::remove_reference<T>::type;
    
    template<typename _Tp, _Tp... _Idx>
    struct integer_sequence
    {
        using value_type = _Tp;
        static constexpr size_t size() { return sizeof...(_Idx); }
    };

    template<typename _Tp, _Tp _Num,
             typename _ISeq = typename _Build_index_tuple<_Num>::__type>
    struct _Make_integer_sequence;

    template<typename _Tp, _Tp _Num,  size_t... _Idx>
    struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>>
    {
        static_assert( _Num >= 0, "Cannot make integer sequence of negative length" );
        using __type = integer_sequence<_Tp, static_cast<_Tp>(_Idx)...>;
    };
    
    template<typename _Tp, _Tp _Num>
    using make_integer_sequence = typename _Make_integer_sequence<_Tp, _Num>::__type;
    
    template<size_t... _Idx>
    using index_sequence = integer_sequence<size_t, _Idx...>;
    
    template<size_t _Num>
    using make_index_sequence = make_integer_sequence<size_t, _Num>;
    
    template<typename... _Types>
    using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
#endif
#if (__cplusplus < 201703L)
    template<class T, class U>
    std::shared_ptr<T> reinterpret_pointer_cast(const std::shared_ptr<U>& r) noexcept
    {
        auto p = reinterpret_cast<typename std::shared_ptr<T>::element_type*>(r.get());
        return std::shared_ptr<T>(r, p);
    }
#endif
} //std

namespace Bloomberg {
namespace quantum {

#if (__cplusplus <= 201402L)
    template <typename FUNC, typename...ARGS>
    struct ReturnOf
    {
        using Type = typename std::result_of<FUNC(ARGS...)>::type;
    };
#else
    template <typename FUNC, typename...ARGS>
    struct ReturnOf
    {
        using Type = std::invoke_result_t<FUNC, ARGS...>;
    };
#endif

template <typename RET, typename FUNC, typename... ARGS, size_t...I, typename...T>
RET apply_impl(FUNC&& func, std::tuple<ARGS...>&& tuple, std::index_sequence<I...>, T&&...t)
{
    return std::forward<FUNC>(func)(std::forward<T>(t)..., std::forward<ARGS>(std::get<I>(std::move(tuple)))...);
}

template <typename RET, typename FUNC, typename... ARGS, typename...T>
RET apply(FUNC&& func, std::tuple<ARGS...>&& tuple, T&&...t)
{
    return apply_impl<RET>(std::forward<FUNC>(func), std::move(tuple), std::index_sequence_for<ARGS...>{}, std::forward<T>(t)...);
}

} //namespace quantum
} //namespace Bloomberg

#endif //BLOOMBERG_QUANTUM_STL_IMPL_H
