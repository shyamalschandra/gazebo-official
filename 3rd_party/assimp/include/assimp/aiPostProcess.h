/*
Open Asset Import Library (ASSIMP)
----------------------------------------------------------------------

Copyright (c) 2006-2008, ASSIMP Development Team
All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the 
following conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

/** @file aiPostProcess.h
 *  @brief Definitions for import post processing steps
 */
#ifndef AI_POSTPROCESS_H_INC
#define AI_POSTPROCESS_H_INC

#include "aiTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------------
/** @enum  aiPostProcessSteps
 *  @brief Defines the flags for all possible post processing steps.
 *
 *  @see Importer::ReadFile
 *  @see aiImportFile
 *  @see aiImportFileEx
 */
// -----------------------------------------------------------------------------------
enum aiPostProcessSteps
{

	// -------------------------------------------------------------------------
	/** <hr>Calculates the tangents and bitangents for the imported meshes. 
	 *
	 * Does nothing if a mesh does not have normals. You might want this post 
	 * processing step to be executed if you plan to use tangent space calculations 
	 * such as normal mapping  applied to the meshes. There's a config setting,
	 * <tt>#AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE</tt>, which allows you to specify
	 * a maximum smoothing angle for the algorithm. However, usually you'll
	 * want to leave it at the default value. Thanks.
	 */
	aiProcess_CalcTangentSpace = 0x1,

	// -------------------------------------------------------------------------
	/** <hr>Identifies and joins identical vertex data sets within all 
	 *  imported meshes. 
	 * 
	 * After this step is run each mesh does contain only unique vertices anymore,
	 * so a vertex is possibly used by multiple faces. You usually want
	 * to use this post processing step. If your application deals with
	 * indexed geometry, this step is compulsory or you'll just waste rendering
	 * time. <b>If this flag is not specified</b>, no vertices are referenced by
	 * more than one face and <b>no index buffer is required</b> for rendering.
	 */
	aiProcess_JoinIdenticalVertices = 0x2,

	// -------------------------------------------------------------------------
	/** <hr>Converts all the imported data to a left-handed coordinate space. 
	 *
	 * By default the data is returned in a right-handed coordinate space which 
	 * for example OpenGL prefers. In this space, +X points to the right, 
	 * +Z points towards the viewer and and +Y points upwards. In the DirectX 
     * coordinate space +X points to the right, +Y points upwards and +Z points 
     * away from the viewer.
	 *
	 * You'll probably want to consider this flag if you use Direct3D for
	 * rendering. The #aiProcess_ConvertToLeftHanded flag supersedes this
	 * setting and bundles all conversions typically required for D3D-based
	 * applications.
	 */
	aiProcess_MakeLeftHanded = 0x4,

	// -------------------------------------------------------------------------
	/** <hr>Triangulates all faces of all meshes. 
	 *
	 * By default the imported mesh data might contain faces with more than 3
	 * indices. For rendering you'll usually want all faces to be triangles. 
	 * This post processing step splits up all higher faces to triangles. 
	 * Line and point primitives are *not* modified!. If you want
	 * 'triangles only' with no other kinds of primitives, try the following
	 * solution:
	 * <ul>
	 * <li>Specify both #aiProcess_Triangulate and #aiProcess_SortByPType </li>
	 * </li>Ignore all point and line meshes when you process assimp's output</li>
	 * </ul>
	 */
	aiProcess_Triangulate = 0x8,

	// -------------------------------------------------------------------------
	/** <hr>Removes some parts of the data structure (animations, materials, 
	 *  light sources, cameras, textures, vertex components).
	 *
	 * The  components to be removed are specified in a separate
	 * configuration option, <tt>#AI_CONFIG_PP_RVC_FLAGS</tt>. This is quite useful
	 * if you don't need all parts of the output structure. Especially vertex 
	 * colors are rarely used today ... . Calling this step to remove unrequired
	 * stuff from the pipeline as early as possible results in an increased 
	 * performance and a better optimized output data structure.
	 * This step is also useful if you want to force Assimp to recompute 
	 * normals or tangents. The corresponding steps don't recompute them if 
	 * they're already there (loaded from the source asset). By using this 
	 * step you can make sure they are NOT there.
	 *
	 * This flag is a poor one, mainly because its purpose is usually
     * misunderstood. Consider the following case: a 3d model has been exported
	 * from a CAD app, it has per-face vertex colors. Vertex positions can't be
	 * shared, thus the #aiProcess_JoinIdenticalVertices step fails to
	 * optimize the data. Just because these nasty, little vertex colors.
	 * Most apps don't even process them, so it's all for nothing. By using
	 * this step, unneeded components are excluded as early as possible
	 * thus opening more room for internal optimzations. 
	 */
	aiProcess_RemoveComponent = 0x10,

