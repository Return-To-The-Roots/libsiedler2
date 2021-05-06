// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cmath>
#include <vector>

template<typename T>
class GammaTable
{
private:
    std::vector<T> table;
    float gamma;

public:
    GammaTable(unsigned s, float g = 1) : gamma(-1)
    {
        table.resize(s > 2 ? s : 2);
        set_gamma(g);
    }

    const float& get_gamma() { return gamma; }

    void set_gamma(float g)
    {
        if(g < 0.001f)
            g = 0.001f;
        if(g == gamma) //-V550
            return;
        gamma = g;

        float sizef = table.size() - 1.f;
        for(unsigned i = 0; i < table.size(); i++)
            table[i] = (T)(std::pow(i / sizef, 1 / gamma) * sizef);
    }

    const T& operator[](const T& i) const { return table[i]; }
};
