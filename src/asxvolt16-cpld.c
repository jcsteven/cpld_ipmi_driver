/*
 * Copyright (C)  Brandon Chuang <brandon_chuang@accton.com.tw>
 *
 * Based on:
 *	pca954x.c from Kumar Gala <galak@kernel.crashing.org>
 * Copyright (C) 2006
 *
 * Based on:
 *	pca954x.c from Ken Harrenstien
 * Copyright (C) 2004 Google, Inc. (Ken Harrenstien)
 *
 * Based on:
 *	i2c-virtual_cb.c from Brian Kuschak <bkuschak@yahoo.com>
 * and
 *	pca9540.c from Jean Delvare <khali@linux-fr.org>.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/stat.h>
#include <linux/sysfs.h>
#include <linux/hwmon-sysfs.h>
#include <linux/ipmi.h>
#include <linux/ipmi_smi.h>
#include <linux/platform_device.h>


#define DRVNAME "asxvolt16_cpld"
#define ACCTON_IPMI_NETFN   0x34
#define IPMI_CPLD_READ_CMD   0x1E
#define IPMI_CPLD_WRITE_CMD  0x1F
#define IPMI_TIMEOUT		(20 * HZ)

static void ipmi_msg_handler(struct ipmi_recv_msg *msg, void *user_msg_data);
static int asxvolt16_cpld_probe(struct platform_device *pdev);
static int asxvolt16_cpld_remove(struct platform_device *pdev);


struct ipmi_data {
	struct completion   read_complete;
	struct ipmi_addr	address;
	ipmi_user_t         user;
	int                 interface;

	struct kernel_ipmi_msg tx_message;
	long                   tx_msgid;

	void            *rx_msg_data;
	unsigned short   rx_msg_len;
	unsigned char    rx_result;
	int              rx_recv_type;

	struct ipmi_user_hndl ipmi_hndlrs;
};

struct asxvolt16_cpld_data {
	struct platform_device *pdev;
	struct mutex     update_lock;
	char             valid;           /* != 0 if registers are valid */
	unsigned long    last_updated;    /* In jiffies */
	unsigned char    ipmi_resp[1];    /* ipmi_resp[1] :
	                                    1. When IPMI_CPLD_READ_CMD, return value of <offset of CPLD>
	                                    2. Whem IPMI_CPLD_WRITE_CMD, the value is defined as:
	                                            COMPCODE_NORMAL 0x00
	                                       Or Error code:
	                                            COMPCODE_TIMEOUT 0xC3 <= I2C Read/Write Timeout
	                                            COMPCODE_NODE_BUSY 0xC0<= I2C Bus busy
	                                            COMPCODE_PARAM_OUT_OF_RANGE 0xC9 <=  parameter out of range
			                  */
	struct ipmi_data ipmi;
	unsigned char ipmi_rx_data[1];  /* ipmi_tx_data[1]: Whem IPMI_CPLD_READ_CMD, 0: cpld_offet */
	unsigned char ipmi_tx_data[2];  /* ipmi_tx_data[2]: Whem IPMI_CPLD_WRITE_CMD, 0: cpld_offet, 1: value to write */
};
/* Functions to talk to the IPMI layer */

struct asxvolt16_cpld_data *data = NULL;

static struct platform_driver asxvolt16_cpld_driver = {
    .probe      = asxvolt16_cpld_probe,
    .remove     = asxvolt16_cpld_remove,
    .driver     = {
        .name   = DRVNAME,
        .owner  = THIS_MODULE,
    },
};

/* Initialize IPMI address, message buffers and user data */
static int init_ipmi_data(struct ipmi_data *ipmi, int iface,
			      struct device *dev)
{
	int err;

	init_completion(&ipmi->read_complete);

	/* Initialize IPMI address */
	ipmi->address.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
	ipmi->address.channel = IPMI_BMC_CHANNEL;
	ipmi->address.data[0] = 0;
	ipmi->interface = iface;

	/* Initialize message buffers */
	ipmi->tx_msgid = 0;
	ipmi->tx_message.netfn = ACCTON_IPMI_NETFN;

	ipmi->ipmi_hndlrs.ipmi_recv_hndl = ipmi_msg_handler;

	/* Create IPMI messaging interface user */
	err = ipmi_create_user(ipmi->interface, &ipmi->ipmi_hndlrs,
			       ipmi, &ipmi->user);
	if (err < 0) {
		dev_err(dev, "Unable to register user with IPMI "
			"interface %d\n", ipmi->interface);
		return -EACCES;
	}