	// -------------------------------------------------------------------------
	/** <hr>Generates normals for all faces of all meshes. 
	 *
	 * This is ignored if normals are already there at the time where this flag 
	 * is evaluated. Model importers try to load them from the source file, so
	 * they're usually already there. Face normals are shared between all points
	 * of a single face, so a single point can have multiple normals, which in
	 * other words, enforces the library to duplicate vertices in some cases.
	 * #aiProcess_JoinIdenticalVertices is *senseless* then.
	 *
	 * This flag may not be specified together with #aiProcess_GenSmoothNormals.
	 */
	aiProcess_GenNormals = 0x20,

	// -------------------------------------------------------------------------
	/** <hr>Generates smooth normals for all vertices in the mesh.
	*
	* This is ignored if normals are already there at the time where this flag 
	* is evaluated. Model importers try to load them from the source file, so
	* they're usually already there. 
	*
	* This flag may (of course) not be specified together with 
	* #aiProcess_GenNormals. There's a configuration option, 
	* <tt>#AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE</tt> which allows you to specify
	* an angle maximum for the normal smoothing algorithm. Normals exceeding
	* this limit are not smoothed, resulting in a a 'hard' seam between two faces.
	* Using a decent angle here (e.g. 80�) results in very good visual
	* appearance.
	*/
	aiProcess_GenSmoothNormals = 0x40,

	// -------------------------------------------------------------------------
	/** <hr>Splits large meshes into smaller submeshes
	*
	* This is quite useful for realtime rendering where the number of triangles
	* which can be maximally processed in a single draw-call is usually limited 
	* by the video driver/hardware. The maximum vertex buffer is usually limited,
	* too. Both requirements can be met with this step: you may specify both a 
	* triangle and vertex limit for a single mesh.
	*
	* The split limits can (and should!) be set through the 
	* <tt>#AI_CONFIG_PP_SLM_VERTEX_LIMIT</tt> and <tt>#AI_CONFIG_PP_SLM_TRIANGLE_LIMIT</tt> 
	* settings. The default values are <tt>#AI_SLM_DEFAULT_MAX_VERTICES</tt> and 
	* <tt>#AI_SLM_DEFAULT_MAX_TRIANGLES</tt>. 
	*
	* Note that splitting is generally a time-consuming task, but not if there's
	* nothing to split. The use of this step is recommended for most users.
	*/
	aiProcess_SplitLargeMeshes = 0x80,

	// -------------------------------------------------------------------------
	/** <hr>Removes the node graph and pre-transforms all vertices with
	* the local transformation matrices of their nodes. The output
	* scene does still contain nodes, however, there is only a
	* root node with children, each one referencing only one mesh,
	* each mesh referencing one material. For rendering, you can
	* simply render all meshes in order, you don't need to pay
	* attention to local transformations and the node hierarchy.
	* Animations are removed during this step.
	* This step is intended for applications without a scenegraph.
	* The step CAN cause some problems: if e.g. a mesh of the asset
	* contains normals and another, using the same material index, does not, 
	* they will be brought together, but the first meshes's part of
	* the normal list is zeroed. However, these artifacts are rare.
	* @note The <tt>#AI_CONFIG_PP_PTV_NORMALIZE</tt> configuration property
	* can be set to normalize the scene's spatial dimension to the -1...1
	* range. 
	*/
	aiProcess_PreTransformVertices = 0x100,

	// -------------------------------------------------------------------------
	/** <hr>Limits the number of bones simultaneously affecting a single vertex
	*  to a maximum value.
	*
	* If any vertex is affected by more than that number of bones, the least 
	* important vertex weights are removed and the remaining vertex weights are
	* renormalized so that the weights still sum up to 1.
	* The default bone weight limit is 4 (defined as <tt>#AI_LMW_MAX_WEIGHTS</tt> in
	* aiConfig.h), but you can use the <tt>#AI_CONFIG_PP_LBW_MAX_WEIGHTS</tt> setting to
	* supply your own limit to the post processing step.
	*
	* If you intend to perform the skinning in hardware, this post processing 
	* step might be of interest for you.
	*/
	aiProcess_LimitBoneWeights = 0x200,

