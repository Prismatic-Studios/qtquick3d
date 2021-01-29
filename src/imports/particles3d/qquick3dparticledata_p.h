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

#ifndef QQUICK3DPARTICLEDATA_H
#define QQUICK3DPARTICLEDATA_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QVector3D>

QT_BEGIN_NAMESPACE

// Maximum time particles can be set to live.
// More than this means unlimited.
static const int PARTICLE_MAX_TIME = 99999;
static const int AFFECTOR_MAX_TIME = PARTICLE_MAX_TIME+1;

struct Color4ub {
    uchar r = 255;
    uchar g = 255;
    uchar b = 255;
    uchar a = 255;
};

struct Vector3b {
    char x = 0;
    char y = 0;
    char z = 0;
};

// Current particle data, only used for currently modified data, so one per system
struct QQuick3DParticleDataCurrent
{
    QVector3D position;
    QVector3D velocity;
    QVector3D rotation;
    QVector3D scale;
    Color4ub color;
};

// Particle data per particle
// Not modified after emits
struct QQuick3DParticleData
{
    QVector3D startPosition;
    QVector3D startVelocity;
    // Would this be worth it in memory consumption vs. performance vs. accuracy?
    // These would need to be actually char and not uchar as rotations can go either direction.
    Vector3b startRotation;
    Vector3b startRotationVelocity;
    Color4ub startColor;
    Color4ub endColor;
    // Seconds, system time when this particle was emitted
    float startTime;
    // Seconds, particle lifetime. >99999 means forever
    float lifetime;
    // Unified scaling among axes
    float startSize;
    float endSize;
    // Index/id of the particle. Used to get unique random values.
    // Might not be necessary, check later
    int index;
    // Size: 12+12+3+3+4+4+4+4+4+4+4 = 58 bytes
};

QT_END_NAMESPACE

#endif // QQUICK3DPARTICLEDATA_H