/*
 * Copyright (c) 2018 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "qdf_mem.h"
#include "qdf_trace.h"
#include "qdf_types.h"
#include "qdf_types_test.h"

#define ut_bool_pass(str, exp) __ut_bool(str, QDF_STATUS_SUCCESS, exp)
#define ut_bool_fail(str) __ut_bool(str, QDF_STATUS_E_FAILURE, false)

static uint32_t
__ut_bool(const char *str, QDF_STATUS exp_status, bool exp_value)
{
	bool value;
	QDF_STATUS status = qdf_bool_parse(str, &value);

	if (status != exp_status) {
		qdf_nofl_alert("FAIL: qdf_bool_parse(\"%s\") -> status %d; expected status %d",
			       str, status, exp_status);
		return 1;
	}

	if (QDF_IS_STATUS_ERROR(status))
		return 0;

	if (value != exp_value) {
		qdf_nofl_alert("FAIL: qdf_bool_parse(\"%s\") -> %s; expected %s",
			       str, value ? "true" : "false",
			       exp_value ? "true" : "false");
		return 1;
	}

	return 0;
}

static uint32_t qdf_types_ut_bool_parse(void)
{
	uint32_t errors = 0;

	errors += ut_bool_pass("1", true);
	errors += ut_bool_pass("y", true);
	errors += ut_bool_pass("Y", true);
	errors += ut_bool_pass("0", false);
	errors += ut_bool_pass("n", false);
	errors += ut_bool_pass("N", false);

	errors += ut_bool_fail("true");
	errors += ut_bool_fail("false");
	errors += ut_bool_fail("日本");

	return errors;
}

#define ut_mac_pass(str, exp) __ut_mac(str, #str, QDF_STATUS_SUCCESS, &(exp))
#define ut_mac_fail(str) __ut_mac(str, #str, QDF_STATUS_E_FAILURE, NULL)

static uint32_t
__ut_mac(const char *str, const char *display_str, QDF_STATUS exp_status,
	 struct qdf_mac_addr *exp_value)
{
	struct qdf_mac_addr value;
	QDF_STATUS status = qdf_mac_parse(str, &value);

	if (status != exp_status) {
		qdf_nofl_alert("FAIL: qdf_mac_parse(%s) -> status %d; expected status %d",
			       display_str, status, exp_status);
		return 1;
	}

	if (QDF_IS_STATUS_ERROR(status))
		return 0;

	if (qdf_mem_cmp(&value, exp_value, sizeof(value))) {
		qdf_nofl_alert("FAIL: qdf_mac_parse(%s) -> " QDF_MAC_ADDR_STR
			       "; expected " QDF_MAC_ADDR_STR,
			       display_str,
			       QDF_MAC_ADDR_ARRAY(value.bytes),
			       QDF_MAC_ADDR_ARRAY(exp_value->bytes));
		return 1;
	}

	return 0;
}

static uint32_t qdf_types_ut_mac_parse(void)
{
	uint32_t errors = 0;
	struct qdf_mac_addr addr_aabbccddeeff = { {
		0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff } };
	struct qdf_mac_addr addr_0123456789ab = { {
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab } };

	errors += ut_mac_fail("");
	errors += ut_mac_fail(" ");
	errors += ut_mac_fail("\t");
	errors += ut_mac_fail("test");
	errors += ut_mac_fail("¥円");
	errors += ut_mac_pass("aabbccddeeff", addr_aabbccddeeff);
	errors += ut_mac_pass("AABBCCDDEEFF", addr_aabbccddeeff);
	errors += ut_mac_fail("aa:bbccddeeff");
	errors += ut_mac_fail("aabbccddee:ff");
	errors += ut_mac_pass("aa:bb:cc:dd:ee:ff", addr_aabbccddeeff);
	errors += ut_mac_pass("01:23:45:67:89:ab", addr_0123456789ab);
	errors += ut_mac_fail("01:23:45:67:89:ab:cd:ef");
	errors += ut_mac_fail("01:23:45\0:67:89:ab");
	errors += ut_mac_pass(" \t01:23:45:67:89:ab\t ", addr_0123456789ab);
	errors += ut_mac_pass("01:23:45:67:89:ab\n", addr_0123456789ab);
	errors += ut_mac_fail("01:23:45:67:89:ab\t ,");

	return errors;
}

#define ut_ipv4_pass(str, exp) __ut_ipv4(str, #str, QDF_STATUS_SUCCESS, &(exp))
#define ut_ipv4_fail(str) __ut_ipv4(str, #str, QDF_STATUS_E_FAILURE, NULL)

static uint32_t
__ut_ipv4(const char *str, const char *display_str, QDF_STATUS exp_status,
	  struct qdf_ipv4_addr *exp_value)
{
	struct qdf_ipv4_addr value;
	QDF_STATUS status = qdf_ipv4_parse(str, &value);

	if (status != exp_status) {
		qdf_nofl_alert("FAIL: qdf_ipv4_parse(%s) -> status %d; expected status %d",
			       display_str, status, exp_status);
		return 1;
	}

	if (QDF_IS_STATUS_ERROR(status))
		return 0;

	if (qdf_mem_cmp(&value, exp_value, sizeof(value))) {
		qdf_nofl_alert("FAIL: qdf_ipv4_parse(%s) -> " QDF_IPV4_ADDR_STR
			       "; expected " QDF_IPV4_ADDR_STR,
			       display_str,
			       QDF_IPV4_ADDR_ARRAY(value.bytes),
			       QDF_IPV4_ADDR_ARRAY(exp_value->bytes));
		return 1;
	}

	return 0;
}

static uint32_t qdf_types_ut_ipv4_parse(void)
{
	uint32_t errors = 0;
	struct qdf_ipv4_addr addr_0000 = { { 0, 0, 0, 0 } };
	struct qdf_ipv4_addr addr_127001 = { { 127, 0, 0, 1 } };
	struct qdf_ipv4_addr addr_0112123 = { { 0, 1, 12, 123 } };
	struct qdf_ipv4_addr addr_255255255255 = { { 255, 255, 255, 255 } };

	errors += ut_ipv4_fail("");
	errors += ut_ipv4_fail(" ");
	errors += ut_ipv4_fail("\t");
	errors += ut_ipv4_fail("test");
	errors += ut_ipv4_fail("¥円");
	errors += ut_ipv4_pass("0.0.0.0", addr_0000);
	errors += ut_ipv4_pass("127.0.0.1", addr_127001);
	errors += ut_ipv4_pass("255.255.255.255", addr_255255255255);
	errors += ut_ipv4_fail(".0.0.1");
	errors += ut_ipv4_fail("127.0.0.");
	errors += ut_ipv4_fail("abc.123.123.123");
	errors += ut_ipv4_fail("256.0.0.0");
	errors += ut_ipv4_pass("0.1.12.123", addr_0112123);
	errors += ut_ipv4_pass(" 0.1.12.123\t\n", addr_0112123);
	errors += ut_ipv4_fail("0.1.12\0.123");
	errors += ut_ipv4_fail("0.1.12.123 ,");

	return errors;
}

#define ut_ipv6_pass(str, exp) __ut_ipv6(str, #str, QDF_STATUS_SUCCESS, &(exp))
#define ut_ipv6_fail(str) __ut_ipv6(str, #str, QDF_STATUS_E_FAILURE, NULL)

static uint32_t
__ut_ipv6(const char *str, const char *display_str, QDF_STATUS exp_status,
	  struct qdf_ipv6_addr *exp_value)
{
	struct qdf_ipv6_addr value;
	QDF_STATUS status = qdf_ipv6_parse(str, &value);

	if (status != exp_status) {
		qdf_nofl_alert("FAIL: qdf_ipv6_parse(%s) -> status %d; expected status %d",
			       display_str, status, exp_status);
		return 1;
	}

	if (QDF_IS_STATUS_ERROR(status))
		return 0;

	if (qdf_mem_cmp(&value, exp_value, sizeof(value))) {
		qdf_nofl_alert("FAIL: qdf_ipv6_parse(%s) -> " QDF_IPV6_ADDR_STR
			       "; expected " QDF_IPV6_ADDR_STR,
			       display_str,
			       QDF_IPV6_ADDR_ARRAY(value.bytes),
			       QDF_IPV6_ADDR_ARRAY(exp_value->bytes));
		return 1;
	}

	return 0;
}

static uint32_t qdf_types_ut_ipv6_parse(void)
{
	uint32_t errors = 0;
	struct qdf_ipv6_addr addr_00000000000000000000000000000000 = { {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	} };
	struct qdf_ipv6_addr addr_00000000000000000000000000000001 = { {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	} };
	struct qdf_ipv6_addr addr_00010000000000000000000000000000 = { {
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	} };
	struct qdf_ipv6_addr addr_0123456789abcdefabcdef0123456789 = { {
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
		0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
	} };
	struct qdf_ipv6_addr addr_20010db885a3000000008a2e03707334 = { {
		0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
		0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34,
	} };
	struct qdf_ipv6_addr addr_ff020000000000000000000000000001 = { {
		0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	} };
	struct qdf_ipv6_addr addr_00000000000000000000ffffc0000280 = { {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x02, 0x80,
	} };
	struct qdf_ipv6_addr addr_00010000000000000000000000000001 = { {
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	} };

	errors += ut_ipv6_fail("");
	errors += ut_ipv6_fail(" ");
	errors += ut_ipv6_fail("\t");
	errors += ut_ipv6_fail("test");
	errors += ut_ipv6_fail("¥円");
	errors += ut_ipv6_pass("::",
			       addr_00000000000000000000000000000000);
	errors += ut_ipv6_pass("::0",
			       addr_00000000000000000000000000000000);
	errors += ut_ipv6_pass("0:0:0:0:0:0:0:0",
			       addr_00000000000000000000000000000000);
	errors += ut_ipv6_pass("::1",
			       addr_00000000000000000000000000000001);
	errors += ut_ipv6_pass("1::",
			       addr_00010000000000000000000000000000);
	errors += ut_ipv6_pass("0:0:0:0:0:0:0:1",
			       addr_00000000000000000000000000000001);
	errors += ut_ipv6_pass("0123:4567:89ab:cdef:ABCD:EF01:2345:6789",
			       addr_0123456789abcdefabcdef0123456789);
	errors += ut_ipv6_pass("2001:0db8:85a3:0000:0000:8a2e:0370:7334",
			       addr_20010db885a3000000008a2e03707334);
	errors += ut_ipv6_pass("2001:db8:85a3:0:0:8a2e:370:7334",
			       addr_20010db885a3000000008a2e03707334);
	errors += ut_ipv6_pass("2001:db8:85a3::8a2e:370:7334",
			       addr_20010db885a3000000008a2e03707334);
	errors += ut_ipv6_pass("ff02::1",
			       addr_ff020000000000000000000000000001);
	errors += ut_ipv6_pass("::ffff:c000:0280",
			       addr_00000000000000000000ffffc0000280);
	errors += ut_ipv6_fail(":0:0:0:0:0:0:1");
	errors += ut_ipv6_fail(":0:0::0:0:1");
	errors += ut_ipv6_fail("0:0:0:0:0:0:0:");
	errors += ut_ipv6_fail("0:0:0::0:0:");
	errors += ut_ipv6_fail("0:0::0:0::0:0");
	errors += ut_ipv6_fail("xyz::zyx");
	errors += ut_ipv6_pass(" 1::1\t\n",
			       addr_00010000000000000000000000000001);
	errors += ut_ipv6_fail("1\0::1");
	errors += ut_ipv6_fail("1::1 ,");
	errors += ut_ipv6_fail("abcd");

	return errors;
}

uint32_t qdf_types_unit_test(void)
{
	uint32_t errors = 0;

	errors += qdf_types_ut_bool_parse();
	errors += qdf_types_ut_mac_parse();
	errors += qdf_types_ut_ipv4_parse();
	errors += qdf_types_ut_ipv6_parse();

	return errors;
}

