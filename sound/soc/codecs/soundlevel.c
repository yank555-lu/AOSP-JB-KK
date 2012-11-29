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

/*
 * Soundlevel mod :
 * ----------------
 *
 * /sys/kernel/soundlevel/speaker (rw)
 *
 *   0 - disabled (default)
 *   1 - override
 *
 * /sys/kernel/soundlevel/headphones (rw)
 *
 *   0 - disabled (default)
 *   1 - override
 *
 * /sys/kernel/soundlevel/speaker_level (rw)
 *
 *   amplifier level to be applied to speaker output (0 - 63)
 *
 * /sys/kernel/soundlevel/headphones_level (rw)
 *
 *   amplifier level to be applied to headphones output (0 - 63)
 *
 * /sys/kernel/soundlevel/version (ro)
 *
 *   display soundlevel mod version information
 *
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/soundlevel.h>

#include <linux/mfd/wm8994/registers.h>
#include "wm8994.h"

/* Force setting speaker volume */

static void set_speaker(void)
{
	unsigned int val;

	/* get current headphones level value and force setting it, soundlevel mod will be handled on wm8994.c */
	val = wm8994_read(soundlevel_codec, WM8994_SPEAKER_VOLUME_LEFT);
        wm8994_write(soundlevel_codec, WM8994_SPEAKER_VOLUME_LEFT, val);

	val = wm8994_read(soundlevel_codec, WM8994_SPEAKER_VOLUME_RIGHT);
        wm8994_write(soundlevel_codec, WM8994_SPEAKER_VOLUME_RIGHT, val | WM8994_SPKOUT_VU); /* Set the update flag to actually apply the new setting */

}

/* Force setting headphone volume */

static void set_headphones(void)
{
	unsigned int val;

	/* get current headphones level value and force setting it, soundlevel mod will be handled on wm8994.c */
	val = wm8994_read(soundlevel_codec, WM8994_LEFT_OUTPUT_VOLUME);
        wm8994_write(soundlevel_codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(soundlevel_codec, WM8994_RIGHT_OUTPUT_VOLUME);
        wm8994_write(soundlevel_codec, WM8994_RIGHT_OUTPUT_VOLUME, val | WM8994_HPOUT1_VU); /* Set the update flag to actually apply the new setting */

}

int speaker;

/* sysfs interface for "speaker" */
static ssize_t speaker_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	switch (speaker) {
		case SPEAKER_DISABLED:		return sprintf(buf, "0 - Disabled (default)\n");
		case SPEAKER_OVERRIDE:		return sprintf(buf, "1 - Override speaker output level\n");
		default:			return sprintf(buf, "something went wrong\n");
	}
}

static ssize_t speaker_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	int new_speaker;

	sscanf(buf, "%d", &new_speaker);

	switch(new_speaker) {
		case SPEAKER_DISABLED:		speaker_level = SPEAKER_LEVEL_DEFAULT;
						set_speaker();
		case SPEAKER_OVERRIDE:		speaker = new_speaker;
						return count;
		default:			return count;
	}
}

static struct kobj_attribute speaker_attribute =
__ATTR(speaker, 0666, speaker_show, speaker_store);

static struct attribute *speaker_attrs[] = {
&speaker_attribute.attr,
NULL,
};

static struct attribute_group speaker_attr_group = {
.attrs = speaker_attrs,
};

/* sysfs interface for "speaker_level" */
static ssize_t speaker_level_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
		return sprintf(buf, "%d\n",speaker_level);
}

static ssize_t speaker_level_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	int new_speaker_level;

	sscanf(buf, "%d", &new_speaker_level);

	if (new_speaker_level >= SPEAKER_LEVEL_MIN && new_speaker_level <= SPEAKER_LEVEL_MAX && speaker == SPEAKER_OVERRIDE) {

		speaker_level = new_speaker_level;
		set_speaker();

	}

	return count;

}

static struct kobj_attribute speaker_level_attribute =
__ATTR(speaker_level, 0666, speaker_level_show, speaker_level_store);