	// -------------------------------------------------------------------------
	/** <hr>Validates the imported scene data structure
	 * This makes sure that all indices are valid, all animations and
	 * bones are linked correctly, all material references are correct .. etc.
	 *
	 * It is recommended to capture Assimp's log output if you use this flag,
	 * so you can easily find ot what's actually wrong if a file fails the
	 * validation. The validator is quite rude and will find *all*
	 * inconsistencies in the data structure ... plugin developers are
	 * recommended to use it to debug their loaders. There are two types of
	 * validation failures:
	 * <ul>
	 * <li>Error: There's something wrong with the imported data. Further
	 *   postprocessing is not possible and the data is not usable at all.
	 *   The import fails. #Importer::GetErrorString() or #aiGetErrorString()
	 *   carry the error message around.</li>
	 * <li>Warning: There are some minor issues (e.g. 1000000 animation 
	 *   keyframes with the same time), but further postprocessing and use
	 *   of the data structure is still safe. Warning details are written
	 *   to the log file, <tt>#AI_SCENE_FLAGS_VALIDATION_WARNING</tt> is set
	 *   in #aiScene::mFlags</li>
	 * </ul>
	 *
	 * This post-processing step is not time-consuming. It's use is not
	 * compulsory, but recommended. 
	*/
	aiProcess_ValidateDataStructure = 0x400,

	// -------------------------------------------------------------------------
	/** <hr>Reorders triangles for better vertex cache locality.
	 *
	 * The step tries to improve the ACMR (average post-transform vertex cache
	 * miss ratio) for all meshes. The implementation runs in O(n) and is 
	 * roughly based on the 'tipsify' algorithm (see <a href="
	 * http://www.cs.princeton.edu/gfx/pubs/Sander_2007_%3ETR/tipsy.pdf">this
	 * paper</a>).
	 *
	 * If you intend to render huge models in hardware, this step might
	 * be of interest for you. The <tt>#AI_CONFIG_PP_ICL_PTCACHE_SIZE</tt>config
	 * setting can be used to fine-tune the cache optimization.
	 */
	aiProcess_ImproveCacheLocality = 0x800,

	// -------------------------------------------------------------------------
	/** <hr>Searches for redundant/unreferenced materials and removes them.
	 *
	 * This is especially useful in combination with the 
	 * #aiProcess_PretransformVertices and #aiProcess_OptimizeMeshes flags. 
	 * Both join small meshes with equal characteristics, but they can't do 
	 * their work if two meshes have different materials. Because several
	 * material settings are always lost during Assimp's import filters,
	 * (and because many exporters don't check for redundant materials), huge
	 * models often have materials which are are defined several times with
	 * exactly the same settings ..
	 *
	 * Several material settings not contributing to the final appearance of
	 * a surface are ignored in all comparisons ... the material name is
	 * one of them. So, if you're passing additional information through the
	 * content pipeline (probably using *magic* material names), don't 
	 * specify this flag. Alternatively take a look at the
	 * <tt>#AI_CONFIG_PP_RRM_EXCLUDE_LIST</tt> setting.
	 */ 
	aiProcess_RemoveRedundantMaterials = 0x1000,

	// -------------------------------------------------------------------------
	/** <hr>This step tries to determine which meshes have normal vectors 
	 * that are facing inwards. The algorithm is simple but effective:
	 * the bounding box of all vertices + their normals is compared against
	 * the volume of the bounding box of all vertices without their normals.
	 * This works well for most objects, problems might occur with planar
	 * surfaces. However, the step tries to filter such cases.
	 * The step inverts all in-facing normals. Generally it is recommended
	 * to enable this step, although the result is not always correct.
	*/
	aiProcess_FixInfacingNormals = 0x2000,

