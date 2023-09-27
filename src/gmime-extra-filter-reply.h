/*
 * Copyright © 2009 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef _GMIME_EXTRA_FILTER_REPLY_H_
#define _GMIME_EXTRA_FILTER_REPLY_H_

#include <gmime/gmime-filter.h>

G_BEGIN_DECLS

#define GMIME_EXTRA_TYPE_FILTER_REPLY (g_mime_extra_filter_reply_get_type())
#define GMIME_EXTRA_FILTER_REPLY(obj)                                          \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GMIME_EXTRA_TYPE_FILTER_REPLY,            \
                              GMimeExtraFilterReply))
#define GMIME_EXTRA_FILTER_REPLY_CLASS(klass)                                  \
  (G_TYPE_CHECK_CLASS_CAST((klass), GMIME_EXTRA_TYPE_FILTER_REPLY,             \
                           GMimeExtraFilterReplyClass))
#define GMIME_EXTRA_IS_FILTER_REPLY(obj)                                       \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GMIME_EXTRA_TYPE_FILTER_REPLY))
#define GMIME_EXTRA_IS_FILTER_REPLY_CLASS(klass)                               \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GMIME_EXTRA_TYPE_FILTER_REPLY))
#define GMIME_EXTRA_FILTER_REPLY_GET_CLASS(obj)                                \
  (G_TYPE_INSTANCE_GET_CLASS((obj), GMIME_EXTRA_TYPE_FILTER_REPLY,             \
                             GMimeExtraFilterReplyClass))

typedef struct _GMimeExtraFilterReply GMimeExtraFilterReply;
typedef struct _GMimeExtraFilterReplyClass GMimeExtraFilterReplyClass;

/**
 * GMimeExtraFilterReply:
 * @parent_object: parent #GMimeFilter
 * @encode: encoding vs decoding reply markers
 * @saw_nl: previous char was a \n
 * @saw_angle: previous char was a >
 *
 * A filter to insert/remove reply markers (lines beginning with >)
 **/
struct _GMimeExtraFilterReply {
  GMimeFilter parent_object;

  gboolean encode;
  gboolean saw_nl;
  gboolean saw_angle;
};

struct _GMimeExtraFilterReplyClass {
  GMimeFilterClass parent_class;
};

GType g_mime_extra_filter_reply_get_type(void);

GMimeFilter *g_mime_extra_filter_reply_new(gboolean encode);

G_END_DECLS

#endif /* _GMIME_EXTRA_FILTER_REPLY_H_ */
