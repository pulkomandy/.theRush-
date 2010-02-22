#ifndef ZTEXTUREOGL_H__
#define ZTEXTUREOGL_H__

GLuint UploadDDS(unsigned char *tgeStream, int &w, int &h, int& nbmml);

class ZTextureOGL: public ZTexture
{
public:

	ZTextureOGL()
	{
		mTexture = 0;
	}
	virtual ~ZTextureOGL()
	{
		if (mTexture)
			glDeleteTextures(1, &mTexture);
	}

	virtual void LoadDDSFromMemory(unsigned char *ptr, int memSize)
	{
		if (mTexture)
			glDeleteTextures(1, &mTexture);

		mTexture = UploadDDS(ptr, mWidth, mHeight, mNbMipmaps);
	}
	virtual void Bind(uint8 aLevel) 
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTextureARB( GL_TEXTURE0 + aLevel );
		glBindTexture(GL_TEXTURE_2D, mTexture);
	}

	virtual void * GetPlatformHandle() const { return (void*)mTexture; }
	GLuint mTexture;
};

#endif