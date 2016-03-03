// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef ARCHIVITEM_H_INCLUDED
#define ARCHIVITEM_H_INCLUDED

#pragma once

#include "enumTypes.h"
#include <string>

namespace libsiedler2
{
    /// Basisklasse der Archivelemente.
    class ArchivItem
    {
        public:
            /// Konstruktor von @p ArchivItem.
            ArchivItem();

            /// virtueller Destruktor von @p ArchivItem.
            virtual ~ArchivItem();

            /// setzt den Bobtype des Items.
            inline void setBobType(BOBTYPES bobtype)
            {
                this->bobtype_ = bobtype;
            }

            /// liefert den Bobtype des Items.
            inline BOBTYPES getBobType() const
            {
                return bobtype_;
            }

            /// setzt den Namen des Items.
            inline void setName(const std::string& name)
            {
                this->name_ = name;
            }

            /// liefert den Namen des Items.
            inline std::string getName() const
            {
                return name_;
            }

        protected:
            BOBTYPES bobtype_; ///< Bobtype des Elements.
            std::string name_; ///< Name des Elements.
    };
}

#endif // !ARCHIVITEM_H_INCLUDED
