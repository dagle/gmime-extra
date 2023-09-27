/*
 * Copyright © 2023 Per Odlund <per.odlund@gmail.com>
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

#ifndef _GMIME_EXTRA_ADDRESS_NOTIFY_H_
#define _GMIME_EXTRA_ADDRESS_NOTIFY_H_

#include <gmime/gmime-message.h>

typedef enum {
  GMIME_EXTRA_DELIVERY_FAILED,
  GMIME_EXTRA_DELIVERY_DELAYED,
  GMIME_EXTRA_DELIVERY_DELIVERED,
  GMIME_EXTRA_DELIVERY_RELAYED,
  GMIME_EXTRA_DELIVERY_EXPANDED
} GMimeExtraDeliveryAction;

typedef struct GMimeExtraDeliveryStatus {
  GMimeExtraDeliveryAction action;
  int status;
  char *mid;
} GMimeExtraDeliveryStatus;

// TODO: add these
typedef enum {
  GMIME_EXTRA_NOTIFICATION_NONE,
  GMIME_EXTRA_NOTIFICATION_REQUIRED,
  GMIME_EXTRA_NOTIFICATION_OPTIONAL
} GMimeExtraNotificationParameter;

GMimeMessage *g_mime_extra_message_make_notification_response(
    GMimeMessage *message, InternetAddressMailbox *from,
    InternetAddressMailbox *to, const char *ua);

char *
g_mime_extra_message_notification(GMimeMessage *message);
// GMimeExtraDeliveryStatus *g_mime_extra_message_delivery(GMimeMessage *message);

#endif /* _GMIME_EXTRA_ADDRESS_NOTIFY_H_ */
