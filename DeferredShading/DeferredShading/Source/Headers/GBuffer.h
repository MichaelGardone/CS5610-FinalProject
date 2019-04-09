#ifndef _G_BUFFER_H_
#define _G_BUFFER_H

#include <vector>

// Position, Albedo, Normals
#define MIN_GBUFFER_DEPTH 3
#define MIN_ATTACHMENTS 3

class GBuffer {
public:
	GBuffer();
	GBuffer(int);
	GBuffer(int, int);
	~GBuffer();

	void resize_gbuffer(int);
	void resize_attachments(int);

	void add_buffer(const char*);
	bool delete_buffer(const char*);
	bool delete_buffer(int);

	void generate_attachments();

	void generate_depthbuffer();

	int get_gbuffer_size() const;
	int get_attach_size() const;

	unsigned int get_gbuffer() const;

	unsigned int get_buffer(const char *) const;

private:
	unsigned int gbuffer;
	unsigned int renderbuffer;
	
	unsigned int* buffers;
	std::vector<char*> buffer_names;

	unsigned int* attachments;

	// DEBUG AND HOUSEKEEPING
	// Size of the gbuffer
	int gbuffer_size;
	int attach_size;
	int gbuffer_curr;
};

#endif