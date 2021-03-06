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

// There are three monkey things here, with two spheres on top.
// The left sphere uses mip level 0 and just colorizes a bit.
// The right sphere samples a different mip level (SCREEN_MIP_TEXTURE).

Rectangle {
    width: 400
    height: 400
    color: "#444845"

    View3D {
        id: v3d
        anchors.fill: parent

//        environment: SceneEnvironment {
//            clearColor: "#444845"
//            backgroundMode: SceneEnvironment.Color
//        }

        camera: camera

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 0, 600)
        }

        DirectionalLight {
            position: Qt.vector3d(-500, 500, -100)
            ambientColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
        }

        Model {
            source: "#Rectangle"
            y: -200
            scale: Qt.vector3d(5, 5, 5)
            eulerRotation.x: -90
            materials: [
                CustomMaterial {
                    shadingMode: CustomMaterial.Shaded
                    vertexShader: "customdiffusespecular.vert"
                    fragmentShader: "customdiffuse.frag"
                    property real uTime: 0.0
                    property real uAmplitude: 0.0
                    property color uDiffuse: "white"
                    property real uShininess: 50
                }
            ]
        }

        Model {
            source: "../shared/models/monkey_object.mesh"
            scale: Qt.vector3d(80, 80, 80)
            eulerRotation.y: 90
            x: -200
            materials: [
                CustomMaterial {
                    fragmentShader: "worldnormal.frag"
                    cullMode: CustomMaterial.NoCulling
                }
            ]
        }

        Model {
            source: "../shared/models/monkey_object.mesh"
            scale: Qt.vector3d(80, 80, 80)
            eulerRotation.y: 90
            materials: [
                CustomMaterial {
                    fragmentShader: "worldnormal.frag"
                    cullMode: CustomMaterial.NoCulling
                }
            ]
        }

        Model {
            source: "../shared/models/monkey_object.mesh"
            scale: Qt.vector3d(80, 80, 80)
            eulerRotation.y: 90
            x: 200
            materials: [
                CustomMaterial {
                    fragmentShader: "worldnormal.frag"
                    cullMode: CustomMaterial.NoCulling
                }
            ]
        }

        Model {
            source: "#Sphere"
            scale: Qt.vector3d(4, 4, 4)
            z: 100
            x: -200
            materials: [
                CustomMaterial {
                    shadingMode: CustomMaterial.Shaded
                    sourceBlend: CustomMaterial.SrcAlpha
                    destinationBlend: CustomMaterial.OneMinusSrcAlpha
                    fragmentShader: "customscreenmiptexture.frag"
                    property real mipLevel: 0
                }
            ]
        }

        Model {
            source: "#Sphere"
            scale: Qt.vector3d(4, 4, 4)
            z: 100
            x: 200
            materials: [
                CustomMaterial {
                    shadingMode: CustomMaterial.Shaded
                    sourceBlend: CustomMaterial.SrcAlpha
                    destinationBlend: CustomMaterial.OneMinusSrcAlpha
                    fragmentShader: "customscreenmiptexture.frag"
                    property real mipLevel: 3
                }
            ]
        }
    }
}
