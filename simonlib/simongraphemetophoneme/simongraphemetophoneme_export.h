/*
 *   Copyright (C) 2010 Peter Grasch <peter.grasch@bedahr.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SIMON_SIMONGRAPHEMETOPHONEME_EXPORT_H_F1032DFE61FA42E3B31933515AF33099
#define SIMON_SIMONGRAPHEMETOPHONEME_EXPORT_H_F1032DFE61FA42E3B31933515AF33099

// needed for KDE_EXPORT and KDE_IMPORT macros
#include <kdemacros.h>

#ifndef SIMONGRAPHEMETOPHONEME_EXPORT
# if defined(MAKE_SIMONGRAPHEMETOPHONEME_LIB)
// We are building this library
#  define SIMONGRAPHEMETOPHONEME_EXPORT KDE_EXPORT
# else
// We are using this library
#  define SIMONGRAPHEMETOPHONEME_EXPORT
# endif
#endif

# ifndef SIMONGRAPHEMETOPHONEME_EXPORT_DEPRECATED
#  define SIMONGRAPHEMETOPHONEME_EXPORT_DEPRECATED KDE_DEPRECATED SIMONGRAPHEMETOPHONEME_EXPORT
# endif
#endif