static struct attribute *speaker_level_attrs[] = {
&speaker_level_attribute.attr,
NULL,
};

static struct attribute_group speaker_level_attr_group = {
.attrs = speaker_level_attrs,
};

int headphones;

/* sysfs interface for "headphones" */
static ssize_t headphones_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	switch (headphones) {
		case HEADPHONES_DISABLED:		return sprintf(buf, "0 - Disabled (default)\n");
		case HEADPHONES_OVERRIDE:		return sprintf(buf, "1 - Override headphones output level\n");
		default:				return sprintf(buf, "something went wrong\n");
	}
}

static ssize_t headphones_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int new_headphones;

	sscanf(buf, "%d", &new_headphones);

	switch(new_headphones) {
		case HEADPHONES_DISABLED:		headphones_level = HEADPHONES_LEVEL_DEFAULT;
							set_headphones();
		case HEADPHONES_OVERRIDE:		headphones = new_headphones;
							return count;
		default:				return count;
	}
}

static struct kobj_attribute headphones_attribute =
__ATTR(headphones, 0666, headphones_show, headphones_store);

static struct attribute *headphones_attrs[] = {
&headphones_attribute.attr,
NULL,
};

static struct attribute_group headphones_attr_group = {
.attrs = headphones_attrs,
};

/* sysfs interface for "headphones_level" */
static ssize_t headphones_level_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n",headphones_level);
}

static ssize_t headphones_level_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int new_headphones_level;

	sscanf(buf, "%d", &new_headphones_level);

	if (new_headphones_level >= HEADPHONES_LEVEL_MIN && new_headphones_level <= HEADPHONES_LEVEL_MAX && headphones == HEADPHONES_OVERRIDE) {

		headphones_level = new_headphones_level;
		set_headphones();

	}

	return count;

}

static struct kobj_attribute headphones_level_attribute =
__ATTR(headphones_level, 0666, headphones_level_show, headphones_level_store);

static struct attribute *headphones_level_attrs[] = {
&headphones_level_attribute.attr,
NULL,
};

static struct attribute_group headphones_level_attr_group = {
.attrs = headphones_level_attrs,
};

/* sysfs interface for "version" */
static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "Soundlevel Mod by Yank555.lu v1.0\n");
}

static ssize_t version_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
/* no user change allowed */
return count;
}

static struct kobj_attribute version_attribute =
__ATTR(version, 0444, version_show, version_store);

static struct attribute *version_attrs[] = {
&version_attribute.attr,
NULL,
};

static struct attribute_group version_attr_group = {
.attrs = version_attrs,
};


/* Initialize fast charge sysfs folder */
static struct kobject *speaker_kobj;

int speaker_init(void)
{
	int speaker_retval;
	int speaker_level_retval;
	int headphones_retval;
	int headphones_level_retval;
	int version_retval;

	speaker          = SPEAKER_DISABLED;         /* Soundlevel mod for speaker    disabled by default */
	headphones       = HEADPHONES_DISABLED;      /* Soundlevel mod for headphones disabled by default */

        speaker_kobj = kobject_create_and_add("soundlevel", kernel_kobj);
        if (!speaker_kobj) {
                return -ENOMEM;
        }

        speaker_retval          = sysfs_create_group(speaker_kobj, &speaker_attr_group);
        speaker_level_retval    = sysfs_create_group(speaker_kobj, &speaker_level_attr_group);
        headphones_retval       = sysfs_create_group(speaker_kobj, &headphones_attr_group);
        headphones_level_retval = sysfs_create_group(speaker_kobj, &headphones_level_attr_group);
        version_retval          = sysfs_create_group(speaker_kobj, &version_attr_group);

        if (speaker_retval && speaker_level_retval && headphones_level_retval && headphones_retval && version_retval)
                kobject_put(speaker_kobj);
        return (speaker_retval && speaker_level_retval && headphones_level_retval && headphones_retval && version_retval);
}
/* end sysfs interface */

void speaker_exit(void)
{
	kobject_put(speaker_kobj);
}

module_init(speaker_init);
module_exit(speaker_exit);
