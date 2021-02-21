/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#include "qquick3dparticleshape_p.h"
#include "qquick3dparticlerandomizer_p.h"
#include "qquick3dparticlesystem_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype ParticleShape3D
    \inherits QtObject
    \inqmlmodule QtQuick3D.Particles3D
    \brief Offers 3D shapes for emitters and affectors.

    The ParticleShape3D element supports shapes like \c Cube, \c Sphere and \c Cylinder for particles needs.
    For example, emitter can use \l {ParticleEmitter3D::shape}{shape} property to emit particles from the
    shape area.

    Shapes don't have position, scale or rotation. Instead, they use parent node for these properties.
    When the shape parent is not a node, you can use \l ShapeNode3D.
*/

QQuick3DParticleShape::QQuick3DParticleShape(QObject *parent)
    : QObject(parent)
{
    m_parentNode = qobject_cast<QQuick3DNode*>(parent);
}

/*!
    \qmlproperty bool ParticleShape3D::fill

    This property defines if the shape should be filled or just use the shape outlines.

    The default value is \c true.
*/
bool QQuick3DParticleShape::fill() const
{
    return m_fill;
}

void QQuick3DParticleShape::setFill(bool fill)
{
    if (m_fill == fill)
        return;

    m_fill = fill;
    Q_EMIT fillChanged();
}

/*!
    \qmlproperty ShapeType ParticleShape3D::type

    This property defines the type of the shape.

    The default value is \c ParticleShape3D.Cube.
*/

/*!
    \qmlproperty enumeration ParticleShape3D::ShapeType

    Defines the type of the shape.

    \value ParticleShape3D.Cube
        Cube shape.
    \value ParticleShape3D.Sphere
        Sphere shape.
    \value ParticleShape3D.Cylinder
        Cylinder shape.
*/

QQuick3DParticleShape::ShapeType QQuick3DParticleShape::type() const
{
    return m_type;
}

void QQuick3DParticleShape::setType(QQuick3DParticleShape::ShapeType type)
{
    if (m_type == type)
        return;

    m_type = type;
    Q_EMIT typeChanged();
}

void QQuick3DParticleShape::componentComplete()
{
    m_parentNode = qobject_cast<QQuick3DNode*>(parent());
    if (!m_parentNode) {
        qWarning() << "Shape requires parent Node to function correctly!";
    }
}

QVector3D QQuick3DParticleShape::randomPosition(int particleIndex) const
{
    if (!m_parentNode || !m_system)
        return QVector3D();

    switch (m_type) {
    case QQuick3DParticleShape::ShapeType::Cube:
        return randomPositionCube(particleIndex);
    case QQuick3DParticleShape::ShapeType::Sphere:
        return randomPositionSphere(particleIndex);
    case QQuick3DParticleShape::ShapeType::Cylinder:
        return randomPositionCylinder(particleIndex);
    default:
        Q_ASSERT(false);
    }
    return QVector3D();
}

QVector3D QQuick3DParticleShape::randomPositionCube(int particleIndex) const
{
    auto rand = m_system->rand();
    QVector3D s = m_parentNode->sceneScale() * 50.0f;
    float x = s.x() - (rand->get(particleIndex, QPRand::Shape1) * s.x() * 2.0f);
    float y = s.y() - (rand->get(particleIndex, QPRand::Shape2) * s.y() * 2.0f);
    float z = s.z() - (rand->get(particleIndex, QPRand::Shape3) * s.z() * 2.0f);
    if (!m_fill) {
        // Random 0..5 for cube sides
        int side = int(rand->get(particleIndex, QPRand::Shape4) * 6);
        if (side == 0)
            x = -s.x();
        else if (side == 1)
            x = s.x();
        else if (side == 2)
            y = -s.y();
        else if (side == 3)
            y = s.y();
        else if (side == 4)
            z = -s.z();
        else
            z = s.z();
    }
    QMatrix4x4 mat;
    mat.rotate(m_parentNode->rotation());
    return mat.mapVector(QVector3D(x, y, z));
}

QVector3D QQuick3DParticleShape::randomPositionSphere(int particleIndex) const
{
    auto rand = m_system->rand();
    QVector3D scale = m_parentNode->sceneScale();
    float theta = rand->get(particleIndex, QPRand::Shape1) * float(M_PI) * 2.0f;
    float v = rand->get(particleIndex, QPRand::Shape2);
    float phi = acos((2.0f * v) - 1.0f);
    float r = m_fill ? pow(rand->get(particleIndex, QPRand::Shape3), 1.0f / 3.0f) : 1.0f;
    float x = r * sin(phi) * cos(theta);
    float y = r * sin(phi) * sin(theta);
    float z = r * cos(phi);
    QVector3D pos(x, y, z);
    pos *= (scale * 50.0f);
    QMatrix4x4 mat;
    mat.rotate(m_parentNode->rotation());
    return mat.mapVector(pos);
}

QVector3D QQuick3DParticleShape::randomPositionCylinder(int particleIndex) const
{
    auto rand = m_system->rand();
    QVector3D scale = m_parentNode->sceneScale();
    float y = (scale.y() * 50.0f) - (rand->get(particleIndex, QPRand::Shape1) * scale.y() * 100.0f);
    float r = 1.0f;
    if (m_fill)
        r = sqrt(rand->get(particleIndex, QPRand::Shape2));
    float theta = rand->get(particleIndex, QPRand::Shape3) * float(M_PI) * 2.0f;
    float x = r * cos(theta);
    float z = r * sin(theta);
    x = x * (scale.x() * 50.0f);
    z = z * (scale.z() * 50.0f);
    QVector3D pos(x, y, z);
    QMatrix4x4 mat;
    mat.rotate(m_parentNode->rotation());
    return mat.mapVector(pos);
}

QT_END_NAMESPACE