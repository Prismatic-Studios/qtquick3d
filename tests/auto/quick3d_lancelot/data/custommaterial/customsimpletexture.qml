/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tests of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick3D
import QtQuick

Rectangle {
    width: 400
    height: 400
    color: "#444845"

    View3D {
        id: v3d
        anchors.fill: parent

        environment: SceneEnvironment {
            backgroundMode: SceneEnvironment.Transparent
        }

        camera: camera

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 0, 600)
        }

        DirectionalLight {
            position: Qt.vector3d(-500, 500, -100)
            color: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            ambientColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
        }

        property real time: 10
        property real amplitude: 4

        // texture disabled (result should be black)
        Model {
            source: "#Sphere"
            scale: Qt.vector3d(2, 2, 2)
            x: -200
            materials: [
                CustomMaterial {
                    property alias time: v3d.time
                    property alias amplitude: v3d.amplitude
                    vertexShader: "customsimpletexture.vert"
                    fragmentShader: "customsimpletexture.frag"
                    property TextureInput tex1: TextureInput {
                        enabled: false
                    }
                }
            ]
        }

        // texture enabled but no actual Texture is given (should survive with
        // dummy texture, result is expected to be black)
        Model {
            source: "#Sphere"
            scale: Qt.vector3d(2, 2, 2)
            materials: [
                CustomMaterial {
                    property alias time: v3d.time
                    property alias amplitude: v3d.amplitude
                    vertexShader: "customsimpletexture.vert"
                    fragmentShader: "customsimpletexture.frag"
                    property TextureInput tex1: TextureInput {
                        enabled: true
                    }
                }
            ]
        }

        Model {
            source: "#Sphere"
            scale: Qt.vector3d(2, 2, 2)
            x: 200
            materials: [
                CustomMaterial {
                    property alias time: v3d.time
                    property alias amplitude: v3d.amplitude
                    vertexShader: "customsimpletexture.vert"
                    fragmentShader: "customsimpletexture.frag"
                    property TextureInput tex1: TextureInput {
                        enabled: true
                        texture: Texture {
                            source: "../shared/maps/oulu_2.jpeg"
                        }
                    }
                }
            ]
        }

        // sample two textures
        Model {
            source: "#Cube"
            scale: Qt.vector3d(1.5, 1.5, 1.5)
            y: -200
            materials: [
                CustomMaterial {
                    property real time: 0
                    property real amplitude: 0
                    vertexShader: "customsimpletexture.vert"
                    fragmentShader: "customsimpletexture2.frag"
                    property TextureInput tex1: TextureInput {
                        enabled: true
                        texture: Texture {
                            source: "../shared/maps/oulu_2.jpeg"
                        }
                    }
                    property TextureInput tex2: TextureInput {
                        enabled: true
                        texture: Texture {
                            source: "../shared/maps/rgba.png"
                        }
                    }
                }
            ]
        }

        // not so simple after all: combine SCREEN_TEXTURE with another texture
        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(1.5, 1.5, 1.5)
            y: 200
            materials: [
                CustomMaterial {
                    property real time: 0
                    property real amplitude: 0
                    vertexShader: "customsimpletexture.vert"
                    fragmentShader: "customsimpletexturescreen.frag"
                    property TextureInput tex1: TextureInput {
                        enabled: true
                        texture: Texture {
                            source: "../shared/maps/oulu_2.jpeg"
                        }
                    }
                }
            ]
        }
    }
}
