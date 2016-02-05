#include <Gizmos.h>
#include <gl_core_4_4.h>
#define GLM_SWIZZLE
#define GLM_FORCE_PURE
#include <glm/glm.hpp>

#include <glm/ext.hpp>

Gizmos* Gizmos::sm_singleton = nullptr;

Gizmos::Gizmos(unsigned int a_maxLines, unsigned int a_maxTris,
			   unsigned int a_max2DLines, unsigned int a_max2DTris)
	: m_maxLines(a_maxLines),
	m_lineCount(0),
	m_lines(new GizmoLine[a_maxLines]),
	m_maxTris(a_maxTris),
	m_triCount(0),
	m_tris(new GizmoTri[a_maxTris]),
	m_transparentTriCount(0),
	m_transparentTris(new GizmoTri[a_maxTris]),
	m_max2DLines(a_max2DLines),
	m_2DlineCount(0),
	m_2Dlines(new GizmoLine[a_max2DLines]),
	m_max2DTris(a_max2DTris),
	m_2DtriCount(0),
	m_2Dtris(new GizmoTri[a_max2DTris])
{
	// create shaders
	const char* vsSource = "#version 150\n \
					 in vec4 Position; \
					 in vec4 Colour; \
					 out vec4 vColour; \
					 uniform mat4 ProjectionView; \
					 void main() { vColour = Colour; gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 150\n \
					 in vec4 vColour; \
                     out vec4 FragColor; \
					 void main()	{ FragColor = vColour; }";
    
    
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, (const char**)&vsSource, 0);
	glCompileShader(vs);

	glShaderSource(fs, 1, (const char**)&fsSource, 0);
	glCompileShader(fs);

	m_shader = glCreateProgram();
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);
	glBindAttribLocation(m_shader, 0, "Position");
	glBindAttribLocation(m_shader, 1, "Colour");
	glLinkProgram(m_shader);
    
	int success = GL_FALSE;
    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link Gizmo shader program!\n%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
    
    // create VBOs
	glGenBuffers( 1, &m_lineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(GizmoLine), m_lines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_triVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_tris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_transparentTriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_transparentTris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DlineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DLines * sizeof(GizmoLine), m_2Dlines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DtriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DTris * sizeof(GizmoTri), m_2Dtris, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_triVAO);
	glBindVertexArray(m_triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_transparentTriVAO);
	glBindVertexArray(m_transparentTriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_2DlineVAO);
	glBindVertexArray(m_2DlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_2DtriVAO);
	glBindVertexArray(m_2DtriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Gizmos::~Gizmos()
{
	delete[] m_lines;
	delete[] m_tris;
	delete[] m_transparentTris;
	glDeleteBuffers( 1, &m_lineVBO );
	glDeleteBuffers( 1, &m_triVBO );
	glDeleteBuffers( 1, &m_transparentTriVBO );
	glDeleteVertexArrays( 1, &m_lineVAO );
	glDeleteVertexArrays( 1, &m_triVAO );
	glDeleteVertexArrays( 1, &m_transparentTriVAO );
	delete[] m_2Dlines;
	delete[] m_2Dtris;
	glDeleteBuffers( 1, &m_2DlineVBO );
	glDeleteBuffers( 1, &m_2DtriVBO );
	glDeleteVertexArrays( 1, &m_2DlineVAO );
	glDeleteVertexArrays( 1, &m_2DtriVAO );
	glDeleteProgram(m_shader);
}

void Gizmos::create(unsigned int a_maxLines /* = 0xffff */, unsigned int a_maxTris /* = 0xffff */,
					unsigned int a_max2DLines /* = 0xff */, unsigned int a_max2DTris /* = 0xff */)
{
	if (sm_singleton == nullptr)
		sm_singleton = new Gizmos(a_maxLines,a_maxTris,a_max2DLines,a_max2DTris);
}

void Gizmos::destroy()
{
	delete sm_singleton;
	sm_singleton = nullptr;
}

void Gizmos::clear()
{
	sm_singleton->m_lineCount = 0;
	sm_singleton->m_triCount = 0;
	sm_singleton->m_transparentTriCount = 0;
	sm_singleton->m_2DlineCount = 0;
	sm_singleton->m_2DtriCount = 0;
}

// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
// at the transform's translation. Optional scale available.
void Gizmos::addTransform(const glm::mat4& a_transform, float a_fScale /* = 1.0f */)
{
	glm::vec4 vXAxis = a_transform[3] + a_transform[0] * a_fScale;
	glm::vec4 vYAxis = a_transform[3] + a_transform[1] * a_fScale;
	glm::vec4 vZAxis = a_transform[3] + a_transform[2] * a_fScale;

	glm::vec4 vRed(1,0,0,1);
	glm::vec4 vGreen(0,1,0,1);
	glm::vec4 vBlue(0,0,1,1);

	addLine(a_transform[3].xyz(), vXAxis.xyz(), vRed, vRed);
	addLine(a_transform[3].xyz(), vYAxis.xyz(), vGreen, vGreen);
	addLine(a_transform[3].xyz(), vZAxis.xyz(), vBlue, vBlue);
}



void Gizmos::addArcRing(const glm::vec3& a_center, float a_rotation, 
	float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*a_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*a_transform * glm::vec4(v2inner, 0)).xyz();
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center + v2outer, a_center + v1outer, a_center + v1inner, a_fillColour);
			addTri(a_center + v1inner, a_center + v2inner, a_center + v2outer, a_fillColour);

			addTri(a_center + v1inner, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v2inner, a_center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1inner, a_center + v2inner, vSolid, vSolid);
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*a_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*a_transform * glm::vec4(v2inner, 0)).xyz();
		}

		addLine(a_center + v1inner, a_center + v1outer, vSolid, vSolid);
		addLine(a_center + v2inner, a_center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addSphere(const glm::vec3& a_center, float a_radius, int a_rows, int a_columns, const glm::vec4& a_fillColour, 
								const glm::mat4* a_transform /*= nullptr*/, float a_longMin /*= 0.f*/, float a_longMax /*= 360*/, 
								float a_latMin /*= -90*/, float a_latMax /*= 90*/)
{
	float inverseRadius = 1/a_radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.0f/float(a_columns);
	float invRows = 1.0f/float(a_rows);

	float DEG2RAD = glm::pi<float>() / 180;
	
	//Lets put everything in radians first
	float latitiudinalRange = (a_latMax - a_latMin) * DEG2RAD;
	float longitudinalRange = (a_longMax - a_longMin) * DEG2RAD;
	// for each row of the mesh
	glm::vec3* v4Array = new glm::vec3[a_rows*a_columns + a_columns];

	for (int row = 0; row <= a_rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis  = ratioAroundXAxis * latitiudinalRange + (a_latMin * DEG2RAD);
		float y  =  a_radius * sin(radiansAboutXAxis);
		float z  =  a_radius * cos(radiansAboutXAxis);
		
		for ( int col = 0; col <= a_columns; ++col )
		{
			float ratioAroundYAxis   = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (a_longMin * DEG2RAD);
			glm::vec3 v4Point( -z * sinf(theta), y, -z * cosf(theta) );
			glm::vec3 v4Normal( inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (a_transform != nullptr)
			{
				v4Point = (*a_transform * glm::vec4(v4Point, 0)).xyz();
				v4Normal = (*a_transform * glm::vec4(v4Normal, 0)).xyz();
			}

			int index = row * a_columns + (col % a_columns);
			v4Array[index] = v4Point;
		}
	}
	
	for (int face = 0; face < (a_rows)*(a_columns); ++face )
	{
		int iNextFace = face + 1;		
		
		if( iNextFace % a_columns == 0 )
		{
			iNextFace = iNextFace - (a_columns);
		}

		addLine(a_center + v4Array[face], a_center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));
		
		if( face % a_columns == 0 && longitudinalRange < (glm::pi<float>() * 2))
		{
				continue;
		}
		addLine(a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));

		addTri( a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face], a_center + v4Array[iNextFace], a_fillColour);
		addTri( a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face+a_columns], a_center + v4Array[face], a_fillColour);		
	}

	delete[] v4Array;	
}


