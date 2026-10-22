import native from 'libimageprocessing.so';

/** 与 Android {@code net.ajcloud.imageprocessing.ImageProcessing} 方法签名一致 */
export class ImageProcessing {
  static rotation(data: Uint8Array, width: number, height: number, rotationDegrees: number): void {
    native.rotation(data, width, height, rotationDegrees);
  }

  static scale(
    src: Uint8Array,
    dst: Uint8Array,
    srcWidth: number,
    srcHeight: number,
    dstWidth: number,
    dstHeight: number
  ): void {
    native.scale(src, dst, srcWidth, srcHeight, dstWidth, dstHeight);
  }

  static initVideoCodec(width: number, height: number, fps: number, bitRate: number): void {
    native.initVideoCodec(width, height, fps, bitRate);
  }

  static releaseVideoCodec(): void {
    native.releaseVideoCodec();
  }

  static yuvEncode(
    inData: Uint8Array | null,
    outData: Uint8Array | null,
    outLen: Int32Array | null,
    type: Int32Array | null
  ): number {
    if (inData === null || outData === null || outLen === null || type === null) {
      return 0;
    }
    return native.yuvEncode(inData, outData, outLen, type) as number;
  }
}
