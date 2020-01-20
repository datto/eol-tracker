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


#include "Package.hpp"

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace std::string_literals;

namespace eoltracker {

Package::Package(int id) :
    m_id(id)
{}

Package::Package(
    const Project& proj,
    const OperatingSystem& os,
    const Version& vers,
    const std::string& intro,
    const std::string& foundName)
{
    auto query = CPreparedSqlQueryThreadForDB(
        "insert into package (version, intro, project, os, foundName) values (:version, :intro, :project, :os, :foundName) "
        "on conflict(project, os) do update set version=excluded.version, intro=excluded.intro, foundName=excluded.foundName;", "eol");
    query.bindValue(":version", std::to_string(vers).c_str());
    query.bindValue(":intro", intro.c_str());
    query.bindValue(":project", proj.getKey());
    query.bindValue(":os", os.getKey());
    query.bindValue(":foundName", foundName.c_str());
    if (!query.exec()) {
        throw;
    }

    m_id = query.lastInsertId().toInt();
}

Project Package::getProject()
{
    auto query = CPreparedSqlQueryThreadForDB("select project from package where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return Project(query.value(0).toInt());
    }
    throw std::runtime_error("Could not find project for package");
}

OperatingSystem Package::getOS()
{
    auto query = CPreparedSqlQueryThreadForDB("select os from package where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return OperatingSystem(query.value(0).toInt());
    }
    throw std::runtime_error("Could not find operating system for package");
}

Version Package::getVersion()
{
    auto query = CPreparedSqlQueryThreadForDB("select version from package where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return Version(query.value(0).toString().toStdString());
    }
    throw std::runtime_error("Could not find version for package");
}

std::string Package::getIntroduction()
{
    auto query = CPreparedSqlQueryThreadForDB("select intro from package where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }
    throw std::runtime_error("Could not find intro for package");
}

std::string Package::getFoundName()
{
    auto query = CPreparedSqlQueryThreadForDB("select foundName from package where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }
    throw std::runtime_error("Could not find found name for package");
}

int Package::getKey() const
{
    return m_id;
}

} // namespace eoltracker