void Gizmos::addLine(const glm::vec3& a_rv0,  const glm::vec3& a_rv1, const glm::vec4& a_colour)
{
	addLine(a_rv0,a_rv1,a_colour,a_colour);
}

void Gizmos::addLine(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec4& a_colour0, const glm::vec4& a_colour1)
{
	if (sm_singleton != nullptr &&
		sm_singleton->m_lineCount < sm_singleton->m_maxLines)
	{
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.x = a_rv0.x;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.y = a_rv0.y;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.z = a_rv0.z;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.w = 1;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.r = a_colour0.r;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.g = a_colour0.g;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.b = a_colour0.b;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.a = a_colour0.a;

		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.x = a_rv1.x;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.y = a_rv1.y;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.z = a_rv1.z;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.w = 1;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.r = a_colour1.r;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.g = a_colour1.g;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.b = a_colour1.b;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.a = a_colour1.a;

		sm_singleton->m_lineCount++;
	}
}

void Gizmos::addTri(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec3& a_rv2, const glm::vec4& a_colour)
{
	if (sm_singleton != nullptr)
	{
		if (a_colour.w == 1)
		{
			if (sm_singleton->m_triCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.x = a_rv0.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.y = a_rv0.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.z = a_rv0.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.w = 1;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.x = a_rv1.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.y = a_rv1.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.z = a_rv1.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.w = 1;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.x = a_rv2.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.y = a_rv2.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.z = a_rv2.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.w = 1;

				sm_singleton->m_tris[sm_singleton->m_triCount].v0.r = a_colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.g = a_colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.b = a_colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.w = a_colour.a;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.r = a_colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.g = a_colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.b = a_colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.w = a_colour.a;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.r = a_colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.g = a_colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.b = a_colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.w = a_colour.a;

				sm_singleton->m_triCount++;
			}
		}
		else
		{
			if (sm_singleton->m_transparentTriCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.x = a_rv0.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.y = a_rv0.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.z = a_rv0.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.w = 1;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.x = a_rv1.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.y = a_rv1.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.z = a_rv1.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.w = 1;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.x = a_rv2.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.y = a_rv2.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.z = a_rv2.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.w = 1;

				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.r = a_colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.g = a_colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.b = a_colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.w = a_colour.a;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.r = a_colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.g = a_colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.b = a_colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.w = a_colour.a;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.r = a_colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.g = a_colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.b = a_colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.w = a_colour.a;

				sm_singleton->m_transparentTriCount++;
			}
		}
	}
}

