
#include <net/if.h>
#include <netlink/socket.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <linux/nl80211.h>

#define COMMAND_NAME_MAX_LEN 10

#define NLM_F_ROOT	0x100
#define NLM_F_MATCH	0x200
#define NLM_F_DUMP	(NLM_F_ROOT|NLM_F_MATCH)

typedef struct nl_sock 		  nl_sock_t;
typedef struct nl_msg  		  nl_msg_t;
typedef struct nl_cb   		  nl_cb_t;
typedef enum nl80211_commands nl80211_cmd_t;
typedef enum nl80211_attrs	  nl80211_attr_t;

/**************************************************************************************************************/
/* Helpers */
/**************************************************************************************************************/

static char modebuf[100];

static const char *ifmodes[NL80211_IFTYPE_MAX + 1] = {
	"unspecified",
	"IBSS",
	"managed",
	"AP",
	"AP/VLAN",
	"WDS",
	"monitor",
	"mesh point",
	"P2P-client",
	"P2P-GO",
	"P2P-device",
	"outside context of a BSS",
	"NAN",
};

static char *channel_type_name(enum nl80211_channel_type channel_type)
{
	switch (channel_type) {
	case NL80211_CHAN_NO_HT:
		return "NO HT";
	case NL80211_CHAN_HT20:
		return "HT20";
	case NL80211_CHAN_HT40MINUS:
		return "HT40-";
	case NL80211_CHAN_HT40PLUS:
		return "HT40+";
	default:
		return "unknown";
	}
}

void mac_addr_n2a(char *mac_addr, const unsigned char *arg)
{
	int i, l;

	l = 0;
	for (i = 0; i < 6 ; i++) {
		if (i == 0) {
			sprintf(mac_addr+l, "%02x", arg[i]);
			l += 2;
		} else {
			sprintf(mac_addr+l, ":%02x", arg[i]);
			l += 3;
		}
	}
}

void print_ssid_escaped(const uint8_t len, const uint8_t *data)
{
	int i;

	for (i = 0; i < len; i++) {
		if (data[i] != ' ' && data[i] != '\\')
			printf("%c", data[i]);
		else if (data[i] == ' ' &&
			 (i != 0 && i != len -1))
			printf(" ");
		else
			printf("\\x%.2x", data[i]);
	}
}

const char *iftype_name(enum nl80211_iftype iftype)
{
	if (iftype <= NL80211_IFTYPE_MAX && ifmodes[iftype])
		return ifmodes[iftype];
	sprintf(modebuf, "Unknown mode (%d)", iftype);
	return modebuf;
}

int ieee80211_frequency_to_channel(int freq)
{
	/* see 802.11-2007 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq <= 45000) /* DMG band lower limit */
		return (freq - 5000) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}

char *channel_width_name(enum nl80211_chan_width width)
{
	switch (width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		return "20 MHz (no HT)";
	case NL80211_CHAN_WIDTH_20:
		return "20 MHz";
	case NL80211_CHAN_WIDTH_40:
		return "40 MHz";
	case NL80211_CHAN_WIDTH_80:
		return "80 MHz";
	case NL80211_CHAN_WIDTH_80P80:
		return "80+80 MHz";
	case NL80211_CHAN_WIDTH_160:
		return "160 MHz";
	case NL80211_CHAN_WIDTH_5:
		return "5 MHz";
	case NL80211_CHAN_WIDTH_10:
		return "10 MHz";
	default:
		return "unknown";
	}
}

/**************************************************************************************************************/
/* Handlers */
/**************************************************************************************************************/

