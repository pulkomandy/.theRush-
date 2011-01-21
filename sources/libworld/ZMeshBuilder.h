#ifndef ZMESHBUILDER_H__
#define ZMESHBUILDER_H__

#include "../libbase/ZBaseMaths.h"

class ZMesh;

ZMesh * CreateWireGrid();

/// Flat untextured box
ZMesh * CreateBox(const tvector3 &size, const tcolor &col);

ZMesh * CreateFacetedBox(const tvector3 &size);
/// Wire box (for selection bounding box)
ZMesh * CreateWireBox(const tvector3 &size, const tcolor &col);

/// sphere
ZMesh* CreateSphere(int nbSeg, float rayon);
ZMesh* CreateGeodesicSphere(int anbIter, float radius);

ZMesh* CreateQuad(const tvector3& vt1, const tvector3& vt2, const tvector3& vt3, const tvector3& vt4,
				  const tvector2& uv1, const tvector2& uv2, const tvector2& uv3, const tvector2& uv4);

ZMesh* CreatePyramid(float width, float depth, float height, const tcolor & col);

ZMesh * Create2DDisk(float size = 1, float segments = 128);

ZMesh * CreateDummySphere();
ZMesh * CreateDummyCone();
ZMesh * CreateCone(float radius, float length, int nbSegments);
ZMesh * CreateSkyBox();
ZMesh * BuildScreenQuad(float SizeX, float SizeY);

class ZMeshInstance;
ZMeshInstance * AddRect(float x, float y, float sx, float sy, float u1, float v1, float u2, float v2);

#endif
