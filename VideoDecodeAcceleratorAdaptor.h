// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIDEO_DECODE_ACCELERATOR_ADAPTOR_H_
#define VIDEO_DECODE_ACCELERATOR_ADAPTOR_H_

#include <vector>

#include "rect.h"
#include "size.h"
#include "video_codecs.h"
#include "video_pixel_format.h"

namespace android {

// The offset and stride of a video frame plane.
struct VideoFramePlane {
    uint32_t offset;
    uint32_t stride;
};

// Video decoder accelerator adaptor interface.
// The adaptor plays the role of providing unified adaptor API functions and client callback to
// codec component side.
// The adaptor API and client callback are modeled after media::VideoDecodeAccelerator which is
// ported from Chrome and are 1:1 mapped with its API functions.
class VideoDecodeAcceleratorAdaptor {
public:
    enum Result {
        SUCCESS = 0,
        ILLEGAL_STATE = 1,
        INVALID_ARGUMENT = 2,
        UNREADABLE_INPUT = 3,
        PLATFORM_FAILURE = 4,
        INSUFFICIENT_RESOURCES = 5,
    };

    // The adaptor client interface. This interface should be implemented in the component side.
    class Client {
    public:
        virtual ~Client() {}

        // Callback to tell client how many and what size of buffers to provide.
        virtual void providePictureBuffers(uint32_t pixelFormat,
                                           uint32_t minNumBuffers,
                                           const media::Size& codedSize) = 0;

        // Callback to dismiss picture buffer that was assigned earlier.
        virtual void dismissPictureBuffer(int32_t picture_id) = 0;

        // Callback to deliver decoded pictures ready to be displayed.
        virtual void pictureReady(int32_t picture_id, int32_t bitstream_id,
                                  const media::Rect& cropRect) = 0;

        // Callback to notify that decoder has decoded the end of the bitstream buffer with
        // specified ID.
        virtual void notifyEndOfBitstreamBuffer(int32_t bitstream_id) = 0;

        // Flush completion callback.
        virtual void notifyFlushDone() = 0;

        // Reset completion callback.
        virtual void notifyResetDone() = 0;

        // Callback to notify about errors. Note that errors in initialize() will not be reported
        // here, instead of by its returned value.
        virtual void notifyError(Result error) = 0;
    };

    // Initializes the video decoder with specific profile. This call is synchronous and returns
    // SUCCESS iff initialization is successful.
    virtual Result initialize(media::VideoCodecProfile profile, Client* client) = 0;

    // Decodes given shared memory buffer with bitstream ID.
    virtual void decode(int32_t bitstream_id, int ashmem_fd, off_t offset, uint32_t bytesUsed) = 0;

    // Assigns a specified number of picture buffer set to the video decoder.
    virtual void assignPictureBuffers(uint32_t numOutputBuffers) = 0;

    // Imports planes as backing memory for picture buffer with specified ID.
    virtual void importBufferForPicture(int32_t picture_id, int dmabuf_fd,
                                        const std::vector<VideoFramePlane>& planes) = 0;

    // Sends picture buffer to be reused by the decoder by its piture ID.
    virtual void reusePictureBuffer(int32_t picture_id) = 0;

    // Flushes the decoder.
    virtual void flush() = 0;

    // Resets the decoder.
    virtual void reset() = 0;

    // Destroys the decoder.
    virtual void destroy() = 0;

    virtual ~VideoDecodeAcceleratorAdaptor() {}
};

}  // namespace android

#endif  // VIDEO_DECODE_ACCELERATOR_ADAPTOR_H_
