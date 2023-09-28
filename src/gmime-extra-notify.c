#include "gmime-extra-notify.h"
#include <gmime/gmime-message-part.h>
#include <gmime/gmime-message.h>
#include <gmime/gmime-multipart.h>
#include <gmime/gmime-object.h>
#include <gmime/gmime-part.h>
#include <gmime/gmime-utils.h>
#include <gmime/internet-address.h>

/**
 * g_mime_extra_address_fqdn:
 * @mb: the senders #InternetAddressMailbox to grab the fqdn from
 *
 * grab the fdqn from th a mailbox
 *
 * Returns: (nullable): the fdqn
 **/
const char *g_mime_extra_address_fqdn(InternetAddressMailbox *mb) {
  const char *fdqn;
  const char *addr = internet_address_mailbox_get_idn_addr(mb);
  // const char *addr = internet_address_mailbox_get_addr(mb);

  g_return_val_if_fail(addr, NULL);

  fdqn = addr + mb->at + 1;
  return fdqn;
}

// TODO: Add error
/**
 * g_mime_extra_message_make_notification_response:
 * @message: a #GMimeMessage to reference
 * @from: Our email address
 * @to: their email address
 * @ua: the name of the user-agent or %NULL
 *
 * Creates an message disposition-notification response for a message
 * for the specified reciever.
 *
 * Returns: (nullable) (transfer full): A message containing
 *disposition-notification response
 **/
GMimeMessage *g_mime_extra_message_make_notification_response(
    GMimeMessage *message, InternetAddressMailbox *from,
    InternetAddressMailbox *to, const char *ua) {
  GMimeMessage *notification;
  GMimeMultipart *mp;
  char *prologe;
  GMimePart *part;
  GMimeMessagePart *msg_part;

  g_return_val_if_fail(GMIME_IS_MESSAGE(message), NULL);
  g_return_val_if_fail(INTERNET_ADDRESS_IS_MAILBOX(to), NULL);

  // if this doesn't have an MDN header, we shouldn't generate a response
  g_return_val_if_fail(g_mime_object_get_header(GMIME_OBJECT(message),
                                                "Disposition-Notification-To"),
                       NULL);

  notification = g_mime_message_new(TRUE);

  g_mime_message_add_mailbox(notification, GMIME_ADDRESS_TYPE_FROM,
                             INTERNET_ADDRESS(from)->name, to->addr);
  g_mime_message_add_mailbox(notification, GMIME_ADDRESS_TYPE_TO,
                             INTERNET_ADDRESS(to)->name, to->addr);

  char *from_str = internet_address_to_string(INTERNET_ADDRESS(from), NULL, 0);

  GDateTime *date = g_mime_message_get_date(message);
  char *date_str = g_mime_utils_header_format_date(date);

  char *mid = g_mime_utils_generate_message_id(g_mime_extra_address_fqdn(from));
  g_mime_message_set_message_id(notification, mid);
  free(mid);

  prologe = g_strdup_printf(
      "The message sent on %s to %s with subject"
      "\"%s\" has been displayed."
      "This is no guarantee that the message has been read or understood.",
      date_str, from_str, g_mime_message_get_subject(message));

  g_free(from_str);
  g_free(date_str);

  mp = g_mime_multipart_new_with_subtype("report");

  g_mime_object_set_content_type_parameter(GMIME_OBJECT(mp), "report-type",
                                           "disposition-notification");

  g_mime_multipart_set_prologue(mp, prologe);
  g_free(prologe);

  part = g_mime_part_new_with_type("message", "disposition-notification");
  if (ua) {
    g_mime_object_set_header(GMIME_OBJECT(part), "Reporting-UA", ua, NULL);
  }

  char *recipient;
  recipient = g_strdup_printf("rfc822;%s", from->addr);
  g_mime_object_set_header(GMIME_OBJECT(part), "Original-Recipient", recipient,
                           NULL);
  g_mime_object_set_header(GMIME_OBJECT(part), "Final-Recipient", recipient,
                           NULL);
  g_free(recipient);

  char *msgid;
  msgid = g_strdup_printf("<%s>", g_mime_message_get_message_id(message));
  g_mime_object_set_header(GMIME_OBJECT(part), "Original-Message-ID", msgid,
                           NULL);
  g_free(msgid);

  g_mime_object_set_header(GMIME_OBJECT(part), "Disposition",
                           "manual-action/MDN-sent-manually; displayed", NULL);

  // Reporting-UA: joes-pc.cs.example.com; Foomail 97.1
  // Original-Recipient: rfc822;Joe_Recipient@example.com
  // Final-Recipient: rfc822;Joe_Recipient@example.com
  // Original-Message-ID: <199509192301.23456@example.org>
  // Disposition: manual-action/MDN-sent-manually; displayed
  g_mime_multipart_add(mp, GMIME_OBJECT(part));
  g_object_unref(part);

  msg_part = g_mime_message_part_new_with_message("rfc822", message);
  g_mime_multipart_add(mp, GMIME_OBJECT(msg_part));

  return notification;
}

static GMimeExtraNotificationParameter parse_option(const char *option) {
  if (!strncmp(option, "required", 6)) {
    return GMIME_EXTRA_NOTIFICATION_REQUIRED;
  }
  return GMIME_EXTRA_NOTIFICATION_OPTIONAL;
}

/**
 * g_mime_extra_message_get_disposition_notification:
 * @message: a #GMimeMessage to reference
 * @mbox: email we want the response to
 *
 * Get the disposition if there is
 *
 **/
