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

#ifndef ROOT_H
#define ROOT_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Root : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    explicit Root(QObject *parent = nullptr);
    ~Root();

    C_ATTR(index, :Path :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void index(Context *c);

    C_ATTR(distros_index, :Chained("/") :PathPart("distros") :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void distros_index(Context* c);

    C_ATTR(distros, :Chained("/") :PathPart("distros") :Args(1) :ActionClass("RenderView") :View("grantlee_view"))
    void distros(Context* c, const QString& collection);

    C_ATTR(view, :Chained("/") :PathPart("view") :Args(1) :ActionClass("RenderView") :View("grantlee_view"))
    void view(Context* c, const QString& collection);

    C_ATTR(data, :Local :Args(0) :ActionClass("RenderView") :View("json_view"))
    void data(Context* c);

    C_ATTR(osData, :Local :Args(0) :ActionClass("RenderView") :View("json_view"))
    void osData(Context* c);

    C_ATTR(issueData, :Local :Args(0) :ActionClass("RenderView") :View("json_view"))
    void issueData(Context* c);

    C_ATTR(defaultPage, :Path)
    void defaultPage(Context *c);

private:
    C_ATTR(End, :ActionClass("RenderView"))
    void End(Context *c) { Q_UNUSED(c); }
};

#endif //ROOT_H

