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

#include "oses.h"

#include <filesystem>
#include <unistd.h>
#include <QtSql>
#include <Cutelyst/Plugins/Utils/Sql>
#include <Cutelyst/Upload>

using namespace Cutelyst;

Oses::Oses(QObject *parent) : Controller(parent)
{
}

Oses::~Oses()
{
}

void Oses::index(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");
    QSqlQuery query = CPreparedSqlQueryThreadForDB("select id, name, version, birth, death, repoFile from operating_system order by name asc, birth asc", "eol");
    if (!query.exec()) {
        c->response()->setStatus(500);
        return;
    }
    c->stash({
        { "title", "OS Settings | EOL Tracker" },
        { "template", "oses.html" },
        { "parent", "/settings" },
        { "activeNav", "Settings" },
        { "activeSidebar", "platforms" },
        { "sidebar", QStringList(QList<QString>({ "dependencies", "issues", "platforms", "projects" })) },
        { "oses", Sql::queryToList(query) },
    });
}

void Oses::oses_create(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    QString name = c->request()->bodyParam("name", "");
    if (name == "") {
        c->response()->setStatus(400);
        return;
    }
    QString version = c->request()->bodyParam("version", "");
    if (version == "") {
        c->response()->setStatus(400);
        return;
    }
    QString birth = c->request()->bodyParam("birth", "") + "T00:00:00";
    if (birth == "") {
        c->response()->setStatus(400);
        return;
    }
    QString death = c->request()->bodyParam("death", "") + "T00:00:00";
    if (death == "") {
        c->response()->setStatus(400);
        return;
    }
    Upload* repoFile = c->request()->upload("repo");
    auto filename = name + " " + version + ".repo";
    if (repoFile)
    {
        auto path = std::filesystem::path("root/static/media") / filename.toStdString();
        unlink(path.c_str());
        if (!repoFile->save(QString(path.c_str())))
        {
            c->response()->setStatus(500);
            return;
        }
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("insert into operating_system (name, version, birth, death) values (:name, :version, :birth, :death) on conflict do nothing", "eol");
    if (repoFile) {
        query = CPreparedSqlQueryThreadForDB("insert into operating_system (name, version, birth, death, repoFile) values (:name, :version, :birth, :death, :repo) on conflict do nothing", "eol");
        query.bindValue(":repo", filename);
    }
    query.bindValue(":name", name);
    query.bindValue(":version", version);
    query.bindValue(":birth", birth);
    query.bindValue(":death", death);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

void Oses::oses_update(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

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
    QString version = c->request()->bodyParam("version", "");
    if (version == "") {
        c->response()->setStatus(400);
        return;
    }
    QString birth = c->request()->bodyParam("birth", "") + "T00:00:00";
    if (birth == "") {
        c->response()->setStatus(400);
        return;
    }
    QString death = c->request()->bodyParam("death", "") + "T00:00:00";
    if (death == "") {
        c->response()->setStatus(400);
        return;
    }
    auto repoFile = c->request()->upload("repo");
    auto filename = name + " " + version + ".repo";
    if (repoFile)
    {
        auto path = std::filesystem::path(WEBROOT "/static/media") / filename.toStdString();
        unlink(path.c_str());
        if (!repoFile->save(QString(path.c_str())))
        {
            c->response()->setStatus(500);
            return;
        }
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("update operating_system set name=:name, version=:version, birth=:birth, death=:death where id=:id", "eol");
    if (repoFile) {
        query = CPreparedSqlQueryThreadForDB("update operating_system set name=:name, version=:version, birth=:birth, death=:death, repoFile=:repo where id=:id", "eol");
        query.bindValue(":repo", filename);
    }
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":version", version);
    query.bindValue(":birth", birth);
    query.bindValue(":death", death);
    if (!query.exec())
    {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}

void Oses::oses_delete(Context* c)
{
    c->response()->setHeader("Cache-Control", "no-store");

    auto id = c->request()->bodyParam("id", "");
    if (id == "") {
        c->response()->body() = "Invalid id";
        c->response()->setStatus(400);
        return;
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB("delete from operating_system where id=:id", "eol");
    query.bindValue(":id", id);
    if (!query.exec()) {
        c->response()->body() = "Server Error";
        c->response()->setStatus(500);
        return;
    }

    c->response()->redirect(c->uriFor(CActionFor("index")));
}
