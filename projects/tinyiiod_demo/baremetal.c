#include <errno.h>
#include <iio/iio-backend.h>
#include <iio/iiod-client.h>
#include "baremetal.h"

extern const struct iio_backend iio_external_backend;

struct iio_context_pdata {
	struct iiod_client_pdata io_ctx;
	struct iiod_client *iiod_client;
};

// const char *fakexml = "xml:<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE context [<!ELEMENT context (device | context-attribute)*><!ELEMENT context-attribute EMPTY><!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*><!ELEMENT channel (scan-element?, attribute*)><!ELEMENT attribute EMPTY><!ELEMENT scan-element EMPTY><!ELEMENT debug-attribute EMPTY><!ELEMENT buffer-attribute EMPTY><!ATTLIST context name CDATA #REQUIRED version-major CDATA #REQUIRED version-minor CDATA #REQUIRED version-git CDATA #REQUIRED description CDATA #IMPLIED><!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED><!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED label CDATA #IMPLIED><!ATTLIST channel id CDATA #REQUIRED type (input|output) #REQUIRED name CDATA #IMPLIED><!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED><!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED><!ATTLIST debug-attribute name CDATA #REQUIRED><!ATTLIST buffer-attribute name CDATA #REQUIRED>]><context name=\"xml\" version-major=\"1\" version-minor=\"0\" version-git=\"d476d1bc\" description=\"no-OS/projects/iio_demo buha/newtinyiiod-e364cf667\" ><device id=\"iio:device0\" name=\"adc_demo\" ><channel id=\"voltage0\" name=\"adc_in_ch0\" type=\"input\" ><scan-element index=\"0\" format=\"le:S16/16&gt;&gt;0\" /><attribute name=\"adc_channel_attr\" filename=\"in_voltage0_adc_channel_attr\" /></channel><channel id=\"voltage1\" name=\"adc_in_ch1\" type=\"input\" ><scan-element index=\"1\" format=\"le:S16/16&gt;&gt;0\" /><attribute name=\"adc_channel_attr\" filename=\"in_voltage1_adc_channel_attr\" /></channel><attribute name=\"adc_global_attr\" /><debug-attribute name=\"direct_reg_access\" /></device><device id=\"iio:device1\" name=\"dac_demo\" ><channel id=\"voltage0\" name=\"dac_out_ch0\" type=\"output\" ><scan-element index=\"0\" format=\"le:S16/16&gt;&gt;0\" /><attribute name=\"dac_channel_attr\" filename=\"out_voltage0_dac_channel_attr\" /></channel><channel id=\"voltage1\" name=\"dac_out_ch1\" type=\"output\" ><scan-element index=\"1\" format=\"le:S16/16&gt;&gt;0\" /><attribute name=\"dac_channel_attr\" filename=\"out_voltage1_dac_channel_attr\" /></channel><attribute name=\"dac_global_attr\" /><debug-attribute name=\"direct_reg_access\" /></device></context>";

static struct iio_context *
baremetal_create_context(const struct iio_context_params *params, const char *args)
{
	int ret;
	struct iio_context *ctx;
	const char *ctx_attrs[] = { "xml" };
	const char *ctx_values[1];
	char uri[255 + 3];
	char *description = "no-OS baremetal backend that enables baremetal usage of libiio.";
	
	// ctx = iio_create_context_from_xml(params, fakexml, &iio_external_backend, description, ctx_attrs, ctx_values, 1);
	// ret = iio_err(ctx);
	// if (ret)
	// 	return iio_err_cast(ctx);

	ctx = iio_context_create_from_backend(params, &iio_external_backend, description, 1, 0, "abcdefg");
	ret = iio_err(ctx);
	if (ret)
		return iio_err_cast(ctx);
	
	struct iio_device * d0 = iio_context_add_device(ctx, "iio:device0", "adc_demo", NULL);
	ret = iio_err(d0);
	if (ret)
		return iio_err_cast(d0);
	
	const struct iio_data_format fmt = {
		.length = 16,
		.bits = 16,
		.is_signed = true,
	};
	struct iio_channel * d0c0 = iio_device_add_channel(d0, 0, "voltage0", "adc_in_ch0", false,
		       true, &fmt);
	ret = iio_err(d0c0);
	if (ret)
		return iio_err_cast(d0c0);

	ret = iio_channel_add_attr(d0c0, "adc_channel_attr", "in_voltage0_adc_channel_attr");
	if (ret)
		return iio_ptr(ret);
	struct iio_channel * d0c1 = iio_device_add_channel(d0, 1, "voltage1", "adc_in_ch1", false,
		       true, &fmt);
	ret = iio_err(d0c1);
	if (ret)
		return iio_err_cast(d0c1);

	ret = iio_channel_add_attr(d0c1, "adc_channel_attr", "in_voltage1_adc_channel_attr");
	if (ret)
		return iio_ptr(ret);

	ret = iio_device_add_attr(d0, "adc_global_attr", IIO_ATTR_TYPE_DEVICE);
	if (ret)
		return iio_ptr(ret);

	ret = iio_device_add_attr(d0, "direct_reg_access", IIO_ATTR_TYPE_DEBUG);
	if (ret)
		return iio_ptr(ret);

	return ctx;
end:
	return iio_ptr(ret);
}

static ssize_t
baremetal_read_attr(const struct iio_attr *attr, char *dst, size_t len)
{
	strcpy(dst, "1111");
	return 4;
}

static ssize_t
baremetal_write_attr(const struct iio_attr *attr, const char *src, size_t len)
{
	return 0;
}

struct iio_buffer_pdata * baremetal_create_buffer(const struct iio_device *dev,
					unsigned int idx,
					struct iio_channels_mask *mask)
{
	struct iio_buffer_pdata *pdata = malloc(10 /* arbitrary, backend specific */);
	if (!pdata)
		return iio_ptr(-ENOMEM);
	
	return pdata;
}

void baremetal_free_buffer(struct iio_buffer_pdata *pdata)
{
	free(pdata);
}

const struct iio_device * baremetal_get_trigger(const struct iio_device *dev)
{
	return iio_ptr(-ENODEV);
}

const struct iio_backend_ops ops = {
	.create = baremetal_create_context,
	.read_attr = baremetal_read_attr,
	.write_attr = baremetal_write_attr,
	.create_buffer = baremetal_create_buffer,
	.free_buffer = baremetal_free_buffer,
	.get_trigger = baremetal_get_trigger,
};

const struct iio_backend iio_external_backend = {
	.name = "baremetal",
	.api_version = IIO_BACKEND_API_V1,
	.default_timeout_ms = 200,
	.uri_prefix = "baremetal:",
	.ops = &ops,
};