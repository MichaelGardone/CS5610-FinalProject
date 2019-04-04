#include "Object.h"

#ifdef NDEBUG
bool debug = false;
#else
bool debug = true;
#endif

/*
public:
	Object();

	virtual ~Object();
	virtual void Reset();
	virtual void Draw();
	virtual void Update();

	// .vert = verex, .frag = fragment, .tcs = tessellation control, .tes = tessellation evaluation, .geom = geometry, .cmpte = compute
	virtual void LoadShaders(char **shaders);

	// Return a pointer to the Matrix4f to speed up return
	virtual cyMatrix4f* model_matrix() const;

private:
	// Position information
	cyPoint3f position;
	cyPoint3f scale;
	cyPoint3f rotation;

	// Matrix/world information
	cyMatrix4f model_matrix;

	// Shader information
	cyGLSLProgram program;
	cyGLSLShader vert, frag, tcs, tes, geom, compute;
*/

Object::Object() :
	position(0.f),
	scale(1.f),
	rotation(0.f)
{
	// Do nothing?
}

// Currently only loads one file rather than compiling multiple
// TODO: Compile multiple shader types (i.e. vertex) to make one massive file
void Object::LoadShaders(char** shaders, int length)
{
	if (debug)
		std::cout << "Loading shaders for " << name << std::endl;

	for (int i = 0; i < length; i++)
	{
		if (strstr(shaders[i], ".vert"))
		{
			vert.CompileFile(shaders[i], GL_VERTEX_SHADER);
		}
		else if (strstr(shaders[i], ".frag"))
		{
			frag.CompileFile(shaders[i], GL_FRAGMENT_SHADER);
		}
		else {
			std::cout << "Shader type not supported at this time!" << std::endl;
		}
	}

	if (vert.IsNull() == false)
		program.AttachShader(vert);

	if (frag.IsNull() == false)
		program.AttachShader(frag);

	// Do not put this in debug only -- if something fails in production, we need to know about it
	if (vert.IsNull() == false && geom.IsNull() == false)
	{
		std::cout << "Error: Unable to load either the geometry shader or vertex shader! Program will not run properly and therefore is exiting!"
			<< std::endl;
		exit(1);
	}

	// Link the shaders together
	program.Link();

	if (debug)
		std::cout << "Shaders loaded for " << name << std::endl;
}


