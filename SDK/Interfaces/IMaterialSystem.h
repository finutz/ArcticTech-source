#pragma once
#include "../../Utils/VitualFunction.h"
#include "../Misc/Matrix.h"
#include "../Misc/Vector.h"
#include "../Misc/QAngle.h"
#include "../Misc/Studio.h"
#include "../Misc/Color.h"
#include "IClientMode.h"
#include "IAppSystem.h"

enum PreviewImageRetVal_t
{
    MATERIAL_PREVIEW_IMAGE_BAD = 0,
    MATERIAL_PREVIEW_IMAGE_OK,
    MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialVarFlags_t
{
    MATERIAL_VAR_DEBUG = (1 << 0),
    MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
    MATERIAL_VAR_NO_DRAW = (1 << 2),
    MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
    MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
    MATERIAL_VAR_VERTEXALPHA = (1 << 5),
    MATERIAL_VAR_SELFILLUM = (1 << 6),
    MATERIAL_VAR_ADDITIVE = (1 << 7),
    MATERIAL_VAR_ALPHATEST = (1 << 8),
    //MATERIAL_VAR_UNUSED = (1 << 9),
    MATERIAL_VAR_ZNEARER = (1 << 10),
    MATERIAL_VAR_MODEL = (1 << 11),
    MATERIAL_VAR_FLAT = (1 << 12),
    MATERIAL_VAR_NOCULL = (1 << 13),
    MATERIAL_VAR_NOFOG = (1 << 14),
    MATERIAL_VAR_IGNOREZ = (1 << 15),
    MATERIAL_VAR_DECAL = (1 << 16),
    MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
    MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
    MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
    MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
    MATERIAL_VAR_TRANSLUCENT = (1 << 21),
    MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
    MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
    MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
    MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
    MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
    MATERIAL_VAR_HALFLAMBERT = (1 << 27),
    MATERIAL_VAR_WIREFRAME = (1 << 28),
    MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
    MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
    MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

typedef unsigned short ModelInstanceHandle_t;
typedef void* LightCacheHandle_t;
typedef void* StudioDecalHandle_t;
typedef int OverrideType_t;
typedef int ImageFormat;
typedef int VertexFormat_t;
typedef int MaterialPropertyTypes_t;
typedef unsigned long MDLHandle_t;

enum
{
    MODEL_INSTANCE_INVALID = (ModelInstanceHandle_t)~0
};

class IClientRenderable;
class IMaterial;
class CStudioHdr;
class IMatRenderContext;
class DataCacheHandle_t;
class ITexture;
class IMaterialVar
{
public:
    void SetFloatValue(float val) {
        CallVFunction<void(__thiscall*)(void*, float)>(this, 4)(this, val);
    }
    void SetIntValue(int val) {
        CallVFunction<void(__thiscall*)(void*, int)>(this, 5)(this, val);
    }
    void SetStringValue(const char* string) {
        CallVFunction<void(__thiscall*)(void*, const char*)>(this, 6)(this, string);
    }
    void SetVecValue(float x, float y, float z) {
        CallVFunction<void(__thiscall*)(void*, float, float, float)>(this, 11)(this, x, y, z);
    }
    void SetVecValue(Color color) {
        SetVecValue(color.r / 255.f, color.g / 255.f, color.b / 255.f);
    }
};
class KeyValues;
struct model_t;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct Ray_t;
struct DrawModelInfo_t;
struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;

enum
{
    RENDER_FLAGS_DISABLE_RENDERING = 0x01,
    RENDER_FLAGS_HASCHANGED = 0x02,
    RENDER_FLAGS_ALTERNATE_SORTING = 0x04,
    RENDER_FLAGS_RENDER_WITH_VIEWMODELS = 0x08,
    RENDER_FLAGS_BLOAT_BOUNDS = 0x10,
    RENDER_FLAGS_BOUNDS_VALID = 0x20,
    RENDER_FLAGS_BOUNDS_ALWAYS_RECOMPUTE = 0x40,
    RENDER_FLAGS_IS_SPRITE = 0x80,
    RENDER_FLAGS_FORCE_OPAQUE_PASS = 0x100,
};

struct RenderableInfo_t
{
    IClientRenderable* m_pRenderable;
    void* m_pAlphaProperty;
    int					m_EnumCount;				// Have I been added to a particular shadow yet?
    int					m_nRenderFrame;
    unsigned short		m_FirstShadow;				// The first shadow caster that cast on it
    unsigned short		m_LeafList;					// What leafs is it in?
    short				m_Area;						// -1 if the renderable spans multiple areas.
    uint16_t				m_Flags;					// rendering flags
    uint16_t				m_bRenderInFastReflection : 1;	// Should we render in the "fast" reflection?
    uint16_t				m_bDisableShadowDepthRendering : 1;	// Should we not render into the shadow depth map?
    uint16_t				m_bDisableCSMRendering : 1;			// Should we not render into the CSM?
    uint16_t				m_bDisableShadowDepthCaching : 1;	// Should we not be cached in the shadow depth map?
    uint16_t				m_nSplitscreenEnabled : 2;	// splitscreen rendering flags
    uint16_t				m_nTranslucencyType : 2;	// RenderableTranslucencyType_t
    uint16_t				m_nModelType : 8;			// RenderableModelType_t
    Vector				m_vecBloatedAbsMins;		// Use this for tree insertion
    Vector				m_vecBloatedAbsMaxs;
    Vector				m_vecAbsMins;			// NOTE: These members are not threadsafe!!
    Vector				m_vecAbsMaxs;			// They can be updated from any viewpoint (based on RENDER_FLAGS_BOUNDS_VALID)
};

struct DrawModelState_t
{
    void* m_pStudioHdr = nullptr;
    studiohwdata_t* m_pStudioHWData = nullptr;
    IClientRenderable* m_pRenderable = nullptr;
    const matrix3x4_t* m_pModelToWorld = nullptr;
    StudioDecalHandle_t     m_decals;
    int                     m_drawFlags;
    int                     m_lod;
};

struct StaticPropRenderInfo_t
{
    const matrix3x4_t* pModelToWorld = nullptr;
    const model_t* pModel = nullptr;
    IClientRenderable* pRenderable = nullptr;
    Vector* pLightingOrigin = nullptr;
    short                   skin;
    ModelInstanceHandle_t   instance;
};

struct ModelRenderInfo_t
{
    Vector                  origin;
    QAngle                  angles;
    char                    pad[4];
    IClientRenderable* pRenderable = nullptr;
    const model_t* pModel = nullptr;
    const matrix3x4_t* pModelToWorld = nullptr;
    const matrix3x4_t* pLightingOffset = nullptr;
    const Vector* pLightingOrigin = nullptr;
    int                     flags;
    int                     entity_index;
    int                     skin;
    int                     body;
    int                     hitboxset;
    ModelInstanceHandle_t   instance;

    ModelRenderInfo_t()
    {
        pModelToWorld = NULL;
        pLightingOffset = NULL;
        pLightingOrigin = NULL;
    }
};

struct LightingQuery_t
{
    Vector                  m_LightingOrigin;
    ModelInstanceHandle_t   m_InstanceHandle;
    bool                    m_bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t
{
    IClientRenderable* m_pRenderable;
};

class IMaterial
{
public:
    virtual const char* GetName() const = 0;
    virtual const char* GetTextureGroupName() const = 0;
    virtual PreviewImageRetVal_t    GetPreviewImageProperties(int* width, int* height, ImageFormat* imageFormat, bool* isTranslucent) const = 0;
    virtual PreviewImageRetVal_t    GetPreviewImage(unsigned char* data, int width, int height, ImageFormat imageFormat) const = 0;
    virtual int                     GetMappingWidth() = 0;
    virtual int                     GetMappingHeight() = 0;
    virtual int                     GetNumAnimationFrames() = 0;
    virtual bool                    InMaterialPage(void) = 0;
    virtual    void                 GetMaterialOffset(float* pOffset) = 0;
    virtual void                    GetMaterialScale(float* pScale) = 0;
    virtual IMaterial* GetMaterialPage(void) = 0;
    virtual IMaterialVar* FindVar(const char* varName, bool* found = nullptr, bool complain = true) = 0;
    virtual void                    IncrementReferenceCount(void) = 0;
    virtual void                    DecrementReferenceCount(void) = 0;
    inline void                     AddRef() { IncrementReferenceCount(); }
    inline void                     Release() { DecrementReferenceCount(); }
    virtual int                     GetEnumerationID(void) const = 0;
    virtual void                    GetLowResColorSample(float s, float t, float* color) const = 0;
    virtual void                    RecomputeStateSnapshots() = 0;
    virtual bool                    IsTranslucent() = 0;
    virtual bool                    IsAlphaTested() = 0;
    virtual bool                    IsVertexLit() = 0;
    virtual VertexFormat_t          GetVertexFormat() const = 0;
    virtual bool                    HasProxy(void) const = 0;
    virtual bool                    UsesEnvCubemap(void) = 0;
    virtual bool                    NeedsTangentSpace(void) = 0;
    virtual bool                    NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
    virtual bool                    NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
    virtual bool                    NeedsSoftwareSkinning(void) = 0;
    virtual void                    AlphaModulate(float alpha) = 0;
    virtual void                    _ColorModulate(float r, float g, float b) = 0;
    virtual void                    SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
    virtual bool                    GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
    virtual void                    GetReflectivity(Vector& reflect) = 0;
    virtual bool                    GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
    virtual bool                    IsTwoSided() = 0;
    virtual void                    SetShader(const char* pShaderName) = 0;
    virtual int                     GetNumPasses(void) = 0;
    virtual int                     GetTextureMemoryBytes(void) = 0;
    virtual void                    Refresh() = 0;
    virtual bool                    NeedsLightmapBlendAlpha(void) = 0;
    virtual bool                    NeedsSoftwareLighting(void) = 0;
    virtual int                     ShaderParamCount() const = 0;
    virtual IMaterialVar** GetShaderParams(void) = 0;
    virtual bool                    IsErrorMaterial() const = 0;
    virtual void                    Unused() = 0;
    virtual float                   GetAlphaModulation() = 0;
    virtual void                    GetColorModulation(float* r, float* g, float* b) = 0;
    virtual bool                    IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
    virtual IMaterialVar* FindVarFast(char const* pVarName, unsigned int* pToken) = 0;
    virtual void                    SetShaderAndParams(KeyValues* pKeyValues) = 0;
    virtual const char* GetShaderName() const = 0;
    virtual void                    DeleteIfUnreferenced() = 0;
    virtual bool                    IsSpriteCard() = 0;
    virtual void                    CallBindProxy(void* proxyData) = 0;
    virtual void                    RefreshPreservingMaterialVars() = 0;
    virtual bool                    WasReloadedFromWhitelist() = 0;
    virtual bool                    SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
    virtual int                     GetReferenceCount() const = 0;

    void ColorModulate(const Color& clr) {
        _ColorModulate(clr.r / 255.f, clr.g / 255.f, clr.b / 255.f);
    }
};

class IVModelRender
{
public:
    virtual int                     DrawModel(int flags, IClientRenderable* pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t* model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t* modelToWorld = NULL, const matrix3x4_t* pLightingOffset = NULL) = 0;
    virtual void                    ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0) = 0;
    virtual bool                    IsForcedMaterialOverride(void) = 0;
    virtual void                    SetViewTarget(const CStudioHdr* pStudioHdr, int nBodyIndex, const Vector& target) = 0;
    virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable* pRenderable, LightCacheHandle_t* pCache = NULL) = 0;
    virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
    virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
    virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
    virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable* pRenderable) = 0;
    virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
    virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
    virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
    virtual void                    RemoveAllDecalsFromAllModels() = 0;
    virtual matrix3x4_t* DrawModelShadowSetup(IClientRenderable* pRenderable, int body, int skin, DrawModelInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
    virtual void                    DrawModelShadow(IClientRenderable* pRenderable, const DrawModelInfo_t& info, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
    virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
    virtual void                    ReleaseAllStaticPropColorData(void) = 0;
    virtual void                    RestoreAllStaticPropColorData(void) = 0;
    virtual int                     DrawModelEx(ModelRenderInfo_t& pInfo) = 0;
    virtual int                     DrawModelExStaticProp(ModelRenderInfo_t& pInfo) = 0;
    virtual bool                    DrawModelSetup(ModelRenderInfo_t& pInfo, DrawModelState_t* pState, matrix3x4_t** ppBoneToWorldOut) = 0;
    virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
    virtual void                    SetupLighting(const Vector& vecCenter) = 0;
    virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t* pProps, int count, bool bShadowDepth) = 0;
    virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
    virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
    virtual void                    SetupLightingEx(const Vector& vecCenter, ModelInstanceHandle_t handle) = 0;
    virtual bool                    GetBrightestShadowingLightSource(const Vector& vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
    virtual void                    ComputeLightingState(int nCount, const LightingQuery_t* pQuery, MaterialLightingState_t* pState, ITexture** ppEnvCubemapTexture) = 0;
    virtual void                    GetModelDecalHandles(StudioDecalHandle_t* pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t* pHandles) = 0;
    virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t* pQuery, MaterialLightingState_t* pState, MaterialLightingState_t* pDecalState, ColorMeshInfo_t** ppStaticLighting, ITexture** ppEnvCubemapTexture, DataCacheHandle_t* pColorMeshHandles) = 0;
    virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t* pColorMeshHandles) = 0;
};

