#include "gmime-extra-address-compare.h"
#include <gmime/gmime-message.h>
#include <gmime/internet-address.h>
#include <string.h>

/**
 * internet_address_clone:
 * @ia: an internet address
 *
 * Clone a #InternetAddress
 *
 * Returns: (transfer full): new copy of the internet address
 **/
InternetAddress *internet_address_clone(InternetAddress *ia) {
  if (INTERNET_ADDRESS_IS_MAILBOX(ia)) {
    InternetAddressMailbox *mb = INTERNET_ADDRESS_MAILBOX(ia);

    return internet_address_mailbox_new(ia->name, mb->addr);
  }

  // we assume the value is group if not a mailbox
  InternetAddressGroup *grp = INTERNET_ADDRESS_GROUP(ia);
  InternetAddressGroup *clone;
  InternetAddressList *list;
  InternetAddress *member;
  InternetAddress *ia_clone;
  int len;

  list = internet_address_group_get_members(grp);
  len = internet_address_list_length(list);

  clone = INTERNET_ADDRESS_GROUP(internet_address_group_new(ia->name));

  for (int i = 0; i < len; i++) {
    member = internet_address_list_get_address(list, i);
    ia_clone = internet_address_clone(member);
    internet_address_group_add_member(clone, ia_clone);
  }

  return INTERNET_ADDRESS(clone);
}

/**
 * internet_address_list_clone:
 * @ia: an internet address
 *
 * Clones an address list
 *
 * Returns: (transfer full): new copy of the adress list
 **/
InternetAddressList *internet_address_list_clone(InternetAddressList *ia_list) {
  char *str;
  InternetAddressList *list;

  str = internet_address_list_to_string(ia_list, NULL, FALSE);
  list = internet_address_list_parse(NULL, str);

  g_free(str);

  return list;
}

InternetAddressMailbox *
internet_address_message_get_our(GMimeMessage *message) {

  return NULL;
}

// We ignore groups for now, we might add them later or just treat them as
// a list
//
//
// An mailbox address can be split into 3 parts:
// name, local part (part before the at sign)
// (@) and the part after the at sign.
//
// Lets ignore name because it's rarely of
// of importance.

// A matcher should take local -> m1 -> m2 -> m3 -> match?
//                       domain -> m4 -> m5 -> m6 -> match?
//
// Both the local and domain needs to match for it to be a match
//
// This could a kind of filter. Where we have base function at the
// bottom?
//
// local filter = address_filter_new()
//
// adress_filter_register_local_function(function compare(a1, ad2))
// adress_filter_register_domain_function(function compare(a1, ad2))
//
// filter:compare(a1, a2); // compare should be able to run either way.
// filters needed:
//
// compare_insesitive_domain..
// unwild (removes +)
//

//
// following functions are needed:
//
// internet_address_message_get_our()
//
// internet_list_contains()
//
// get_backup_addresses()? / get_backup?
//
// response_addr()
//
//
//
struct _InternetAddressCompare {
  GObject parent_object;

  // 2 lists of closures
  GPtrArray *local;
  GPtrArray *domain;
};

typedef struct _InternetAddressCompare InternetAddressCompare;

typedef gboolean (*GMimeExtraAddressCompare)(InternetAddress *ia1,
                                             InternetAddress *ia2,
                                             void *gpointer);

#define adress_filter_register_local_func(comp)                                \
  (adress_filter_register_local_function(comp, NULL, NULL))

void adress_filter_register_local_function(InternetAddressCompare *filter,
                                           GMimeExtraAddressCompare *comp,
                                           gpointer user_data,
                                           GClosureNotify destroy_data) {
  GClosure *closure;
  closure = g_cclosure_new(G_CALLBACK(comp), user_data, destroy_data);

  g_ptr_array_add(filter->local, closure);
}

#define adress_filter_register_domain_func(comp)                               \
  (adress_filter_register_local_function(comp, NULL, NULL))
void adress_filter_register_domain_function(InternetAddressCompare *filter,
                                            GMimeExtraAddressCompare *comp,
                                            gpointer user_data,
                                            GClosureNotify destroy_data) {

  GClosure *closure;
  closure = g_cclosure_new(G_CALLBACK(comp), user_data, destroy_data);

  g_ptr_array_add(filter->domain, closure);
}

/**
 * adress_filter_register_domain_closure:
 * @filter: a filter
 * @closure: a cluser to match addresses
 *
 * Add a closure to the local checker
 *
 **/
void adress_filter_register_local_closure(InternetAddressCompare *filter,
                                          GClosure *closure) {
  g_object_ref(closure);
  g_ptr_array_add(filter->local, closure);
}

/**
 * adress_filter_register_domain_closure:
 * @filter: an internet address
 * @closure: an internet address
 *
 * Add a closure to the domain checker
 *
 **/
