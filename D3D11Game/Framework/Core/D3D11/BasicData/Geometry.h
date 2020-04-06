#pragma once
#include "Framework.h"

template <class T>
class Geometry
{
public:
    Geometry() = default;
    virtual ~Geometry();

    //=====================================
    // [Vertex]
    //=====================================
    auto GetVertexCount() const -> const uint { return static_cast<uint>(vertices.size()); }
    auto GetVertexByteWidth() const -> const uint { return GetVertexCount() * sizeof(T); }
    auto GetVertexPointer() -> T* { return vertices.data(); }
    auto GetVertices() const -> const std::vector<T>& { return vertices; }
    auto GetVertices(const uint& offset, const uint& count) -> const std::vector<T>;

    void AddVertex(const T& vertex);
    void AddVertices(const std::vector<T>& vertices);
    void SetVertices(const std::vector<T>& vertices);

    //=====================================
    // [Index]
    //=====================================
    auto GetIndexCount() const -> const uint { return static_cast<uint>(indices.size()); }
    auto GetIndexByteWidth() const -> const uint { return GetIndexCount() * sizeof(uint); }
    auto GetIndexPointer() -> uint* { return indices.data(); }
    auto GetIndices() const -> const std::vector<uint>& { return indices; }
    auto GetIndices(const uint& offset, const uint& count) -> const std::vector<uint>;

    void AddIndex(const uint& index);
    void AddIndices(const std::vector<uint>& indices);
    void SetIndices(const std::vector<uint>& indices);

    //=====================================
    // [Mics]
    //=====================================
    void Clear();

protected:
    std::vector<T> vertices;
    std::vector<uint> indices;
};
#include "Geometry.inl"