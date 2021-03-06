/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Quick 3D.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquick3dcubemaptexture_p.h"
#include "qquick3dobject_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype CubeMapTexture
    \inherits Texture
    \inqmlmodule QtQuick3D
    \brief Defines a cube map texture for use in 3D scenes.

    CubeMapTexture is a Texture that represents a cube map texture. A cube map
    texture has 6 faces (X+, X-, Y+, Y-, Z+, Z-), where each face is an
    individual 2D image. CubeMapTexture allows \l{CustomMaterial}{custom
    materials} and \l{Effect}{post-processing effects} to work with cube map
    textures in their shaders.

    \qml
    CustomMaterial {
        property TextureInput customTexture: TextureInput {
            texture: CubeMapTexture {
                source: "cubemap.ktx"
            }
        }
        fragmentShader: "shader.frag"
    }
    \endqml

    Here shader.frag can be implemented assuming \c customTexture is sampler
    uniform with the GLSL type a \c samplerCube. This means that the
    \c{texture()} GLSL function takes a \c vec3 as the texture coordinate for
    that sampler. If we used \l Texture, the type would have been \c sampler2D.

    \badcode
    void MAIN()
    {
        vec4 c = texture(customTexture, NORMAL);
        BASE_COLOR = vec4(c.rgb, 1.0);
    }
    \endcode

    Sourcing a Texture from a container with a cubemap only loads face 0 (X+)
    and results in a 2D texture. Whereas sourcing a CubeMapTexture from the
    same asset loads all 6 faces and results in a cubemap texture.

    CubeMapTexture inherits all its properties from Texture. The important
    difference is that \l source must refer to a image file containing a
    cubemap. In practice this means a \l{https://www.khronos.org/ktx/}{KTX}
    container containing 6 face images.

    \note Sourcing image data via other means, such as \l sourceItem or \l
    textureData is not supported for CubeMapTexture at the moment.

    \sa Texture, CustomMaterial, Effect
*/

QQuick3DCubeMapTexture::QQuick3DCubeMapTexture(QQuick3DObject *parent)
    : QQuick3DTexture(*(new QQuick3DObjectPrivate(QQuick3DObjectPrivate::Type::ImageCube)), parent)
{
}

QQuick3DCubeMapTexture::~QQuick3DCubeMapTexture()
{
}

QT_END_NAMESPACE
