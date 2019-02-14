/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_ADAPT_HPP
#define XTENSOR_ADAPT_HPP

#include <array>
#include <cstddef>
#include <memory>
#include <type_traits>

#include <xtl/xsequence.hpp>

#include "xarray.hpp"
#include "xtensor.hpp"
#include "xfixed.hpp"
#include "xbuffer_adaptor.hpp"

namespace xt
{
    namespace detail
    {
        template <class>
        struct array_size_impl;

        template <class T, std::size_t N>
        struct array_size_impl<std::array<T, N>>
        {
            static constexpr std::size_t value = N;
        };

        template <class C>
        using array_size = array_size_impl<std::decay_t<C>>;

        template <class P>
        struct default_allocator_for_ptr
        {
            using type = std::allocator<std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<P>>>>;
        };

        template <class P>
        using default_allocator_for_ptr_t = typename default_allocator_for_ptr<P>::type;

        template <class T>
        using not_an_array = xtl::negation<is_array<T>>;

        template <class T>
        using not_a_pointer = xtl::negation<std::is_pointer<T>>;

        template <class T>
        using not_a_layout = xtl::negation<std::is_same<layout_type,T>>;
    }

    /**************************
     * xarray_adaptor builder *
     **************************/

    /**
     * Constructs an xarray_adaptor of the given stl-like container,
     * with the specified shape and layout.
     * @param container the container to adapt
     * @param shape the shape of the xarray_adaptor
     * @param l the layout_type of the xarray_adaptor
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C, class SC,
              XTL_REQUIRES(detail::not_an_array<std::decay_t<SC>>,
                           detail::not_a_pointer<C>)>
    inline xarray_adaptor<xtl::closure_type_t<C>, L, std::decay_t<SC>>
    adapt(C&& container, const SC& shape, layout_type l = L)
    {
        using return_type = xarray_adaptor<xtl::closure_type_t<C>, L, std::decay_t<SC>>;
        return return_type(std::forward<C>(container), shape, l);
    }

    /**
     * Constructs an non-owning xarray_adaptor from a pointer with the specified shape and layout.
     * @param pointer the container to adapt
     * @param shape the shape of the xarray_adaptor
     * @param l the layout_type of the xarray_adaptor
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C, class SC,
              XTL_REQUIRES(detail::not_an_array<std::decay_t<SC>>,
                           std::is_pointer<C>)>
    inline auto adapt(C&& pointer, const SC& shape, layout_type l = L)
    {
        using buffer_type = xbuffer_adaptor<C, xt::no_ownership, detail::default_allocator_for_ptr_t<C>>;
        using return_type = xarray_adaptor<buffer_type, L, std::decay_t<SC>>;
        std::size_t size = compute_size(shape);
        return return_type(buffer_type(pointer, size), shape, l);
    }

    /**
     * Constructs an xarray_adaptor of the given stl-like container,
     * with the specified shape and strides.
     * @param container the container to adapt
     * @param shape the shape of the xarray_adaptor
     * @param strides the strides of the xarray_adaptor
     */
    template <class C, class SC, class SS,
              XTL_REQUIRES(detail::not_an_array<std::decay_t<SC>>,
                           detail::not_a_layout<std::decay_t<SS>>)>
    inline xarray_adaptor<xtl::closure_type_t<C>, layout_type::dynamic, std::decay_t<SC>>
    adapt(C&& container, SC&& shape, SS&& strides)
    {
        using return_type = xarray_adaptor<xtl::closure_type_t<C>, layout_type::dynamic, std::decay_t<SC>>;
        return return_type(std::forward<C>(container),
                           xtl::forward_sequence<typename return_type::inner_shape_type, SC>(shape),
                           xtl::forward_sequence<typename return_type::inner_strides_type, SS>(strides));
    }

