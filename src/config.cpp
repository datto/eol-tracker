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


#include "config.h"

Config::Config(const std::filesystem::path& configFile) : m_toml(cpptoml::parse_file(configFile))
{
}

std::string Config::getDatabaseName() const
{
    return m_toml->get_qualified_as<std::string>("db.database").value_or("");
}

std::string Config::getDatabaseUsername() const
{
    return m_toml->get_qualified_as<std::string>("db.username").value_or("");
}

std::string Config::getDatabasePassword() const
{
    return m_toml->get_qualified_as<std::string>("db.password").value_or("");
}

std::string Config::getDatabaseOptions() const
{
    return m_toml->get_qualified_as<std::string>("db.options").value_or("");
}

std::string Config::getDatabaseLocation() const
{
    return m_toml->get_qualified_as<std::string>("db.location").value_or("");
}

int Config::getDatabasePort() const
{
    return m_toml->get_qualified_as<int>("db.port").value_or(5432);
}

std::filesystem::path Config::getRepoCacheDir() const
{
    return m_toml->get_qualified_as<std::string>("dnf.cacheDir").value_or("");
}
