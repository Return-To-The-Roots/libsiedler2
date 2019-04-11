// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifndef DYNAMIC_UNIQUE_CAST_H_INCLUDED
#define DYNAMIC_UNIQUE_CAST_H_INCLUDED

#include <memory>
#include <type_traits>

namespace libsiedler2 {
class ArchivItem;

template<class T, class U>
std::unique_ptr<T> dynamicUniqueCast(std::unique_ptr<U>&& src)
{
    static_assert(std::has_virtual_destructor<T>::value, "Can't cast this unique_ptr");
    return std::unique_ptr<T>(dynamic_cast<T*>(src.release()));
}
template<class T, class U>
std::unique_ptr<T> dynamicUniqueCast(std::unique_ptr<U>& src)
{
    return dynamicUniqueCast<T>(std::move(src));
}

} // namespace libsiedler2

#endif // DYNAMIC_UNIQUE_CAST_H_INCLUDED