enum class OverrideType {
    Normal = 0,
    BuildShadows,
    DepthWrite,
    CustomMaterial, // weapon skins
    SsaoDepthWrite
};

class IStudioRender {
    std::byte pad_0[592];
    IMaterial* materialOverride;
    std::byte pad_1[12];
    int overrideType;
public:
    bool IsForcedMaterialOverride() {
        if (!materialOverride)
            return overrideType == 2 || overrideType == 3; // see CStudioRenderContext::IsForcedMaterialOverride
        return strstr(materialOverride->GetName(), "dev/glow");
    }

    void ForcedMaterialOverride(IMaterial* material, OverrideType type = OverrideType::Normal, int index = -1) {
        CallVFunction<void(__thiscall*)(IStudioRender*, IMaterial*, OverrideType, int)>(this, 33)(this, material, type, index);
    }
};
struct vcollide_t;
struct virtualmodel_t;
struct RenderableTranslucencyType_t;
struct CUtlBuffer;

class IVModelInfo
{
public:
    virtual                                 ~IVModelInfo(void) {}
    virtual const model_t* GetModel(int modelindex) const = 0;
    virtual int                             GetModelIndex(const char* name) const = 0;
    virtual const char* GetModelName(const model_t* model) const = 0;
    virtual vcollide_t* GetVCollide(const model_t* model) const = 0;
    virtual vcollide_t* GetVCollide(int modelindex) const = 0;
    virtual void                            GetModelBounds(const model_t* model, Vector& mins, Vector& maxs) const = 0;
    virtual void                            GetModelRenderBounds(const model_t* model, Vector& mins, Vector& maxs) const = 0;
    virtual int                             GetModelFrameCount(const model_t* model) const = 0;
    virtual int                             GetModelType(const model_t* model) const = 0;
    virtual void* GetModelExtraData(const model_t* model) = 0;
    virtual bool                            ModelHasMaterialProxy(const model_t* model) const = 0;
    virtual bool                            IsTranslucent(model_t const* model) const = 0;
    virtual bool                            IsTranslucentTwoPass(const model_t* model) const = 0;
    virtual void                            Unused0() {};
    virtual RenderableTranslucencyType_t    ComputeTranslucencyType(const model_t* model, int nSkin, int nBody) = 0;
    virtual int                             GetModelMaterialCount(const model_t* model) const = 0;
    virtual void                            GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial) = 0;
    virtual bool                            IsModelVertexLit(const model_t* model) const = 0;
    virtual const char* GetModelKeyValueText(const model_t* model) = 0;
    virtual bool                            GetModelKeyValue(const model_t* model, CUtlBuffer& buf) = 0;
    virtual float                           GetModelRadius(const model_t* model) = 0;
    virtual CStudioHdr* GetStudioHdr(MDLHandle_t handle) = 0;
    virtual const studiohdr_t* FindModel(const studiohdr_t* pStudioHdr, void** cache, const char* modelname) const = 0;
    virtual const studiohdr_t* FindModel(void* cache) const = 0;
    virtual virtualmodel_t* GetVirtualModel(const studiohdr_t* pStudioHdr) const = 0;
    virtual uint8_t* GetAnimBlock(const studiohdr_t* pStudioHdr, int iBlock) const = 0;
    virtual void                            GetModelMaterialColorAndLighting(const model_t* model, Vector const& origin, Vector const& angles, void* pTrace, Vector& lighting, Vector& matColor) = 0;
    virtual void                            GetIlluminationPoint(const model_t* model, IClientRenderable* pRenderable, Vector const& origin, Vector const& angles, Vector* pLightingCenter) = 0;
    virtual int                             GetModelContents(int modelIndex) const = 0;
    virtual void							unused_30() = 0;
    virtual void							unused_31() = 0;
    virtual studiohdr_t* GetStudioModel(const model_t* mod) = 0;
    virtual int                             GetModelSpriteWidth(const model_t* model) const = 0;
    virtual int                             GetModelSpriteHeight(const model_t* model) const = 0;
    virtual void                            SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
    virtual void                            GetLevelScreenFadeRange(float* pMinArea, float* pMaxArea) const = 0;
    virtual void                            SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;
    virtual unsigned char                   ComputeLevelScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
    virtual unsigned char                   ComputeViewScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
    virtual int                             GetAutoplayList(const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList) const = 0;
    virtual void* GetCollideForVirtualTerrain(int index) = 0;
    virtual bool                            IsUsingFBTexture(const model_t* model, int nSkin, int nBody, IClientRenderable** pClientRenderable) const = 0;
    virtual const model_t* FindOrLoadModel(const char* name) const = 0;
    virtual MDLHandle_t                     GetCacheHandle(const model_t* model) const = 0;
    virtual int                             GetBrushModelPlaneCount(const model_t* model) const = 0;
    virtual void                            GetBrushModelPlane(const model_t* model, int nIndex, int& plane, Vector* pOrigin) const = 0;
    virtual int                             GetSurfacepropsForVirtualTerrain(int index) = 0;
    virtual bool                            UsesEnvCubemap(const model_t* model) const = 0;
    virtual bool                            UsesStaticLighting(const model_t* model) const = 0;
};

