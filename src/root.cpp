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


#include "root.h"

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

Root::Root(QObject *parent) : Controller(parent)
{
}

Root::~Root()
{
}

void Root::index(Context *c)
{
    c->response()->setHeader("Cache-Control", "no-store");
    QSqlQuery query = CPreparedSqlQueryThreadForDB("select name from collection order by name asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(400);
        return;
    }
    if (query.next())
    {
        int nameField = query.record().indexOf("name");
        auto defaultCollection = query.value(nameField).toString();
        c->response()->redirect("view/" + defaultCollection);
    }
    else {
        c->response()->redirect(c->uriFor(CActionFor("settings")));
    }
}

void Root::distros_index(Context* c)
{
    QSqlQuery query = CPreparedSqlQueryThreadForDB("select name from collection order by name asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(400);
        return;
    }
    int nameField = query.record().indexOf("name");
    if (query.next())
    {
        int nameField = query.record().indexOf("name");
        auto defaultCollection = query.value(nameField).toString();
        c->response()->redirect("distros/" + defaultCollection);
    }
    else {
        c->response()->redirect(c->uriFor(CActionFor("settings")));
    }
}

void Root::distros(Context* c, const QString& col)
{
    c->response()->setHeader("Cache-Control", "no-store");
    QSqlQuery query = CPreparedSqlQueryThreadForDB("select name from collection order by name asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(400);
        return;
    }
    int nameField = query.record().indexOf("name");
    QStringList entries;
    while (query.next())
    {
        entries.push_back(query.value(nameField).toString());
    }

    auto collection = col;
    if (collection == "")
    {
        collection = entries.front();
    }

    c->stash({
        {"template", "distros.html"},
        {"parent", "/distros"},
        {"title", "Distros | EOL Tracker"},
        {"activeNav", "Distros"},
        {"activeSidebar", collection},
        {"sidebar", entries},
    });
}

void Root::view(Context* c, const QString& col)
{
    c->response()->setHeader("Cache-Control", "no-store");
    QSqlQuery query = CPreparedSqlQueryThreadForDB("select name from collection order by name asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(400);
        return;
    }
    int nameField = query.record().indexOf("name");
    QStringList entries;
    while (query.next())
    {
        entries.push_back(query.value(nameField).toString());
    }

    auto collection = col;
    if (collection == "")
    {
        collection = entries.front();
    }

    c->stash({
        {"template", "view.html"},
        {"parent", "/view"},
        {"title", collection + " | EOL Tracker"},
        {"activeNav", "Collections"},
        {"activeSidebar", collection},
        {"sidebar", entries},
    });
}

void Root::data(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");
    
    auto collection = c->request()->queryParam("collection", "");
    if (collection == "") {
        c->response()->setStatus(400);
        return;
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB(
        "select project.name, package.version, operating_system.name, operating_system.version, date(operating_system.birth), date(operating_system.death), package.foundName from collection "
        "inner join collection_project_membership on collection_project_membership.collection=collection.id "
        "inner join collection_os_membership on collection_os_membership.collection=collection.id "
        "inner join project on project.id=collection_project_membership.project "
        "inner join operating_system on operating_system.id=collection_os_membership.os "
        "inner join package on package.project=project.id and operating_system.id=package.os "
        "where collection.name=:collectionName "
        "order by project.name asc, operating_system.name asc, operating_system.birth asc, package.version asc", "eol");
    query.bindValue(":collectionName", collection);
    if (query.exec())
    {
        auto array = Sql::queryToJsonArray(query);
        std::transform(array.begin(), array.end(), array.begin(), [](const QJsonValue& v) -> QJsonValue {
            return QJsonValue(QJsonObject({
                {"name",        v[0]},
                {"version",     v[1]},
                {"os",          v[2]},
                {"os_version",  v[3]},
                {"start",       v[4]},
                {"end",         v[5]},
                {"found",       v[6]},
            }));
        });
        c->response()->setJsonArrayBody(array);
    }
    else {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
    }
}

void Root::issueData(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    auto os = c->request()->queryParam("os", "");
    if (os == "") {
        c->response()->setStatus(400);
        return;
    }

    auto collection = c->request()->queryParam("collection", "");
    if (collection == "") {
        c->response()->setStatus(400);
        return;
    }
    
    QSqlQuery query = CPreparedSqlQueryThreadForDB(
        "select issue.name, issue.description, issue.severity from os_issue "
        "left join collection_issue on os_issue.issue=collection_issue.issue "
        "left join collection on collection.id=collection_issue.collection "
        "left join issue on issue.id=os_issue.issue "
        "where os_issue.os=:os and collection.name=:collection "
        "order by issue.name asc", "eol");
    query.bindValue(":os", os);
    query.bindValue(":collection", collection);
    if (query.exec())
    {
        auto array = Sql::queryToJsonArray(query);
        std::transform(array.begin(), array.end(), array.begin(), [](const QJsonValue& v) -> QJsonValue {
            return QJsonValue(QJsonObject({
                {"name",        v[0]},
                {"description", v[1]},
                {"difficulty",  v[2]},
            }));
        });
        c->response()->setJsonArrayBody(array);
    }
    else {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
    }
}

void Root::osData(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    auto collection = c->request()->queryParam("collection", "");
    if (collection == "") {
        c->response()->setStatus(400);
        return;
    }
    
    QSqlQuery query = CPreparedSqlQueryThreadForDB(
        "select operating_system.id, operating_system.name, operating_system.version, date(operating_system.birth), date(operating_system.death), collection_os_membership.supportStatus, COALESCE(SUM(issue.severity),0) from operating_system "
        "inner join collection_os_membership on operating_system.id=collection_os_membership.os "
        "inner join collection on collection_os_membership.collection=collection.id "
        "left join collection_issue on collection.id=collection_issue.collection "
        "left join os_issue on operating_system.id=os_issue.os and collection_issue.issue=os_issue.issue "
        "left join issue on issue.id=os_issue.issue "
        "where collection.name=:collectionName "
        "group by operating_system.id, collection_os_membership.supportStatus "
        "order by operating_system.death asc", "eol");
    query.bindValue(":collectionName", collection);
    if (query.exec())
    {
        auto array = Sql::queryToJsonArray(query);
        std::transform(array.begin(), array.end(), array.begin(), [](const QJsonValue& v) -> QJsonValue {
            return QJsonValue(QJsonObject({
                {"id",              v[0]},
                {"name",            v[1]},
                {"version",         v[2]},
                {"start",           v[3]},
                {"end",             v[4]},
                {"supportStatus",   v[5]},
                {"difficulty",      v[6]},
            }));
        });
        c->response()->setJsonArrayBody(array);
    }
    else {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
    }
}

void Root::defaultPage(Context *c)
{
    c->response()->body() = "Page not found!";
    c->response()->setStatus(404);
}
