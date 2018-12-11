#include "flexbox_vertex_buffer.h"
#include "OgreHlms.h"
#include "OgreHlmsManager.h"
#include "OgreRoot.h"
namespace flexbox {
struct plane_2d_vertex {
  float px, py;      //Position
  float ux, uy;      //UVs
  float nx, ny, nz;  //UVs

  plane_2d_vertex() {
  }
  plane_2d_vertex(float _px, float _py,
                  float _ux, float _uy)
    : px(_px), py(_py),
      ux(_ux), uy(_uy),
      nx(0.0), ny(0.0), nz(1.0) {
  }
};

using namespace Ogre;

const plane_2d_vertex c_originalVertices[] =
    {
      plane_2d_vertex(-0.0f, -1.0f, 0.0f, 1.0f),
      plane_2d_vertex(1.f, -1.0f, 1.0f, 1.0f),
      plane_2d_vertex(-0.0f, 0.0f, 0.0f, 0.0f),
      plane_2d_vertex(1.0f, 0.0f, 1.0f, 0.0f)
    };

flexbox_vertex_buffer::flexbox_vertex_buffer(IdType id, ObjectMemoryManager *objectMemoryManager,
                                             SceneManager *manager, uint8 renderQueueId, Ogre::HlmsUnlitDatablock *block)
  : MovableObject(id, objectMemoryManager, manager, renderQueueId),
    Renderable() {
  //Set the bounds!!! Very important! If you don't set it, the object will not
  //appear on screen as it will always fail the frustum culling.
  //This example uses an infinite aabb; but you really want to use an Aabb as tight
  //as possible for maximum efficiency (so Ogre avoids rendering an object that
  //is off-screen)
  //Note the WorldAabb and the WorldRadius will be automatically updated by Ogre
  //every frame as rendering begins (it's calculated based on the local version
  //combined with the scene node's transform).
  Aabb aabb(Aabb::BOX_INFINITE);
  mObjectData.mLocalAabb->setFromAabb(aabb, mObjectData.mIndex);
  mObjectData.mWorldAabb->setFromAabb(aabb, mObjectData.mIndex);
  mObjectData.mLocalRadius[mObjectData.mIndex] = std::numeric_limits<Real>::max();
  mObjectData.mWorldRadius[mObjectData.mIndex] = std::numeric_limits<Real>::max();

  createBuffers();

  //This is very important!!! A MovableObject must tell what Renderables to render
  //through this array. Since we derive from both MovableObject & Renderable, add
  //ourselves to the array. Otherwise, nothing will be rendered.
  //Tip: You can use this array as a rough way to show or hide Renderables
  //that belong to this MovableObject.
  mRenderables.push_back(this);

  //If we don't set a datablock, we'll crash Ogre.
  this->setDatablock(block);
}
//-----------------------------------------------------------------------------------
flexbox_vertex_buffer::~flexbox_vertex_buffer() {
  VaoManager *vaoManager = mManager->getDestinationRenderSystem()->getVaoManager();

  VertexArrayObjectArray::const_iterator itor = mVaoPerLod[0].begin();
  VertexArrayObjectArray::const_iterator end  = mVaoPerLod[0].end();
  while (itor != end) {
    VertexArrayObject *vao = *itor;

    const VertexBufferPackedVec &         vertexBuffers = vao->getVertexBuffers();
    VertexBufferPackedVec::const_iterator itBuffers     = vertexBuffers.begin();
    VertexBufferPackedVec::const_iterator enBuffers     = vertexBuffers.end();

    while (itBuffers != enBuffers) {
      vaoManager->destroyVertexBuffer(*itBuffers);
      ++itBuffers;
    }

    if (vao->getIndexBuffer())
      vaoManager->destroyIndexBuffer(vao->getIndexBuffer());
    vaoManager->destroyVertexArrayObject(vao);

    ++itor;
  }
}
//-----------------------------------------------------------------------------------
void flexbox_vertex_buffer::createBuffers(void) {
  const Ogre::uint16 triangulated_face[] = {
    0, 1, 2, 1, 3, 2
  };

  Ogre::uint16 *face_indices = reinterpret_cast<Ogre::uint16 *>(OGRE_MALLOC_SIMD(
      sizeof(Ogre::uint16) * sizeof(triangulated_face),
      Ogre::MEMCATEGORY_GEOMETRY));
  memcpy(face_indices, triangulated_face, sizeof(triangulated_face));

  VaoManager *             vaoManager  = mManager->getDestinationRenderSystem()->getVaoManager();
  Ogre::IndexBufferPacked *indexBuffer = 0;

  try {
    indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
                                                6,
                                                Ogre::BT_IMMUTABLE,
                                                face_indices, true);
  } catch (Ogre::Exception &e) {
    // When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
    // However if for some weird reason there is an exception raised, the memory will
    // not be freed, so it is up to us to do so.
    // The reasons for exceptions are very rare. But we're doing this for correctness.
    OGRE_FREE_SIMD(indexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
    indexBuffer = 0;
    throw e;
  }

  //Create the vertex buffer

  //Vertex declaration
  VertexElement2Vec vertexElements;
  vertexElements.push_back(VertexElement2(VET_FLOAT2, VES_POSITION));
  vertexElements.push_back(VertexElement2(VET_FLOAT2, VES_TEXTURE_COORDINATES));
  vertexElements.push_back(VertexElement2(VET_FLOAT3, VES_NORMAL));

  //For immutable buffers, it is mandatory that plane_2d_vertex is not a null pointer.
  plane_2d_vertex *vertices = reinterpret_cast<plane_2d_vertex *>(OGRE_MALLOC_SIMD(
      sizeof(plane_2d_vertex) * 4,
      Ogre::MEMCATEGORY_GEOMETRY));
  //Fill the data.
  memcpy(vertices, c_originalVertices, sizeof(plane_2d_vertex) * 4);

  Ogre::VertexBufferPacked *vertexBuffer = 0;
  try {
    //Create the actual vertex buffer.
    vertexBuffer = vaoManager->createVertexBuffer(vertexElements, 4,
                                                  BT_IMMUTABLE,
                                                  vertices, true);
  } catch (Ogre::Exception &e) {
    OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
    vertexBuffer = 0;
    throw e;
  }

  //Now the Vao. We'll just use one vertex buffer source (multi-source not working yet)
  VertexBufferPackedVec vertexBuffers;
  vertexBuffers.push_back(vertexBuffer);
  Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
      vertexBuffers, indexBuffer, OT_TRIANGLE_LIST);

