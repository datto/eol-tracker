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

#include <string>

namespace eoltracker { class Version; }

namespace std {

std::string to_string(const eoltracker::Version& v);

} // namespace std

namespace eoltracker {

class Version
{
public:
    explicit Version(const std::string& sVersion);
    virtual ~Version() = default;

    /**
     * @return version string given to us in the ctor
     * @note string representation is not necessarily indicative of how
     * comparison will behave. Test and use at your own risk!
     */
    friend std::string std::to_string(const Version& v);

    bool operator<(const Version& robjVersion) const;

private:
    int m_nMajor{ 0 };
    int m_nMinor{ 0 };
    int m_nRevision{ 0 };
    int m_nBuild{ 0 };
    std::string m_versionString{};
};

} // namespace eoltracker 