	// -------------------------------------------------------------------------
	/** <hr>This step splits meshes with more than one primitive type in 
	 *  homogeneous submeshes. 
	 *
	 *  The step is executed after the triangulation step. After the step
	 *  returns, just one bit is set in aiMesh::mPrimitiveTypes. This is 
	 *  especially useful for real-time rendering where point and line
	 *  primitives are often ignored or rendered separately.
	 *  You can use the <tt>#AI_CONFIG_PP_SBP_REMOVE</tt> option to specify which
	 *  primitive types you need. This can be used to easily exclude
	 *  lines and points, which are rarely used, from the import.
	*/
	aiProcess_SortByPType = 0x8000,

	// -------------------------------------------------------------------------
	/** <hr>This step searches all meshes for degenerated primitives and
	 *  converts them to proper lines or points.
	 *
	 * A face is 'degenerated' if one or more of its points are identical.
	 * To have the degenerated stuff not only detected and collapsed but
	 * also removed, try one of the following procedures:
	 * <br><b>1.</b> (if you support lines&points for rendering but don't
	 *    want the degenerates)</br>
	 * <ul>
	 *   <li>Specify the #aiProcess_FindDegenerates flag.
	 *   </li>
	 *   <li>Set the <tt>AI_CONFIG_PP_FD_REMOVE</tt> option to 1. This will 
	 *       cause the step to remove degenerated triangles from the import
	 *       as soon as they're detected. They won't pass any further
	 *       pipeline steps.
	 *   </li>
	 * </ul>
	 * <br><b>2.</b>(if you don't support lines&points at all ...)</br>
	 * <ul>
	 *   <li>Specify the #aiProcess_FindDegenerates flag.
	 *   </li>
	 *   <li>Specify the #aiProcess_SortByPType flag. This moves line and
	 *     point primitives to separate meshes.
	 *   </li>
	 *   <li>Set the <tt>AI_CONFIG_PP_SBP_REMOVE</tt> option to 
	 *       @code aiPrimitiveType_POINTS | aiPrimitiveType_LINES
	 *       @endcode to cause SortByPType to reject point
	 *       and line meshes from the scene.
	 *   </li>
	 * </ul>
	 * @note Degenerated polygons are not necessarily evil and that's why
	 * they're not removed by default. There are several file formats which 
	 * don't support lines or points ... some exporters bypass the
	 * format specification and write them as degenerated triangle instead.
	*/
	aiProcess_FindDegenerates = 0x10000,

	// -------------------------------------------------------------------------
	/** <hr>This step searches all meshes for invalid data, such as zeroed
	 *  normal vectors or invalid UV coords and removes/fixes them. This is
	 *  intended to get rid of some common exporter errors.
	 *
	 * This is especially useful for normals. If they are invalid, and
	 * the step recognizes this, they will be removed and can later
	 * be recomputed, i.e. by the #aiProcess_GenSmoothNormals flag.<br>
	 * The step will also remove meshes that are infinitely small and reduce
	 * animation tracks consisting of hundreds if redundant keys to a single
	 * key. The <tt>AI_CONFIG_PP_FID_ANIM_ACCURACY</tt> config property decides
	 * the accuracy of the check for duplicate animation tracks.
	*/
	aiProcess_FindInvalidData = 0x20000,

	// -------------------------------------------------------------------------
	/** <hr>This step converts non-UV mappings (such as spherical or
	 *  cylindrical mapping) to proper texture coordinate channels.
	 *
	 * Most applications will support UV mapping only, so you will
	 * probably want to specify this step in every case. Note tha Assimp is not
	 * always able to match the original mapping implementation of the 
	 * 3d app which produced a model perfectly. It's always better to let the
	 * father app compute the UV channels, at least 3ds max, maja, blender, 
	 * lightwave, modo, ... are able to achieve this.
	 *
	 * @note If this step is not requested, you'll need to process the
	 * <tt>#AI_MATKEY_MAPPING</tt> material property in order to display all assets
	 * properly.
	 */
	aiProcess_GenUVCoords = 0x40000,

	// -------------------------------------------------------------------------
	/** <hr>This step applies per-texture UV transformations and bakes
	 *  them to stand-alone vtexture coordinate channelss.
	 *  
	 * UV transformations are specified per-texture - see the 
	 * <tt>#AI_MATKEY_UVTRANSFORM</tt> material key for more information. 
	 * This step processes all textures with 
	 * transformed input UV coordinates and generates new (pretransformed) UV channel 
	 * which replace the old channel. Most applications won't support UV 
	 * transformations, so you will probably want to specify this step.
     *
	 * @note UV transformations are usually implemented in realtime apps by 
	 * transforming texture coordinates at vertex shader stage with a 3x3
	 * (homogenous) transformation matrix.
	*/
	aiProcess_TransformUVCoords = 0x80000,

