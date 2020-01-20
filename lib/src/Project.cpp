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


#include "Project.hpp"

#include <numeric>
#include <sstream>
#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace std::string_literals;

namespace eoltracker {

Project::Project(int id) :
    m_id(id)
{}

Project::Project(const std::string& name, const std::vector<std::string>& alt)
{
    auto query = CPreparedSqlQueryThreadForDB("select id from project where name=:name", "eol");
    query.bindValue(":name", name.c_str());
    if (query.exec() && query.next())
    {
        m_id = query.value(0).toInt();
    }
    else {
        std::string colonSep = alt.empty() ? "" : std::accumulate(std::next(alt.begin()), alt.end(), *alt.begin(),
            [](std::string a, std::string i) {
                return std::move(a) + ":" + std::move(i);
            }
        );

        query = CPreparedSqlQueryThreadForDB("insert or ignore into project (name, alt) values (:name, :alt)", "eol");
        query.bindValue(":name", name.c_str());
        query.bindValue(":alt", colonSep.c_str());
        if (!query.exec()) {
            throw;
        }

        m_id = query.lastInsertId().toInt();
    }
}

std::string Project::getName()
{
    auto query = CPreparedSqlQueryThreadForDB("select name from project where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }
    throw std::runtime_error("Could not get name for project");
}

std::vector<std::string> Project::getAlternateNames()
{
    std::vector<std::string> ret;
    auto query = CPreparedSqlQueryThreadForDB("select alt from project where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        std::stringstream colonSep(query.value(0).toString().toStdString());
        std::string token = "";
        while (std::getline(colonSep, token, ':')) {
            ret.push_back(token);
        }
        return ret;
    }
    else {
        throw std::runtime_error("Could not get alternate names for project");
    }
}

int Project::getKey() const
{
    return m_id;
}

} // namespace eoltracker
