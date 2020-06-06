#pragma once

#include <glad/glad.h>
#include <algorithm>
#include <string>

namespace gl
{
    enum class QueryType : GLenum
    {
        SamplesPassed = GL_SAMPLES_PASSED,
        AnySamplesPassed = GL_ANY_SAMPLES_PASSED,
        PrimitivesGenerated = GL_PRIMITIVES_GENERATED,
        TransformFeedbackPrimitivesWritten = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
        TimeElapsed = GL_TIME_ELAPSED
    };

    class Query final
    {
        GLuint query;
        QueryType type;

    public:
        Query(QueryType type) : type(type) { glGenQueries(1, &query); }
        ~Query() { glDeleteQueries(1, &query); }

        // Disallow copying
        Query(const Query&) = delete;
        Query& operator=(const Query&) = delete;

        // Enable moving
        Query(Query&& o) noexcept : query(o.query), type(o.type) { o.query = 0; }
        Query& operator=(Query&& o) noexcept
        {
            std::swap(query, o.query);
            type = o.type;
            return *this;
        }

        void setName(const std::string& name)
        {
            glObjectLabelKHR(GL_QUERY, query, name.size(), name.data());
        }

        void begin() const { glBeginQuery(static_cast<GLenum>(type), query); }
        void end() const { glEndQuery(static_cast<GLenum>(type)); }

        bool available() const
        {
            GLint param;
            glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &param);
            return param;
        }

        GLuint64 result() const
        {
            GLuint64 param;
            glGetQueryObjectui64v(query, GL_QUERY_RESULT, &param);
            return param;
        }
    };
}