void adress_filter_register_domain_closure(InternetAddressCompare *filter,
                                           GClosure *closure) {
  g_object_ref(closure);
  g_ptr_array_add(filter->domain, closure);
}
/* list->array = g_ptr_array_new (); */

InternetAddressCompare *address_filter_new(void) {
  InternetAddressCompare *filter;

  filter = g_slice_new(InternetAddressCompare);
  filter->local = g_ptr_array_new();
  filter->domain = g_ptr_array_new();
  return filter;
}

static void internet_address_list_finalize(GObject *object) {
  InternetAddressCompare *filter = (InternetAddressCompare *)object;
  GClosure *closure;
  guint i;

  for (i = 0; i < filter->local->len; i++) {
    closure = (GClosure *)filter->local->pdata[i];
    g_object_unref(closure);
  }

  for (i = 0; i < filter->domain->len; i++) {
    closure = (GClosure *)filter->domain->pdata[i];
    g_object_unref(closure);
  }

  g_ptr_array_free(filter->local, TRUE);
  g_ptr_array_free(filter->domain, TRUE);
}

gboolean compare(InternetAddressCompare *comp, InternetAddress *ia1,
                 InternetAddress *ia2) {

  return FALSE;
}

/* gboolean internet_address_compare_idn() { return TRUE; } */

// What is shit?

