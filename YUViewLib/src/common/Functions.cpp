/*  This file is part of YUView - The YUV player with advanced analytics toolset
 *   <https://github.com/IENT/YUView>
 *   Copyright (C) 2015  Institut für Nachrichtentechnik, RWTH Aachen University, GERMANY
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   In addition, as a special exception, the copyright holders give
 *   permission to link the code of portions of this program with the
 *   OpenSSL library under certain conditions as described in each
 *   individual source file, and distribute linked combinations including
 *   the two.
 *
 *   You must obey the GNU General Public License in all respects for all
 *   of the code used other than OpenSSL. If you modify file(s) with this
 *   exception, you may extend this exception to your version of the
 *   file(s), but you are not obligated to do so. If you do not wish to do
 *   so, delete this exception statement from your version. If you delete
 *   this exception statement from all source files in the program, then
 *   also delete it here.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Functions.h"

#ifdef Q_OS_MAC
#include <sys/sysctl.h>
#include <sys/types.h>
#elif defined(Q_OS_UNIX)
#include <unistd.h>
#elif defined(Q_OS_WIN32)
#include <windows.h>
#endif

#include <algorithm>

#include <QThread>

namespace functions
{

unsigned int getOptimalThreadCount()
{
  int nrThreads = QThread::idealThreadCount() - 1;
  if (nrThreads > 0)
    return (unsigned int)nrThreads;
  else
    return 1;
}

unsigned int systemMemorySizeInMB()
{
  static unsigned int memorySizeInMB;
  if (!memorySizeInMB)
  {
    // Fetch size of main memory - assume 2 GB first.
    // Unfortunately there is no Qt API for doing this so this is platform dependent.
    memorySizeInMB = 2 << 10;
#ifdef Q_OS_MAC
    int      mib[2]  = {CTL_HW, HW_MEMSIZE};
    u_int    namelen = sizeof(mib) / sizeof(mib[0]);
    uint64_t size;
    size_t   len = sizeof(size);

    if (sysctl(mib, namelen, &size, &len, NULL, 0) == 0)
      memorySizeInMB = size >> 20;
#elif defined Q_OS_UNIX
    long pages     = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    memorySizeInMB = (pages * page_size) >> 20;
#elif defined Q_OS_WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    memorySizeInMB = status.ullTotalPhys >> 20;
#endif
  }
  return memorySizeInMB;
}

QStringList getThemeNameList()
{
  QStringList ret{};
  ret.append("Default");
  ret.append("Simple Dark/Blue");
  ret.append("Simple Dark/Orange");
  return ret;
}

QString getThemeFileName(QString themeName)
{
  if (themeName == "Simple Dark/Blue" || themeName == "Simple Dark/Orange")
    return ":YUViewSimple.qss";
  return "";
}

QStringList getThemeColors(QString themeName)
{
  if (themeName == "Simple Dark/Blue")
    return QStringList() << "#262626"
                         << "#E0E0E0"
                         << "#808080"
                         << "#3daee9";
  if (themeName == "Simple Dark/Orange")
    return QStringList() << "#262626"
                         << "#E0E0E0"
                         << "#808080"
                         << "#FFC300 ";
  return QStringList();
}

QString formatDataSize(double size, bool isBits)
{
  unsigned divCounter = 0;
  bool     isNegative = size < 0;
  if (isNegative)
    size = -size;
  while (divCounter < 5 && size >= 1000)
  {
    size = size / 1000;
    divCounter++;
  }
  QString valueString;
  if (isNegative)
    valueString += "-";
  valueString += QString("%1").arg(size, 0, 'f', 2);

  if (divCounter > 0 && divCounter < 5)
  {
    const auto bitsUnits = QStringList() << "kbit"
                                         << "Mbit"
                                         << "Gbit"
                                         << "Tbit";
    const auto bytesUnits = QStringList() << "kB"
                                          << "MB"
                                          << "GB"
                                          << "TB";
    if (isBits)
      return QString("%1 %2").arg(valueString).arg(bitsUnits[divCounter - 1]);
    else
      return QString("%1 %2").arg(valueString).arg(bytesUnits[divCounter - 1]);
  }

  return valueString;
}

QStringList toQStringList(const std::vector<std::string> &stringVec)
{
  QStringList list;
  for (const auto &s : stringVec)
    list.append(QString::fromStdString(s));
  return list;
}

std::string toLower(std::string str)
{
  std::transform(
      str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
  return str;
}

std::string to_string(const Size &size)
{
  return std::to_string(size.width) + "x" + std::to_string(size.height);
}

std::string to_string(const StringVec &items, const std::string &seperator)
{
  std::string str;
  auto        it = items.begin();
  while (it != items.end())
  {
    if (it != items.begin())
      str += seperator;
    str += *it;
    ++it;
  }
  return str;
}

std::string vstring(const char *format, va_list vargs)
{
  std::string result;
  va_list     args_copy;

  va_copy(args_copy, vargs);

  int len = vsnprintf(nullptr, 0, format, vargs);
  if (len < 0)
  {
    va_end(args_copy);
    throw std::runtime_error("vsnprintf error");
  }

  if (len > 0)
  {
    result.resize(len);
    vsnprintf(result.data(), len + 1, format, args_copy);
  }

  va_end(args_copy);

  return result;
}

std::string formatString(std::string format, const std::initializer_list<std::string> &arguments)
{
  int counter = 0;
  for (auto argument : arguments)
  {
    const auto toReplace  = "%" + std::to_string(counter);
    auto       replacePos = format.find(toReplace);
    if (replacePos == std::string::npos)
      return format;

    format.replace(replacePos, 2, argument);

    ++counter;
  }
  return format;
}

std::optional<unsigned long> toUnsigned(const std::string &text)
{
  try
  {
    auto index = std::stoul(text);
    return index;
  }
  catch (...)
  {
    return {};
  }
}

} // namespace functions
