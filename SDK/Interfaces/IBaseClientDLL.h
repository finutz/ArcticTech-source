#pragma once

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

enum EClientFrameStage
{
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END
};

// Used by RenderView
enum RenderViewInfo_t
{
    RENDERVIEW_UNSPECIFIED = 0,
    RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
    RENDERVIEW_DRAWHUD = (1 << 1),
    RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

class IClientNetworkable;

typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

enum SendPropType
{
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY,
    DPT_String,
    DPT_Array,
    DPT_DataTable,
    DPT_Int64,
    DPT_NUMSendPropTypes
};

class DVariant
{
public:
    union
    {
        float	   m_Float;
        long	   m_Int;
        char* m_pString;
        void* m_pData;
        float	   m_Vector[3];
        __int64  m_Int64;
    };
    SendPropType  m_Type;
};

class RecvTable;
class RecvProp;

class CRecvProxyData
{
public:
    const RecvProp* m_pRecvProp;        // The property it's receiving.
    DVariant		    m_Value;            // The value given to you to store.
    int				    m_iElement;         // Which array element you're getting.
    int				    m_ObjectID;         // The object being referred to.
};

typedef void(*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);
using RecvVarProxy_t = void( __cdecl* )( CRecvProxyData* data, void* struct_ptr, void* out );
typedef void(*ArrayLengthRecvProxyFn)(void* pStruct, int objectID, int currentArrayLength);
typedef void(*DataTableRecvVarProxyFn)(const RecvProp* pProp, void** pOut, void* pData, int objectID);

class RecvProp
{
public:
    char* m_pVarName;
    SendPropType            m_RecvType;
    int                     m_Flags;
    int                     m_StringBufferSize;
    int                     m_bInsideArray;
    const void* m_pExtraData;
    RecvProp* m_pArrayProp;
    ArrayLengthRecvProxyFn  m_ArrayLengthProxy;
    RecvVarProxy_t			 m_ProxyFn;
    DataTableRecvVarProxyFn m_DataTableProxyFn;
    RecvTable* m_pDataTable;
    int                     m_Offset;
    int                     m_ElementStride;
    int                     m_nElements;
    const char* m_pParentArrayPropName;

    RecvVarProxy_t			GetProxyFn() const;
    void					SetProxyFn( RecvVarProxy_t fn);
    DataTableRecvVarProxyFn	GetDataTableProxyFn() const;
    void					SetDataTableProxyFn(DataTableRecvVarProxyFn fn);

};

class RecvTable
{
public:
    RecvProp* m_pProps;
    int                     m_nProps;
    void* m_pDecoder;
    char* m_pNetTableName;
    bool                    m_bInitialized;
    bool                    m_bInMainList;
};

inline RecvVarProxy_t RecvProp::GetProxyFn() const
{
    return m_ProxyFn;
}

inline void RecvProp::SetProxyFn( RecvVarProxy_t fn)
{
    m_ProxyFn = fn;
}

inline DataTableRecvVarProxyFn RecvProp::GetDataTableProxyFn() const
{
    return m_DataTableProxyFn;
}

inline void RecvProp::SetDataTableProxyFn(DataTableRecvVarProxyFn fn)
{
    m_DataTableProxyFn = fn;
}

class ClientClass {
public:
    CreateClientClassFn m_pCreateFn;
    void* m_pCreateEventFn;
    char* m_pNetworkName;
    RecvTable* m_pRecvTable;
    ClientClass* m_pNext;
    int             m_ClassID;
};

class IBaseClientDLL
{
public:
    virtual int              Connect(CreateInterfaceFn appSystemFactory, void* pGlobals) = 0;
    virtual int              Disconnect(void) = 0;
    virtual int              Init(CreateInterfaceFn appSystemFactory, void* pGlobals) = 0;
    virtual void             PostInit() = 0;
    virtual void             Shutdown(void) = 0;
    virtual void             LevelInitPreEntity(char const* pMapName) = 0;
    virtual void             LevelInitPostEntity() = 0;
    virtual void             LevelShutdown(void) = 0;
    virtual ClientClass* GetAllClasses(void) = 0;
};