// Copyright 2008-present Contributors to the OpenImageIO project.
// SPDX-License-Identifier: BSD-3-Clause
// https://github.com/OpenImageIO/oiio/blob/master/LICENSE.md

#include <cstdio>

#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/imagebufalgo.h>
#include <OpenImageIO/imageio.h>

#include "R3DSDK.h"

OIIO_PLUGIN_NAMESPACE_BEGIN

namespace r3d_pvt {

// The R3D SDK requires that the output buffer is 16-byte aligned
// and the number of bytes per row is as well. The latter is always
// true in the current situation with the RED ONE resolutions, the
// 16-bit planar RGB the library is outputting and an aligned buffer
// sizeNeeded will be updated to indicate how many bytes were needed
// to properly align the buffer
unsigned char * AlignedMalloc(size_t & sizeNeeded)
{
	// alloc 15 bytes more to make sure we can align the buffer in case it isn't
	unsigned char * buffer = (unsigned char *)malloc(sizeNeeded + 15U);

	if (!buffer)
		return NULL;

	sizeNeeded = 0U;

	// cast to a 32-bit or 64-bit (depending on platform) integer so we can do the math
	uintptr_t ptr = (uintptr_t)buffer;

	// check if it's already aligned, if it is we're done
	if ((ptr % 16U) == 0U)
		return buffer;

	// calculate how many bytes we need
	sizeNeeded = 16U - (ptr % 16U);

	return buffer + sizeNeeded;
}


using namespace R3DSDK;

class R3DInput final : public ImageInput {
public:
    R3DInput() { init(); }
    virtual ~R3DInput() { close(); }
    virtual const char* format_name() const override { return "r3d"; }
    virtual bool open(const std::string& name, ImageSpec& spec) override;
    virtual bool read_native_scanline(int subimage, int miplevel, int y, int z,
                                      void* data) override;
    virtual bool seek_subimage(int subimage, int miplevel) override;
    void read_frame(int pos);
    virtual bool close() override;

private:
    std::string m_filename;

    unsigned char * r3d_imgbuffer;
    // Memory needed per frame
    size_t r3d_memNeeded;
    // Adjusted copy of r3d_memNeeded
    size_t r3d_memNeeded_adjusted;
    // The actual clip object
    Clip *r3d_clip;
    // Count of frames
    int64_t m_nsubimages;
    // Current frame
    int m_subimage;
    // Current frame has been read
    bool m_read_frame = false;
    // Byte width of a scanline
    size_t m_stride;
    // Byte Length per colour. 2 for 16-bit
    size_t m_byte_length;
    // Channel count
    size_t m_nchannels;

    bool R3DInit(const std::string& filename, const ImageSpec& spec);

    void init(void)
    {

        m_subimage         = 0;
        m_stride          = 0;
        m_filename.clear();
        //Initialize the R3DSDK prior to using any R3DSDK objects.
        InitializeStatus status = InitializeSdk(".", OPTION_RED_NONE);
        if ( status != ISInitializeOK)
        {
            error("Failed to initialize SDK: %d\n", status);
        }
    }
};


OIIO_PLUGIN_EXPORTS_BEGIN

OIIO_EXPORT int r3d_imageio_version = OIIO_PLUGIN_VERSION;
OIIO_EXPORT ImageInput *r3d_input_imageio_create () {
return new r3d_pvt::R3DInput;
}
OIIO_EXPORT const char *r3d_input_extensions[] = { "r3d", nullptr };
OIIO_EXPORT const char* r3d_imageio_library_version () {
    return R3DSDK::GetSdkVersion();
}

OIIO_PLUGIN_EXPORTS_END


bool
R3DInput::close()
{
    if (r3d_imgbuffer)
    {
        free(r3d_imgbuffer - r3d_memNeeded_adjusted);
    }
    delete r3d_clip;
    FinalizeSdk();
}

bool
R3DInput::R3DInit(const std::string& filename, const ImageSpec& spec) {
    r3d_clip = new Clip(filename.c_str());

    if (r3d_clip->Status() != LSClipLoaded)
    {
        errorf("Error loading %s\n", filename);
        delete r3d_clip;
        return false;
    }

    m_nsubimages = r3d_clip->VideoFrameCount();
    size_t width = r3d_clip->Width();
    size_t height = r3d_clip->Height();
    
    // n channels in 16-bit (2 bytes) need this much memory

    m_spec = ImageSpec(width, height, 3);

    int rate[2] = { r3d_clip->MetadataItemAsInt(RMD_FRAMERATE_NUMERATOR), r3d_clip->MetadataItemAsInt(RMD_FRAMERATE_DENOMINATOR) };
    m_spec.attribute("FramesPerSecond", TypeRational, &rate);
    m_spec.attribute("oiio:Movie", true);
    return true;
}


bool
R3DInput::open(const std::string& name, ImageSpec& spec)
{
    m_filename = name;
    m_spec      = spec;

    if (m_spec.nchannels != 3 && m_spec.nchannels != 4) {
        error("%s does not support %d-channel images\n", format_name(),
              m_spec.nchannels);
        return false;
    }

    if (!R3DInit(m_filename, spec)) {
        error("Couldn't initialize R3D\n");
        close();
        return false;
    }

    return true;
}


bool
R3DInput::seek_subimage(int subimage, int miplevel)
{
    if (subimage < 0 || subimage >= m_nsubimages || miplevel > 0) {
        return false;
    }
    if (subimage == m_subimage) {
        return true;
    }
    m_subimage = subimage;
    m_read_frame = false;
    return true;
}


void
R3DInput::read_frame(int frame)
{
    size_t width = r3d_clip->Width();
    size_t height = r3d_clip->Height();
    size_t m_byte_length = 2U;
    m_stride = (size_t)(width * m_nchannels * m_byte_length);

    // make a copy for AlignedMalloc (it will change it)
    size_t r3d_memNeeded = width * height * m_nchannels * m_byte_length;
    r3d_memNeeded_adjusted = r3d_memNeeded;

    // alloc this memory 16-byte aligned
    r3d_imgbuffer = AlignedMalloc(r3d_memNeeded_adjusted);

    if (r3d_imgbuffer == NULL)
    {
        errorf("Failed to allocate %d bytes of memory for output image\n", static_cast<unsigned int>(r3d_memNeeded));
        // return false;
    }
    VideoDecodeJob job;
    // 16-bit decode
    job.BytesPerRow = width * m_byte_length;
    job.OutputBufferSize = r3d_memNeeded;
    job.Mode = R3DSDK::DECODE_FULL_RES_PREMIUM;
    job.OutputBuffer = r3d_imgbuffer;
    job.PixelType = PixelType_16Bit_RGB_Planar;

    if (r3d_clip->DecodeVideoFrame(frame, job) != DSDecodeOK)
    {
        error("Decode failed?\n");
        // close();
        // return false;
    }

    m_read_frame = true;
}


bool
R3DInput::read_native_scanline(int subimage, int miplevel, int y, int z,
                               void* data)
{
    if (!seek_subimage(subimage, miplevel))
        return false;
    if (!m_read_frame) {
        read_frame(m_subimage);
    }
    if (!m_read_frame) {
        error("Failed reading frame %s", subimage);
        return false;
    }

    memcpy(
        data,
        r3d_imgbuffer + ( y * m_stride ),
        m_stride
    );
    return true;
}





}

OIIO_PLUGIN_NAMESPACE_END