    /**
     * Constructs an xarray_adaptor of the given dynamically allocated C array,
     * with the specified shape and layout.
     * @param pointer the pointer to the beginning of the dynamic array
     * @param size the size of the dynamic array
     * @param ownership indicates whether the adaptor takes ownership of the array.
     *        Possible values are ``no_ownership()`` or ``acquire_ownership()``
     * @param shape the shape of the xarray_adaptor
     * @param l the layout_type of the xarray_adaptor
     * @param alloc the allocator used for allocating / deallocating the dynamic array
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class P, class O, class SC, class A = detail::default_allocator_for_ptr_t<P>,
              XTL_REQUIRES(detail::not_an_array<std::decay_t<SC>>)>
    inline xarray_adaptor<xbuffer_adaptor<xtl::closure_type_t<P>, O, A>, L, SC>
    adapt(P&& pointer, typename A::size_type size, O ownership, const SC& shape, layout_type l = L, const A& alloc = A())
    {
        (void)ownership;
        using buffer_type = xbuffer_adaptor<xtl::closure_type_t<P>, O, A>;
        using return_type = xarray_adaptor<buffer_type, L, SC>;
        buffer_type buf(std::forward<P>(pointer), size, alloc);
        return return_type(std::move(buf), shape, l);
    }

    /**
     * Constructs an xarray_adaptor of the given dynamically allocated C array,
     * with the specified shape and layout.
     * @param pointer the pointer to the beginning of the dynamic array
     * @param size the size of the dynamic array
     * @param ownership indicates whether the adaptor takes ownership of the array.
     *        Possible values are ``no_ownership()`` or ``acquire_ownership()``
     * @param shape the shape of the xarray_adaptor
     * @param strides the strides of the xarray_adaptor
     * @param alloc the allocator used for allocating / deallocating the dynamic array
     */
    template <class P, class O, class SC, class SS, class A = detail::default_allocator_for_ptr_t<P>,
              XTL_REQUIRES(detail::not_an_array<std::decay_t<SC>>,
                           detail::not_a_layout<std::decay_t<SS>>)>
    inline xarray_adaptor<xbuffer_adaptor<xtl::closure_type_t<P>, O, A>, layout_type::dynamic, std::decay_t<SC>>
    adapt(P&& pointer, typename A::size_type size, O ownership, SC&& shape, SS&& strides, const A& alloc = A())
    {
        (void)ownership;
        using buffer_type = xbuffer_adaptor<xtl::closure_type_t<P>, O, A>;
        using return_type = xarray_adaptor<buffer_type, layout_type::dynamic, std::decay_t<SC>>;
        buffer_type buf(std::forward<P>(pointer), size, alloc);
        return return_type(std::move(buf),
                           xtl::forward_sequence<typename return_type::inner_shape_type, SC>(shape),
                           xtl::forward_sequence<typename return_type::inner_strides_type, SS>(strides));
    }

    /***************************
     * xtensor_adaptor builder *
     ***************************/

