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

const path = require('path');

module.exports = {
    entry: './js/timeline.js',
    mode: 'none',
    output: {
        filename: 'main.js',
        path: path.resolve(__dirname, 'root/static/js'),
        libraryTarget: 'var',
        library: 'EOLTracker',
    },
    module: {
        rules: [
            {
                test: /node_modules[\\\/]vis[\\\/].*\.js$/,
                loader: 'babel-loader',
                query: {
                    cacheDirectory: true,
                    presets: [ "@babel/preset-env" ].map(require.resolve),
                    plugins: [
                        //"transform-es3-property-literals", // #2452
                        //"transform-es3-member-expression-literals", // #2566
                        "@babel/plugin-transform-runtime" // #2566
                    ],
                    sourceType: 'unambiguous'
                }
            },
            {
                test: /\.css$/,
                use: [
                    'style-loader',
                    'css-loader'
                ]
            },
            {
                test: /.*\.png$/i,
                loaders: [
                    'file-loader', {
                        loader: 'image-webpack-loader',
                        query: {
                            progressive: true,
                            pngquant: {
                                quality: '65-90',
                                speed: 4
                            }
                        }
                    }
                ]
            }
        ]
    }
};
