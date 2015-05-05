/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 1996-2009 by Andrew Church <achurch@achurch.org>
 *  Copyright (c) 2012-2015 ircd-hybrid development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 */

/*! \file conf_db.h
 * \brief Includes file utilities for database handling
 * \version $Id$
 */

#ifndef DATAFILES_H
#define DATAFILES_H

struct dbFILE
{
  char mode;  /**< 'r' for reading, 'w' for writing */
  FILE *fp;  /**< The file pointer itself */
  char filename[HYB_PATH_MAX + 1];  /**< Name of the database file */
  char tempname[HYB_PATH_MAX + 1];  /**< Name of the temporary file (for writing) */
};

extern int read_uint16(uint16_t *, struct dbFILE *);
extern int write_uint16(uint16_t, struct dbFILE *);
extern int read_uint32(uint32_t *, struct dbFILE *);
extern int write_uint32(uint32_t, struct dbFILE *);
extern int read_uint64(uint64_t *, struct dbFILE *);
extern int write_uint64(uint64_t, struct dbFILE *);
extern int read_string(char **, struct dbFILE *);
extern int write_string(const char *, struct dbFILE *);

extern void load_kline_database(void);
extern void save_kline_database(void);
extern void load_dline_database(void);
extern void save_dline_database(void);
extern void load_xline_database(void);
extern void save_xline_database(void);
extern void load_resv_database(void);
extern void save_resv_database(void);
extern void save_all_databases(void *);

#define DATABASE_UPDATE_TIMEOUT 300
#define KLINE_DB_VERSION 1
#endif
