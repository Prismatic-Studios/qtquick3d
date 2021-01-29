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

#include "qquick3dparticletargetdirection_p.h"
#include "qquick3dparticlerandomizer_p.h"

QT_BEGIN_NAMESPACE

QQuick3DParticleTargetDirection::QQuick3DParticleTargetDirection(QObject *parent)
    : QQuick3DParticleDirection(parent)
{
}

QVector3D QQuick3DParticleTargetDirection::position() const
{
    return m_position;
}

QVector3D QQuick3DParticleTargetDirection::positionVariation() const
{
    return m_positionVariation;
}

void QQuick3DParticleTargetDirection::setPosition(const QVector3D &position)
{
    if (m_position == position)
        return;

    m_position = position;
    Q_EMIT positionChanged();
}

void QQuick3DParticleTargetDirection::setPositionVariation(const QVector3D &positionVariation)
{
    if (m_positionVariation == positionVariation)
        return;

    m_positionVariation = positionVariation;
    Q_EMIT positionVariationChanged();
}

bool QQuick3DParticleTargetDirection::normalized() const
{
    return m_normalized;
}
void QQuick3DParticleTargetDirection::setNormalized(bool normalized)
{
    if (m_normalized == normalized)
        return;

    m_normalized = normalized;
    Q_EMIT normalizedChanged();
}

float QQuick3DParticleTargetDirection::magnitude() const
{
    return m_magnitude;
}
void QQuick3DParticleTargetDirection::setMagnitude(float magnitude)
{
    if (qFuzzyCompare(m_magnitude, magnitude))
        return;

    m_magnitude = magnitude;
    Q_EMIT magnitudeChanged();
}

float QQuick3DParticleTargetDirection::magnitudeVariation() const
{
    return m_magnitudeVariation;
}
void QQuick3DParticleTargetDirection::setMagnitudeVariation(float magnitudeVariation)
{
    if (qFuzzyCompare(m_magnitudeVariation, magnitudeVariation))
        return;

    m_magnitudeVariation = magnitudeVariation;
    Q_EMIT magnitudeChangedVariation();
}

QVector3D QQuick3DParticleTargetDirection::sample(const QVector3D &from)
{
    QVector3D ret = m_position - from;
    // TODO: Pseudorandom so end result is predictable?
    ret.setX(ret.x() - m_positionVariation.x() + QPRand::get() * m_positionVariation.x() * 2);
    ret.setY(ret.y() - m_positionVariation.y() + QPRand::get() * m_positionVariation.y() * 2);
    ret.setZ(ret.z() - m_positionVariation.z() + QPRand::get() * m_positionVariation.z() * 2);
    if (m_normalized)
        ret.normalize();
    ret *= (m_magnitude - m_magnitudeVariation + QPRand::get() * m_magnitudeVariation * 2);
    return ret;
}

QT_END_NAMESPACE