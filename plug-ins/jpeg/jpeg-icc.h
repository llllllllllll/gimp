/* jpeg-icc.h
 *
 * This code was originally copied from the jpegicc tool as found in
 * the lcms source code. This code comes with the following copyright
 * notice:
 *
 * Little cms
 * Copyright (C) 1998-2004 Marti Maria
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 */

void jpeg_icc_write_profile (j_compress_ptr  cinfo,
                             const guchar   *icc_data_ptr,
                             guint           icc_data_len);


/*
 * Reading a JPEG file that may contain an ICC profile requires two steps:
 *
 * 1. After jpeg_create_decompress() but before jpeg_read_header(),
 *    ask the IJG library to save in memory any APP2 markers it may find
 *    in the file.
 *
 * 2. After jpeg_read_header(), call jpeg_icc_read_profile() to find
 *    out whether there was a profile and obtain it if so.
 *
 * See if there was an ICC profile in the JPEG file being read;
 * if so, reassemble and return the profile data.
 *
 * TRUE is returned if an ICC profile was found, FALSE if not.
 * If TRUE is returned, *icc_data_ptr is set to point to the
 * returned data, and *icc_data_len is set to its length.
 *
 * IMPORTANT: the data at **icc_data_ptr has been allocated with malloc()
 * and must be freed by the caller with free() when the caller no longer
 * needs it.  (Alternatively, we could write this routine to use the
 * IJG library's memory allocator, so that the data would be freed implicitly
 * at jpeg_finish_decompress() time.  But it seems likely that many apps
 * will prefer to have the data stick around after decompression finishes.)
 */

gboolean jpeg_icc_read_profile (j_decompress_ptr   cinfo,
                                guchar           **icc_data_ptr,
                                guint             *icc_data_len);