    /**
     * Constructs a 1-D xtensor_adaptor of the given stl-like container,
     * with the specified layout_type.
     * @param container the container to adapt
     * @param l the layout_type of the xtensor_adaptor
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C>
    inline xtensor_adaptor<C, 1, L>
    adapt(C&& container, layout_type l = L)
    {
        const std::array<typename std::decay_t<C>::size_type, 1> shape{container.size()};
        using return_type = xtensor_adaptor<xtl::closure_type_t<C>, 1, L>;
        return return_type(std::forward<C>(container), shape, l);
    }

    /**
     * Constructs an xtensor_adaptor of the given stl-like container,
     * with the specified shape and layout_type.
     * @param container the container to adapt
     * @param shape the shape of the xtensor_adaptor
     * @param l the layout_type of the xtensor_adaptor
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C, class SC,
              XTL_REQUIRES(detail::is_array<std::decay_t<SC>>,
                           detail::not_a_pointer<C>)>
    inline xtensor_adaptor<C, detail::array_size<SC>::value, L>
    adapt(C&& container, const SC& shape, layout_type l = L)
    {
        constexpr std::size_t N = detail::array_size<SC>::value;
        using return_type = xtensor_adaptor<xtl::closure_type_t<C>, N, L>;
        return return_type(std::forward<C>(container), shape, l);
    }

    /**
     * Constructs an non-owning xtensor_adaptor from a pointer with the specified shape and layout.
     * @param pointer the pointer to adapt
     * @param shape the shape of the xtensor_adaptor
     * @param l the layout_type of the xtensor_adaptor
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C, class SC,
              XTL_REQUIRES(detail::is_array<std::decay_t<SC>>,
                           std::is_pointer<C>)>
    inline auto adapt(C&& pointer, const SC& shape, layout_type l = L)
    {
        using buffer_type = xbuffer_adaptor<C, xt::no_ownership, detail::default_allocator_for_ptr_t<C>>;
        constexpr std::size_t N = detail::array_size<SC>::value;
        using return_type = xtensor_adaptor<buffer_type, N, L>;
        return return_type(buffer_type(pointer, compute_size(shape)), shape, l);
    }

    /**
     * Constructs an xtensor_adaptor of the given stl-like container,
     * with the specified shape and strides.
     * @param container the container to adapt
     * @param shape the shape of the xtensor_adaptor
     * @param strides the strides of the xtensor_adaptor
     */
    template <class C, class SC, class SS,
              XTL_REQUIRES(detail::is_array<std::decay_t<SC>>,
                           detail::not_a_layout<std::decay_t<SS>>)>
    inline xtensor_adaptor<C, detail::array_size<SC>::value, layout_type::dynamic>
    adapt(C&& container, SC&& shape, SS&& strides)
    {
        constexpr std::size_t N = detail::array_size<SC>::value;
        using return_type = xtensor_adaptor<xtl::closure_type_t<C>, N, layout_type::dynamic>;
        return return_type(std::forward<C>(container),
                           xtl::forward_sequence<typename return_type::inner_shape_type, SC>(shape),
                           xtl::forward_sequence<typename return_type::inner_strides_type, SS>(strides));
    }