  mVaoPerLod[0].push_back(vao);
  //Use the same geometry for shadow casting. You can optimize performance by creating
  //a different Vao that only uses VES_POSITION, VES_BLEND_INDICES & VES_BLEND_WEIGHTS
  //(packed together to fit the caches) and avoids duplicated vertices (usually
  //needed by normals, UVs, etc)
  mVaoPerLod[1].push_back(vao);
}
//-----------------------------------------------------------------------------------
const String &flexbox_vertex_buffer::getMovableType(void) const {
  return BLANKSTRING;
}
//-----------------------------------------------------------------------------------
const LightList &flexbox_vertex_buffer::getLights(void) const {
  return this->queryLights();  //Return the data from our MovableObject base class.
}
//-----------------------------------------------------------------------------------
void flexbox_vertex_buffer::getRenderOperation(v1::RenderOperation &op, bool casterPass) {
  OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
              "renderable_3d_widget do not implement getRenderOperation."
              " You've put a v2 object in "
              "the wrong RenderQueue ID (which is set to be compatible with "
              "v1::Entity). Do not mix v2 and v1 objects",
              "flexbox_vertex_buffer::getRenderOperation");
}
//-----------------------------------------------------------------------------------
void flexbox_vertex_buffer::getWorldTransforms(Matrix4 *xform) const {
  OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
              "renderable_3d_widget do not implement getWorldTransforms."
              " You've put a v2 object in "
              "the wrong RenderQueue ID (which is set to be compatible with "
              "v1::Entity). Do not mix v2 and v1 objects",
              "flexbox_vertex_buffer::getRenderOperation");
}
//-----------------------------------------------------------------------------------
bool flexbox_vertex_buffer::getCastsShadows(void) const {
  OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
              "renderable_3d_widget do not implement getCastsShadows."
              " You've put a v2 object in "
              "the wrong RenderQueue ID (which is set to be compatible with "
              "v1::Entity). Do not mix v2 and v1 objects",
              "flexbox_vertex_buffer::getRenderOperation");
}
}