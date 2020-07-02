/****************************************************************************
**
** Copyright (C) 2008-2012 NVIDIA Corporation.
** Copyright (C) 2019 The Qt Company Ltd.
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

#include "qssgrendercustommaterialsystem_p.h"

#include <QtQuick3DUtils/private/qssgutils_p.h>

#include <QtQuick3DRuntimeRender/private/qssgrendercustommaterialrendercontext_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendercontextcore_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderbuffermanager_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderresourcemanager_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendermesh_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendercamera_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderlight_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderlayer_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderableimage_p.h>
#include <QtQuick3DRuntimeRender/private/qssgvertexpipelineimpl_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendererimpllayerrenderdata_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendermodel_p.h>
#include <QtQuick3DRuntimeRender/private/qssgruntimerenderlogging_p.h>

#include <QtCore/qbitarray.h>

QT_BEGIN_NAMESPACE

QSSGCustomMaterialVertexPipeline::QSSGCustomMaterialVertexPipeline(QSSGRenderContextInterface *inContext)
    : QSSGVertexPipelineBase(inContext->shaderProgramGenerator())
    , m_context(inContext)
{
}

// Responsible for beginning all vertex and fragment generation (void main() { etc).
void QSSGCustomMaterialVertexPipeline::beginVertexGeneration()
{
    QSSGShaderGeneratorStageFlags theStages(QSSGProgramGenerator::defaultFlags());

    programGenerator()->beginProgram(theStages);


    QSSGStageGeneratorBase &vertexShader(vertex());

    // thinks we need
    vertexShader.addInclude("viewProperties.glsllib");
    vertexShader.addInclude("customMaterial.glsllib");

    vertexShader.addIncoming("attr_pos", "vec3");
    vertexShader << "void main()"
                 << "\n"
                 << "{"
                 << "\n";

    vertexShader.addUniform("modelViewProjection", "mat4");
    vertexShader.append("\tgl_Position = modelViewProjection * vec4(attr_pos, 1.0);");
}

void QSSGCustomMaterialVertexPipeline::beginFragmentGeneration()
{
    fragment().addUniform("objectOpacity", "float");
    fragment() << "void main()"
               << "\n"
               << "{"
               << "\n";
}

void QSSGCustomMaterialVertexPipeline::assignOutput(const QByteArray &inVarName, const QByteArray &inVarValue)
{
    vertex() << "\t" << inVarName << " = " << inVarValue << ";\n";
}

void QSSGCustomMaterialVertexPipeline::generateUVCoords(quint32 inUVSet, const QSSGShaderDefaultMaterialKey &inKey)
{
    if (inUVSet == 0 && setCode(GenerationFlag::UVCoords))
        return;
    if (inUVSet == 1 && setCode(GenerationFlag::UVCoords1))
        return;

    Q_ASSERT(inUVSet == 0 || inUVSet == 1);

    if (inUVSet == 0)
        addInterpolationParameter("varTexCoord0", "vec3");
    else if (inUVSet == 1)
        addInterpolationParameter("varTexCoord1", "vec3");

    doGenerateUVCoords(inUVSet, inKey);
}

void QSSGCustomMaterialVertexPipeline::generateWorldNormal(const QSSGShaderDefaultMaterialKey &inKey)
{
    if (setCode(GenerationFlag::WorldNormal))
        return;
    addInterpolationParameter("varNormal", "vec3");
    doGenerateWorldNormal(inKey);
}

void QSSGCustomMaterialVertexPipeline::generateObjectNormal()
{
    if (setCode(GenerationFlag::ObjectNormal))
        return;
    doGenerateObjectNormal();
}

void QSSGCustomMaterialVertexPipeline::generateVarTangentAndBinormal(const QSSGShaderDefaultMaterialKey &inKey)
{
    if (setCode(GenerationFlag::TangentBinormal))
        return;
    addInterpolationParameter("varTangent", "vec3");
    addInterpolationParameter("varBinormal", "vec3");
    addInterpolationParameter("varObjTangent", "vec3");
    addInterpolationParameter("varObjBinormal", "vec3");
    doGenerateVarTangentAndBinormal(inKey);
}

void QSSGCustomMaterialVertexPipeline::generateWorldPosition()
{
    if (setCode(GenerationFlag::WorldPosition))
        return;

    activeStage().addUniform("modelMatrix", "mat4");
    addInterpolationParameter("varWorldPos", "vec3");
    addInterpolationParameter("varObjPos", "vec3");
    doGenerateWorldPosition();
}

// responsible for closing all vertex and fragment generation
void QSSGCustomMaterialVertexPipeline::endVertexGeneration(bool customShader)
{
    if (!customShader)
        vertex().append("}");
}

void QSSGCustomMaterialVertexPipeline::endFragmentGeneration(bool customShader)
{
    if (!customShader)
        fragment().append("}");
}

QSSGStageGeneratorBase &QSSGCustomMaterialVertexPipeline::activeStage()
{
    return vertex();
}

void QSSGCustomMaterialVertexPipeline::addInterpolationParameter(const QByteArray &inName, const QByteArray &inType)
{
    m_interpolationParameters.insert(inName, inType);
    vertex().addOutgoing(inName, inType);
    fragment().addIncoming(inName, inType);
}

void QSSGCustomMaterialVertexPipeline::doGenerateUVCoords(quint32 inUVSet, const QSSGShaderDefaultMaterialKey &inKey)
{
    Q_ASSERT(inUVSet == 0 || inUVSet == 1);

    Q_UNUSED(inKey); // ###

    if (inUVSet == 0) {
        vertex().addIncoming("attr_uv0", "vec2");
        vertex() << "\tvec3 texCoord0 = vec3( attr_uv0, 0.0 );"
                 << "\n";
        assignOutput("varTexCoord0", "texCoord0");
    } else if (inUVSet == 1) {
        vertex().addIncoming("attr_uv1", "vec2");
        vertex() << "\tvec3 texCoord1 = vec3( attr_uv1, 1.0 );"
                 << "\n";
        assignOutput("varTexCoord1", "texCoord1");
    }
}

void QSSGCustomMaterialVertexPipeline::doGenerateWorldNormal(const QSSGShaderDefaultMaterialKey &inKey)
{
    Q_UNUSED(inKey); // ###

    QSSGStageGeneratorBase &vertexGenerator(vertex());
    vertexGenerator.addIncoming("attr_norm", "vec3");
    vertexGenerator.addUniform("normalMatrix", "mat3");
    vertex().append("\tvarNormal = normalize( normalMatrix * attr_norm );");
}

void QSSGCustomMaterialVertexPipeline::doGenerateObjectNormal()
{
    addInterpolationParameter("varObjectNormal", "vec3");
    vertex().append("\tvarObjectNormal = attr_norm;");
}

void QSSGCustomMaterialVertexPipeline::doGenerateWorldPosition()
{
    vertex().append("\tvarObjPos = attr_pos;");
    vertex().append("\tvec4 worldPos = (modelMatrix * vec4(attr_pos, 1.0));");
    assignOutput("varWorldPos", "worldPos.xyz");
}

void QSSGCustomMaterialVertexPipeline::doGenerateVarTangentAndBinormal(const QSSGShaderDefaultMaterialKey &inKey)
{
    Q_UNUSED(inKey); // ###

    vertex().addIncoming("attr_textan", "vec3");
    vertex().addIncoming("attr_binormal", "vec3");

    vertex() << "\tvarTangent = normalMatrix * attr_textan;"
             << "\n"
             << "\tvarBinormal = normalMatrix * attr_binormal;"
             << "\n";

    vertex() << "\tvarObjTangent = attr_textan;"
             << "\n"
             << "\tvarObjBinormal = attr_binormal;"
             << "\n";
}

void QSSGCustomMaterialVertexPipeline::doGenerateVertexColor(const QSSGShaderDefaultMaterialKey &inKey)
{
    Q_UNUSED(inKey); // ###

    vertex().addIncoming("attr_color", "vec3");
    vertex().append("\tvarColor = attr_color;");
}

struct QSSGShaderMapKey
{
    QByteArray m_name;
    ShaderFeatureSetList m_features;
    QSSGShaderDefaultMaterialKey m_materialKey;
    size_t m_hashCode;
    QSSGShaderMapKey(const QByteArray &inName,
                     const ShaderFeatureSetList &inFeatures,
                     QSSGShaderDefaultMaterialKey inMaterialKey)
        : m_name(inName), m_features(inFeatures), m_materialKey(inMaterialKey)
    {
        m_hashCode = qHash(m_name) ^ hashShaderFeatureSet(m_features)
                ^ qHash(inMaterialKey.hash());
    }
    bool operator==(const QSSGShaderMapKey &inKey) const
    {
        return m_name == inKey.m_name && m_features == inKey.m_features
                && m_materialKey == inKey.m_materialKey;
    }
};

size_t qHash(const QSSGShaderMapKey &key)
{
    return key.m_hashCode;
}

QSSGMaterialSystem::QSSGMaterialSystem(QSSGRenderContextInterface *ct)
    : context(ct)
{
}

QSSGMaterialSystem::~QSSGMaterialSystem()
{
}

QSSGLayerGlobalRenderProperties QSSGMaterialSystem::getLayerGlobalRenderProperties(QSSGCustomMaterialRenderContext &inRenderContext)
{
    const QSSGRenderLayer &theLayer = inRenderContext.layer;
    const QSSGLayerRenderData &theData = inRenderContext.layerData;

    QVector<QVector3D> tempDirection;

    const bool isYUpInFramebuffer = context->rhiContext()->isValid()
            ? context->rhiContext()->rhi()->isYUpInFramebuffer()
            : true;

    return QSSGLayerGlobalRenderProperties{ theLayer,
                const_cast<QSSGRenderCamera &>(inRenderContext.camera),
                theData.cameraDirection,
                theData.shadowMapManager,
                inRenderContext.rhiDepthTexture,
                inRenderContext.rhiAoTexture,
                theLayer.lightProbe,
                theLayer.lightProbe2,
                theLayer.probeHorizon,
                theLayer.probeBright,
                theLayer.probe2Window,
                theLayer.probe2Pos,
                theLayer.probe2Fade,
                theLayer.probeFov,
                isYUpInFramebuffer };
}

bool QSSGMaterialSystem::prepareForRender(const QSSGRenderModel &,
                                          const QSSGRenderSubset &,
                                          QSSGRenderCustomMaterial &inMaterial)
{
    return inMaterial.isDirty();
}

void QSSGMaterialSystem::setRenderContextInterface(QSSGRenderContextInterface *inContext)
{
    context = inContext;
}

QSSGRef<QSSGRhiShaderStagesWithResources> QSSGMaterialSystem::prepareRhiShader(const QSSGRenderContextInterface &renderContext,
                                                                               QSSGCustomMaterialRenderContext &inRenderContext,
                                                                               const QSSGRenderCustomMaterial &inMaterial,
                                                                               const ShaderFeatureSetList &inFeatureSet)
{
    const QSSGShaderMapKey skey = QSSGShaderMapKey(inMaterial.m_shaderPathKey,
                                                   inFeatureSet,
                                                   inRenderContext.materialKey);

    QSSGShaderPreprocessorFeature noFragOutputFeature("NO_FRAG_OUTPUT", true);
    ShaderFeatureSetList features(inFeatureSet);
    features.push_back(noFragOutputFeature);

    QSSGRef<QSSGRhiShaderStagesWithResources> result;
    auto it = rhiShaderMap.find(skey);
    if (it == rhiShaderMap.end()) {
        const auto &theMaterialGenerator(context->customMaterialShaderGenerator());
        QSSGCustomMaterialVertexPipeline thePipeline(context);
        QSSGRef<QSSGRhiShaderStages> shaderStages = theMaterialGenerator->generateRhiShaderStages(renderContext,
                                                                                                  inMaterial,
                                                                                                  inRenderContext.materialKey,
                                                                                                  thePipeline,
                                                                                                  features,
                                                                                                  inRenderContext.lights,
                                                                                                  inRenderContext.firstImage,
                                                                                                  inMaterial.m_hasTransparency,
                                                                                                  QByteArrayLiteral("custom material pipeline-- "),
                                                                                                  inMaterial.m_shaderPathKey);
        if (shaderStages)
            result = QSSGRhiShaderStagesWithResources::fromShaderStages(shaderStages);
        // insert it no matter what, no point in trying over and over again
        rhiShaderMap.insert(skey, result);
    } else {
        result = it.value();
    }
    return result;
}

static const QRhiShaderResourceBinding::StageFlags VISIBILITY_ALL =
        QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage;

void QSSGMaterialSystem::rhiPrepareRenderable(QSSGCustomMaterialRenderContext &customMaterialContext,
                                              const ShaderFeatureSetList &featureSet,
                                              QSSGRhiGraphicsPipelineState *ps,
                                              QSSGCustomMaterialRenderable &renderable)
{
    const QSSGRenderCustomMaterial &material(customMaterialContext.material);
    QSSGRef<QSSGRhiShaderStagesWithResources> shaderPipeline;

    shaderPipeline = prepareRhiShader(*context,
                                      customMaterialContext,
                                      material,
                                      featureSet);
    if (shaderPipeline)
        shaderPipeline->resetExtraTextures();

    QRhiGraphicsPipeline::TargetBlend blend; // no blending by default
    if (material.m_hasTransparency && material.m_hasBlending) {
        blend.enable = true;
        blend.srcColor = material.m_srcBlend;
        blend.srcAlpha = material.m_srcBlend;
        blend.dstColor = material.m_dstBlend;
        blend.dstAlpha = material.m_dstBlend;
    }

    const QSSGCullFaceMode cullMode = material.m_cullMode;

    if (shaderPipeline) {
        ps->shaderStages = shaderPipeline->stages();

        const auto &rhiCtx = context->rhiContext();
        const QMatrix4x4 clipSpaceCorrMatrix = rhiCtx->rhi()->clipSpaceCorrMatrix();

        const auto &materialGenerator = context->customMaterialShaderGenerator();
        // FIXME: this is null bones.
        // It should be replaced with custom material's boneTransforms
        QSSGDataView<QMatrix4x4> boneGlobals;
        QSSGDataView<QMatrix3x3> boneNormals;
        materialGenerator->setRhiMaterialProperties(*context,
                                                    shaderPipeline,
                                                    ps,
                                                    material,
                                                    QVector2D(1.0, 1.0),
                                                    customMaterialContext.modelViewProjection,
                                                    customMaterialContext.normalMatrix,
                                                    customMaterialContext.modelMatrix,
                                                    clipSpaceCorrMatrix,
                                                    boneGlobals,
                                                    boneNormals,
                                                    customMaterialContext.firstImage,
                                                    customMaterialContext.opacity,
                                                    getLayerGlobalRenderProperties(customMaterialContext),
                                                    customMaterialContext.lights);

        //shaderPipeline->dumpUniforms();

        QRhiCommandBuffer *cb = rhiCtx->commandBuffer();

        ps->samples = rhiCtx->mainPassSampleCount();

        ps->cullMode = QSSGRhiGraphicsPipelineState::toCullMode(cullMode);

        ps->targetBlend = blend;

        ps->ia = customMaterialContext.subset.rhi.ia;
        ps->ia.bakeVertexInputLocations(*shaderPipeline);

        QRhiResourceUpdateBatch *resourceUpdates = rhiCtx->rhi()->nextResourceUpdateBatch();
        QSSGRhiUniformBufferSet &uniformBuffers(rhiCtx->uniformBufferSet({ &customMaterialContext.layer,
                                                                           &customMaterialContext.model,
                                                                           &material,
                                                                           QSSGRhiUniformBufferSetKey::Main }));
        shaderPipeline->bakeMainUniformBuffer(&uniformBuffers.ubuf, resourceUpdates);

        // non-area lights
        shaderPipeline->bakeLightsUniformBuffer(QSSGRhiShaderStagesWithResources::LightBuffer0,
                                                &uniformBuffers.lightsUbuf0,
                                                resourceUpdates);
        // area lights
        shaderPipeline->bakeLightsUniformBuffer(QSSGRhiShaderStagesWithResources::LightBuffer1,
                                                &uniformBuffers.lightsUbuf1,
                                                resourceUpdates);

        QRhiTexture *dummyTexture = rhiCtx->dummyTexture({}, resourceUpdates);
        QRhiTexture *dummyCubeTexture = rhiCtx->dummyTexture(QRhiTexture::CubeMap, resourceUpdates);

        cb->resourceUpdate(resourceUpdates);

        QSSGRhiContext::ShaderResourceBindingList bindings;

        bindings.append(QRhiShaderResourceBinding::uniformBuffer(0, VISIBILITY_ALL, uniformBuffers.ubuf));

        // cbBufferLights
        bindings.append(QRhiShaderResourceBinding::uniformBuffer(1, VISIBILITY_ALL, uniformBuffers.lightsUbuf0));

        // cbBufferAreaLights
        bindings.append(QRhiShaderResourceBinding::uniformBuffer(2, VISIBILITY_ALL, uniformBuffers.lightsUbuf1));

        QVector<QShaderDescription::InOutVariable> samplerVars =
                shaderPipeline->stages()->fragmentStage()->shader().description().combinedImageSamplers();
        int maxSamplerBinding = -1;
        for (const QShaderDescription::InOutVariable &var : samplerVars)
            maxSamplerBinding = qMax(maxSamplerBinding, var.binding);

        // Will need to set unused image-samplers to something dummy
        // because the shader code contains all custom property textures,
        // and not providing a binding for all of them is invalid with some
        // graphics APIs (and will need a real texture because setting a
        // null handle or similar is not permitted with some of them so the
        // srb does not accept null QRhiTextures either; but first let's
        // figure out what bindings are unused in this frame)
        QBitArray samplerBindingsSpecified(maxSamplerBinding + 1);

        if (shaderPipeline->lightProbeTexture()) {
            int binding = shaderPipeline->bindingForTexture(QByteArrayLiteral("lightProbe"));
            if (binding >= 0) {
                samplerBindingsSpecified.setBit(binding);
                QPair<QSSGRenderTextureCoordOp, QSSGRenderTextureCoordOp> tiling = shaderPipeline->lightProbeTiling();
                QRhiSampler *sampler = rhiCtx->sampler({ QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::Linear, // enables mipmapping
                                                         toRhi(tiling.first), toRhi(tiling.second) });
                bindings.append(QRhiShaderResourceBinding::sampledTexture(binding,
                                                                          QRhiShaderResourceBinding::FragmentStage,
                                                                          shaderPipeline->lightProbeTexture(), sampler));
            } else {
                qWarning("Could not find sampler for light probe");
            }
        }

        if (shaderPipeline->depthTexture()) {
            int binding = shaderPipeline->bindingForTexture(QByteArrayLiteral("depthTexture"));
            if (binding >= 0) {
                samplerBindingsSpecified.setBit(binding);
                // nearest min/mag, no mipmap
                QRhiSampler *sampler = rhiCtx->sampler({ QRhiSampler::Nearest, QRhiSampler::Nearest, QRhiSampler::None,
                                                         QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge });
                bindings.append(QRhiShaderResourceBinding::sampledTexture(binding,
                                                                          QRhiShaderResourceBinding::FragmentStage,
                                                                          shaderPipeline->depthTexture(), sampler));
            } // else ignore, not an error
        }

        if (shaderPipeline->ssaoTexture()) {
            int binding = shaderPipeline->bindingForTexture(QByteArrayLiteral("aoTexture"));
            if (binding >= 0) {
                samplerBindingsSpecified.setBit(binding);
                // linear min/mag, no mipmap
                QRhiSampler *sampler = rhiCtx->sampler({ QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None,
                                                         QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge });
                bindings.append(QRhiShaderResourceBinding::sampledTexture(binding,
                                                                          QRhiShaderResourceBinding::FragmentStage,
                                                                          shaderPipeline->ssaoTexture(), sampler));
            } // else ignore, not an error
        }

        QVarLengthArray<QRhiShaderResourceBinding::TextureAndSampler, 16> texSamplers;
        QRhiSampler *dummySampler = rhiCtx->sampler({ QRhiSampler::Nearest, QRhiSampler::Nearest, QRhiSampler::None,
                                                      QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge });

        for (int i = 0, ie = shaderPipeline->shadowMapArrayCount(); i < ie; ++i) {
            QSSGRhiShadowMapArrayProperties &p(shaderPipeline->shadowMapArrayAt(i));
            if (p.shadowMapTextures.isEmpty())
                continue;
            if (p.cachedBinding < 0) {
                const QVector<int> *arrayDims = nullptr;
                p.cachedBinding = shaderPipeline->bindingForTexture(p.shadowMapArrayUniformName, &arrayDims);
                if (arrayDims && !arrayDims->isEmpty()) {
                    p.shaderArrayDim = arrayDims->first();
                } else {
                    qWarning("No array dimension for array of shadow map textures '%s', this should not happen.",
                             p.shadowMapArrayUniformName.constData());
                    continue;
                }
            }
            if (p.cachedBinding < 0) {
                qWarning("No combined image sampler for array of shadow map textures '%s'",
                         p.shadowMapArrayUniformName.constData());
                continue;
            }
            QRhiSampler *sampler = rhiCtx->sampler({ QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None,
                                                     QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge });
            samplerBindingsSpecified.setBit(p.cachedBinding);
            texSamplers.clear();
            for (QRhiTexture *texture : p.shadowMapTextures)
                texSamplers.append({ texture, sampler });
            // fill the rest with dummy texture-sampler pairs (the array must always be fully specified)
            if (texSamplers.count() < p.shaderArrayDim) {
                for (int dummyIdx = texSamplers.count(); dummyIdx < p.shaderArrayDim; ++dummyIdx)
                    texSamplers.append({ p.isCubemap ? dummyCubeTexture : dummyTexture, dummySampler });
            }
            bindings.append(QRhiShaderResourceBinding::sampledTextures(p.cachedBinding,
                                                                       QRhiShaderResourceBinding::FragmentStage,
                                                                       texSamplers.count(),
                                                                       texSamplers.constData()));
        }

        if (maxSamplerBinding >= 0) {
            int customTexCount = shaderPipeline->extraTextureCount();
            for (int i = 0; i < customTexCount; ++i) {
                const QSSGRhiTexture &t(shaderPipeline->extraTextureAt(i));
                const int samplerBinding = shaderPipeline->bindingForTexture(t.name);
                if (samplerBinding >= 0) {
                    samplerBindingsSpecified.setBit(samplerBinding);
                    QRhiSampler *sampler = rhiCtx->sampler(t.samplerDesc);
                    bindings.append(QRhiShaderResourceBinding::sampledTexture(samplerBinding,
                                                                              QRhiShaderResourceBinding::FragmentStage,
                                                                              t.texture,
                                                                              sampler));
                }
            }

            for (const QShaderDescription::InOutVariable &var : samplerVars) {
                QRhiTexture *t = var.type == QShaderDescription::SamplerCube ? dummyCubeTexture : dummyTexture;
                texSamplers.clear();
                const int count = var.arrayDims.isEmpty() ? 1 : var.arrayDims.first();
                for (int i = 0; i < count; ++i)
                    texSamplers.append({ t, dummySampler });
                if (!samplerBindingsSpecified.testBit(var.binding)) {
                    bindings.append(QRhiShaderResourceBinding::sampledTextures(var.binding,
                                                                               QRhiShaderResourceBinding::FragmentStage,
                                                                               texSamplers.count(),
                                                                               texSamplers.constData()));
                }
            }
        }

        QRhiShaderResourceBindings *srb = rhiCtx->srb(bindings);

        const QSSGGraphicsPipelineStateKey pipelineKey { *ps, rhiCtx->mainRenderPassDescriptor(), srb };
        renderable.rhiRenderData.mainPass.pipeline = rhiCtx->pipeline(pipelineKey);
        renderable.rhiRenderData.mainPass.srb = srb;
    }
}

void QSSGMaterialSystem::setShaderResources(const QSSGRenderCustomMaterial &inMaterial,
                                            const QByteArray &inPropertyName,
                                            const QVariant &propertyValue,
                                            QSSGRenderShaderDataType inPropertyType,
                                            const QSSGRef<QSSGRhiShaderStagesWithResources> &shaderPipeline)
{
    Q_UNUSED(inMaterial);

    if (inPropertyType == QSSGRenderShaderDataType::Texture2D) {
        QSSGRenderCustomMaterial::TextureProperty *textureProperty =
                reinterpret_cast<QSSGRenderCustomMaterial::TextureProperty *>(propertyValue.value<void *>());
        QSSGRenderImage *image = textureProperty->texImage;
        if (image) {
            const auto &imageSource = image->m_imagePath;
            const QSSGRef<QSSGBufferManager> &theBufferManager(context->bufferManager());
            if (!imageSource.isEmpty()) {
                QSSGRenderImageTextureData theTextureData = theBufferManager->loadRenderImage(imageSource);
                if (theTextureData.m_rhiTexture) {
                    const QSSGRhiTexture t = {
                        inPropertyName,
                        theTextureData.m_rhiTexture,
                        { toRhi(textureProperty->minFilterType),
                          toRhi(textureProperty->magFilterType),
                          theTextureData.m_mipmaps > 0 ? QRhiSampler::Linear : QRhiSampler::None,
                          toRhi(textureProperty->clampType),
                          toRhi(textureProperty->clampType) }
                    };
                    shaderPipeline->addExtraTexture(t);
                }
            }
        }
    } else {
        shaderPipeline->setUniformValue(inPropertyName, propertyValue, inPropertyType);
    }
}

void QSSGMaterialSystem::applyRhiShaderPropertyValues(const QSSGRenderCustomMaterial &material,
                                                      const QSSGRef<QSSGRhiShaderStagesWithResources> &shaderPipeline)
{
    const auto &properties = material.m_properties;
    for (const auto &prop : properties)
        setShaderResources(material, prop.name, prop.value, prop.shaderDataType, shaderPipeline);

    const auto textProps = material.m_textureProperties;
    for (const auto &prop : textProps)
        setShaderResources(material, prop.name, QVariant::fromValue((void *)&prop), prop.shaderDataType, shaderPipeline);
}

void QSSGMaterialSystem::rhiRenderRenderable(QSSGRhiContext *rhiCtx,
                                             QSSGCustomMaterialRenderable &renderable,
                                             QSSGLayerRenderData &inData,
                                             bool *needsSetViewport)
{
    QRhiGraphicsPipeline *ps = renderable.rhiRenderData.mainPass.pipeline;
    QRhiShaderResourceBindings *srb = renderable.rhiRenderData.mainPass.srb;
    if (!ps || !srb)
        return;

    QRhiBuffer *vertexBuffer = renderable.subset.rhi.ia.vertexBuffer->buffer();
    QRhiBuffer *indexBuffer = renderable.subset.rhi.ia.indexBuffer ? renderable.subset.rhi.ia.indexBuffer->buffer() : nullptr;

    QRhiCommandBuffer *cb = rhiCtx->commandBuffer();
    cb->setGraphicsPipeline(ps);
    cb->setShaderResources(srb);

    if (*needsSetViewport) {
        cb->setViewport(rhiCtx->graphicsPipelineState(&inData)->viewport);
        *needsSetViewport = false;
    }

    QRhiCommandBuffer::VertexInput vb(vertexBuffer, 0);
    if (indexBuffer) {
        cb->setVertexInput(0, 1, &vb, indexBuffer, 0, renderable.subset.rhi.ia.indexBuffer->indexFormat());
        cb->drawIndexed(renderable.subset.count, 1, renderable.subset.offset);
    } else {
        cb->setVertexInput(0, 1, &vb);
        cb->draw(renderable.subset.count, 1, renderable.subset.offset);
    }
}

QT_END_NAMESPACE
