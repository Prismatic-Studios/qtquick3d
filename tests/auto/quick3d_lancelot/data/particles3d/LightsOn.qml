/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

import QtQuick
import QtQuick3D
import QtQuick3D.Particles3D
import QtQuick.Timeline

Item {
    id: mainWindow
    width: 400
    height: 400

    Timeline {
        id: timeline
        enabled: true
        startFrame: 0
        endFrame: 120
        animations: [
            TimelineAnimation {
                id: timelineAnimation
                running: true
                duration: 2000
                from: timeline.currentFrame
                to: 120
            }
        ]
        keyframeGroups: [
            KeyframeGroup {
                target: psystem
                property: "time"
                Keyframe { frame: 0; value: 0 }
                Keyframe { frame: 120; value: 2000 }
            }
        ]
    }

    View3D {
        anchors.fill: parent

        environment: SceneEnvironment {
            clearColor: "#202020"
            backgroundMode: SceneEnvironment.Color
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 50, 200)
        }

        DirectionalLight {
            id: lightDirectionalRed
            color: Qt.rgba(1.0, 0.0, 0.0, 1.0)
            brightness: 0.5
        }
        DirectionalLight {
            id: lightDirectionalGreen
            color: Qt.rgba(0.0, 1.0, 0.0, 1.0)
            brightness: 0.5
            // Scoped to only affect the particles
            scope: psystem
        }

        SpotLight {
            id: lightSpot
            color: Qt.rgba(0.0, 0.0, 1.0, 1.0)
            position: Qt.vector3d(0, -50, 200)
            visible: true
            brightness: 20
            coneAngle: 50
            innerConeAngle: 30
            constantFade: 4.0
            linearFade: 3.0
            quadraticFade: 2.0
        }

        PointLight {
            id: lightPoint
            color: Qt.rgba(1.0, 1.0, 0.0, 1.0)
            position: Qt.vector3d(200, 200, 10)
            visible: true
            brightness: 20
            quadraticFade: 6.0
        }

        Model {
            z: -100
            source: "#Rectangle"
            scale: Qt.vector3d(3, 2, 0)
            materials: DefaultMaterial {
                diffuseColor: "#808080"
            }
        }

        ParticleSystem3D {
            id: psystem
            useRandomSeed: false
            running: false
            y: -50

            // Particle with color table
            SpriteParticle3D {
                id: particleFire
                sprite: Texture {
                    source: "images/sphere.png"
                }
                colorTable: Texture {
                    source: "images/colorTable.png"
                }
                maxAmount: 250
                color: "#ffffff"
                billboard: true
                blendMode: SpriteParticle3D.Screen
                lights: [lightDirectionalGreen, lightSpot]
            }

            // Simple particle
            SpriteParticle3D {
                id: particleSimple
                sprite: Texture {
                    source: "images/sphere.png"
                }
                maxAmount: 30
                color: "#808080"
                billboard: true
                lights: [lightPoint]
            }

            ParticleEmitter3D {
                particle: particleFire
                position: Qt.vector3d(0, 0, 0)
                particleScale: 10
                particleScaleVariation: 1
                velocity: VectorDirection3D {
                    direction: Qt.vector3d(0, 100, 0)
                    directionVariation: Qt.vector3d(10, 10, 0)
                }
                emitRate: 125
                lifeSpan: 2000
                scale: Qt.vector3d(0.2, 0.1, 0.2)
                shape: ParticleShape3D {
                    type: ParticleShape3D.Cylinder
                }
            }
            ParticleEmitter3D {
                particle: particleSimple
                position: Qt.vector3d(80, 80, 0)
                particleScale: 10
                velocity: VectorDirection3D {
                    direction: Qt.vector3d(0, 100, 0)
                    directionVariation: Qt.vector3d(20, 20, 0)
                }
                emitRate: 10
                lifeSpan: 1500
                scale: Qt.vector3d(0.2, 0.1, 0.2)
                shape: ParticleShape3D {
                    type: ParticleShape3D.Cylinder
                }
            }
            ParticleEmitter3D {
                particle: particleSimple
                position: Qt.vector3d(-80, 80, 0)
                particleScale: 10
                velocity: VectorDirection3D {
                    direction: Qt.vector3d(0, 100, 0)
                    directionVariation: Qt.vector3d(20, 20, 0)
                }
                emitRate: 10
                lifeSpan: 1500
                scale: Qt.vector3d(0.2, 0.1, 0.2)
                shape: ParticleShape3D {
                    type: ParticleShape3D.Cylinder
                }
            }
        }
    }
}
