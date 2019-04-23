#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <System.h>

class SBuffer
{
public:

	SBuffer()
	{
		// Generate buffer
		glGenFramebuffers(1, &stencilBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, stencilBuffer);

		glGenTextures(1, &stencil);
		glBindTexture(GL_TEXTURE_2D, stencil);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stencil, 0);

		glDrawBuffers(1, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, stencilBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Blit()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, stencilBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int getSBuffer()
	{
		return stencilBuffer;
	}

private:
	unsigned int stencilBuffer, stencil;
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
};