	// -------------------------------------------------------------------------
	/** <hr>This step searches for duplicate meshes and replaces duplicates
	 *  with references to the first mesh.
	 *
	 *  This step takes a while, don't use it if you have no time.
	 *  Its main purpose is to workaround the limitation that many export
	 *  file formats don't support instanced meshes, so exporters need to
	 *  duplicate meshes. This step removes the duplicates again. Please 
 	 *  note that Assimp does currently not support per-node material
	 *  assignment to meshes, which means that identical meshes with
	 *  differnent materials are currently *not* joined, although this is 
	 *  planned for future versions.
	 */
	aiProcess_FindInstances = 0x100000,

	// -------------------------------------------------------------------------
	/** <hr>A postprocessing step to reduce the number of meshes.
	 *
	 *  In fact, it will reduce the number of drawcalls.
	 *
	 *  This is a very effective optimization and is recommended to be used
	 *  together with #aiProcess_OptimizeGraph, if possible. The flag is fully
	 *  compatible with both #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.
	*/
	aiProcess_OptimizeMeshes  = 0x200000, 


	// -------------------------------------------------------------------------
	/** <hr>A postprocessing step to optimize the scene hierarchy. 
	 *
	 *  Nodes with no animations, bones, lights or cameras assigned are 
	 *  collapsed and joined.
	 *
	 *  Node names can be lost during this step. If you use special 'tag nodes'
	 *  to pass additional information through your content pipeline, use the
	 *  <tt>#AI_CONFIG_PP_OG_EXCLUDE_LIST</tt> setting to specify a list of node 
	 *  names you want to be kept. Nodes matching one of the names in this list won't
	 *  be touched or modified.
	 *
	 *  Use this flag with caution. Most simple files will be collapsed to a 
	 *  single node, complex hierarchies are usually completely lost. That's not
	 *  the right choice for editor environments, but probably a very effective
	 *  optimization if you just want to get the model data, convert it to your
	 *  own format and render it as fast as possible. 
	 *
	 *  This flag is designed to be used with #aiProcess_OptimizeMeshes for best
	 *  results.
	 *
	 *  @note 'crappy' scenes with thousands of extremely small meshes packed
	 *  in deeply nested nodes exist for almost all file formats.
	 *  #aiProcess_OptimizeMeshes in combination with #aiProcess_OptimizeGraph 
	 *  usually fixes them all and makes them renderable. 
	*/
	aiProcess_OptimizeGraph  = 0x400000, 

	// -------------------------------------------------------------------------
	/** <hr>This step flips all UV coordinates along the y-axis and adjusts
	 * material settings and bitangents accordingly.
	 * <br><b>Output UV coordinate system:</b>
	 * @code
	 * 0y|0y ---------- 1x|0y 
     * |                 |
     * |                 |
     * |                 |
     * 0x|1y ---------- 1x|1y
	 * @endcode
	 *
	 * You'll probably want to consider this flag if you use Direct3D for
	 * rendering. The #aiProcess_ConvertToLeftHanded flag supersedes this
	 * setting and bundles all conversions typically required for D3D-based
	 * applications.
	*/
	aiProcess_FlipUVs = 0x800000, 

	// -------------------------------------------------------------------------
	/** <hr>This step adjusts the output face winding order to be cw.
	 *
	 * The default face winding order is counter clockwise.
	 * <br><b>Output face order:</b>
	 * @code
	 *       x2
	 *           
	 *                         x0
	 *  x1
	 * @endcode
	*/
	aiProcess_FlipWindingOrder  = 0x1000000

	// aiProcess_GenEntityMeshes = 0x100000,
	// aiProcess_OptimizeAnimations = 0x200000
	// aiProcess_FixTexturePaths = 0x200000
};


