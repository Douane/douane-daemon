/**
 * This file is a common file for Kernel and user space.
 *
 * It describe the message that is passed from Kernel space to user space.
 */

/* Possible values for in_or_out */
#define INCOMING_PACKET		0
#define OUTCOMING_PACKET	1

#define KIND_HAND_CHECK		1
#define KIND_SENDING_RULE	2
#define KIND_GOODBYE		3
#define KIND_DELETE_RULE	4

/**
 *  Douane network activity message
 */
struct network_activity {
	int		kind;
	int		process_id;
	char	process_path[PATH_MAX * 4];
	char	process_name[16];					// Send the allowed process name from daemon to kernel module
	int		allowed;
	int		in_or_out;
	char	devise_name[16]; // IFNAMSIZ = 16
	int		protocol;
	char	ip_source[16];
	int		port_source;
	char	ip_destination[16];
	int		port_destination;
	int		size;
};

/**
 *	This network_activity message is used in both ways:
 *		- From Kernel to User space in order to forward network activities
 *		- From User space to Kernel in order to send rules
 *
 *	The field process_id is used in both ways.
 *	The field allowed is used only from User space to the Kernel
 */