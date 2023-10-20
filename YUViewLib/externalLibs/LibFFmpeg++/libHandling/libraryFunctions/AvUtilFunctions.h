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

#pragma once

#include <common/Expected.h>
#include <common/FFMpegLibrariesTypes.h>
#include <libHandling/SharedLibraryLoader.h>

namespace LibFFmpeg::functions
{

struct AvUtilFunctions
{
  std::function<unsigned()>            avutil_version;
  std::function<AVFrame *()>           av_frame_alloc;
  std::function<void(AVFrame **frame)> av_frame_free;
  std::function<void(size_t size)>     av_mallocz;
  std::function<int(AVDictionary **pm, const char *key, const char *value, int flags)> av_dict_set;
  std::function<AVDictionaryEntry *(
      AVDictionary *m, const char *key, const AVDictionaryEntry *prev, int flags)>
      av_dict_get;
  std::function<AVFrameSideData *(const AVFrame *frame, AVFrameSideDataType type)>
                                                                            av_frame_get_side_data;
  std::function<AVDictionary *(const AVFrame *frame)>                       av_frame_get_metadata;
  std::function<void(void (*callback)(void *, int, const char *, va_list))> av_log_set_callback;
  std::function<void(int level)>                                            av_log_set_level;
  std::function<AVPixFmtDescriptor *(AVPixelFormat pix_fmt)>                av_pix_fmt_desc_get;
  std::function<AVPixFmtDescriptor *(const AVPixFmtDescriptor *prev)>       av_pix_fmt_desc_next;
  std::function<AVPixelFormat(const AVPixFmtDescriptor *desc)>              av_pix_fmt_desc_get_id;
};

std::optional<AvUtilFunctions> tryBindAVUtilFunctionsFromLibrary(SharedLibraryLoader &lib,
                                                                 Log &                log);

} // namespace LibFFmpeg::functions
