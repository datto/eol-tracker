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

#include "issues.h"

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

Issues::Issues(QObject *parent) : Controller(parent)
{
}

Issues::~Issues()
{
}

void Issues::index(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("select id, name, description from issue order by name asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(500);
        return;
    }
    c->stash({
        { "title", "Issue Settings | EOL Tracker" },
        { "template", "issues.html" },
        { "parent", "/settings" },
        { "activeNav", "Settings" },
        { "activeSidebar", "issues" },
        { "sidebar", QStringList(QList<QString>({ "dependencies", "issues", "platforms", "projects" })) },
        { "issues", Sql::queryToList(query) },
    });
}

void Issues::issues_create(Context* c)
{
    QString name = c->request()->bodyParam("name", "");
    if (name == "") {
        c->response()->setStatus(400);
        return;
    }
    QString description = c->request()->bodyParam("description", "");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into issue (name, description, severity) values (:name, :description, 1);", "eol");
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        qCritical() << query.lastError().text();
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

void Issues::issues_update(Context* c)
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
    QString description = c->request()->bodyParam("description", "");

    QSqlQuery query = CPreparedSqlQueryThreadForDB("update issue set name=:name, description=:description where id=:id", "eol");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

void Issues::issues_delete(Context* c)
{
    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->setStatus(400);
        return;
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from issue where id=:id", "eol");
    query.bindValue(":id", id);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