// static gint galore_utf8_strncmp(const char *str1, gssize len1, const char
// *str2, gssize len2) { 	gchar *fold1; 	gchar *fold2; 	gint ret;
//
// 	fold1 = g_utf8_casefold(str1, len1);
// 	fold2 = g_utf8_casefold(str2, len2);
//
// 	ret = g_utf8_collate(fold1, fold2);
// 	g_free(fold1);
// 	g_free(fold2);
//
// 	return ret;
// }
//
// static gint galore_utf8_strcmp(char *str1, char *str2) {
// 	gssize len1 = strlen(str1);
// 	gssize len2 = strlen(str2);
//
// 	return galore_utf8_strncmp(str1, len1, str2, len2);
// }
//
// static gint galore_strn_cmp(const char *str1, gssize len1, const char *str2,
// gssize len2) { 	int i = 0; 	for (; i < len1 && i < len2; i++) {
// if (str1[i] != str2[i]) { 			return str1[i] - str2[i];
// 		}
// 	}
//
// 	if (len1 == len2) {
// 		return 0;
// 	}
//
// 	i++;
// 	if (i > (len1 - 1)) {
// 		return -str2[i];
// 	}
//
// 	return str1[i];
// }
//
// static char *strnchr(const char *s, gssize len, int c) {
// 	for (int i = 0; i < len; i++) {
// 		if (*s == c) {
// 			return (char *)s;
// 		}
// 		s++;
// 	}
// 	return NULL;
// }
//
// static char *strnchrs(const char *s, gssize len, const char *cs) {
// 	for (int i = 0; i < len; i++) {
// 		for (const char *c = cs; c; c++) {
// 			if (*s == *c) {
// 				return (char *)s;
// 			}
// 		}
// 		s++;
// 	}
// 	return NULL;
// }
//
// static gboolean is_sub_address(InternetAddressMailbox *mb) {
// 	int i = 0;
// 	for (char *str = mb->addr + mb->at + 1; *str; str++) {
// 		if (*str == '.') {
// 			i++;
// 		}
// 	}
// 	return i > 1;
// }
//
// /**
//  * galore_address_sub_to_plus:
//  * @mb: email we want to base the new address on
//  *
//  * Converts a mailbox of the format local@sub.domain to local+sub@domain
//  * This function should only be called for addresss you know that sub
//  * is part of the local address. Typically addresses you own and is of
//  * this format. The reason for doing this is that it makes comparisons
//  easier.
//  *
//  * Returns: (transfer full): a new InternetAddress
//  */
// InternetAddress *galore_address_sub_to_plus(InternetAddressMailbox *mb) {
// 	g_return_val_if_fail(INTERNET_ADDRESS_IS_MAILBOX(mb), NULL);
//
// 	GString *str = NULL;
// 	const char *domain, *sub, *prefix;
// 	int sublen;
//
// 	if (!is_sub_address(mb)) {
// 		return NULL;
// 	}
//
// 	sub = mb->addr + mb->at + 1;
// 	domain = strchr(sub, '.');
//
// 	if (!domain) {
// 		return NULL;
// 	}
//
// 	sublen = domain - sub;
//
// 	prefix = mb->addr;
//
// 	str = g_string_new_len(NULL, strlen(mb->addr));
//
// 	g_string_printf(str, "%.*s+%.*s@%s", sublen, prefix, mb->at-1, sub,
// domain);
//
// 	InternetAddress *plus = internet_address_mailbox_new(
// 			INTERNET_ADDRESS(mb)->name, str->str);
//
// 	g_string_free(str, TRUE);
//
// 	return plus;
// }
//
// /**
//  * galore_address_plus_to_sub:
//  * @mb: email we want to base the new address on
//  *
//  * Converts a mailbox of the format local+sub@domain to local@sub.domain
//  * This function should only be called for addresss you know that sub
//  * from the local is part of the domain. Typically addresses you own and is
//  of
//  * this format. The reason for doing this is that it makes comparisons
//  easier.
//  *
//  * Returns: (transfer full): a new InternetAddress
//  */
// InternetAddress *galore_address_plus_to_sub(InternetAddressMailbox *mb) {
// 	g_return_val_if_fail(INTERNET_ADDRESS_IS_MAILBOX(mb), NULL);
//
// 	GString *str = NULL;
// 	const char *domain, *sub, *prefix;
// 	int sublen;
// 	int prefixlen;
//
// 	char *s1 = strnchrs(mb->addr, mb->at, "+");
//
// 	if (!s1) {
// 		return NULL;
// 	}
//
// 	prefixlen = s1 - mb->addr;
// 	sublen = mb->at - prefixlen;
// 	prefix = mb->addr;
//
// 	str = g_string_new_len(NULL, strlen(mb->addr));
//
// 	g_string_printf(str, "%.*s@%.*s.%s", prefixlen, prefix, sublen, sub,
// domain);
//
// 	InternetAddress *plus = internet_address_mailbox_new(
// 			INTERNET_ADDRESS(mb)->name, str->str);
//
// 	g_string_free(str, TRUE);
//
// 	return plus;
// }
//
// static gint galore_address_plus_len(const char *address, int len, const char
// *cs) { 	g_return_val_if_fail(address, 0); 	g_return_val_if_fail(cs,
// len);
//
// 	char *s1 = strnchrs(address, len, cs);
// 	if (s1) {
// 		return address-s1;
// 	}
//
// 	return len;
// }
//
// /**
//  * galore_address_compare_local:
//  * @mb1: First address to compare
//  * @mb2: Second address to compare
//  * @seperators: (nullable): Characters that seperates an email address
//  * @mode: how we want to compare the local part of the email address
//  *
//  * Compares the local parts of 2 email addresses. The seperators is used to
//  be
//  * able to compare addresses with plus addressing, local+sub@domain being
//  equal
//  * to local@domain
//  *
//  * Returns: %TRUE if we concider the addresses to be equal
//  */
// gboolean galore_address_compare_local(InternetAddressMailbox *mb1,
// 	InternetAddressMailbox *mb2, const char *seperators, GaloreCompareLocal
// mode) { 	gssize len1; 	gssize len2;
//
// 	g_return_val_if_fail(mb1, FALSE);
// 	g_return_val_if_fail(mb1, FALSE);
//
// 	len1 = galore_address_plus_len(mb1->addr, mb1->at, seperators);
// 	len2 = galore_address_plus_len(mb2->addr, mb2->at, seperators);
//
// 	if (mode & GALORE_COMPARE_INSENSITIVE_LOCAL) {
// 		return galore_utf8_strncmp(mb1->addr, len1, mb2->addr, len2);
// 	} else {
// 		return galore_strn_cmp(mb1->addr, len1, mb2->addr, len2);
// 	}
// }
//
// /**
//  * galore_address_compare_domain:
//  * @mb1: First address to compare
//  * @mb2: Second address to compare
//  * @mode: how we want to compare the domain of the email address
//  *
//  * Compares the domain of 2 email addresses.
//  *
//  * Returns: %TRUE if we concider the addresses to be equal
//  */
// gboolean galore_address_compare_domain(InternetAddressMailbox *mb1,
// 		InternetAddressMailbox *mb2, GaloreCompareDomain mode) {
//
// 	g_return_val_if_fail(mb1, FALSE);
// 	g_return_val_if_fail(mb1, FALSE);
//
// 	const char *d1;
// 	const char *d2;
//
// 	if (mode & GALORE_COMPARE_DOMAIN_IDN) {
// 		d1 = internet_address_mailbox_get_idn_addr(mb1) + mb1->at + 1;
// 		d2 = internet_address_mailbox_get_idn_addr(mb2) + mb2->at + 1;
// 	} else {
// 		d1 = mb1->addr + mb1->at + 1;
// 		d2 = mb2->addr + mb2->at + 1;
// 	}
//
// 	gssize len1 = strlen(d1);
// 	gssize len2 = strlen(d2);
//
// 	if (mode & GALORE_COMPARE_INSENSITIVE_DOMAIN) {
// 		return galore_utf8_strncmp(d1, len1, d2, len2);
// 	} else {
// 		return galore_strn_cmp(d1, len1, d2, len2);
// 	}
// }
