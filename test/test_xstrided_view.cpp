/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xtensor/xarray.hpp"
#include "xtensor/xbuilder.hpp"
#include "xtensor/xstrided_view.hpp"

#include "xtensor/xio.hpp"

namespace xt
{
    using std::size_t;
    using shape_t = std::vector<std::size_t>;

    TEST(xstrided_view, transpose_assignment)
    {
        xarray<double> e = xt::arange<double>(24);
        e.resize({2, 2, 6});
        auto vt = transpose(e);

        vt(0, 0, 1) = 123;
        EXPECT_EQ(123, e(1, 0, 0));
        auto val = vt[{1, 0, 1}];
        EXPECT_EQ(e(1, 0, 1), val);
        EXPECT_ANY_THROW(vt.at(10, 10, 10));
        EXPECT_ANY_THROW(vt.at(0, 0, 0, 0));
    }

    TEST(xstrided_view, expression_adapter)
    {
        auto e = xt::arange<double>(24);
        auto sv = slice_vector({range(2, 10, 3)});
        auto vt = dynamic_view(e, sv);

        EXPECT_EQ(vt(0), 2);
        EXPECT_EQ(vt(1), 5);

        xt::xarray<double> assigned = vt;
        EXPECT_EQ(assigned, vt);
        EXPECT_EQ(assigned(1), 5);
    }

    TEST(xstrided_view, transpose_layout_swap)
    {
        xarray<double, layout_type::row_major> a = xt::ones<double>({5, 5});

        auto tv = transpose(a);
        EXPECT_EQ(tv.layout(), layout_type::column_major);

        auto tvt = transpose(tv);
        EXPECT_EQ(tvt.layout(), layout_type::row_major);

        xarray<double, layout_type::column_major> b = xt::ones<double>({5, 5, 5});
        auto cbt = transpose(b);
        EXPECT_EQ(cbt.layout(), layout_type::row_major);

        auto cbw1 = transpose(b, {0, 1 ,2});
        auto cbw2 = transpose(b, {2, 1, 0});
        auto cbw3 = transpose(b, {2, 0, 1});
        EXPECT_EQ(cbw1.layout(), layout_type::column_major);
        EXPECT_EQ(cbw2.layout(), layout_type::row_major);
        EXPECT_EQ(cbw3.layout(), layout_type::dynamic);
    }

    TEST(xstrided_view, transpose_function)
    {
        xarray<int, layout_type::row_major> a = { { 0, 1, 2 }, { 3, 4, 5 } };
        xarray<int, layout_type::row_major> b = { { 0, 1, 2 }, { 3, 4, 5 } };
        auto fun = a + b;
        auto tr = transpose(fun);
        EXPECT_EQ(fun(0, 0), tr(0, 0));
        EXPECT_EQ(fun(0, 1), tr(1, 0));
        EXPECT_EQ(fun(0, 2), tr(2, 0));
        EXPECT_EQ(fun(1, 0), tr(0, 1));
        EXPECT_EQ(fun(1, 1), tr(1, 1));
        EXPECT_EQ(fun(1, 2), tr(2, 1));

        xarray<int, layout_type::column_major> a2 = { { 0, 1, 2 },{ 3, 4, 5 } };
        xarray<int, layout_type::column_major> b2 = { { 0, 1, 2 },{ 3, 4, 5 } };
        auto fun2 = a2 + b2;
        auto tr2 = transpose(fun2);
        EXPECT_EQ(fun2(0, 0), tr2(0, 0));
        EXPECT_EQ(fun2(0, 1), tr2(1, 0));
        EXPECT_EQ(fun2(0, 2), tr2(2, 0));
        EXPECT_EQ(fun2(1, 0), tr2(0, 1));
        EXPECT_EQ(fun2(1, 1), tr2(1, 1));
        EXPECT_EQ(fun2(1, 2), tr2(2, 1));
    }

    TEST(xstrided_view, ravel)
    {
        xarray<int, layout_type::row_major> a = { { 0, 1, 2 },{ 3, 4, 5 } };

        auto flat = ravel<layout_type::row_major>(a);
        EXPECT_EQ(flat(0), a(0, 0));
        EXPECT_EQ(flat(1), a(0, 1));
        EXPECT_EQ(flat(2), a(0, 2));
        EXPECT_EQ(flat(3), a(1, 0));
        EXPECT_EQ(flat(4), a(1, 1));
        EXPECT_EQ(flat(5), a(1, 2));

        auto flat_c = ravel<layout_type::column_major>(a);
        EXPECT_EQ(flat_c(0), a(0, 0));
        EXPECT_EQ(flat_c(1), a(1, 0));
        EXPECT_EQ(flat_c(2), a(0, 1));
        EXPECT_EQ(flat_c(3), a(1, 1));
        EXPECT_EQ(flat_c(4), a(0, 2));
        EXPECT_EQ(flat_c(5), a(1, 2));

        auto flat2 = flatten(a);
        EXPECT_EQ(flat, flat2);
    }
}