static int print_iface_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
	unsigned int *wiphy = (unsigned int *)arg;
	const char *indent = "";

	nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (wiphy && tb_msg[NL80211_ATTR_WIPHY]) {
		unsigned int thiswiphy = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]);
		indent = "\t";
		if (*wiphy != thiswiphy)
			printf("phy#%d\n", thiswiphy);
		*wiphy = thiswiphy;
	}

	if (tb_msg[NL80211_ATTR_IFNAME])
		printf("%sInterface %s\n", indent, nla_get_string(tb_msg[NL80211_ATTR_IFNAME]));
	else
		printf("%sUnnamed/non-netdev interface\n", indent);
	if (tb_msg[NL80211_ATTR_IFINDEX])
		printf("%s\tifindex %d\n", indent, nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]));
	if (tb_msg[NL80211_ATTR_WDEV])
		printf("%s\twdev 0x%llx\n", indent,
		       (unsigned long long)nla_get_u64(tb_msg[NL80211_ATTR_WDEV]));
	if (tb_msg[NL80211_ATTR_MAC]) {
		char mac_addr[20];
		mac_addr_n2a(mac_addr, (const unsigned char *)nla_data(tb_msg[NL80211_ATTR_MAC]));
		printf("%s\taddr %s\n", indent, mac_addr);
	}
	if (tb_msg[NL80211_ATTR_SSID]) {
		printf("%s\tssid ", indent);
		print_ssid_escaped(nla_len(tb_msg[NL80211_ATTR_SSID]),
				   (const uint8_t *)nla_data(tb_msg[NL80211_ATTR_SSID]));
		printf("\n");
	}
	if (tb_msg[NL80211_ATTR_IFTYPE])
		printf("%s\ttype %s\n", indent, iftype_name((nl80211_iftype)nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE])));
	if (!wiphy && tb_msg[NL80211_ATTR_WIPHY])
		printf("%s\twiphy %d\n", indent, nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]));
	if (tb_msg[NL80211_ATTR_WIPHY_FREQ]) {
		uint32_t freq = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FREQ]);

		printf("%s\tchannel %d (%d MHz)", indent,
		       ieee80211_frequency_to_channel(freq), freq);

		if (tb_msg[NL80211_ATTR_CHANNEL_WIDTH]) {
			printf(", width: %s",
				channel_width_name((nl80211_chan_width)nla_get_u32(tb_msg[NL80211_ATTR_CHANNEL_WIDTH])));
			if (tb_msg[NL80211_ATTR_CENTER_FREQ1])
				printf(", center1: %d MHz",
					nla_get_u32(tb_msg[NL80211_ATTR_CENTER_FREQ1]));
			if (tb_msg[NL80211_ATTR_CENTER_FREQ2])
				printf(", center2: %d MHz",
					nla_get_u32(tb_msg[NL80211_ATTR_CENTER_FREQ2]));
		} else if (tb_msg[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
			enum nl80211_channel_type channel_type;

			channel_type = (nl80211_channel_type)nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_CHANNEL_TYPE]);
			printf(" %s", channel_type_name(channel_type));
		}

		printf("\n");
	}

	if (tb_msg[NL80211_ATTR_WIPHY_TX_POWER_LEVEL]) {
		uint32_t txp = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_TX_POWER_LEVEL]);

		printf("%s\ttxpower %d.%.2d dBm\n",
		       indent, txp / 100, txp % 100);
	}

	// if (tb_msg[NL80211_ATTR_TXQ_STATS]) {
	// 	char buf[150];
	// 	parse_txq_stats(buf, sizeof(buf), tb_msg[NL80211_ATTR_TXQ_STATS], 1, -1, indent);
	// 	printf("%s\tmulticast TXQ:%s\n", indent, buf);
	// }

	if (tb_msg[NL80211_ATTR_4ADDR]) {
		uint8_t use_4addr = nla_get_u8(tb_msg[NL80211_ATTR_4ADDR]);
		if (use_4addr)
			printf("%s\t4addr: on\n", indent);
	}

	return NL_SKIP;
}

static int  (*get_iface_handler)(struct nl_msg *, void *) = print_iface_handler;
static int   iface_handler_data						      = -1;
static void  *get_iface_handler_data					  = &iface_handler_data;

int valid_handler(struct nl_msg *msg, void *arg)
{
	if (get_iface_handler)
	{
		return get_iface_handler(msg, get_iface_handler_data);
	}

	return NL_OK;
}

