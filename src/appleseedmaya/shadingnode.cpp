
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Esteban Tovagliari, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "appleseedmaya/shadingnode.h"

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>

// appleseed.foundation headers.
#include "foundation/utility/string.h"

// appleseed.maya headers.
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnoderegistry.h"

namespace asf = foundation;

namespace
{

void makeInput(MFnAttribute& attr)
{
    attr.setStorable(true);
    attr.setWritable(true);
    attr.setReadable(true);
    attr.setKeyable(true);
}

void makeOutput(MFnAttribute& attr)
{
    attr.setStorable(false);
    attr.setReadable(true);
    attr.setWritable(false);
    attr.setKeyable(false);
}

MObject createPointAttribute(
    MFnNumericAttribute&    numAttrFn,
    const OSLParamInfo&     p)
{
    MObject attr = numAttrFn.createPoint(
        p.mayaAttributeName,
        p.mayaAttributeShortName);

    if (!p.isOutput && p.validDefault)
    {
        // todo: set default here...
    }

    return attr;
}

void initializeAttribute(MFnAttribute& attr, const OSLParamInfo& p)
{
    if (p.label.length() != 0)
        attr.setNiceNameOverride(p.label);

    if (p.isOutput)
        makeOutput(attr);
    else
        makeInput(attr);
}

// Stores the current shader info of the shader being registered.
const OSLShaderInfo *g_currentShaderInfo = 0;

} // unnamed.

void ShadingNode::setCurrentShaderInfo(const OSLShaderInfo *shaderInfo)
{
    g_currentShaderInfo = shaderInfo;
}

void *ShadingNode::creator()
{
    return new ShadingNode();
}

MStatus ShadingNode::initialize()
{
    assert(g_currentShaderInfo);
    const OSLShaderInfo *shaderInfo = g_currentShaderInfo;
    g_currentShaderInfo = 0;

    for(size_t i = 0, e = shaderInfo->paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& p = shaderInfo->paramInfo[i];
        MObject attr;

        if (p.paramType == "color")
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.createColor(
                p.mayaAttributeName,
                p.mayaAttributeShortName);

            numAttrFn.setUsedAsColor(true);

            if (!p.isOutput && p.validDefault)
            {
                // todo: set default here...
            }

            initializeAttribute(numAttrFn, p);
        }
        else if (p.paramType == "float")
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.create(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                MFnNumericData::kFloat);

            if (!p.isOutput && p.validDefault)
            {
                // todo: set default here...
            }

            initializeAttribute(numAttrFn, p);
        }
        else if (p.paramType == "int")
        {
            // Check to see if we need to create an int, bool or enum
            if (p.widget.asChar(), "mapper")
            {
                MFnEnumAttribute enumAttrFn;
                // todo: parse options here and build enum...
            }
            if (p.widget.asChar(), "checkBox")
            {
                MFnNumericAttribute numAttrFn;
                attr = numAttrFn.create(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName,
                    MFnNumericData::kBoolean);
                initializeAttribute(numAttrFn, p);
            }
            else
            {
                MFnNumericAttribute numAttrFn;
                attr = numAttrFn.create(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName,
                    MFnNumericData::kInt);
                initializeAttribute(numAttrFn, p);
            }
        }
        else if (p.paramType == "matrix")
        {
            MFnMatrixAttribute matrixAttrFn;
            attr = matrixAttrFn.create(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                MFnMatrixAttribute::kFloat);
            initializeAttribute(matrixAttrFn, p);
        }
        else if (p.paramType == "normal")
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p);
            initializeAttribute(numAttrFn, p);
        }
        else if (p.paramType == "point")
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p);
            initializeAttribute(numAttrFn, p);
        }
        else if (p.paramType == "pointer") // closure color
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.createColor(
                p.mayaAttributeName,
                p.mayaAttributeShortName);

            if (!p.isOutput && p.validDefault)
            {
                // todo: set default here...
            }

            initializeAttribute(numAttrFn, p);
        }
        else if (p.paramType == "vector")
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p);
            initializeAttribute(numAttrFn, p);
        }
        else if (p.paramType == "string")
        {
            // Check to see if we need to create a string or an enum
            if (p.widget == "popup")
            {
                if (p.options.length() != 0)
                {
                    std::vector<std::string> fields;
                    asf::tokenize(p.options.asChar(), "|", fields);

                    // todo: get the default value here...
                    const size_t defaultValue = 0;

                    MFnEnumAttribute enumAttrFn;
                    attr = enumAttrFn.create(
                        p.mayaAttributeName,
                        p.mayaAttributeShortName,
                        defaultValue);

                    for (size_t i = 0, e = fields.size(); i < e; ++i)
                        enumAttrFn.addField(fields[i].c_str(), i);

                    initializeAttribute(enumAttrFn, p);
                }
                else
                {
                    // todo: what here...?
                }
            }
            else
            {
                MFnTypedAttribute typedAttrFn;
                attr = typedAttrFn.create(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName,
                    MFnData::kString);

                if (p.widget == "popup")
                    typedAttrFn.setUsedAsFilename(true);

                if (!p.isOutput && p.validDefault)
                {
                    // todo: set default here...
                }

                initializeAttribute(typedAttrFn, p);
            }
        }
        else
        {
            RENDERER_LOG_WARNING(
                "Ignoring param %s of shader %s",
                p.paramName.asChar(),
                shaderInfo->shaderName.asChar());
            continue;
        }

        if (!attr.isNull())
            addAttribute(attr);
    }

    return MS::kSuccess;
}

ShadingNode::ShadingNode()
{
}

void ShadingNode::postConstructor()
{
    MPxNode::postConstructor();
    setMPSafe(true);
}