	return 0;
}

/* Send an IPMI command */
static int ipmi_send_message(struct ipmi_data *ipmi, unsigned char cmd,
                                     unsigned char *tx_data, unsigned short tx_len,
                                     unsigned char *rx_data, unsigned short rx_len)
{
	int err;

	ipmi->tx_message.cmd      = cmd;
	ipmi->tx_message.data     = tx_data;
	ipmi->tx_message.data_len = tx_len;
	ipmi->rx_msg_data         = rx_data;
	ipmi->rx_msg_len          = rx_len;

	err = ipmi_validate_addr(&ipmi->address, sizeof(ipmi->address));
	if (err)
		goto addr_err;

	ipmi->tx_msgid++;
	err = ipmi_request_settime(ipmi->user, &ipmi->address, ipmi->tx_msgid,
				   &ipmi->tx_message, ipmi, 0, 0, 0);
	if (err)
		goto ipmi_req_err;

	err = wait_for_completion_timeout(&ipmi->read_complete, IPMI_TIMEOUT);
	if (!err)
		goto ipmi_timeout_err;

	return 0;

ipmi_timeout_err:
	err = -ETIMEDOUT;
	dev_err(&data->pdev->dev, "request_timeout=%x\n", err);
	return err;
ipmi_req_err:
	dev_err(&data->pdev->dev, "request_settime=%x\n", err);
	return err;
addr_err:
	dev_err(&data->pdev->dev, "validate_addr=%x\n", err);
	return err;
}

/* Dispatch IPMI messages to callers */
static void ipmi_msg_handler(struct ipmi_recv_msg *msg, void *user_msg_data)
{
	unsigned short rx_len;
	struct ipmi_data *ipmi = user_msg_data;

	if (msg->msgid != ipmi->tx_msgid) {
		dev_err(&data->pdev->dev, "Mismatch between received msgid "
			"(%02x) and transmitted msgid (%02x)!\n",
			(int)msg->msgid,
			(int)ipmi->tx_msgid);
		ipmi_free_recv_msg(msg);
		return;
	}

	ipmi->rx_recv_type = msg->recv_type;
	if (msg->msg.data_len > 0)
		ipmi->rx_result = msg->msg.data[0];
	else
		ipmi->rx_result = IPMI_UNKNOWN_ERR_COMPLETION_CODE;

	if (msg->msg.data_len > 1) {
		rx_len = msg->msg.data_len - 1;
		if (ipmi->rx_msg_len < rx_len)
			rx_len = ipmi->rx_msg_len;
		ipmi->rx_msg_len = rx_len;
		memcpy(ipmi->rx_msg_data, msg->msg.data + 1, ipmi->rx_msg_len);
	} else
		ipmi->rx_msg_len = 0;

	ipmi_free_recv_msg(msg);
	complete(&ipmi->read_complete);
}


static struct asxvolt16_cpld_data *asxvolt16_cpld_update_device(void)
{
	int status = 0;
#if 0
	if (time_before(jiffies, data->last_updated + HZ * 5) && data->valid) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_update_device-Exit-1.\n", __FUNCTION__,__LINE__);
		return data;
	}
#endif
	data->valid = 0;
	status = ipmi_send_message(&data->ipmi, IPMI_CPLD_READ_CMD,data->ipmi_rx_data, sizeof(data->ipmi_rx_data),
                                data->ipmi_resp, sizeof(data->ipmi_resp));
	if (unlikely(status != 0)) {
		goto exit;
	}

	if (unlikely(data->ipmi.rx_result != 0)) {
		status = -EIO;
		goto exit;
	}

	data->last_updated = jiffies;
	data->valid = 1;

exit:
	return data;
}

int asxvolt16_cpld_read(unsigned short cpld_addr, u8 reg)
{
	int ret = -EIO;
	//dev_info(&data->pdev->dev,"%s:%i-Info: asxvolt16_cpld_read-Start.\n", __FUNCTION__,__LINE__);
	mutex_lock(&data->update_lock);
	if (cpld_addr!=0x62) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_read-Exit-1.\n", __FUNCTION__,__LINE__);
		ret = -EIO;
		goto exit;
	}

	/* Send IPMI read command */
	data->ipmi_rx_data[0] = reg;
	data = asxvolt16_cpld_update_device();
	if (!data->valid) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_read-Exit-2.\n", __FUNCTION__,__LINE__);
		ret = -EIO;
		goto exit;
	}
	ret=(unsigned char)data->ipmi_resp[0];
	//dev_info(&data->pdev->dev,"%s:%i-Info: asxvolt16_cpld_read-OK.\n", __FUNCTION__,__LINE__);
