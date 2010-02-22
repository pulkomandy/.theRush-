#ifndef ISTREAMARRAYS_H__
#define ISTREAMARRAYS_H__


enum IMESH_VAF
{
    VAF_XYZ = 1,
    VAF_XYZRHW = 1<<1,
    VAF_NORMAL = 1<<2,
    VAF_COLOR = 1<<3,
    VAF_BINORMAL = 1<<4,
    VAF_BITANGENT = 1<<5,
    VAF_TEX0 = 1<<6,
    VAF_TEX1 = 1<<7,
    VAF_TEX2 = 1<<8,
    VAF_TEX3 = 1<<9,
	VAF_HWI_MATRIX = 1<<10
};

//VAU for flag in vertexarray::init
enum IMESH_VAU
{
	VAU_STATIC,
	VAU_STREAM,
	VAU_DYNAMIC
};

enum IMESH_CREATION_FLAG
{
    VACF_NORMAL,
    VACF_DYNAMIC
};

enum IMESH_LOCK
{
    VAL_READONLY = 1,
    VAL_WRITE = 2
};


DECLAREZCLASS(ZVertexArrayDX9);
DECLAREZCLASS(ZIndexArrayDX9);
DECLAREZCLASS(ZVertexArrayOGL);
DECLAREZCLASS(ZIndexArrayOGL);
DECLAREZCLASS(ZVertexArray);
DECLAREZCLASS(ZIndexArray);



class IVertexArray : public ZBaseClass
{
public:
    virtual void Init(uint32 aFormat, int aQty, bool aKeepVBORam = false, uint32 aFlag = VACF_NORMAL) = 0;
    virtual uint32 GetFormat() const = 0;
    virtual uint32 GetVertexSize() const = 0;
    virtual uint32 GetVertexCount() const = 0;

	// reset (clear VRAM pointer but don't destroy ram things)
	virtual void Reset() {}

    virtual void * Lock(IMESH_LOCK aFlag) = 0;
    virtual void Unlock() = 0;

    virtual bool Bind() = 0;
	virtual void Unbind() = 0;
	virtual void BindHWI(IVertexArray *pInstancesStream) = 0;
};

unsigned int GetVertexSizeFromFormat(uint32 aFormat);

class IIndexArray : public ZBaseClass
{
public:
    virtual void Init(int aQty, uint32 aFlag = VACF_NORMAL) = 0;
    virtual uint32 GetIndexCount() = 0;

	// reset (clear VRAM pointer but don't destroy ram things)
	virtual void Reset() {}

    virtual void * Lock(IMESH_LOCK aFlag) = 0;
    virtual void Unlock() = 0;

    virtual void Bind() = 0;
	virtual void Unbind() = 0;
};



#endif
