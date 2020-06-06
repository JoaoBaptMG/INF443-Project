#include "ModelMesh.hpp"

#include "Model.hpp"
#include "ModelUtils.hpp"
#include "resources/bufferUtils.hpp"
#include "wrappers/glParamFromType.hpp"
#include "assimpVectorTraits.hpp"

using namespace model;

ModelMesh::ModelMesh(const gl::Program& program, const aiMesh* mesh)
{
    materialIndex = mesh->mMaterialIndex;

    // Generate the vertex array
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // generate the first attributes
    positionBuffer = gl::createAndConfigureVertexArray(mesh->mVertices, mesh->mNumVertices,
        program.getAttributeLocation("inPosition"));

    normalBuffer = gl::createAndConfigureVertexArray(mesh->mNormals, mesh->mNumVertices,
        program.getAttributeLocation("inNormal"));

    // Now, generate the colors and texcoords
    auto numColors = mesh->GetNumColorChannels();
    colorBuffers.resize(numColors);
    for (unsigned int i = 0; i < numColors; i++)
    {
        std::string name = "inColor[" + std::to_string(i) + ']';
        colorBuffers[i] = gl::createAndConfigureVertexArray(mesh->mColors[i], mesh->mNumVertices,
            program.getAttributeLocation(name.c_str()));
    }

    auto numTexcoords = mesh->GetNumUVChannels();
    texcoordBuffers.resize(numTexcoords);
    for (unsigned int i = 0; i < numTexcoords; i++)
    {
        std::string name = "inTexcoord[" + std::to_string(i) + ']';
        texcoordBuffers[i] = gl::createAndConfigureVertexArray(mesh->mTextureCoords[i], mesh->mNumVertices,
            program.getAttributeLocation(name.c_str()));
    }

    configureAnimationBones(mesh, program);

    // Check the primitive type
    elementBuffer = 0;
    switch (mesh->mPrimitiveTypes)
    {
    case aiPrimitiveType_POINT: primitiveType = gl::PrimitiveType::Points; numElements = mesh->mNumFaces; break;
    case aiPrimitiveType_LINE: primitiveType = gl::PrimitiveType::Lines; numElements = 2 * mesh->mNumFaces; break;
    case aiPrimitiveType_TRIANGLE: primitiveType = gl::PrimitiveType::Triangles; numElements = 3 * mesh->mNumFaces; break;
    default: throw ModelException("Unrecognized primitive type!");
    }
    
    // Build the element buffer
    std::vector<unsigned int> elements;
    elements.reserve(numElements);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            elements.push_back(mesh->mFaces[i].mIndices[j]);
    elementBuffer = gl::createAndFillBuffer(elements, GL_ELEMENT_ARRAY_BUFFER);

    // Unbind the vertex array
    glBindVertexArray(0);
}

void model::ModelMesh::configureAnimationBones(const aiMesh* mesh, const gl::Program& program)
{
    // Now, onwards to do the bone and the weight buffer
    std::vector<glm::ivec4> bones(mesh->mNumVertices, glm::ivec4(-1, -1, -1, -1));
    std::vector<glm::vec4> weights(mesh->mNumVertices, glm::vec4(0, 0, 0, 0));

    // Attach names to the nodes
    boneMatrices.resize(mesh->mNumBones);
    boneNames.resize(mesh->mNumBones);
    boneNodeIndices.resize(mesh->mNumBones);

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        auto bone = mesh->mBones[i];
        boneMatrices[i] = toGlm(mesh->mBones[i]->mOffsetMatrix);
        boneNames[i] = mesh->mBones[i]->mName.C_Str();

        for (unsigned int j = 0; j < bone->mNumWeights; j++)
        {
            // Try to find an empty slot
            auto idx = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            for (unsigned int k = 0; k < 4; k++)
                if (bones[idx][k] == -1)
                {
                    bones[idx][k] = i;
                    weights[idx][k] = weight;
                    break;
                }
        }
    }

    // Fill the bone buffer with zeros
    for (auto& bone : bones)
    {
        if (bone.x == -1) bone.x = 0;
        if (bone.y == -1) bone.y = 0;
        if (bone.z == -1) bone.z = 0;
        if (bone.w == -1) bone.w = 0;
    }

    // Create the bone buffer
    boneBuffer = gl::createAndConfigureVertexArrayInteger(bones, program.getAttributeLocation("inBoneIDs"));
    weightBuffer = gl::createAndConfigureVertexArray(weights, program.getAttributeLocation("inBoneWeights"));
}

ModelMesh::~ModelMesh()
{
    // Cleanup everything
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(colorBuffers.size(), colorBuffers.data());
    glDeleteBuffers(texcoordBuffers.size(), texcoordBuffers.data());
    glDeleteBuffers(1, &boneBuffer);
    glDeleteBuffers(1, &weightBuffer);
    glDeleteBuffers(1, &elementBuffer);
}

ModelMesh::ModelMesh(ModelMesh&& o) noexcept
    : vertexArray(o.vertexArray), positionBuffer(o.positionBuffer), normalBuffer(o.normalBuffer),
    colorBuffers(std::move(o.colorBuffers)), texcoordBuffers(std::move(o.texcoordBuffers)),
    boneBuffer(o.boneBuffer), weightBuffer(o.weightBuffer), elementBuffer(o.elementBuffer),
    numElements(o.numElements), primitiveType(o.primitiveType), materialIndex(o.materialIndex),
    boneMatrices(std::move(boneMatrices)), boneNames(std::move(boneNames)), boneNodeIndices(std::move(boneNodeIndices))
{
    o.vertexArray = 0;
    o.positionBuffer = 0;
    o.normalBuffer = 0;
    o.boneBuffer = 0;
    o.weightBuffer = 0;
    o.elementBuffer = 0;
}

ModelMesh& ModelMesh::operator=(ModelMesh&& o) noexcept
{
    using std::swap;
    swap(vertexArray, o.vertexArray);
    swap(positionBuffer, o.positionBuffer);
    swap(normalBuffer, o.normalBuffer);
    swap(colorBuffers, o.colorBuffers);
    swap(texcoordBuffers, o.texcoordBuffers);
    swap(boneBuffer, o.boneBuffer);
    swap(weightBuffer, o.weightBuffer);
    swap(elementBuffer, o.elementBuffer);
    swap(numElements, o.numElements);
    swap(primitiveType, o.primitiveType);
    swap(materialIndex, o.materialIndex);
    swap(boneMatrices, o.boneMatrices);
    swap(boneNames, o.boneNames);
    swap(boneNodeIndices, o.boneNodeIndices);
    return *this;
}

void ModelMesh::draw(const glm::mat4& model) const
{
    if (numElements == 0) return;

    // Bind the vertex array
    glBindVertexArray(vertexArray);

    // Bind the vertex attribute
    glVertexAttrib4fv(4, glm::value_ptr(model[0]));
    glVertexAttrib4fv(5, glm::value_ptr(model[1]));
    glVertexAttrib4fv(6, glm::value_ptr(model[2]));
    glVertexAttrib4fv(7, glm::value_ptr(model[3]));

    // Use the appropriate draw function
    glDrawElements(static_cast<GLenum>(primitiveType), numElements, GL_UNSIGNED_INT, nullptr);
}