GMimeExtraMdnRequest *
g_mime_extra_message_get_disposition_notification(GMimeMessage *message) {
  GMimeExtraMdnRequest *request;
  const char *hnt;
  const char *hno;
  const char *ret;
  gboolean ask = FALSE;

  g_return_val_if_fail(GMIME_IS_MESSAGE(message), NULL);

  hnt = g_mime_object_get_header(GMIME_OBJECT(message),
                                 "Disposition-Notification-To");
  g_return_val_if_fail(hnt, NULL);

  hno = g_mime_object_get_header(GMIME_OBJECT(message),
                                 "Disposition-Notification-Option");

  ret = g_mime_object_get_header(GMIME_OBJECT(message), "Return-Path");

  if (!ret || strcmp(hnt, ret)) {
    ask = TRUE;
  }

  request = g_slice_new(GMimeExtraMdnRequest);
  request->ask = ask;
  request->recipients = internet_address_list_parse(NULL, hno);
  request->option = parse_option(hno);

  return request;
}

/**
 * g_mime_extra_message_get_disposition_notification:
 * @message: a #GMimeMessage to reference
 * @mbox: email we want the response to
 *
 * Inserts a disposition notification into the email from address.
 * from the specified mailbox
 *
 **/
void g_mime_extra_message_set_disposition_notification(
    GMimeMessage *message, InternetAddressMailbox *mbox) {

  g_return_if_fail(GMIME_IS_MESSAGE(message));
  g_return_if_fail(INTERNET_ADDRESS_IS_MAILBOX(mbox));

  char *str = internet_address_to_string(INTERNET_ADDRESS(mbox), NULL, FALSE);

  g_mime_object_set_header(GMIME_OBJECT(message), "Disposition-Notification-To",
                           str, NULL);

  g_mime_object_set_header(GMIME_OBJECT(message), "Return-Path", str, NULL);

  g_free(str);
}

// G_DEFINE_BOXED_TYPE (GMimeParserOptions, g_mime_parser_options,
// g_mime_parser_options_clone, g_mime_parser_options_free)

/**
 * g_mime_extra_message_notification:
 * @message: message we try to find a notification from
 *
 * Tries to extra the original message-id from a message
 * of type Content-Type: multipart/report; report-type=disposition-notification;
 *
 * Returns: (nullable): returns message-id of the original message or %NULL
 * if it's not a message-disposition-notification
 **/
char *g_mime_extra_message_notification(GMimeMessage *message) {
  GMimeObject *root, *part;
  GMimeMultipart *report;
  GMimeContentType *ct;
  const char *report_type;
  const char *mid;

  g_return_val_if_fail(GMIME_IS_MESSAGE(message), NULL);

  root = g_mime_message_get_mime_part(message);
  g_return_val_if_fail(GMIME_IS_MULTIPART(root), NULL);

  report = GMIME_MULTIPART(root);

  ct = g_mime_object_get_content_type(root);
  g_return_val_if_fail(ct, NULL);

  if (strcmp(ct->type, "multipart") || strcmp(ct->subtype, "report")) {
    return NULL;
  }
  report_type = g_mime_object_get_content_type_parameter(root, "report-type");

  g_return_val_if_fail(report_type, NULL);
  g_return_val_if_fail(g_mime_multipart_get_count(report) >= 3, NULL);
  g_return_val_if_fail(strcmp(report_type, "disposition-notification"), NULL);

  part = g_mime_multipart_get_part(report, 1);
  g_return_val_if_fail(part, NULL);

  ct = g_mime_object_get_content_type(part);
  g_return_val_if_fail(ct, NULL);

  if (strcmp(ct->type, "message") ||
      strcmp(ct->subtype, "disposition-notification")) {
    return NULL;
  }
  mid = g_mime_object_get_header(part, "Original-Message-ID");
  return g_mime_utils_decode_message_id(mid);
}

/**
 * g_mime_extra_message_delivery:
 * @message: A message to check for a message delivery
 *
 * Tries to extract the delivery status of a message
 * of type multipart/report; report-type=delivery-status
 *
 * Returns: (nullable): returns a delivery status of the message or %NULL
 * if it's not a message-delivery-status
 **/
GMimeExtraDeliveryStatus *g_mime_extra_message_delivery(GMimeMessage *message) {
  GMimeObject *root, *part, *old_part;
  GMimeMultipart *report;
  GMimeContentType *ct;
  const char *report_type;
  GMimeMessage *old_message;

  g_return_val_if_fail(GMIME_IS_MESSAGE(message), NULL);

  root = g_mime_message_get_mime_part(message);
  g_return_val_if_fail(GMIME_IS_MULTIPART(root), NULL);

  report = GMIME_MULTIPART(root);

  ct = g_mime_object_get_content_type(root);
  g_return_val_if_fail(ct, NULL);

  if (strcmp(ct->type, "multipart") || strcmp(ct->subtype, "report")) {
    return NULL;
  }
  report_type = g_mime_object_get_content_type_parameter(root, "report-type");

  g_return_val_if_fail(report_type, NULL);
  g_return_val_if_fail(g_mime_multipart_get_count(report) >= 3, NULL);
  g_return_val_if_fail(strcmp(report_type, "delivery-status"), NULL);

  part = g_mime_multipart_get_part(report, 1);
  g_return_val_if_fail(part, NULL);

  ct = g_mime_object_get_content_type(part);
  g_return_val_if_fail(ct, NULL);

  if (strcmp(ct->type, "message") || strcmp(ct->subtype, "delivery-status")) {
    return NULL;
  }

  old_part = g_mime_multipart_get_part(report, 2);
  g_return_val_if_fail(GMIME_IS_MESSAGE_PART(old_part), NULL);

  old_message = g_mime_message_part_get_message(GMIME_MESSAGE_PART(part));
  // get message id
  // date?
  // get action
  // status
  g_mime_message_get_message_id(old_message);

  return NULL;
  /* return part; */
}
