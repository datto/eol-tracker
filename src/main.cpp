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

#include <QCoreApplication>
#include <Cutelyst/WSGI/wsgi.h>
#include <memory>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    auto myapp = std::make_unique<frontend>();
    auto wsgi = std::make_unique<CWSGI::WSGI>();
    wsgi->setHttpSocket({
        {QStringLiteral(":3000")},
    });
    wsgi->setUpgradeH2c(true);
    wsgi->setBufferSize(16393);
    wsgi->setHttp2Socket({
        {QStringLiteral(":3001")},
    });
    wsgi->setFastcgiSocket({
        {QStringLiteral(":3002")},
    });
    wsgi->setMaster(true);
    wsgi->setAutoReload(true);
    wsgi->setLazy(true);
    wsgi->exec(myapp.get());

    return 0;
}
