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

#include "settings.h"

#include "OperatingSystem.hpp"
#include "Version.hpp"

#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>

using namespace Cutelyst;

Settings::Settings(QObject *parent) : Controller(parent)
{
}

Settings::~Settings()
{
}

void Settings::index(Context *c)
{
    c->response()->redirect(c->uriFor(CActionFor("projects")));
}

static QString supportString(int val) {
    switch (val)
    {
        case 0:
            return "Updates";
        case 1:
            return "No Updates";
        case 2:
            return "No Release Yet";
        default:
            // do nothing
            return "";
    }
}

void
Settings::projects(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");
    QVariantMap table;

    {
        QSqlQuery colQuery = CPreparedSqlQueryThreadForDB("select collection.id, collection.name from collection", "eol");
        if (!colQuery.exec()) {
            c->response()->setStatus(500);
            return;
        }
        int idField = colQuery.record().indexOf("collection.id");
        int nameField = colQuery.record().indexOf("collection.name");
        while (colQuery.next()) {
            table[colQuery.value(nameField).toString()] = QVariant(QList<QVariant>({ colQuery.value(idField), QVariant(QVariantMap({})), QVariant(QVariantMap({})), QVariant(QVariantMap({})) }));
        }
    }

    {
        QSqlQuery osQuery = CPreparedSqlQueryThreadForDB(
            "select collection.id, collection.name, operating_system.id, operating_system.name, operating_system.version, collection_os_membership.supportStatus from collection "
            "inner join collection_os_membership on collection_os_membership.collection=collection.id "
            "inner join operating_system on operating_system.id=collection_os_membership.os "
            "order by collection.name asc, operating_system.name asc, operating_system.birth asc", "eol");
        if (!osQuery.exec()) {
            c->response()->setStatus(500);
            return;
        }
        int idField = osQuery.record().indexOf("collection.id");
        int nameField = osQuery.record().indexOf("collection.name");
        int osIdField = osQuery.record().indexOf("operating_system.id");
        int osField = osQuery.record().indexOf("operating_system.name");
        int versionField = osQuery.record().indexOf("operating_system.version");
        int supportField = osQuery.record().indexOf("collection_os_membership.supportStatus");
        while (osQuery.next())
        {
            QString fullname = osQuery.value(osField).toString() + " " + osQuery.value(versionField).toString();
            QString supportStatus = supportString(osQuery.value(supportField).toInt());
            QString key = osQuery.value(nameField).toString();
            auto thing = table[key].toList()[1].toMap();
            thing[fullname] = QVariantList({osQuery.value(osIdField), supportStatus});
            auto thing2 = table[key].toList();
            thing2[1] = thing;
            table[key] = thing2;
        }

        QSqlQuery projQuery = CPreparedSqlQueryThreadForDB(
            "select collection.id, collection.name, project.id, project.name from collection "
            "inner join collection_project_membership on collection_project_membership.collection=collection.id "
            "inner join project on project.id=collection_project_membership.project "
            "order by collection.name asc, project.name asc", "eol");
        if (!projQuery.exec()) {
            c->response()->setStatus(500);
            return;
        }
        idField = projQuery.record().indexOf("collection.id");
        nameField = projQuery.record().indexOf("collection.name");
        auto projIdField = projQuery.record().indexOf("project.id");
        auto projField = projQuery.record().indexOf("project.name");
        while (projQuery.next()) {
            QString proj = projQuery.value(projField).toString();
            QString key = projQuery.value(nameField).toString();
            auto thing = table[key].toList()[2].toMap();
            thing[proj] = QVariantList({projQuery.value(projIdField)});
            auto thing2 = table[key].toList();
            thing2[2] = thing;
            table[key] = thing2;
        }

        QSqlQuery issueQuery = CPreparedSqlQueryThreadForDB(
            "select collection.id, collection.name, issue.id, issue.name from collection "
            "inner join collection_issue on collection_issue.collection=collection.id "
            "inner join issue on issue.id=collection_issue.issue "
            "order by collection.name asc, issue.name asc", "eol");
        if (!issueQuery.exec()) {
            c->response()->setStatus(500);
            return;
        }
        idField = issueQuery.record().indexOf("collection.id");
        nameField = issueQuery.record().indexOf("collection.name");
        auto issueIdField = issueQuery.record().indexOf("issue.id");
        auto issueField = issueQuery.record().indexOf("issue.name");
        while (issueQuery.next()) {
            QString issue = issueQuery.value(issueField).toString();
            QString key = issueQuery.value(nameField).toString();
            auto thing = table[key].toList()[3].toMap();
            thing[issue] = QVariantList({issueQuery.value(issueIdField)});
            auto thing2 = table[key].toList();
            thing2[3] = thing;
            table[key] = thing2;
        }
    }

    QVariantMap platformList;
    {
        QSqlQuery osList = CPreparedSqlQueryThreadForDB("select id, name, version from operating_system;", "eol");
        if (!osList.exec()) {
            c->response()->setStatus(500);
            return;
        }
        auto osIdField = osList.record().indexOf("operating_system.id");
        auto osField = osList.record().indexOf("operating_system.name");
        auto versionField = osList.record().indexOf("operating_system.version");
        while (osList.next())
        {
            QString fullname = osList.value(osField).toString() + " " + osList.value(versionField).toString();
            platformList[fullname] = osList.value(osIdField);
        }
    }

    QVariantMap depList;
    {
        QSqlQuery packageList = CPreparedSqlQueryThreadForDB("select id, name from project;", "eol");
        if (!packageList.exec()) {
            c->response()->setStatus(500);
            return;
        }
        auto depIdField = packageList.record().indexOf("project.id");
        auto depField = packageList.record().indexOf("project.name");
        while (packageList.next())
        {
            depList[packageList.value(depField).toString()] = packageList.value(depIdField);
        }
    }

    QVariantMap issueList;
    {
        QSqlQuery issueQ = CPreparedSqlQueryThreadForDB("select id, name from issue;", "eol");
        if (!issueQ.exec()) {
            c->response()->setStatus(500);
            return;
        }
        auto issueIdField = issueQ.record().indexOf("issue.id");
        auto issueField = issueQ.record().indexOf("issue.name");
        while (issueQ.next())
        {
            issueList[issueQ.value(issueField).toString()] = issueQ.value(issueIdField);
        }
    }

    c->stash({
        { "title", "Collection Settings | EOL Tracker" },
        { "template", "collections.html" },
        { "parent", "/settings" },
        { "activeNav", "Settings" },
        { "activeSidebar", "projects" },
        { "sidebar", QStringList(QList<QString>({ "dependencies", "issues", "platforms", "projects" })) },
        { "collections", table },
        { "platforms", platformList },
        { "dependencies", depList },
        { "issues", issueList },
    });
}