class IVModelInfoClient : public IVModelInfo
{
public:
};
class KeyValues;

enum
{
    MDLHANDLE_INVALID = static_cast<MDLHandle_t>(~0)
};

enum MDLCacheDataType_t : int
{
    MDLCACHE_STUDIOHDR = 0,
    MDLCACHE_STUDIOHWDATA,
    MDLCACHE_VCOLLIDE,
    MDLCACHE_ANIMBLOCK,
    MDLCACHE_VIRTUALMODEL,
    MDLCACHE_VERTEXES,
    MDLCACHE_DECODEDANIMBLOCK,
};

enum MDLCacheFlush_t : unsigned int
{
    MDLCACHE_FLUSH_STUDIOHDR = 0x01,
    MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
    MDLCACHE_FLUSH_VCOLLIDE = 0x04,
    MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
    MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
    MDLCACHE_FLUSH_AUTOPLAY = 0x20,
    MDLCACHE_FLUSH_VERTEXES = 0x40,
    MDLCACHE_FLUSH_COMBINED_DATA = 0x80,
    MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
    MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};

class IMDLCacheNotify
{
public:
    virtual void OnDataLoaded(MDLCacheDataType_t nType, MDLHandle_t hModel) = 0;
    virtual void OnCombinerPreCache(MDLHandle_t hOldHandle, MDLHandle_t hNewHandle) = 0;
    virtual void OnDataUnloaded(MDLCacheDataType_t nType, MDLHandle_t hModel) = 0;
    virtual bool ShouldSupressLoadWarning(MDLHandle_t hModel) = 0;
};

