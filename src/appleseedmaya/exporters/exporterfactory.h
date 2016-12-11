
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

#ifndef APPLESEED_MAYA_EXPORTERS_NODEEXPORTER_FACTORY_H
#define APPLESEED_MAYA_EXPORTERS_NODEEXPORTER_FACTORY_H

// Maya headers.
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

// appleseed.maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/exporters/dagnodeexporterfwd.h"
#include "appleseedmaya/exporters/shadingengineexporterfwd.h"
#include "appleseedmaya/exporters/shadingnetworkexporterfwd.h"
#include "appleseedmaya/exporters/shadingnodeexporterfwd.h"

// Forward declarations.
namespace renderer { class Assembly; }
namespace renderer { class Project; }
namespace renderer { class ShaderGroup; }

class NodeExporterFactory
{
  public:

    static MStatus initialize(const MString& pluginPath);
    static MStatus uninitialize();

    typedef DagNodeExporter* (*CreateDagNodeExporterFn)(
        const MDagPath&,
        renderer::Project&,
        AppleseedSession::SessionMode);

    static void registerDagNodeExporter(
        const MString&                  mayaTypeName,
        CreateDagNodeExporterFn         createFn);

    static DagNodeExporter* createDagNodeExporter(
        const MDagPath&                 path,
        renderer::Project&              project,
        AppleseedSession::SessionMode   sessionMode);

    static ShadingEngineExporter* createShadingEngineExporter(
        const MObject&                  object,
        renderer::Assembly&             mainAssembly,
        AppleseedSession::SessionMode   sessionMode);

    static ShadingNetworkExporter* createShadingNetworkExporter(
        const ShadingNetworkContext     context,
        const MObject&                  object,
        const MPlug&                    outputPlug,
        renderer::Assembly&             mainAssembly,
        AppleseedSession::SessionMode   sessionMode);

    typedef ShadingNodeExporter* (*CreateShadingNodeExporterFn)(
        const MObject&,
        renderer::ShaderGroup&);

    static void registerShadingNodeExporter(
      const MString&                    mayaTypeName,
      CreateShadingNodeExporterFn       createFn);

    static ShadingNodeExporter* createShadingNodeExporter(
        const MObject&                  object,
        renderer::ShaderGroup&          shaderGroup);
};

#endif  // !APPLESEED_MAYA_EXPORTERS_NODEEXPORTER_FACTORY_H