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


#include "Version.hpp"

#include <cstdio>

namespace std { 

std::string to_string(const eoltracker::Version& v)
{
    return v.m_versionString;
}

} // namespace std

namespace eoltracker {

Version::Version(const std::string& sVersion) :
    m_versionString(sVersion)
{
    sscanf(
        sVersion.c_str(),
        "%d.%d.%d.%d",
        &m_nMajor,
        &m_nMinor,
        &m_nRevision,
        &m_nBuild
    );
}

bool Version::operator<(const Version& robjVersion) const
{
    if (m_nMajor != robjVersion.m_nMajor) {
        return m_nMajor < robjVersion.m_nMajor;
    }

    if (m_nMinor != robjVersion.m_nMinor) {
        return m_nMinor < robjVersion.m_nMinor;
    }

    if (m_nRevision != robjVersion.m_nRevision) {
        return m_nRevision < robjVersion.m_nRevision;
    }

    return m_nBuild < robjVersion.m_nBuild;
}

} //namespace eoltracker
