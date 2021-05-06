// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace libsiedler2 {
class ArchivItem;
/// Class for an archiv. That is a collection of @p ArchivItem to which the archive holds ownership
/// So all entries are deleted when the archive is destroyed
class Archiv
{
public:
    Archiv();
    Archiv(const Archiv& info);
    Archiv(Archiv&&) noexcept;
    /// Destruktor von @p Archiv, räumt automatisch auf.
    virtual ~Archiv();
    Archiv& operator=(const Archiv& info);
    Archiv& operator=(Archiv&&) noexcept;
    /// Creates a dataset with a given size. Deletes all current entries
    void alloc(size_t count);
    /// Increases the element count by the given amount
    void alloc_inc(size_t increment);
    /// Release all data and set size to 0
    void clear();
    /// Set the item at the given position
    void set(size_t index, std::unique_ptr<ArchivItem> item);
    /// Set the item at the given position to a copy of the given item
    void setC(size_t index, const ArchivItem& item);
    /// Adds an element to the end.
    void push(std::unique_ptr<ArchivItem> item);
    /// Add a copy of the element to the end
    void pushC(const ArchivItem& item);
    /// Return the item at the given index or nullptr if the index is out of bounds
    ArchivItem* get(size_t index) { return (index < size()) ? data[index].get() : nullptr; }
    /// Return the item at the given index or nullptr if the index is out of bounds
    const ArchivItem* get(size_t index) const { return (index < size()) ? data[index].get() : nullptr; }
    /// Return the first item with the given name
    ArchivItem* find(const std::string& name);
    /// Return the first item with the given name
    const ArchivItem* find(const std::string& name) const;
    /// Return the item at the given position and remove it from the archive
    std::unique_ptr<ArchivItem> release(size_t index);
    /// Return the number of entries (includes nullptr entries)
    size_t size() const { return data.size(); }
    /// True iff no entries stored
    bool empty() const { return data.empty(); }
    /// Return the item at the given index or nullptr if the index is out of bounds
    /// Note: Pointer is const -> archiv cannot be modified with this operator
    const ArchivItem* operator[](size_t index) const { return get(index); }
    ArchivItem* operator[](size_t index) { return get(index); }

    // Range-based for support
    friend auto begin(Archiv& archive) { return archive.data.begin(); }
    friend auto end(Archiv& archive) { return archive.data.end(); }
    friend auto begin(const Archiv& archive) { return archive.data.begin(); }
    friend auto end(const Archiv& archive) { return archive.data.end(); }

private:
    std::vector<std::unique_ptr<ArchivItem>> data; /// elements
};
} // namespace libsiedler2