// ---------------------------------------------------------------------------------------
/** @def aiProcess_ConvertToLeftHanded
 *  @brief Shortcut flag for Direct3D-based applications. 
 *
 *  Supersedes the #aiProcess_MakeLeftHanded and #aiProcess_FlipUVs and
 *  #aiProcess_FlipWindingOrder flags.
 *  The output data matches Direct3D's conventions: left-handed geometry, upper-left
 *  origin for UV coordinates and finally clockwise face order, suitable for CCW culling.
 *
 *  @deprecated 
 */
#define aiProcess_ConvertToLeftHanded ( \
	aiProcess_MakeLeftHanded     | \
	aiProcess_FlipUVs            | \
	aiProcess_FlipWindingOrder   | \
	0 ) 


// ---------------------------------------------------------------------------------------
/** @def aiProcessPreset_TargetRealtimeUse_Fast
 *  @brief Default postprocess configuration optimizing the data for real-time rendering.
 *  
 *  Applications would want to use this preset to load models on end-user PCs,
 *  maybe for direct use in game.
 *
 * If you're using DirectX, don't forget to combine this value with
 * the #aiProcess_ConvertToLeftHanded step. If you don't support UV transformations
 * in your application apply the #aiProcess_TransformUVCoords step, too.
 *  @note Please take the time to read the doc to the steps enabled by this preset. 
 *  Some of them offer further configurable properties, some of them might not be of
 *  use for you so it might be better to not specify them.
 */
#define aiProcessPreset_TargetRealtime_Fast ( \
	aiProcess_CalcTangentSpace		|  \
	aiProcess_GenNormals			|  \
	aiProcess_JoinIdenticalVertices |  \
	aiProcess_Triangulate			|  \
	aiProcess_GenUVCoords           |  \
	aiProcess_SortByPType           |  \
	0 )

 // ---------------------------------------------------------------------------------------
 /** @def aiProcessPreset_TargetRealtime_Quality
  *  @brief Default postprocess configuration optimizing the data for real-time rendering.
  *
  *  Unlike #aiProcessPreset_TargetRealtime_Fast, this configuration
  *  performs some extra optimizations to improve rendering speed and
  *  to minimize memory usage. It could be a good choice for a level editor
  *  environment where import speed is not so important.
  *  
  *  If you're using DirectX, don't forget to combine this value with
  *  the #aiProcess_ConvertToLeftHanded step. If you don't support UV transformations
  *  in your application apply the #aiProcess_TransformUVCoords step, too.
  *  @note Please take the time to read the doc for the steps enabled by this preset. 
  *  Some of them offer further configurable properties, some of them might not be of
  *  use for you so it might be better to not specify them.
  */
#define aiProcessPreset_TargetRealtime_Quality ( \
	aiProcess_CalcTangentSpace				|  \
	aiProcess_GenSmoothNormals				|  \
	aiProcess_JoinIdenticalVertices			|  \
	aiProcess_ImproveCacheLocality			|  \
	aiProcess_LimitBoneWeights				|  \
	aiProcess_RemoveRedundantMaterials      |  \
	aiProcess_SplitLargeMeshes				|  \
	aiProcess_Triangulate					|  \
	aiProcess_GenUVCoords                   |  \
	aiProcess_SortByPType                   |  \
	aiProcess_FindDegenerates               |  \
	aiProcess_FindInvalidData               |  \
	0 )

 // ---------------------------------------------------------------------------------------
 /** @def aiProcessPreset_TargetRealtime_MaxQuality
  *  @brief Default postprocess configuration optimizing the data for real-time rendering.
  *
  *  This preset enables almost every optimization step to achieve perfectly
  *  optimized data. It's your choice for level editor environments where import speed 
  *  is not important.
  *  
  *  If you're using DirectX, don't forget to combine this value with
  *  the #aiProcess_ConvertToLeftHanded step. If you don't support UV transformations
  *  in your application, apply the #aiProcess_TransformUVCoords step, too.
  *  @note Please take the time to read the doc for the steps enabled by this preset. 
  *  Some of them offer further configurable properties, some of them might not be of
  *  use for you so it might be better to not specify them.
  */
#define aiProcessPreset_TargetRealtime_MaxQuality ( \
	aiProcessPreset_TargetRealtime_Quality   |  \
	aiProcess_FindInstances                  |  \
	aiProcess_ValidateDataStructure          |  \
	aiProcess_OptimizeMeshes                 |  \
	0 )


#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // AI_POSTPROCESS_H_INC
