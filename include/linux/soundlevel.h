/*
 * Author: Jean-Pierre Rasquin <yank555.lu@gmail.com>
 *
 * Thanx to andip71 for his invaluable insight !
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#ifndef _LINUX_SNDLVL_H
#define _LINUX_SNDLVL_H

extern int speaker;

#define SPEAKER_DISABLED 0          /* default */
#define SPEAKER_OVERRIDE 1

extern int speaker_level;

#define SPEAKER_LEVEL_MIN 0
#define SPEAKER_LEVEL_DEFAULT 57    /* default */
#define SPEAKER_LEVEL_MAX 63

extern int headphones;

#define HEADPHONES_DISABLED 0       /* default */
#define HEADPHONES_OVERRIDE 1

extern int headphones_level;

#define HEADPHONES_LEVEL_MIN 0
#define HEADPHONES_LEVEL_DEFAULT 57 /* default */
#define HEADPHONES_LEVEL_MAX 63

extern struct snd_soc_codec *soundlevel_codec; /* Pointer to codec structure */

#endif
