package net.ajcloud.imageprocessing

object ImageProcessing {
    init {
        System.loadLibrary("imageprocessing")
    }

    external fun rotation(data: ByteArray, width: Int, height: Int, rotationDegrees: Int)
    external fun scale(
        src: ByteArray,
        dst: ByteArray,
        srcWidth: Int,
        srcHeight: Int,
        dstWidth: Int,
        dstHeight: Int
    )

    external fun initVideoCodec(width: Int, height: Int, fps: Int, bitRate: Int)

    external fun releaseVideoCodec()

    external fun yuvEncode(
        inData: ByteArray?,
        outData: ByteArray?,
        outLen: IntArray?,
        type: IntArray?
    ): Int
}