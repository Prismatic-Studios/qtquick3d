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

#include "qquick3dparticletrailemitter_p.h"

QT_BEGIN_NAMESPACE

QQuick3DParticleTrailEmitter::QQuick3DParticleTrailEmitter(QQuick3DNode *parent)
    : QQuick3DParticleEmitter(parent)
{
}

QQuick3DParticleModelParticle* QQuick3DParticleTrailEmitter::follow() const
{
    return m_follow;
}
void QQuick3DParticleTrailEmitter::setFollow(QQuick3DParticleModelParticle *follow)
{
    if (m_follow == follow)
        return;

    m_follow = follow;
    Q_EMIT followChanged();
}

void QQuick3DParticleTrailEmitter::burst(int count)
{
    m_burstAmount += count;
}

// Called to emit set of particles
void QQuick3DParticleTrailEmitter::emitTrailParticles(QQuick3DParticleDataCurrent *d, int emitAmount)
{
    if (!m_system)
        return;

    if (!m_enabled)
        return;

    if (!d)
        return;

    QVector3D centerPos = d->position;

    for (auto particle : qAsConst(m_system->m_particles)) {
        if (particle == m_particle) {
            emitAmount = std::min(emitAmount, int(particle->maxAmount()));
            for (int i = 0; i < emitAmount; i++) {
                // Distribute evenly between previous and current time, important especially
                // when time has jumped a lot (like a starttime).
                float startTime = (m_prevEmitTime / 1000.0) + (float(1+i) / emitAmount) * ((m_system->timeInt - m_prevEmitTime) / 1000.0);
                emitParticle(particle, startTime, centerPos);
            }
        }
    }

    m_prevEmitTime = m_system->timeInt;
}

QT_END_NAMESPACE
