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

#include <QTest>
#include <QSignalSpy>
#include <QScopedPointer>

#include <QtQuick3DParticles/private/qquick3dparticleemitburst_p.h>
#include <QtQuick3DParticles/private/qquick3dparticledynamicburst_p.h>


class tst_QQuick3DParticleEmitBurst : public QObject
{
    Q_OBJECT

private slots:
    void testEmitBurst();
    void testDynamicBurst();
};

void tst_QQuick3DParticleEmitBurst::testEmitBurst()
{
    QQuick3DParticleEmitBurst *burst = new QQuick3DParticleEmitBurst();

    QCOMPARE(burst->time(), 0);
    QCOMPARE(burst->amount(), 0);
    QCOMPARE(burst->duration(), 0);

    burst->setTime(1000);
    QCOMPARE(burst->time(), 1000);

    burst->setAmount(100);
    QCOMPARE(burst->amount(), 100);

    burst->setAmount(-100);
    QCOMPARE(burst->amount(), 100);

    burst->setDuration(100);
    QCOMPARE(burst->duration(), 100);

    burst->setDuration(-100);
    QCOMPARE(burst->duration(), 100);

    delete burst;
}

void tst_QQuick3DParticleEmitBurst::testDynamicBurst()
{
    QQuick3DParticleDynamicBurst *burst = new QQuick3DParticleDynamicBurst();

    // Derived properties
    QCOMPARE(burst->time(), 0);
    QCOMPARE(burst->amount(), 0);
    QCOMPARE(burst->duration(), 0);

    QCOMPARE(burst->enabled(), true);
    QCOMPARE(burst->amountVariation(), 0);
    QCOMPARE(burst->triggerMode(), QQuick3DParticleDynamicBurst::TriggerTime);

    burst->setEnabled(false);
    QCOMPARE(burst->enabled(), false);

    burst->setAmountVariation(20);
    QCOMPARE(burst->amountVariation(), 20);

    burst->setTriggerMode(QQuick3DParticleDynamicBurst::TriggerEnd);
    QCOMPARE(burst->triggerMode(), QQuick3DParticleDynamicBurst::TriggerEnd);

    delete burst;
}

QTEST_APPLESS_MAIN(tst_QQuick3DParticleEmitBurst)
#include "tst_qquick3dparticleemitburst.moc"
