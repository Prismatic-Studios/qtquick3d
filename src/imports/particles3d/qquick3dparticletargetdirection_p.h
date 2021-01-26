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

#ifndef QQUICK3DPARTICLETARGETDIRECTION_H
#define QQUICK3DPARTICLETARGETDIRECTION_H

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
#include "qquick3dparticledirection_p.h"
#include <QQmlEngine>

QT_BEGIN_NAMESPACE

class QQuick3DParticleTargetDirection : public QQuick3DParticleDirection
{
    Q_OBJECT
    // Target position.
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D positionVariation READ positionVariation WRITE setPositionVariation NOTIFY positionVariationChanged)
    // When set to true, direction is normalized and strength comes from magnitude & magnitudeVariation
    // Default value false.
    Q_PROPERTY(bool normalized READ normalized WRITE setNormalized NOTIFY normalizedChanged)
    // Magnitude of direction. When normalized is false, this is proportional to target position.
    // Magnitude can be also negetive to move away from the target position.
    // Default value 1.0.
    Q_PROPERTY(float magnitude READ magnitude WRITE setMagnitude NOTIFY magnitudeChanged)
    // Magnitude variation of direction. When normalized is false, this is proportional to target position.
    // Default value 0.0 (no variation).
    Q_PROPERTY(float magnitudeVariation READ magnitudeVariation WRITE setMagnitudeVariation NOTIFY magnitudeChangedVariation)
    QML_NAMED_ELEMENT(TargetDirection3D)

public:
    QQuick3DParticleTargetDirection(QObject *parent = nullptr);

    QVector3D position() const;
    void setPosition(const QVector3D &position);
    bool normalized() const;
    float magnitude() const;
    float magnitudeVariation() const;

public Q_SLOTS:
    QVector3D positionVariation() const;
    void setPositionVariation(const QVector3D &positionVariation);
    void setNormalized(bool normalized);
    void setMagnitude(float magnitude);
    void setMagnitudeVariation(float magnitudeVariation);

Q_SIGNALS:
    void positionChanged();
    void positionVariationChanged();
    void normalizedChanged();
    void magnitudeChanged();
    void magnitudeChangedVariation();

private:
    QVector3D sample(const QVector3D &from) override;
    QVector3D m_position;
    QVector3D m_positionVariation;
    bool m_normalized = false;
    float m_magnitude = 1.0f;
    float m_magnitudeVariation = 0.0f;

};

QT_END_NAMESPACE

#endif // QQUICK3DPARTICLETARGETDIRECTION_H