static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_SKIP;
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_STOP;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	struct nlmsghdr *nlh = (struct nlmsghdr *)err - 1;
	int len = nlh->nlmsg_len;
	struct nlattr *attrs;
	struct nlattr *tb[NLMSGERR_ATTR_MAX + 1];
	int *ret = (int *)arg;
	int ack_len = sizeof(*nlh) + sizeof(int) + sizeof(*nlh);

	*ret = err->error;

	if (!(nlh->nlmsg_flags & NLM_F_ACK_TLVS))
		return NL_STOP;

	if (!(nlh->nlmsg_flags & NLM_F_CAPPED))
		ack_len += err->msg.nlmsg_len - sizeof(*nlh);

	if (len <= ack_len)
		return NL_STOP;

	attrs = (nlattr *)((unsigned char *)nlh + ack_len);
	len -= ack_len;

	nla_parse(tb, NLMSGERR_ATTR_MAX, attrs, len, NULL);
	if (tb[NLMSGERR_ATTR_MSG]) {
		len = strnlen((char *)nla_data(tb[NLMSGERR_ATTR_MSG]),
			      nla_len(tb[NLMSGERR_ATTR_MSG]));
		fprintf(stderr, "kernel reports: %*s\n", len,
			(char *)nla_data(tb[NLMSGERR_ATTR_MSG]));
	}

	return NL_STOP;
}

/**************************************************************************************************************/
/* Main */
/**************************************************************************************************************/

int main(int argc, char ** argv)
{
	/** TODO:
	 * error handling
	 * other types than NL_CB_DEBUG
	 * command typedef (with message flags, nlcommand, ... )
	 */

	#ifdef NLDBG
		printf("Debug level: %d\n", NLDBG);
	#endif

	// find interface index

	signed long long device_index = if_nametoindex("wlp2s0");
	printf("interface index: %d\n", device_index);

	nl_sock_t * p_nl_sock;
	nl_msg_t  * p_nl_msg;
	nl_cb_t   * p_cb,
			  * p_sock_cb;
	int 		family_nl80211_id,
				ret = 1;

	// Allocate a new socket

	p_nl_sock = nl_socket_alloc();

	// Create socket fd and bind socket to the NETLINK GENERIC protocol

	genl_connect(p_nl_sock);

	// Find corresponding numeric id for "nl80211" family
	// NOTE: Genl connect must be done before.

    family_nl80211_id = genl_ctrl_resolve(p_nl_sock, "nl80211");

	// Allocate a new netlink message

	p_nl_msg = nlmsg_alloc();

	// Allocate a new callback handle

	p_cb 	  = nl_cb_alloc(NL_CB_DEBUG);
	p_sock_cb = nl_cb_alloc(NL_CB_DEBUG);

	// Prepare header values

	int 		  message_flags = NLM_F_DUMP;
	nl80211_cmd_t command		= NL80211_CMD_GET_WIPHY;

	// Fill and add the header to the message

	genlmsg_put(p_nl_msg,      	   // message
				NL_AUTO_PORT,	   // port (auto)
				NL_AUTO_SEQ,	   // sequence (auto)
				family_nl80211_id, // family (nl80211) id
				0,				   // user header len
		    	message_flags,     // message flags
				command,	 	   // command
				0);				   // interface version

	// Add attribute type NL80211_ATTR_IFINDEX (interface index) to the message

	nl80211_attr_t attribute = NL80211_ATTR_IFINDEX;

	NLA_PUT_U32(p_nl_msg, attribute, device_index);

	// Set socket callback

	nl_socket_set_cb(p_nl_sock, p_sock_cb);

	// Send message

	nl_send_auto_complete(p_nl_sock, p_nl_msg);

	// Set up callbacks

	nl_cb_err(p_cb, NL_CB_CUSTOM, error_handler, &ret);
	nl_cb_set(p_cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &ret);
	nl_cb_set(p_cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &ret);
	nl_cb_set(p_cb, NL_CB_VALID, NL_CB_CUSTOM, valid_handler, NULL);

	// Receive messages

	while (ret > 0)
	{
		nl_recvmsgs(p_nl_sock, p_cb);
	}

	// Cleanup

	nl_cb_put(p_cb);
	nl_cb_put(p_sock_cb);
	nlmsg_free(p_nl_msg);
	return 0;

nla_put_failure:

	fprintf(stderr, "building message failed\n");
	return 0;
}