void Settings::projects_edit(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");
    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->body() = "Missing id";
        c->response()->setStatus(400);
        return;
    }

    auto name = c->request()->bodyParam("name", "");
    if (name != "") {
        c->response()->setJsonBody("{\n\t\"name\": \"" + name + "\"\n}");
        QSqlQuery query = CPreparedSqlQueryThreadForDB("update collection set name=:name where id=:id", "eol");
        query.bindValue(":id", id);
        query.bindValue(":name", name);
        if (!query.exec()) {
            c->response()->body() = "Can't set name: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }

    auto platform = c->request()->bodyParam("platform", "");
    if (platform != "") {
        auto osId = c->request()->bodyParam("osid", "");
        if (osId == "") {
            c->response()->body() = "Missing osid";
            c->response()->setStatus(400);
            return;
        }

        QSqlQuery osName = CPreparedSqlQueryThreadForDB("select * from operating_system where id=:id", "eol");
        osName.bindValue(":id", platform);
        if (!osName.exec()) {
            c->response()->body() = "Can't get os: " + osName.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
        osName.next();
        c->response()->setJsonBody("{\n\t\"osid\": \"" + platform + "\",\n\t\"platform\": \""
            + osName.value(osName.record().indexOf("name")).toString() + " "
            + osName.value(osName.record().indexOf("version")).toString() + "\"\n}");

        QSqlQuery query = CPreparedSqlQueryThreadForDB("update collection_os_membership set os=:os where os=:old and collection=:id", "eol");
        query.bindValue(":id", id);
        query.bindValue(":old", osId);
        query.bindValue(":os", platform);
        if (!query.exec()) {
            c->response()->body() = "Can't set os: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }

    auto supportStatus = c->request()->bodyParam("supportStatus", "");
    if (supportStatus != "") {
        auto osId = c->request()->bodyParam("osid", "");
        if (osId == "") {
            c->response()->body() = "Missing osid";
            c->response()->setStatus(400);
            return;
        }
        c->response()->setJsonBody("{\n\t\"supportStatus\": \"" + supportString(supportStatus.toInt()) + "\"\n}");

        QSqlQuery query = CPreparedSqlQueryThreadForDB("update collection_os_membership set supportStatus=:status where os=:os and collection=:id", "eol");
        query.bindValue(":id", id);
        query.bindValue(":os", osId);
        query.bindValue(":status", supportStatus);
        if (!query.exec()) {
            c->response()->body() = "Can't set support status: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }

    auto dep = c->request()->bodyParam("dependency", "");
    if (dep != "") {
        auto depId = c->request()->bodyParam("depid", "");
        if (depId == "") {
            c->response()->body() = "Missing depid";
            c->response()->setStatus(400);
            return;
        }

        QSqlQuery projName = CPreparedSqlQueryThreadForDB("select * from project where id=:id", "eol");
        projName.bindValue(":id", dep);
        if (!projName.exec()) {
            c->response()->body() = "Can't get dependency: " + projName.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
        projName.next();
        c->response()->setJsonBody("{\n\t\"depid\": \"" + dep + "\",\n\t\"dependency\": \""
            + projName.value(projName.record().indexOf("name")).toString() + "\"\n}");

        QSqlQuery query = CPreparedSqlQueryThreadForDB("update collection_project_membership set project=:proj where project=:old and collection=:id", "eol");
        query.bindValue(":id", id);
        query.bindValue(":old", depId);
        query.bindValue(":proj", dep);
        if (!query.exec()) {
            c->response()->body() = "Can't set dependency: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }

    auto issue = c->request()->bodyParam("issue", "");
    if (issue != "") {
        auto issueId = c->request()->bodyParam("issueid", "");
        if (issueId == "") {
            c->response()->body() = "Missing issueid";
            c->response()->setStatus(400);
            return;
        }

        QSqlQuery issueName = CPreparedSqlQueryThreadForDB("select * from issue where id=:id", "eol");
        issueName.bindValue(":id", issue);
        if (!issueName.exec()) {
            c->response()->body() = "Can't get issue: " + issueName.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
        issueName.next();
        c->response()->setJsonBody("{\n\t\"issueid\": \"" + issue + "\",\n\t\"issue\": \""
            + issueName.value(issueName.record().indexOf("name")).toString() + "\"\n}");

        QSqlQuery query = CPreparedSqlQueryThreadForDB("update collection_issue set issue=:issue where issue=:old and collection=:id", "eol");
        query.bindValue(":id", id);
        query.bindValue(":old", issueId);
        query.bindValue(":issue", issue);
        if (!query.exec()) {
            c->response()->body() = "Can't set issue: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }

    c->response()->setStatus(200);
}

void Settings::projects_create(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    QString name = c->request()->bodyParam("name", "");
    if (name == "") {
        c->response()->setStatus(400);
        return;
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into collection (name) values (:name);", "eol");
    query.bindValue(":name", name);
    if (!query.exec()) {
        c->response()->body() = "Server Error: " + query.lastError().text().toUtf8();
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("projects")));
}

void Settings::projects_delete(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->body() = "Invalid id";
        c->response()->setStatus(400);
        return;
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from collection where id=:id", "eol");
    query.bindValue(":id", id);
    if (!query.exec()) {
        c->response()->body() = "Server Error: " + query.lastError().text().toUtf8();
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("projects")));
}

void Settings::projects_add(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");
    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->body() = "Missing id";
        c->response()->setStatus(400);
        return;
    }

    auto platform = c->request()->bodyParam("platform", "");
    auto supportStatus = c->request()->bodyParam("supportStatus", "");
    auto dependency = c->request()->bodyParam("dependency", "");
    auto issue = c->request()->bodyParam("issue", "");
    if (platform != "") {
        if (supportStatus == "") {
            supportStatus = "0";
        }
        QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into collection_os_membership (collection, os, supportStatus) values (:id, :osid, :support);", "eol");
        query.bindValue(":id", id);
        query.bindValue(":osid", platform);
        query.bindValue(":support", supportStatus);
        if (!query.exec()) {
            c->response()->body() = "Can't create platform relationship: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }
    else if (dependency != "") {
        QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into collection_project_membership (collection, project) values (:id, :depid);", "eol");
        query.bindValue(":id", id);
        query.bindValue(":depid", dependency);
        if (!query.exec()) {
            c->response()->body() = "Can't create dependency relationship: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }
    else if (issue != "") {
        QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into collection_issue (collection, issue) values (:id, :issueid);", "eol");
        query.bindValue(":id", id);
        query.bindValue(":issueid", issue);
        if (!query.exec()) {
            c->response()->body() = "Can't create issue relationship: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
            return;
        }
    }
    else {
        c->response()->setStatus(501);
    }
}

void Settings::projects_remove(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->body() = "Missing id";
        c->response()->setStatus(400);
        return;
    }

    auto osId = c->request()->bodyParam("osid", "");
    if (osId != "") {
        QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from collection_os_membership where collection=:id and os=:osid", "eol");
        query.bindValue(":id", id);
        query.bindValue(":osid", osId);
        if (!query.exec()) {
            c->response()->body() = "Can't delete os: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
        }
        return;
    }

    auto depId = c->request()->bodyParam("depid", "");
    if (depId != "") {
        QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from collection_project_membership where collection=:id and project=:depid", "eol");
        query.bindValue(":id", id);
        query.bindValue(":depid", depId);
        if (!query.exec()) {
            c->response()->body() = "Can't delete dependency: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
        }
        return;
    }

    auto issueId = c->request()->bodyParam("issueid", "");
    if (issueId != "") {
        QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from collection_issue where collection=:id and issue=:issueid", "eol");
        query.bindValue(":id", id);
        query.bindValue(":issueid", issueId);
        if (!query.exec()) {
            c->response()->body() = "Can't delete issue: " + query.lastError().text().toUtf8();
            c->response()->setStatus(500);
        }
        return;
    }

    c->response()->setStatus(501);
}
