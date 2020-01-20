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


#include "RepoScan.hpp"

#include "Chroot.hpp"

#include <ctime>
#include <QtGlobal>
#include <QtDebug>

namespace eoltracker {

RepoScan::RepoScan(OperatingSystem& os, const std::filesystem::path& chrootCollectionDir) : m_os(os)
{
    qInfo() << "RepoScan init for " << m_os.getName().c_str() << " " << std::to_string(m_os.getVersion()).c_str();
    GError* err_base = nullptr;
    auto err = std::unique_ptr<GError*, std::function<void(GError**)>>(&err_base, [](GError** e) {
        if (*e != nullptr) {
            g_error_free(*e);
        }
    });
    m_context = std::unique_ptr<DnfContext, std::function<void(DnfContext*)>>(dnf_context_new(), [](DnfContext* c) {
        g_object_unref(c);
    });
    auto chroot = m_os.makeChroot(chrootCollectionDir);
    chroot.setup(); // setup just in case, can't hurt
    dnf_context_set_install_root(m_context.get(), chroot.getRootDir().c_str());
    dnf_context_set_source_root(m_context.get(), chroot.getRootDir().c_str());
    dnf_context_set_solv_dir(m_context.get(), chroot.getTmpDir().c_str());
    dnf_context_set_repo_dir(m_context.get(), chroot.getRepoDir().c_str());
    dnf_context_set_cache_dir(m_context.get(), chroot.getCacheDir().c_str());
    dnf_context_set_lock_dir(m_context.get(), "./");
    //dnf_context_set_arch(m_context.get(), "x86_64");
    if (!dnf_context_setup(m_context.get(), nullptr, err.get())) {
        throw std::runtime_error(std::string("dnf context setup: ") + (*err)->message);
    }

    auto state = dnf_context_get_state(m_context.get());
    if (!dnf_context_setup_sack(m_context.get(), state, err.get())) {
        throw std::runtime_error(std::string("dnf sack setup: ") + (*err)->message);
    }
}

RepoScan::~RepoScan()
{
}

std::unique_ptr<GPtrArray, std::function<void(GPtrArray*)>> RepoScan::getPackageList(const std::string& name)
{
    auto sack = dnf_context_get_sack(m_context.get());

    HyQuery query = hy_query_create(sack);
    hy_query_filter(query, HY_PKG_NAME, HY_EQ, name.c_str());
    auto packages = std::unique_ptr<GPtrArray, std::function<void(GPtrArray*)>>(hy_query_run(query), [](GPtrArray* ptr)
    {
        if (ptr) {
            g_ptr_array_unref(ptr);
        }
    });
    hy_query_free(query);

    return packages;
}

Package RepoScan::getPackage(Project& proj)
{
    Version newestVersion("0.0.0");
    DnfPackage* newest = nullptr; 

    std::string projName = proj.getName();
    qDebug() << "RepoScan search started for " << projName.c_str() << " for " << m_os.getName().c_str() << " " << std::to_string(m_os.getVersion()).c_str();
    std::string found = projName;
    auto packages = getPackageList(projName);
    for (unsigned int i = 0; i < packages->len; i++) {
        auto p = reinterpret_cast<DnfPackage*>(g_ptr_array_index(packages.get(), i));
        Version v(std::string(dnf_package_get_version(p)));
        if (newestVersion < v) {
            if (newest != nullptr) {
                g_object_unref(newest);
            }
            g_object_ref(p);
            newest = p;
            newestVersion = v;
        }
    } 

    auto altNames = proj.getAlternateNames();        
    int count = 0;
    for (auto name : altNames) {
        packages = getPackageList(name);
        for (unsigned int i = 0; i < packages->len; i++) {
            auto p = reinterpret_cast<DnfPackage*>(g_ptr_array_index(packages.get(), i));
            Version v(std::string(dnf_package_get_version(p)));
            if (newestVersion < v) {
                if (newest != nullptr) {
                    g_object_unref(newest);
                }
                g_object_ref(p);
                newest = p;
                newestVersion = v;
                found = name;
            }
            count++;
        } 
    }

    if (!newest) {
        throw std::runtime_error{"No packages found for project \"" + projName + "\" in " + m_os.getName() + " " + std::to_string(m_os.getVersion())};
    }

    if (found != projName) {
        qDebug() << "Altername name " << found.c_str() << " found for " << m_os.getName().c_str() << " " << std::to_string(m_os.getVersion()).c_str();
    }

    std::time_t builtUnix = dnf_package_get_buildtime(newest);
    std::tm* built = std::gmtime(&builtUnix);
    std::string timeString(25, '\0');
    strftime(timeString.data(), 25, "%Y-%m-%dT%H:%M:%S", built);
    g_object_unref(newest);
    return Package(proj, m_os, newestVersion, timeString, found);
}

} // namespace eoltracker
