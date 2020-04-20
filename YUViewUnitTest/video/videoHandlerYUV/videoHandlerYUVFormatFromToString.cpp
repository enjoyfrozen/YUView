#include <QtTest>

#include <video/yuvPixelFormat.h>

class videoHandlerYUVFormatFromToString : public QObject
{
  Q_OBJECT

public:
  videoHandlerYUVFormatFromToString() {};
  ~videoHandlerYUVFormatFromToString() {};

private slots:
  void testFormatFromToString();
};

QList<YUV_Internals::yuvPixelFormat> getAllFormats()
{
  QList<YUV_Internals::yuvPixelFormat> allFormats;

  for (auto subsampling : YUV_Internals::subsamplingList)
  {
    for (auto bitsPerSample : YUV_Internals::bitDepthList)
    {
      QList<bool> endianList = (bitsPerSample > 8) ? (QList<bool>() << false << true) : (QList<bool>() << false);

      // Planar
      for (auto planeOrder : YUV_Internals::planeOrderList)
      {
        for (auto bigEndian : endianList)
        {
          auto pixelFormat = YUV_Internals::yuvPixelFormat(subsampling, bitsPerSample, planeOrder, bigEndian);
          allFormats.append(pixelFormat);
        }
      }
      // Packet
      for (auto packingOrder : YUV_Internals::getSupportedPackingFormats(subsampling))
      {
        for (auto bytePacking : {false, true})
        {
          for (auto bigEndian : endianList)
          {
            auto pixelFormat = YUV_Internals::yuvPixelFormat(subsampling, bitsPerSample, packingOrder, bytePacking, bigEndian);
            allFormats.append(pixelFormat);
          }
        }
      }
    }
  }

  return allFormats;
}

void videoHandlerYUVFormatFromToString::testFormatFromToString()
{
  for (auto fmt : getAllFormats())
  {
    QVERIFY(fmt.isValid());
    auto name = fmt.getName();
    auto fmtNew = YUV_Internals::yuvPixelFormat(name);
    if (fmt != fmtNew)
    {
      QFAIL(QString("Comparison failed. Names: %1").arg(name).toLocal8Bit().data());
    }
    if (fmt.subsampling != fmtNew.subsampling ||
        fmt.bitsPerSample != fmtNew.bitsPerSample ||
        fmt.planar != fmtNew.planar ||
        fmt.chromaOffset[0] != fmtNew.chromaOffset[0] ||
        fmt.chromaOffset[1] != fmtNew.chromaOffset[1] ||
        fmt.planeOrder != fmtNew.planeOrder ||
        fmt.uvInterleaved != fmtNew.uvInterleaved ||
        fmt.packingOrder != fmtNew.packingOrder ||
        fmt.bytePacking != fmtNew.bytePacking)
    {
      QFAIL(QString("Comparison of parameters failed. Names: %1").arg(name).toLocal8Bit().data());
    }
    if (fmt.bitsPerSample > 8 && fmt.bigEndian != fmtNew.bigEndian)
    {
      QFAIL(QString("Comparison of parameters failed. Endianess wrong. Names: %1").arg(name).toLocal8Bit().data());
    }
  }
}

QTEST_MAIN(videoHandlerYUVFormatFromToString)

#include "videoHandlerYUVFormatFromToString.moc"