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


#include "OperatingSystem.hpp"

#include <fstream>
#include <unistd.h>
#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace std::string_literals;

static auto versionQuery = CPreparedSqlQueryThreadForDB("select version from operating_system where id=:id", "eol");

namespace eoltracker {

OperatingSystem::OperatingSystem(int id) :
    m_id(id)
{}

OperatingSystem::OperatingSystem(std::string name, const Version& version, const std::string& birth,
    const std::string& death, const std::filesystem::path& repoFile)
{
    auto query = CPreparedSqlQueryThreadForDB(
        "select id from operating_system where name=:name and version=:version", "eol");
    query.bindValue(":name", name.c_str());
    query.bindValue(":version", std::to_string(version).c_str());
    if (query.exec() && query.next())
    {
        m_id = query.value(0).toInt();
    }
    else {
        query = CPreparedSqlQueryThreadForDB(
            "insert into operating_system (name, version, birth, death, repoFile) values (:name, :version, :birth, :death, :repo) on conflict do nothing", "eol");
        query.bindValue(":name", name.c_str());
        query.bindValue(":version", std::to_string(version).c_str());
        query.bindValue(":birth", birth.c_str());
        query.bindValue(":death", death.c_str());
        query.bindValue(":repo", repoFile.c_str());
        if (!query.exec())
        {
            throw;
        }

        m_id = query.lastInsertId().toInt();
    }
}

std::string OperatingSystem::getName() const
{
    auto query = CPreparedSqlQueryThreadForDB("select name from operating_system where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }
    qInfo() << m_id << " " << query.boundValue(":id") << " " << query.executedQuery() << " " << query.lastError().text();
    throw std::runtime_error("Could not get name for operating system");
}

Version OperatingSystem::getVersion() const
{
    auto query = CPreparedSqlQueryThreadForDB("select version from operating_system where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return Version(query.value(0).toString().toStdString());
    }
    throw std::runtime_error("Could not get version for operating system: " + query.lastError().text().toStdString());
}

std::string OperatingSystem::getBirth() const
{
    auto query = CPreparedSqlQueryThreadForDB("select birth from operating_system where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }
    throw std::runtime_error("Could not get birth for operating system");
}

std::string OperatingSystem::getDeath() const
{
    auto query = CPreparedSqlQueryThreadForDB("select death from operating_system where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }
    throw std::runtime_error("Could not get death for operating system");
}

std::filesystem::path OperatingSystem::getRepoFile() const
{
    auto query = CPreparedSqlQueryThreadForDB("select repoFile from operating_system where id=:id", "eol");
    query.bindValue(":id", m_id);
    if (query.exec() && query.next()) {
        return std::filesystem::path(query.value(0).toString().toStdString());
    }
    throw std::runtime_error("Could not get repo file for operating system");
}

int OperatingSystem::getKey() const
{
    return m_id;
}

Chroot OperatingSystem::makeChroot(const std::filesystem::path& chrootCollectionDir) const
{
    auto path = chrootCollectionDir / std::filesystem::path(getName() + " "s + std::to_string(getVersion()));
    auto c = Chroot(path);
    c.setup();

    std::filesystem::path os_release = c.getRootDir() / "etc" / "os-release";
    if (!std::filesystem::exists(os_release)) {
        std::ofstream osr(os_release);
        osr << "NAME=" << getName() << "\nVERSION_ID=" << std::to_string(getVersion()) << std::endl;
    }

    auto repo = c.getRepoDir() / (getName() + ".repo"s);
    unlink(repo.c_str());
    if (symlink((std::filesystem::path(WEBROOT "/static/media") / getRepoFile()).c_str(), repo.c_str())) {
        auto err = errno;
        qCritical() << "Failed to make repo file symlink: " << err;
    }

    return c;
}

} // namespace eoltracker