void Gizmos::draw(const glm::mat4& a_projection, const glm::mat4& a_view)
{
	draw(a_projection * a_view);
}

void Gizmos::draw(const glm::mat4& a_projectionView)
{
	if ( sm_singleton != nullptr && (sm_singleton->m_lineCount > 0 || sm_singleton->m_triCount > 0 || sm_singleton->m_transparentTriCount > 0))
	{
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		glUseProgram(sm_singleton->m_shader);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_shader,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionView));

		if (sm_singleton->m_lineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_lineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_lineCount * sizeof(GizmoLine), sm_singleton->m_lines);

			glBindVertexArray(sm_singleton->m_lineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_lineCount * 2);
		}

		if (sm_singleton->m_triCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_triVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_triCount * sizeof(GizmoTri), sm_singleton->m_tris);

			glBindVertexArray(sm_singleton->m_triVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_triCount * 3);
		}

		if (sm_singleton->m_transparentTriCount > 0)
		{
			// not ideal to store these, but Gizmos must work stand-alone
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);
			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
			int src, dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			// setup blend states
			if (blendEnabled == GL_FALSE)
				glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_transparentTriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_transparentTriCount * sizeof(GizmoTri), sm_singleton->m_transparentTris);

			glBindVertexArray(sm_singleton->m_transparentTriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_transparentTriCount * 3);

			// reset state
			glDepthMask(depthMask);
			glBlendFunc(src, dst);
			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(shader);
	}
}

void Gizmos::draw2D(const glm::mat4& a_projection)
{
	if ( sm_singleton != nullptr && (sm_singleton->m_2DlineCount > 0 || sm_singleton->m_2DtriCount > 0))
	{
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		glUseProgram(sm_singleton->m_shader);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_shader,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projection));

		if (sm_singleton->m_2DlineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DlineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DlineCount * sizeof(GizmoLine), sm_singleton->m_2Dlines);

			glBindVertexArray(sm_singleton->m_2DlineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_2DlineCount * 2);
		}

		if (sm_singleton->m_2DtriCount > 0)
		{
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);

			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

			int src, dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			if (blendEnabled == GL_FALSE)
				glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DtriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DtriCount * sizeof(GizmoTri), sm_singleton->m_2Dtris);

			glBindVertexArray(sm_singleton->m_2DtriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_2DtriCount * 3);

			glDepthMask(depthMask);

			glBlendFunc(src, dst);

			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(shader);
	}
}