    /**
     * Constructs a 1-D xtensor_adaptor of the given dynamically allocated C array,
     * with the specified layout.
     * @param pointer the pointer to the beginning of the dynamic array
     * @param size the size of the dynamic array
     * @param ownership indicates whether the adaptor takes ownership of the array.
     *        Possible values are ``no_ownership()`` or ``acquire_ownership()``
     * @param l the layout_type of the xtensor_adaptor
     * @param alloc the allocator used for allocating / deallocating the dynamic array
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class P, class O, class A = detail::default_allocator_for_ptr_t<P>>
    inline xtensor_adaptor<xbuffer_adaptor<xtl::closure_type_t<P>, O, A>, 1, L>
    adapt(P&& pointer, typename A::size_type size, O ownership, layout_type l = L, const A& alloc = A())
    {
        (void)ownership;
        using buffer_type = xbuffer_adaptor<xtl::closure_type_t<P>, O, A>;
        using return_type = xtensor_adaptor<buffer_type, 1, L>;
        buffer_type buf(std::forward<P>(pointer), size, alloc);
        const std::array<typename A::size_type, 1> shape{size};
        return return_type(std::move(buf), shape, l);
    }

    /**
     * Constructs an xtensor_adaptor of the given dynamically allocated C array,
     * with the specified shape and layout.
     * @param pointer the pointer to the beginning of the dynamic array
     * @param size the size of the dynamic array
     * @param ownership indicates whether the adaptor takes ownership of the array.
     *        Possible values are ``no_ownership()`` or ``acquire_ownership()``
     * @param shape the shape of the xtensor_adaptor
     * @param l the layout_type of the xtensor_adaptor
     * @param alloc the allocator used for allocating / deallocating the dynamic array
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class P, class O, class SC, class A = detail::default_allocator_for_ptr_t<P>,
              XTL_REQUIRES(detail::is_array<std::decay_t<SC>>)>
    inline xtensor_adaptor<xbuffer_adaptor<xtl::closure_type_t<P>, O, A>, detail::array_size<SC>::value, L>
    adapt(P&& pointer, typename A::size_type size, O ownership, const SC& shape, layout_type l = L, const A& alloc = A())
    {
        (void)ownership;
        using buffer_type = xbuffer_adaptor<xtl::closure_type_t<P>, O, A>;
        constexpr std::size_t N = detail::array_size<SC>::value;
        using return_type = xtensor_adaptor<buffer_type, N, L>;
        buffer_type buf(std::forward<P>(pointer), size, alloc);
        return return_type(std::move(buf), shape, l);
    }

    /**
     * Constructs an xtensor_adaptor of the given dynamically allocated C array,
     * with the specified shape and strides.
     * @param pointer the pointer to the beginning of the dynamic array
     * @param size the size of the dynamic array
     * @param ownership indicates whether the adaptor takes ownership of the array.
     *        Possible values are ``no_ownership()`` or ``acquire_ownership()``
     * @param shape the shape of the xtensor_adaptor
     * @param strides the strides of the xtensor_adaptor
     * @param alloc the allocator used for allocating / deallocating the dynamic array
     */
    template <class P, class O, class SC, class SS, class A = detail::default_allocator_for_ptr_t<P>,
              XTL_REQUIRES(detail::is_array<std::decay_t<SC>>,
                           detail::not_a_layout<std::decay_t<SS>>)>
    inline xtensor_adaptor<xbuffer_adaptor<xtl::closure_type_t<P>, O, A>, detail::array_size<SC>::value, layout_type::dynamic>
    adapt(P&& pointer, typename A::size_type size, O ownership, SC&& shape, SS&& strides, const A& alloc = A())
    {
        (void)ownership;
        using buffer_type = xbuffer_adaptor<xtl::closure_type_t<P>, O, A>;
        constexpr std::size_t N = detail::array_size<SC>::value;
        using return_type = xtensor_adaptor<buffer_type, N, layout_type::dynamic>;
        buffer_type buf(std::forward<P>(pointer), size, alloc);
        return return_type(std::move(buf),
                           xtl::forward_sequence<typename return_type::inner_shape_type, SC>(shape),
                           xtl::forward_sequence<typename return_type::inner_strides_type, SS>(strides));
    }

    /**
     * Constructs an non-owning xtensor_fixed_adaptor from a pointer with the
     * specified shape and layout.
     * @param pointer the pointer to adapt
     * @param shape the shape of the xtensor_fixed_adaptor
     */
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C, std::size_t... X,
              XTL_REQUIRES(std::is_pointer<C>)>
    inline auto adapt(C&& ptr, const fixed_shape<X...>& /*shape*/)
    {
        using buffer_type = xbuffer_adaptor<C, xt::no_ownership, detail::default_allocator_for_ptr_t<C>>;
        using return_type = xfixed_adaptor<buffer_type, fixed_shape<X...>, L>;
        return return_type(buffer_type(ptr, detail::fixed_compute_size<fixed_shape<X...>>::value));
    }

#ifndef X_OLD_CLANG
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C, class T,  std::size_t N>
    inline auto adapt(C&& ptr, const T(&shape)[N])
    {
        using shape_type = std::array<std::size_t, N>;
        return adapt(std::forward<C>(ptr), xtl::forward_sequence<shape_type, decltype(shape)>(shape));
    }
#else
    template <layout_type L = XTENSOR_DEFAULT_LAYOUT, class C>
    inline auto adapt(C&& ptr, std::initializer_list<std::size_t> shape)
    {
        using shape_type = xt::dynamic_shape<std::size_t>;
        return adapt(std::forward<C>(ptr), xtl::forward_sequence<shape_type, decltype(shape)>(shape));
    }
#endif
}

#endif