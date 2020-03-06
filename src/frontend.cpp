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

#include "frontend.h"

#include "root.h"
#include "settings.h"
#include "dependencies.h"
#include "oses.h"
#include "issues.h"

#include <Cutelyst/Plugins/View/Grantlee/grantleeview.h>
#include <Cutelyst/Plugins/View/JSON/viewjson.h>
#include <Cutelyst/Plugins/StaticSimple/staticsimple.h>

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

frontend::frontend(QObject *parent) : Application(parent), m_config(CONFDIR "/eol-tracker.conf"), m_updateThread(m_config)
{
}

frontend::~frontend()
{
}

bool frontend::init()
{
    new Root(this);
    new Settings(this);
    new Dependencies(this);
    new Oses(this);
    new Issues(this);
    new ViewJson(this, "json_view");
    auto view = new GrantleeView(this, "grantlee_view");
    view->setIncludePaths({ WEBROOT "/templates" });
    view->setWrapper("base.html");
    auto statics = new StaticSimple(this);
    statics->setDirs({ "static" });
    statics->setIncludePaths({ WEBROOT "/static" });

    return true;
}

bool frontend::postFork()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", Cutelyst::Sql::databaseNameThread("eol"));
    db.setDatabaseName(QString::fromStdString(m_config.getDatabaseName()));
    db.setHostName(QString::fromStdString(m_config.getDatabaseLocation()));
    db.setPort(m_config.getDatabasePort());
    db.setUserName(QString::fromStdString(m_config.getDatabaseUsername()));
    if (m_config.getDatabasePassword() != "") {
        db.setPassword(QString::fromStdString(m_config.getDatabasePassword()));
    }
    if (m_config.getDatabaseOptions() != "") {
        db.setConnectOptions(QString::fromStdString(m_config.getDatabaseOptions()));
    }
    if (!db.open()) {
        qCritical() << "Failed to open database: " << db.lastError().text();
        return false;
    }

    auto query = QSqlQuery(
        "create table if not exists project ("
            "id serial primary key not null, "
            "name text unique not null, "
            "alt text"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create project table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists operating_system ("
            "id serial primary key not null,"
            "name text not null,"
            "version text not null,"
            "birth date not null,"
            "death date not null,"
            "repoFile text,"
            "unique(name, version)"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create operating system table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists package ("
            "id serial primary key not null,"
            "version text not null,"
            "intro date not null,"
            "project integer not null,"
            "os integer not null,"
            "foundName text not null,"
            "foreign key(project) references project(id) on delete cascade,"
            "foreign key(os) references operating_system(id) on delete cascade,"
            "unique(project, os)"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create package table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists collection ("
            "id serial primary key not null,"
            "name text unique not null"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create collection table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists collection_os_membership ("
            "collection integer not null,"
            "os integer not null,"
            "supportStatus integer not null default 0,"
            "foreign key(collection) references collection(id) on delete cascade,"
            "foreign key(os) references operating_system(id) on delete cascade,"
            "unique(collection, os)"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create collection os membership table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists collection_project_membership ("
            "collection integer not null,"
            "project integer not null,"
            "foreign key(collection) references collection(id) on delete cascade,"
            "foreign key(project) references project(id) on delete cascade,"
            "unique(collection, project)"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create collection project membership table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists issue ("
            "id serial primary key not null,"
            "name text unique not null,"
            "description text,"
            "severity integer not null"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create issue table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists os_issue ("
            "os integer not null,"
            "issue integer not null,"
            "foreign key(os) references operating_system(id) on delete cascade,"
            "foreign key(issue) references issue(id) on delete cascade,"
            "unique(os, issue)"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create os issue table: " << query.lastError().text();
        return false;
    }

    query = QSqlQuery(
        "create table if not exists collection_issue ("
            "collection integer not null,"
            "issue integer not null,"
            "foreign key(collection) references collection(id) on delete cascade,"
            "foreign key(issue) references issue(id) on delete cascade,"
            "unique(collection, issue)"
        ");", db);
    if (query.lastError().type() != QSqlError::NoError) {
        qCritical() << "Failed to create os issue table: " << query.lastError().text();
        return false;
    }

    m_updateThread.start();

    return true;
}