class IMDLCache : public IAppSystem
{
public:
    virtual void SetCacheNotify(IMDLCacheNotify* pNotify) = 0;
    virtual MDLHandle_t FindMDL(const char* szMDLRelativePath) = 0;
    virtual int AddReference(MDLHandle_t hModel) = 0;
    virtual int Release(MDLHandle_t hModel) = 0;
    virtual int GetReference(MDLHandle_t hModel) = 0;
    virtual studiohdr_t* GetStudioHdr(MDLHandle_t hModel) = 0;
    virtual studiohwdata_t* GetHardwareData(MDLHandle_t hModel) = 0;
    virtual vcollide_t* GetVCollide(MDLHandle_t hModel) = 0;
    virtual vcollide_t* GetVCollide(MDLHandle_t hModel, float flScale) = 0;
    virtual unsigned char* GetAnimBlock(MDLHandle_t hModel, int nBlock, bool bPreloadIfMissing) = 0;
    virtual bool HasAnimBlockBeenPreloaded(MDLHandle_t hModel, int nBlock) = 0;
    virtual virtualmodel_t* GetVirtualModel(MDLHandle_t hModel) = 0;
    virtual int GetAutoplayList(MDLHandle_t hModel, unsigned short** pOut) = 0;
    virtual void* GetVertexData(MDLHandle_t hModel) = 0;
    virtual void TouchAllData(MDLHandle_t hModel) = 0;
    virtual void SetUserData(MDLHandle_t hModel, void* pData) = 0;
    virtual void* GetUserData(MDLHandle_t hModel) = 0;
    virtual bool IsErrorModel(MDLHandle_t hModel) = 0;
    virtual bool IsOverBudget(MDLHandle_t hModel) = 0;
    virtual void Flush(MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;
    virtual void Flush(MDLHandle_t hModel, MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;
    virtual const char* GetModelName(MDLHandle_t hModel) = 0;
    virtual void* GetCacheSection(MDLCacheDataType_t nType) = 0;
    virtual virtualmodel_t* GetVirtualModelFast(const studiohdr_t* pStudioHdr, MDLHandle_t hModel) = 0;
    virtual void BeginLock() = 0;
    virtual void EndLock() = 0;
    virtual int* GetFrameUnlockCounterPtrOLD() = 0;
    virtual void FinishPendingLoads() = 0;
    virtual vcollide_t* GetVCollideEx(MDLHandle_t hModel, bool bSynchronousLoad = true) = 0;
    virtual bool GetVCollideSize(MDLHandle_t hModel, int* pVCollideSize) = 0;
    virtual bool GetAsyncLoad(MDLCacheDataType_t nType) = 0;
    virtual bool SetAsyncLoad(MDLCacheDataType_t nType, bool bAsync) = 0;
    virtual void BeginMapLoad() = 0;
    virtual void EndMapLoad() = 0;
    virtual void MarkAsLoaded(MDLHandle_t hModel) = 0;
    virtual void InitPreloadData(bool bRebuild) = 0;
    virtual void ShutdownPreloadData() = 0;
    virtual bool IsDataLoaded(MDLHandle_t hModel, MDLCacheDataType_t nType) = 0;
    virtual int* GetFrameUnlockCounterPtr(MDLCacheDataType_t nType) = 0;
    virtual studiohdr_t* LockStudioHdr(MDLHandle_t hModel) = 0;
    virtual void UnlockStudioHdr(MDLHandle_t hModel) = 0;
    virtual bool PreloadModel(MDLHandle_t hModel) = 0;
    virtual void ResetErrorModelStatus(MDLHandle_t hModel) = 0;
    virtual void MarkFrame() = 0;
    virtual void BeginCoarseLock() = 0;
    virtual void EndCoarseLock() = 0;
    virtual void ReloadVCollide(MDLHandle_t hModel) = 0;
    virtual bool ReleaseAnimBlockAllocator() = 0;
    virtual bool RestoreHardwareData(MDLHandle_t hModel, void* pAsyncVTXControl, void* pAsyncVVDControl) = 0;
    virtual void DisableVCollideLoad() = 0;
    virtual void EnableVCollideLoad() = 0;
    virtual void DisableFileNotFoundWarnings() = 0;
    virtual void EnableFileNotFoundWarnings() = 0;
    virtual bool ProcessPendingHardwareRestore() = 0;
    virtual void UnloadQueuedHardwareData() = 0;
    virtual void DumpDictionaryState() = 0;
    virtual MDLHandle_t	CreateCombinedModel(const char* szModelName) = 0;
    virtual bool CreateCombinedModel(MDLHandle_t hModel) = 0;
    virtual bool SetCombineModels(MDLHandle_t hModel, const void* vecModelsToCombine) = 0;
    virtual bool FinishCombinedModel(MDLHandle_t hModel, void* pFunction, void* pUserData = nullptr) = 0;
    virtual bool IsCombinedPlaceholder(MDLHandle_t hModel) = 0;
    virtual bool IsCombinedModel(MDLHandle_t hModel) = 0;
    virtual int GetNumCombinedSubModels(MDLHandle_t hModel) = 0;
    virtual void GetCombinedSubModelFilename(MDLHandle_t hModel, int nSubModelIndex, char* szResult, int nResultSize) = 0;
    virtual void* GetCombinedMaterialKV(MDLHandle_t hModel, int nAtlasGroup = 0) = 0;
    virtual void UpdateCombiner() = 0;
    virtual void* GetCombinedInternalAsset(int nAssetType, const char* szAssetID = nullptr, int* nSize = nullptr) = 0;
    virtual void SetCombinerFlags(unsigned int nFlags) = 0;
    virtual void ClearCombinerFlags(unsigned int nFlags) = 0;
    virtual void DebugCombinerInfo() = 0;
};

class IMaterialSystem
{
public:
    const char* GetName()
    {
        return CallVFunction< const char* (__thiscall*)(decltype(this)) >(this, 0)(this);
    }

    IMaterial* CreateMaterial(const char* material_name, KeyValues* kv)
    {
        return CallVFunction< IMaterial* (__thiscall*)(decltype(this), const char*, KeyValues*) >(this, 83)(this, material_name, kv);
    }

    IMaterial* FindMaterial(const char* material_name, const char* texture_group_name = nullptr, bool complain = true, const char* complain_prefix = nullptr)
    {
        return CallVFunction< IMaterial* (__thiscall*)(decltype(this), const char*, const char*, bool, const char*) >(this, 84)(this, material_name, texture_group_name, complain, complain_prefix);
    }

    int FirstMaterial()
    {
        return CallVFunction< int(__thiscall*)(decltype(this)) >(this, 86)(this);
    }

    int NextMaterial(int handle)
    {
        return CallVFunction< int(__thiscall*)(decltype(this), int) >(this, 87)(this, handle);
    }

    int InvalidMaterial()
    {
        return CallVFunction< int(__thiscall*)(decltype(this)) >(this, 88)(this);
    }

    IMaterial* GetMaterial(short handle)
    {
        return CallVFunction< IMaterial* (__thiscall*)(decltype(this), short) >(this, 89)(this, handle);
    }

    void* FindTexture(char const* pTextureName, const char* pTextureGroupName, bool complain = true)
    {
        return CallVFunction< void* (__thiscall*)(decltype(this), char const*, const char*, bool)>(this, 91)(this, pTextureName, pTextureGroupName, complain);
    }

    void* GetRenderContext()
    {
        return CallVFunction< void* (__thiscall*)(decltype(this)) >(this, 115)(this);
    }
};

enum
{
    DRAWWORLDLISTS_DRAW_STRICTLYABOVEWATER = 0x001,
    DRAWWORLDLISTS_DRAW_STRICTLYUNDERWATER = 0x002,
    DRAWWORLDLISTS_DRAW_INTERSECTSWATER = 0x004,
    DRAWWORLDLISTS_DRAW_WATERSURFACE = 0x008,
    DRAWWORLDLISTS_DRAW_SKYBOX = 0x010,
    DRAWWORLDLISTS_DRAW_CLIPSKYBOX = 0x020,
    DRAWWORLDLISTS_DRAW_SHADOWDEPTH = 0x040,
    DRAWWORLDLISTS_DRAW_REFRACTION = 0x080,
    DRAWWORLDLISTS_DRAW_REFLECTION = 0x100,
    DRAWWORLDLISTS_DRAW_WORLD_GEOMETRY = 0x200,
    DRAWWORLDLISTS_DRAW_DECALS_AND_OVERLAYS = 0x400,
};

enum
{
    MAT_SORT_GROUP_STRICTLY_ABOVEWATER = 0,
    MAT_SORT_GROUP_STRICTLY_UNDERWATER,
    MAT_SORT_GROUP_INTERSECTS_WATER_SURFACE,
    MAT_SORT_GROUP_WATERSURFACE,

    MAX_MAT_SORT_GROUPS
};

//-----------------------------------------------------------------------------
// Leaf index
//-----------------------------------------------------------------------------
typedef unsigned short LeafIndex_t;
enum
{
    INVALID_LEAF_INDEX = (LeafIndex_t)~0
};

struct WorldListLeafData_t
{
    LeafIndex_t     leafIndex;    // 16 bits
    int16_t         waterData;
    uint16_t        firstTranslucentSurface;    // engine-internal list index
    uint16_t        translucentSurfaceCount;    // count of translucent surfaces+disps
};

struct WorldListInfo_t
{
    int                     m_ViewFogVolume;
    int                     m_LeafCount;
    bool                    m_bHasWater;
    WorldListLeafData_t* m_pLeafDataList;
};

class IWorldRenderList /*: public IRefCounted*/
{
};

//-----------------------------------------------------------------------------
// Describes the fog volume for a particular point
//-----------------------------------------------------------------------------
struct VisibleFogVolumeInfo_t
{
    int            m_nVisibleFogVolume;
    int            m_nVisibleFogVolumeLeaf;
    bool        m_bEyeInFogVolume;
    float       m_flDistanceToWater;
    float       m_flWaterHeight;
    IMaterial* m_pFogVolumeMaterial;
};

struct VPlane
{
    Vector        m_Normal;
    float        m_Dist;
};
#define FRUSTUM_NUMPLANES    6
typedef VPlane Frustum[FRUSTUM_NUMPLANES];
//-----------------------------------------------------------------------------
// Vertex format for brush models
//-----------------------------------------------------------------------------
struct BrushVertex_t //-V690
{
    Vector        m_Pos;
    Vector        m_Normal;
    Vector        m_TangentS;
    Vector        m_TangentT;
    Vector2    m_TexCoord;
    Vector2    m_LightmapCoord;

private:
    BrushVertex_t(const BrushVertex_t& src);
};

//-----------------------------------------------------------------------------
// Visibility data for area portal culling
//-----------------------------------------------------------------------------
struct VisOverrideData_t
{
    Vector        m_vecVisOrigin;                    // The point to to use as the viewpoint for area portal backface cull checks.
    float        m_fDistToAreaPortalTolerance;    // The distance from an area portal before using the full screen as the viewable portion.
};


//-----------------------------------------------------------------------------
// interface for asking about the Brush surfaces from the client DLL
//-----------------------------------------------------------------------------

class IBrushSurface
{
public:
    // Computes texture coordinates + lightmap coordinates given a world position
    virtual void ComputeTextureCoordinate(Vector const& worldPos, Vector2& texCoord) = 0;
    virtual void ComputeLightmapCoordinate(Vector const& worldPos, Vector2& lightmapCoord) = 0;

    // Gets the vertex data for this surface
    virtual int  GetVertexCount() const = 0;
    virtual void GetVertexData(BrushVertex_t* pVerts) = 0;

    // Gets at the material properties for this surface
    virtual IMaterial* GetMaterial() = 0;
};


//-----------------------------------------------------------------------------
// interface for installing a new renderer for brush surfaces
//-----------------------------------------------------------------------------

class IBrushRenderer
{
public:
    // Draws the surface; returns true if decals should be rendered on this surface
    virtual bool RenderBrushModelSurface(void* pBaseEntity, IBrushSurface* pBrushSurface) = 0;
};

#define MAX_VIS_LEAVES    32
#define MAX_AREA_STATE_BYTES        32
#define MAX_AREA_PORTAL_STATE_BYTES 24

class IVRenderView
{
    enum
    {
        VIEW_SETUP_VIS_EX_RETURN_FLAGS_USES_RADIAL_VIS = 0x00000001
    };
public:
    virtual void                DrawBrushModel(void* baseentity, model_t* model, const Vector& origin, const Vector& angles, bool sort) = 0;
    virtual void                DrawIdentityBrushModel(IWorldRenderList* pList, model_t* model) = 0;
    virtual void                TouchLight(struct dlight_t* light) = 0;
    virtual void                Draw3DDebugOverlays(void) = 0;
    virtual void                SetBlend(float blend) = 0;
    virtual float               GetBlend(void) = 0;
    virtual void                SetColorModulation(float const* blend) = 0;
    inline void                 SetColorModulation(float r, float g, float b)
    {
        float clr[3] = { r, g, b };
        SetColorModulation(clr);
    }
    virtual void                GetColorModulation(float* blend) = 0;
    virtual void                SceneBegin(void) = 0;
    virtual void                SceneEnd(void) = 0;
    virtual void                GetVisibleFogVolume(const Vector& eyePoint, VisibleFogVolumeInfo_t* pInfo) = 0;
    virtual IWorldRenderList* CreateWorldList() = 0;
    virtual void                BuildWorldLists(IWorldRenderList* pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = NULL, bool bShadowDepth = false, float* pReflectionWaterHeight = NULL) = 0;
    virtual void                DrawWorldLists(IWorldRenderList* pList, unsigned long flags, float waterZAdjust) = 0;
    virtual int                 GetNumIndicesForWorldLists(IWorldRenderList* pList, unsigned long nFlags) = 0;
    virtual void                DrawTopView(bool enable) = 0;
    virtual void                TopViewBounds(Vector2 const& mins, Vector2 const& maxs) = 0;
    virtual void                DrawLights(void) = 0;
    virtual void                DrawMaskEntities(void) = 0;
    virtual void                DrawTranslucentSurfaces(IWorldRenderList* pList, int* pSortList, int sortCount, unsigned long flags) = 0;
    virtual void                DrawLineFile(void) = 0;
    virtual void                DrawLightmaps(IWorldRenderList* pList, int pageId) = 0;
    virtual void                ViewSetupVis(bool novis, int numorigins, const Vector origin[]) = 0;
    virtual bool                AreAnyLeavesVisible(int* leafList, int nLeaves) = 0;
    virtual    void             VguiPaint(void) = 0;
    virtual void                ViewDrawFade(uint8_t* color, IMaterial* pMaterial) = 0;
    virtual void                OLD_SetProjectionMatrix(float fov, float zNear, float zFar) = 0;
    virtual unsigned long       GetLightAtPoint(Vector& pos) = 0;
    virtual int                 GetViewEntity(void) = 0;
    virtual bool                IsViewEntity(int entindex) = 0;
    virtual float               GetFieldOfView(void) = 0;
    virtual unsigned char** GetAreaBits(void) = 0;
    virtual void                SetFogVolumeState(int nVisibleFogVolume, bool bUseHeightFog) = 0;
    virtual void                InstallBrushSurfaceRenderer(IBrushRenderer* pBrushRenderer) = 0;
    virtual void                DrawBrushModelShadow(IClientRenderable* pRenderable) = 0;
    virtual    bool             LeafContainsTranslucentSurfaces(IWorldRenderList* pList, int sortIndex, unsigned long flags) = 0;
    virtual bool                DoesBoxIntersectWaterVolume(const Vector& mins, const Vector& maxs, int leafWaterDataID) = 0;
    virtual void                SetAreaState(unsigned char chAreaBits[MAX_AREA_STATE_BYTES], unsigned char chAreaPortalBits[MAX_AREA_PORTAL_STATE_BYTES]) = 0;
    virtual void                VGui_Paint(int mode) = 0;
    virtual void                Push3DView(const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
    virtual void                Push2DView(const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
    virtual void                PopView(Frustum frustumPlanes) = 0;
    virtual void                SetMainView(const Vector& vecOrigin, const Vector& angles) = 0;
    virtual void                ViewSetupVisEx(bool novis, int numorigins, const Vector origin[], unsigned int& returnFlags) = 0;
    virtual void                OverrideViewFrustum(Frustum custom) = 0;
    virtual void                DrawBrushModelShadowDepth(void* baseentity, model_t* model, const Vector& origin, const Vector& angles, bool bSort) = 0;
    virtual void                UpdateBrushModelLightmap(model_t* model, IClientRenderable* pRenderable) = 0;
    virtual void                BeginUpdateLightmaps(void) = 0;
    virtual void                EndUpdateLightmaps(void) = 0;
    virtual void                OLD_SetOffCenterProjectionMatrix(float fov, float zNear, float zFar, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight) = 0;
    virtual void                OLD_SetProjectionMatrixOrtho(float left, float top, float right, float bottom, float zNear, float zFar) = 0;
    virtual void                Push3DView(const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes, ITexture* pDepthTexture) = 0;
    virtual void                GetMatricesForView(const CViewSetup& view, ViewMatrix* pWorldToView, ViewMatrix* pViewToProjection, ViewMatrix* pWorldToProjection, ViewMatrix* pWorldToPixels) = 0;
};