exit:
	mutex_unlock(&data->update_lock);
	return ret;
}
EXPORT_SYMBOL(asxvolt16_cpld_read);

int asxvolt16_cpld_write(unsigned short cpld_addr, u8 reg, u8 value)
{
	int status = -EIO;
	//dev_info(&data->pdev->dev,"%s:%i-Info: asxvolt16_cpld_write-Start.\n", __FUNCTION__,__LINE__);
	mutex_lock(&data->update_lock);

	if (cpld_addr!=0x62) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_write-Exit-1.\n", __FUNCTION__,__LINE__);
		status = -EIO;
		goto exit;
	}

	/* Send IPMI write command */
	data->ipmi_tx_data[0] = reg;
	data->ipmi_tx_data[1] = value;

	status = ipmi_send_message(&data->ipmi, IPMI_CPLD_WRITE_CMD,data->ipmi_tx_data, sizeof(data->ipmi_tx_data),
                                data->ipmi_resp, sizeof(data->ipmi_resp));
	if (unlikely(status != 0)) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_write-Exit-2.\n", __FUNCTION__,__LINE__);
		goto exit;
	}

	if (unlikely(data->ipmi.rx_result != 0)) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_write-Exit-3.\n", __FUNCTION__,__LINE__);
		status = -EIO;
		goto exit;
	}
#if 0
	/* However the ipmi return the value, it is still errot if data->ipmi_resp[0] is not zero  */
	if (data->ipmi_resp[0] != 0) {
		dev_err(&data->pdev->dev, "%s:%i-Info: asxvolt16_cpld_write-Exit-4. ipmi_resp[0]=%02X\n", __FUNCTION__,__LINE__,data->ipmi_resp[0]);		
		status = -EIO;
		goto exit;
	}
	status = (unsigned char)data->ipmi_resp[0];
#endif
	//dev_info(&data->pdev->dev,"%s:%i-Info: asxvolt16_cpld_write-OK. status=%d\n", __FUNCTION__,__LINE__,status);

exit:
	mutex_unlock(&data->update_lock);
	return status;
}
EXPORT_SYMBOL(asxvolt16_cpld_write);

static int asxvolt16_cpld_probe(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "device created\n");
    return 0;
}

static int asxvolt16_cpld_remove(struct platform_device *pdev)
{
    return 0;
}

static int __init asxvolt16_cpld_init(void)
{
    int ret;

    data = kzalloc(sizeof(struct asxvolt16_cpld_data), GFP_KERNEL);
    if (!data) {
        ret = -ENOMEM;
        goto alloc_err;
    }

	mutex_init(&data->update_lock);
    data->valid = 0;

    ret = platform_driver_register(&asxvolt16_cpld_driver);
    if (ret < 0) {
        goto dri_reg_err;
    }

    data->pdev = platform_device_register_simple(DRVNAME, -1, NULL, 0);
    if (IS_ERR(data->pdev)) {
        ret = PTR_ERR(data->pdev);
        goto dev_reg_err;
    }

	/* Set up IPMI interface */
	ret = init_ipmi_data(&data->ipmi, 0, &data->pdev->dev);
	if (ret)
		goto ipmi_err;

    return 0;

ipmi_err:
    platform_device_unregister(data->pdev);
dev_reg_err:
    platform_driver_unregister(&asxvolt16_cpld_driver);
dri_reg_err:
    kfree(data);
alloc_err:
    return ret;
}

static void __exit asxvolt16_cpld_exit(void)
{
    ipmi_destroy_user(data->ipmi.user);
    platform_device_unregister(data->pdev);
    platform_driver_unregister(&asxvolt16_cpld_driver);
    kfree(data);
}


MODULE_AUTHOR("JC Yu <jcyu@edge-core.com>");
MODULE_DESCRIPTION("ASXVOLT CPLD2 through IPMI driver");
MODULE_LICENSE("GPL");

module_init(asxvolt16_cpld_init);
module_exit(asxvolt16_cpld_exit);
