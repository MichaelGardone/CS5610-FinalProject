#include "GBuffer.h"
#include "Headers/System.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <cstring>

/*
public:
	GBuffer();
	GBuffer(int);
	GBuffer(int, int);
	~GBuffer();

	void add_buffer(const char*);
	bool delete_buffer(const char*);
	bool delete_buffer(int);

	void generate_attachments();

	void generate_depthbuffer();

	int get_gbuffer_size() const;
	int get_attach_size() const;

	unsigned int get_gbuffer() const;

private:
	unsigned int gbuffer;
	unsigned int renderbuffer;

	unsigned int* buffers;
	unsigned char** buffer_names;

	unsigned int* attachments;

	// DEBUG AND HOUSEKEEPING
	// Size of the gbuffer
	int gbuffer_size;
*/

GBuffer::GBuffer()
{
	this->buffer_names.resize(MIN_GBUFFER_DEPTH);
	this->buffers = new unsigned int[MIN_GBUFFER_DEPTH];

	this->attachments = new unsigned int[MIN_ATTACHMENTS];

	this->gbuffer_size = MIN_GBUFFER_DEPTH;
	this->attach_size = MIN_ATTACHMENTS;
	this->gbuffer_curr = 0;
}

GBuffer::GBuffer(int bc)
{
	this->buffer_names.resize(bc);
	this->buffers = new unsigned int[bc];

	this->attachments = new unsigned int[MIN_ATTACHMENTS];

	this->gbuffer_size = bc;
	this->attach_size = MIN_ATTACHMENTS;
	this->gbuffer_curr = 0;
}

GBuffer::GBuffer(int bc, int ac)
{
	this->buffer_names.resize(bc);
	this->buffers = new unsigned int[bc];

	this->attachments = new unsigned int[ac];

	this->gbuffer_size = bc;
	this->attach_size = ac;
	this->gbuffer_curr = 0;
}

GBuffer::~GBuffer()
{
	for (int i = 0; i < this->gbuffer_size; i++)
	{
		delete buffer_names[i];
		buffer_names[i] = NULL;
	}

	delete[] buffers;
	buffers = NULL;
	delete[] attachments;
	attachments = NULL;
}

void GBuffer::add_buffer(const char* name)
{
	glGenTextures(1, &this->buffers[gbuffer_curr]);
	glBindTexture(GL_TEXTURE_2D, this->buffers[gbuffer_curr]);

	if(!strstr(name, "albedo"))
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	switch (this->gbuffer_curr)
	{
	case 0:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->buffers[gbuffer_curr], 0);
		break;
	case 1:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->buffers[gbuffer_curr], 0);
		break;
	case 2:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->buffers[gbuffer_curr], 0);
		break;
	case 3:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, this->buffers[gbuffer_curr], 0);
		break;
	}

	char *s = new char[strlen(name)+1];
	strcpy(s, name);
	this->buffer_names[gbuffer_curr] = s;

	gbuffer_curr++;
}

void GBuffer::resize_gbuffer(int size)
{
	delete[] this->buffers;
	this->buffers = new unsigned int[size];
	this->gbuffer_size = size;
}

void GBuffer::resize_attachments(int size)
{
	delete[] this->attachments;
	this->attachments = new unsigned int[size];
	this->attach_size = size;
}

bool GBuffer::delete_buffer(const char* name)
{
	return false;
}

bool GBuffer::delete_buffer(int name)
{
	return false;
}

void GBuffer::generate_attachments()
{
	glDrawBuffers(this->attach_size, this->attachments);
}

void GBuffer::generate_depthbuffer()
{
	glGenRenderbuffers(1, &this->renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, this->renderbuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "### WARNING: Framebuffer incomplete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int GBuffer::get_gbuffer_size() const
{
	return this->gbuffer_size;
}

int GBuffer::get_attach_size() const
{
	return this->attach_size;
}

unsigned int GBuffer::get_gbuffer() const
{
	return this->gbuffer;
}

unsigned int GBuffer::get_buffer(const char* buffer) const
{
	for (int i = 0; i < this->gbuffer_size; i++)
	{
		if (strcmp((const char*)buffer_names.at(i), buffer))
		{
			return this->buffers[i];
		}
	}
	std::cout << "ERROR: Unable to find buffer type" << std::endl;
	return -1;
}