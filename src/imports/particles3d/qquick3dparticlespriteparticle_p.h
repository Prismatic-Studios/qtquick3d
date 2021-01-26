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

#ifndef QQUICK3DSPRITEPARTICLE_H
#define QQUICK3DSPRITEPARTICLE_H

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

#include <QColor>
#include <QVector4D>

#include "qquick3dparticle_p.h"
#include "qquick3dparticlesystem_p.h"
#include "qquick3dparticledata_p.h"

QT_BEGIN_NAMESPACE

class QQuick3DParticleSpriteParticle : public QQuick3DParticle
{
    Q_OBJECT
    Q_PROPERTY(Lighting lighting READ lighting WRITE setLighting NOTIFY lightingChanged)
    Q_PROPERTY(BlendMode blendMode READ blendMode WRITE setBlendMode NOTIFY blendModeChanged)

    Q_PROPERTY(QQuick3DTexture* sprite READ sprite WRITE setSprite NOTIFY spriteChanged)
    Q_PROPERTY(int spriteImages READ spriteImages WRITE setSpriteImages NOTIFY spriteImagesChanged)
    Q_PROPERTY(bool blendImages READ blendImages WRITE setBlendImages NOTIFY blendImagesChanged)
    Q_PROPERTY(bool receivesShadows READ receivesShadows WRITE setReceivesShadows NOTIFY receivesShadowsChanged)
    QML_NAMED_ELEMENT(SpriteParticle3D)

public:
    enum SpriteImages { Gradient = 0, SingleImage = 1 };
    Q_ENUM(SpriteImages)

    enum Lighting { NoLighting = 0, VertexLighting, FragmentLighting };
    Q_ENUM(Lighting)

    enum BlendMode { SourceOver = 0, Screen, Multiply };
    Q_ENUM(BlendMode)

    QQuick3DParticleSpriteParticle(QQuick3DNode *parent = nullptr);
    ~QQuick3DParticleSpriteParticle() override;

    Lighting lighting() const;
    BlendMode blendMode() const;
    QQuick3DTexture *sprite() const;
    int spriteImages() const;
    bool blendImages() const;
    bool receivesShadows() const;

public Q_SLOTS:
    void setLighting(Lighting lighting);
    void setBlendMode(BlendMode blendMode);
    void setSprite(QQuick3DTexture *sprite);
    void setSpriteImages(int imageCount);
    void setBlendImages(bool blend);
    void setReceivesShadows(bool receive);

Q_SIGNALS:
    void lightingChanged();
    void blendModeChanged();
    void spriteChanged();
    void spriteImagesChanged();
    void blendImagesChanged();
    void receivesShadowsChanged();

protected:
    void itemChange(ItemChange, const ItemChangeData &) override;
    QSSGRenderGraphObject *updateSpatialNode(QSSGRenderGraphObject *node) override;
    void reset() override;

    void componentComplete() override;
    void handleMaxAmountChanged(int amount);
    void setParticleData(int particleIndex,
                         const QVector3D &position,
                         const QVector3D &rotation,
                         const QVector4D &color,
                         float size, float age);
    void commitParticles()
    {
        markAllDirty();
        update();
    }

private:
    friend class QQuick3DParticleSystem;
    friend class QQuick3DParticleEmitter;

    struct SpriteParticleData
    {
        QVector3D position;
        QVector3D rotation;
        QVector4D color;
        float size;
        float age;
    };

    void updateSceneManager(QQuick3DSceneManager *window);
    void updateParticleBuffer();

    QHash<QByteArray, QMetaObject::Connection> m_connections;
    Lighting m_lighting = VertexLighting;
    BlendMode m_blendMode = SourceOver;
    QQuick3DTexture *m_sprite = nullptr;
    int m_spriteImageCount = SingleImage;
    bool m_blendImages = true;
    bool m_receivesShadows = true;
    bool m_bufferUpdated = false;
    bool m_dirty = true;
    QVector<SpriteParticleData> m_spriteParticleData;

    const float c_MaxParticleSize = 100.0f;
};

QT_END_NAMESPACE

#endif
