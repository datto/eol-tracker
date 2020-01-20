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

#include "dependencies.h"

#include <sstream>

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

Dependencies::Dependencies(QObject *parent) : Controller(parent)
{
}

Dependencies::~Dependencies()
{
}

void Dependencies::index(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("select id, name, alt from project order by name asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(500);
        return;
    }
    int idField = query.record().indexOf("id");
    int nameField = query.record().indexOf("name");
    int altField = query.record().indexOf("alt");
    QVariantList table;
    while (query.next()) {
        QStringList alternateNames;
        std::stringstream stream(query.value(altField).toString().toStdString());
        std::string tmp;
        while (std::getline(stream, tmp, ':')) {
            alternateNames.push_back(QString(tmp.c_str()));
        }
        table.push_back(QList<QVariant>({query.value(idField).toString(), query.value(nameField).toString(), alternateNames}));
    }
    c->stash({
        { "title", "Project Settings | EOL Tracker" },
        { "template", "projects.html" },
        { "parent", "/settings" },
        { "activeNav", "Settings" },
        { "activeSidebar", "dependencies" },
        { "sidebar", QStringList(QList<QString>({ "dependencies", "issues", "platforms", "projects" })) },
        { "projects", table },
    });
}

void Dependencies::create(Context* c)
{
    QString name = c->request()->bodyParam("name", "");
    if (name == "") {
        c->response()->setStatus(400);
        return;
    }
    QString alt = c->request()->bodyParam("alt", "");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into project (name, alt) values (:name, :alt);", "eol");
    query.bindValue(":name", name);
    query.bindValue(":alt", alt);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

void Dependencies::update(Context* c)
{
    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->setStatus(400);
        return;
    }

    QString name = c->request()->bodyParam("name", "");
    if (name == "") {
        c->response()->setStatus(400);
        return;
    }
    QString alt = c->request()->bodyParam("alt", "");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("update project set name=:name, alt=:alt where id=:id", "eol");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":alt", alt);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

void Dependencies::remove(Context* c)
{
    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->setStatus(400);
        return;
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from project where id=:id", "eol");
    query.bindValue(":id", id);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

