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

#ifndef FRONTEND_H
#define FRONTEND_H

#include "config.h"
#include "UpdateThread.h"

#include <glib-2.0/gio/gio.h>
#include <Cutelyst/Application>

using namespace Cutelyst;

class frontend : public Application
{
    Q_OBJECT
    CUTELYST_APPLICATION(IID "frontend")
public:
    Q_INVOKABLE explicit frontend(QObject *parent = nullptr);
    ~frontend();

    bool init();

    virtual bool postFork() override;

private:
    Config m_config;
    UpdateThread m_updateThread;
};

#endif //FRONTEND_H

