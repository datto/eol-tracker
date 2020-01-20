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

#ifndef OSES_H
#define OSES_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Oses : public Controller
{
    Q_OBJECT
public:
    explicit Oses(QObject *parent = nullptr);
    ~Oses();

    C_ATTR(index, :Path("/settings/platforms") :AutoArgs :ActionClass("RenderView") :View("grantlee_view"))
    void index(Context* c);

    C_ATTR(oses_create, :Path("/settings/platforms/create") :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void oses_create(Context* c);

    C_ATTR(oses_update, :Path("/settings/platforms/update") :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void oses_update(Context* c);

    C_ATTR(oses_delete, :Path("/settings/platforms/delete") :Args(0) :ActionClass("RenderView") :View("grantlee_view"))
    void oses_delete(Context* c);

};

#endif //OSES_H

