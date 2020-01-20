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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Settings : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    C_ATTR(index, :Path("/settings") :AutoArgs :ActionClass("RenderView") :View("grantlee_view"))
    void index(Context *c);

    C_ATTR(projects, :Path("/settings/projects") :AutoArgs :ActionClass("RenderView") :View("grantlee_view"))
    void projects(Context* c);

    C_ATTR(projects_create, :Path("/settings/projects/create") :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void projects_create(Context* c);

    C_ATTR(projects_delete, :Path("/settings/projects/delete") :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void projects_delete(Context* c);

    C_ATTR(projects_edit, :Path("/settings/projects/edit") :Args(0) :ActionClass("RenderView") :View("json_view"))
    void projects_edit(Context* c);

    C_ATTR(projects_add, :Path("/settings/projects/add") :Args(0) :ActionClass("RenderView") :View("json_view"))
    void projects_add(Context* c);

    C_ATTR(projects_remove, :Path("/settings/projects/remove") :Args(0) :ActionClass("RenderView") :View("json_view"))
    void projects_remove(Context* c);
};

#endif //SETTINGS_H

