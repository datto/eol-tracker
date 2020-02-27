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


#include "UpdateThread.h"

#include "OperatingSystem.hpp"
#include "Project.hpp"
#include "RepoScan.hpp"

#include <algorithm>
#include <exception>
#include <execution>
#include <vector>

#include <QtGlobal>
#include <QtDebug>
#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace std::string_literals;

UpdateThread::UpdateThread(Config& config) : m_config(config)
{
}

UpdateThread::~UpdateThread()
{

}

void UpdateThread::run()
{
    qInfo() << "update thread start";

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", Cutelyst::Sql::databaseNameThread("eol"));
    db.setDatabaseName(QString::fromStdString(m_config.getDatabaseName()));
    db.setHostName(QString::fromStdString(m_config.getDatabaseLocation()));
    db.setUserName(QString::fromStdString(m_config.getDatabaseUsername()));
    if (m_config.getDatabasePassword() != "") {
        db.setPassword(QString::fromStdString(m_config.getDatabasePassword()));
    }
    if (m_config.getDatabaseOptions() != "") {
        db.setConnectOptions(QString::fromStdString(m_config.getDatabaseOptions()));
    }

    if (!db.open()) {
        qCritical() << "Failed to open database on update thread:" << db.lastError().text();
        return;
    }

    while (true) {
        try {
            qInfo() << "update loop start";
            auto query = QSqlQuery("select id from project;", db);
            if (query.lastError().isValid()) {
                qCritical() << "Failed to get list of projects:" << db.lastError().text();
                throw;
            }
            std::vector<eoltracker::Project> projects;
            while (query.next()) {
                projects.emplace_back(query.value(0).toInt());
            }

            query = QSqlQuery("select id from operating_system;", db);
            if (query.lastError().isValid()) {
                qCritical() << "Failed to get list of operating systems:" << db.lastError().text();
                throw;
            }
            std::vector<eoltracker::OperatingSystem> oses;
            while (query.next()) {
                oses.emplace_back(query.value(0).toInt());
            }

            //std::for_each(std::begin(oses), std::end(oses), [&projects](eoltracker::OperatingSystem& os) {
            for (auto& os : oses) {
                auto name = os.getName() + " "s + std::to_string(os.getVersion());
                try {
                    eoltracker::RepoScan scan(os, m_config.getRepoCacheDir());
                    for (auto& proj : projects)
                    {
                        try {
                            scan.getPackage(proj);
                        }
                        catch (const std::exception& e) { // can't find the package
                            qWarning() << e.what();
                        }
                    }
                }
                catch (const std::exception& e) {
                    qCritical() << "Failure for " << name.c_str() << ": " << e.what();
                }
            }
            //);
        }
        catch (const std::exception& e) {
            qCritical() << "update loop caught exception: " << e.what();
        }
        catch (...) {}

        const auto waitHours = 1;
        qInfo() << "update loop end, waiting " << waitHours << " hour(s) to check again";

        QThread::sleep(waitHours * 60 * 60);
    }

    db.close();

    qInfo() << "update thread stop";
}