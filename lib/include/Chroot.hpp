/* This file is part of eol-tracker.
 *
 * Copyright © 2020 Datto, Inc.
 * Author: Dakota Williams <drwilliams@datto.com>
 *
 * Licensed under the GNU Lesser General Public License Version 3
 * Fedora-License-Identifier: LGPLv3+
 * SPDX-2.0-License-Identifier: LGPL-3.0+
 * SPDX-3.0-License-Identifier: LGPL-3.0-or-later
 *
 * eol-tracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eol-tracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with eol-tracker.  If not, see <https://www.gnu.org/licenses/>.
 */


#pragma once

#include <filesystem>

namespace eoltracker {

class Chroot
{
public:
    Chroot(const std::filesystem::path& base) : m_root(base) {}
    virtual ~Chroot() = default;

    void setup() const {
        std::filesystem::create_directories(getRepoDir());
        std::filesystem::create_directories(getCacheDir());
        std::filesystem::create_directories(getTmpDir());
    }

    std::filesystem::path getRootDir() const       { return m_root; }
    std::filesystem::path getRepoDir() const    { return m_root / "etc/yum.repos.d"; }
    std::filesystem::path getCacheDir() const   { return m_root / "var/cache/dnf"; }
    std::filesystem::path getTmpDir() const     { return m_root / "var/tmp/dnf"; }

private:
    std::filesystem::path m_root;
};

} // namespace eoltracker